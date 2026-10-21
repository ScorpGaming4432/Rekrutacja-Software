#ifndef STEPPER_DRIVER_H
#define STEPPER_DRIVER_H

#include "main.h"

typedef enum {
    DIR_LEFT,
    DIR_RIGHT
} MotorDirection;

void stepper_init(void);

void stepper_move(MotorDirection dir, uint32_t steps);

#endif // STEPPER_DRIVER_H
