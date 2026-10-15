#ifndef MAIN_H
#define MAIN_H

#include "stm32f4xx_hal.h"

#define STEP_DIR_GPIO_Port  GPIOB
#define STEP_DIR_Pin        GPIO_PIN_0

#define STEP_PULSE_GPIO_Port GPIOB
#define STEP_PULSE_Pin       GPIO_PIN_1

#define USER_BUTTON_GPIO_Port GPIOA
#define USER_BUTTON_Pin       GPIO_PIN_0


void SystemClock_Config(void);

#endif // !MAIN_H