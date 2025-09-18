#include "stm32l4xx_it.h"

#include "main.h"

namespace {
// Fault handlers share this stub so we halt cleanly and keep the debugger attached
[[noreturn]] void TrapForever() {
    __disable_irq();
    while (true) {
    }
}
}  // namespace

extern "C" {

// These core fault handlers should never return; if they fire we park in TrapForever
void NMI_Handler() {
    TrapForever();
}

void HardFault_Handler() {
    TrapForever();
}

void MemManage_Handler() {
    TrapForever();
}

void BusFault_Handler() {
    TrapForever();
}

void UsageFault_Handler() {
    TrapForever();
}

// Supervisor and debug hooks are unused in this bare-metal build
void SVC_Handler() {}

void DebugMon_Handler() {}

void PendSV_Handler() {}

// Keep the HAL tick running for HAL_Delay and timeouts
void SysTick_Handler() {
    HAL_IncTick();
}

}  // extern "C"
