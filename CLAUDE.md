# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

STM32F103C8T6-based smart curtain controller. Event-driven FSM architecture. No unit tests — all testing is manual via hardware debugging with ST-Link.

## Build

This project uses **EIDE (Embedded IDE for VSCode)** as the primary build tool, with **Keil µVision** as an alternative.

```
# EIDE (VSCode)
EIDE: Build Project      # Compile
EIDE: Rebuild Project    # Clean + compile
EIDE: Flash (ST-Link)    # Download to MCU

# Keil µVision
F7   # Build
F8   # Download to device
```

Hardware configuration is managed in **`Smart_Curtain.ioc`** (STM32CubeMX). Regenerating from CubeMX overwrites `Core/` files — keep custom code inside `/* USER CODE BEGIN */` / `/* USER CODE END */` blocks.

**Never modify `Drivers/`** (STM32 HAL and CMSIS).

## Architecture

### Layered Structure

```
Application (Core/Src/main.c)  ← FSM state handlers, System_Dispatch
    ↕ events (EVENT/)
BSP/ drivers                   ← one folder per peripheral
    ↕ HAL calls
Drivers/ (HAL + CMSIS)         ← DO NOT MODIFY
```

### Event-Driven FSM (Core/Src/main.c)

All external inputs are abstracted as events and routed through a central dispatcher (`System_Dispatch`). No module directly modifies system state or calls hardware outside its BSP layer.

**9 FSM states** (defined in `Core/Inc/main.h`):
- `FSM_IDLE_LUX` / `FSM_IDLE_TIM` / `FSM_IDLE_MANUAL` — three idle modes
- `FSM_OPENING` / `FSM_CLOSING` — motor in motion
- `FSM_SET_OPEN_TIME` / `FSM_SET_CLOSE_TIME` / `FSM_SET_SYSTEM_TIME` — config
- `FSM_ERROR`

Each state has one handler: `Handle_<StateName>(Event_t *evt)` in `main.c`. Unrecognized events in a state are silently ignored (whitelist model).

**Event types** (`EVENT/event.h`): `EVT_KEY_PRESS`, `EVT_BT_CMD`, `EVT_SENSOR_LIGHT`, `EVT_SENSOR_CO`, `EVT_MOTOR_STATE`, `EVT_TIMER_CTRL`, `EVT_SET_COMPLETE`, `EVT_ERROR`.

Event queue is a 10-slot ring buffer. Enqueue from ISR/callbacks; dequeue in main loop.

### Software Timer (TIME/soft_time.c)

TIM2 fires at 1 kHz → `SoftTime_Tick_1ms()`. Maintains system time (H:M:S) with interrupt-disabled atomic access. Triggers `EVT_TIMER_CTRL` at scheduled hours (fires once per hour via flag).

### Sensor Aggregator (BSP/SENSOR/sensor.c)

Scans all sensors every 1 second. Provides unified `SensorData_t` (temp, humi, lux, CO). Light threshold uses hysteresis (two thresholds with dead-band) to prevent motor jitter.

### Key Input (BSP/KEY/bsp_key.c)

Non-blocking state-machine debounce (no `HAL_Delay`). States: IDLE → DEBOUNCE → PRESSED. 10 ms scan interval.

## Code Conventions

### Naming

| Type | Convention | Example |
|------|------------|---------|
| Functions | `Module_Action()` | `Motor_Start()`, `Event_Queue()` |
| Variables | `snake_case` | `key_scan_tick` |
| Global variables | `g_` prefix | `g_setting_hour` |
| Constants/Macros | `UPPER_SNAKE` | `LIGHT_THRESHOLD_ON` |
| Enums | `Module_Type_t` | `Motor_Dir_t` |
| Structs | `Module_t` | `SysStatus_t` |

### BSP module layout

```
BSP/MODULE/
├── bsp_module.h    # Public API; header guard: __BSP_MODULENAME_H
└── bsp_module.c    # Implementation; include "main.h" and own header
```

### ISR rules

Keep ISRs minimal — set flags or call lightweight tick functions only. Do real work in the main loop or BSP callbacks.

```c
// stm32f1xx_it.c
void TIM2_IRQHandler(void) { HAL_TIM_IRQHandler(&htim2); }

// bsp_xxx.c
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
    if (htim->Instance == TIM2) SoftTime_Tick_1ms();
}
```

### Memory

- Prefer `static` for persistent local variables (especially in scan functions).
- Avoid `malloc`; use static buffers. Stack is ~1 KB.
- IWDG (watchdog) is enabled — must be fed in the main loop.

## Anti-Patterns

- **Never** enqueue events from state handlers and also directly call `Motor_Start()` — pick one path (event queue).
- **Never** call `HAL_Delay()` in ISR context.
- **Never** access GPIO/peripherals directly from `main.c` — always go through BSP functions.
- **Avoid** adding global variables outside of `Sys_Context` / `SysStatus_t`.
- **Avoid** magic numbers — use `#define` or enums.

## Peripheral Map (quick reference)

| Peripheral | Role |
|-----------|------|
| ADC1 CH0/CH1 (PA0/PA1) + DMA1_Ch1 | Light (GL5528) & CO (MQ-7) |
| I2C1 (PB6/PB7) | OLED SSD1306 |
| USART1 (PA9/PA10) | Debug |
| USART2 (PA2/PA3) | Bluetooth HC-05 |
| USART3 (PB10/PB11) | Voice JR6001 |
| TIM2 | 1 kHz software-timer tick |
| PB8/PB9/PA12/PA15 | Stepper motor (28BYJ-48) |
| PB12–PB15 | Button inputs |
| PB0 / PC13 | LED / Buzzer |
| PA11 | DHT11 (1-wire) |
