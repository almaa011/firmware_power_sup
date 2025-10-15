"""
Flask application providing a simple UI and REST API for adjusting the PWM duty cycle.

The actual communication with the embedded firmware should be implemented inside
`apply_duty_cycle_to_hardware`. Right now it simply stores the requested duty
cycle so the UI can function even without the hardware connection.
"""

from __future__ import annotations

import os
import threading
from datetime import datetime, timezone
from typing import Callable, Optional

from flask import Flask, jsonify, render_template, request

try:
    import serial  # type: ignore
except ImportError:  # pragma: no cover - handled at runtime
    serial = None  # pyserial is optional until hardware is connected

PERCENT_MIN = 0.0
PERCENT_MAX = 100.0
DEFAULT_DUTY_CYCLE = 50.0  # percent
SERIAL_PORT = os.environ.get("PWM_SERIAL_PORT", "COM5")
SERIAL_BAUD = int(os.environ.get("PWM_SERIAL_BAUD", "115200"))
SERIAL_TIMEOUT = float(os.environ.get("PWM_SERIAL_TIMEOUT", "1.0"))
SERIAL_ACK_ENABLED = os.environ.get("PWM_REQUIRE_ACK", "0") == "1"
SERIAL_ACK_EXPECTED = os.environ.get("PWM_ACK_STRING", "OK").encode("ascii")


class PwmSerialBridge:
    """
    Minimal adapter that sends PWM duty cycle updates to the STM32 over USB CDC.

    The firmware is expected to echo an ASCII acknowledgement (default: `OK`)
    terminated by a newline. Disable acknowledgement checks by leaving
    `PWM_REQUIRE_ACK` unset.
    """

    def __init__(self, port: str, baudrate: int, timeout: float) -> None:
        self._port = port
        self._baudrate = baudrate
        self._timeout = timeout
        self._lock = threading.Lock()
        self._serial: Optional["serial.Serial"] = None

    def _ensure_open(self) -> "serial.Serial":
        if serial is None:
            raise RuntimeError(
                "pyserial is not installed. Install it with `pip install pyserial`."
            )

        if self._serial and self._serial.is_open:
            return self._serial

        self._serial = serial.Serial(
            port=self._port,
            baudrate=self._baudrate,
            timeout=self._timeout,
            write_timeout=self._timeout,
        )
        # Flush any stale bytes left in the buffer so we read fresh ACKs only.
        self._serial.reset_input_buffer()
        self._serial.reset_output_buffer()
        return self._serial

    def send_duty_cycle(self, duty_cycle_fraction: float) -> None:
        """
        Serialize the duty cycle as ASCII and emit it over the COM port.

        The format is `PWM:<fraction>\n`. Example for 42.5% => `PWM:0.4250`.
        """
        message = f"PWM:{duty_cycle_fraction:.4f}\n".encode("ascii")
        with self._lock:
            connection = self._ensure_open()
            connection.write(message)
            connection.flush()
            if SERIAL_ACK_ENABLED:
                ack = connection.readline().strip()
                if ack != SERIAL_ACK_EXPECTED:
                    raise RuntimeError(
                        f"Unexpected ACK from MCU: {ack!r} (expected {SERIAL_ACK_EXPECTED!r})"
                    )


serial_bridge = PwmSerialBridge(SERIAL_PORT, SERIAL_BAUD, SERIAL_TIMEOUT)


def apply_duty_cycle_to_hardware(duty_cycle_fraction: float) -> None:
    """Send the requested duty cycle to the STM32 using the serial bridge."""
    serial_bridge.send_duty_cycle(duty_cycle_fraction)


class DutyCycleController:
    """Stores and validates the PWM duty cycle before delegating to the hardware."""

    def __init__(self, on_change: Optional[Callable[[float], None]] = None) -> None:
        self._lock = threading.Lock()
        self._duty_cycle = DEFAULT_DUTY_CYCLE
        self._updated_at = datetime.now(timezone.utc)
        self._on_change = on_change

    def get_duty_cycle(self) -> float:
        with self._lock:
            return self._duty_cycle

    def get_last_updated_iso(self) -> Optional[str]:
        with self._lock:
            updated_at = self._updated_at

        if updated_at is None:
            return None

        return updated_at.isoformat().replace("+00:00", "Z")

    def set_duty_cycle(self, duty_cycle_percent: float) -> float:
        duty_cycle = float(duty_cycle_percent)
        if duty_cycle < PERCENT_MIN or duty_cycle > PERCENT_MAX:
            raise ValueError(
                f"Duty cycle must be between {PERCENT_MIN} and {PERCENT_MAX} percent."
            )

        duty_cycle = round(duty_cycle, 2)
        duty_cycle_fraction = duty_cycle / 100.0
        with self._lock:
            self._duty_cycle = duty_cycle
            self._updated_at = datetime.now(timezone.utc)

        if self._on_change is not None:
            try:
                self._on_change(duty_cycle_fraction)
            except Exception as exc:  # pylint: disable=broad-except
                # Surface the error so it can be handled by the caller.
                raise RuntimeError("Failed to apply duty cycle to hardware.") from exc

        return duty_cycle


app = Flask(__name__, template_folder="templates", static_folder="static")
controller = DutyCycleController(on_change=apply_duty_cycle_to_hardware)


@app.route("/", methods=["GET"])
def index():
    """Render the main UI."""
    return render_template(
        "index.html",
        duty_cycle=controller.get_duty_cycle(),
        last_updated=controller.get_last_updated_iso(),
    )


@app.route("/api/duty-cycle", methods=["GET"])
def api_get_duty_cycle():
    """Return the currently configured duty cycle as JSON."""
    return jsonify(
        {
            "duty_cycle": controller.get_duty_cycle(),
            "last_updated": controller.get_last_updated_iso(),
        }
    )


@app.route("/api/duty-cycle", methods=["POST"])
def api_set_duty_cycle():
    """Update the PWM duty cycle via JSON payload."""
    payload = request.get_json(silent=True) or {}
    if "duty_cycle" not in payload:
        return jsonify({"error": "Missing 'duty_cycle' in request body."}), 400

    try:
        duty_cycle = float(payload["duty_cycle"])
    except (TypeError, ValueError):
        return jsonify({"error": "Duty cycle must be a number."}), 400

    try:
        updated = controller.set_duty_cycle(duty_cycle)
    except ValueError as exc:
        return jsonify({"error": str(exc)}), 400
    except RuntimeError as exc:
        return jsonify({"error": str(exc)}), 500

    return jsonify(
        {"duty_cycle": updated, "last_updated": controller.get_last_updated_iso()}
    )


def create_app() -> Flask:
    """Entry point for WSGI servers."""
    return app


if __name__ == "__main__":
    debug_mode = os.environ.get("FLASK_DEBUG")
    port = int(os.environ.get("PWM_UI_PORT", "5000"))
    app.run(host="0.0.0.0", port=port, debug=debug_mode == "1")
