document.addEventListener("DOMContentLoaded", () => {
  const card = document.querySelector(".card[data-initial-duty]");
  if (!card) {
    return;
  }

  const slider = document.getElementById("dutyCycleSlider");
  const valueLabel = document.getElementById("dutyCycleValue");
  const lastSavedValue = document.getElementById("lastSavedValue");
  const lastUpdatedValue = document.getElementById("lastUpdatedValue");
  const statusMessage = document.getElementById("statusMessage");
  const applyButton = document.getElementById("applyButton");

  if (!slider || !valueLabel || !statusMessage || !applyButton || !lastSavedValue || !lastUpdatedValue) {
    return;
  }

  const {
    initialDuty = "0",
    lastUpdated = "",
    apiDuty: apiDutyEndpoint = "",
    apiDutyUpdate: apiDutyUpdateEndpoint = ""
  } = card.dataset;

  const toFixedString = (value) => Number.parseFloat(value).toFixed(1);

  const formatTimestamp = (isoString) => {
    if (!isoString) {
      return "\u2014";
    }

    const parsed = new Date(isoString);
    if (Number.isNaN(parsed.getTime())) {
      return isoString;
    }

    return parsed.toLocaleString();
  };

  const updateLabel = (value) => {
    valueLabel.textContent = toFixedString(value);
  };

  const updateLastSaved = (value, isoString) => {
    lastSavedValue.textContent = toFixedString(value);
    lastUpdatedValue.textContent = formatTimestamp(isoString);
  };

  const showStatus = (message, isError = false) => {
    statusMessage.textContent = message;
    statusMessage.classList.toggle("error", Boolean(isError));
  };

  const fetchCurrentDutyCycle = async () => {
    if (!apiDutyEndpoint) {
      return;
    }

    try {
      const response = await fetch(apiDutyEndpoint);
      if (!response.ok) {
        throw new Error(`Request failed: ${response.status}`);
      }
      const data = await response.json();
      const dutyCycle = data.duty_cycle;
      const updatedAt = data.last_updated;
      slider.value = dutyCycle;
      updateLabel(dutyCycle);
      updateLastSaved(dutyCycle, updatedAt);
      showStatus("");
    } catch (error) {
      console.error(error);
      showStatus("Unable to read current duty cycle.", true);
    }
  };

  const sendDutyCycle = async (value) => {
    if (!apiDutyUpdateEndpoint) {
      showStatus("Duty cycle endpoint unavailable.", true);
      return;
    }

    applyButton.disabled = true;
    showStatus("Updating...");
    try {
      const response = await fetch(apiDutyUpdateEndpoint, {
        method: "POST",
        headers: {
          "Content-Type": "application/json"
        },
        body: JSON.stringify({ duty_cycle: Number.parseFloat(value) })
      });

      const data = await response.json().catch(() => ({}));
      if (!response.ok) {
        const errorMessage = data.error || `Request failed: ${response.status}`;
        throw new Error(errorMessage);
      }

      if (typeof data.duty_cycle === "number") {
        slider.value = data.duty_cycle;
        updateLabel(data.duty_cycle);
        updateLastSaved(data.duty_cycle, data.last_updated || "");
        showStatus("Duty cycle updated.");
      } else {
        showStatus("Unexpected response from server.", true);
      }
    } catch (error) {
      console.error(error);
      showStatus(error.message, true);
    } finally {
      applyButton.disabled = false;
    }
  };

  slider.addEventListener("input", (event) => {
    updateLabel(event.target.value);
  });

  applyButton.addEventListener("click", () => {
    const value = slider.value;
    const confirmationValue = toFixedString(value);
    const confirmed = window.confirm(`Apply ${confirmationValue}% PWM duty cycle?`);
    if (!confirmed) {
      showStatus("Change cancelled.");
      return;
    }
    sendDutyCycle(value);
  });

  slider.value = initialDuty;
  updateLabel(initialDuty);
  updateLastSaved(initialDuty, lastUpdated);
  fetchCurrentDutyCycle();
});
