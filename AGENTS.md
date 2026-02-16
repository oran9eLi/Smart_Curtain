# Smart_Curtain - Agent Knowledge Base

**Generated**: 2026-02-16  
**Platform**: STM32F103C8 (ARM Cortex-M3)  
**Toolchain**: Keil MDK-ARM / EIDE

---

## Project Overview

Event-driven FSM (Finite State Machine) smart curtain control system.  
Uses HAL library, non-blocking architecture, unified event dispatch.

---

## Build Commands

```bash
# Using EIDE (Embedded IDE for VSCode)
EIDE: Build Project          # Compile
EIDE: Rebuild Project        # Clean + compile
EIDE: Flash (ST-Link)        # Download to MCU
EIDE: Download & Run        # Flash + debug

# Via Keil uVision
Build (F7)                   # Compile
Rebuild (F7)                 # Clean build
Download (F8)                # Flash to device
```

**No unit tests exist** - testing is manual via hardware debugging.

---

## Code Style Guidelines

### Naming Conventions

| Type | Convention | Example |
|------|------------|---------|
| Functions | `Module_Action()` | `Motor_Start()`, `Event_Queue()` |
| Variables | `snake_case` | `key_scan_tick`, `Global_State` |
| Constants/Defines | `UPPER_SNAKE` | `GPIO_PIN_8`, `MOTOR_CW` |
| Enums | `Module_Type_t` | `Motor_Dir_t`, `Event_Type` |
| Structs | `Module_t` | `Event_t`, `SysStatus_t` |
| Global variables | `g_` prefix | `g_setting_hour` |

### File Organization

```
BSP/MODULE/
├── bsp_module.h    # Public API + includes
└── bsp_module.c   # Implementation
```

**Header guard**: `__BSP_MODULENAME_H`

 Structure### Code

```c
/* Includes ---------------------------------------------------------------*/
#include "main.h"
#include "bsp_xxx.h"

/* Private defines --------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private variables ------------------------------------------------------*/
/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -------------------------------------------*/

/* Private user code -----------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  Function description
  * @param  param: description
  * @retval description
  */
```

### Comments

- Chinese comments for business logic (most common in this codebase)
- English for API documentation
- Use `/** */` for Doxygen-style function docs
- Keep comments concise

### Event-Driven Architecture

**ALWAYS** go through event queue - never modify state directly:

```c
// WRONG - direct hardware access
if (key_pressed) {
    Motor_Start(MOTOR_CW);
}

// CORRECT - event-based
if (key_pressed) {
    Event_Queue(EVT_KEY_PRESS, KEY2);
}
```

### State Machine Rules

- All state transitions via FSM handlers in `main.c`
- One handler per state: `Handle_<StateName>(Event_t *evt)`
- Use hysteresis for sensor thresholds (avoid jitter)

### ISR (Interrupt) Guidelines

- Keep ISRs short - only set flags, do actual work in main loop
- Use `__weak` marked HAL callbacks
- Example:

```c
// In stm32f1xx_it.c - ISR
void TIM2_IRQHandler(void) {
    HAL_TIM_IRQHandler(&htim2);
}

// In bsp_xxx.c - weak callback
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
    if (htim->Instance == TIM2) {
        SoftTime_Tick_1ms();  // Light work only
    }
}
```

### Memory & Performance

- Use `static` for local variables in ISRs
- Minimize `malloc()` - prefer static buffers
- Watch stack usage (default ~1KB for Cortex-M3)

---

## Directory Reference

| Path | Purpose |
|------|---------|
| `Core/Src/main.c` | FSM state handlers, main loop |
| `EVENT/event.c` | Event queue, dispatch |
| `TIME/soft_time.c` | Software timer |
| `BSP/*/` | Hardware drivers |
| `Drivers/` | STM32 HAL (DO NOT modify) |

---

## Anti-Patterns

- **NEVER** modify HAL library files directly
- **NEVER** put blocking `HAL_Delay()` in ISR context
- **NEVER** access hardware directly from event handlers - use BSP functions
- **AVOID** global variables - prefer `Sys_Context` struct
- **AVOID** magic numbers - use `#define` or enums

---

## Key Patterns

### Event Queue Usage

```c
// Producer (ISR or callback)
Event_Queue(EVT_SENSOR_LIGHT, LUX_LOW);

// Consumer (main loop)
void main_loop(void) {
    evt = Event_Dequeue();
    if (evt.type != EVT_NONE) {
        System_Dispatch(&evt);
    }
}
```

### Hysteresis Sensor Threshold

```c
#define LIGHT_THRESHOLD_ON    2000  // Dark → close
#define LIGHT_THRESHOLD_OFF  3000  // Bright → open
// Buffer zone: 2000-3000 = no state change
```

### Debouncing

Uses state-machine debounce (non-blocking), not delay.

---

## IDE Configuration

- **EIDE**: `.eide/` folder - manages Keil project
- **CubeMX**: `Smart_Curtain.ioc` - hardware config
- **Keil**: `MDK-ARM/Smart_Curtain.uvprojx`

---

## Common Development Tasks

| Task | Location | Notes |
|------|----------|-------|
| Add new sensor | `BSP/SENSOR/` | Implement `Sensor_Read()` |
| Add new mode | `main.c` Handle_* | Add FSM state + transitions |
| Modify timing | `TIME/soft_time.c` | Timer tick rate |
| Debug | Breakpoints in `main.c` | State handlers |

---

## Notes

- Chinese comments throughout codebase - maintain consistency
- Uses `stdint.h` types (`uint8_t`, `uint32_t`, etc.)
- No unit tests - manual testing on hardware required
- Independent watchdog (IWDG) enabled - must feed in main loop
