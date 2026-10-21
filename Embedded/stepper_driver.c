#include "stepper_driver.h"

static volatile uint32_t steps_to_move = 0;
static volatile bool move_active = false;

TIM_HandleTypeDef htim2;

void stepper_init(void)
{
    // TODO All timing should depend on SystemClock_Config that's literally what's it's for
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_TIM2_CLK_ENABLE();

    GPIO_InitStruct.Pin = STEP_DIR_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(STEP_DIR_GPIO_Port, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = STEP_PULSE_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(STEP_PULSE_GPIO_Port, &GPIO_InitStruct);

    htim2.Instance = TIM2;
    htim2.Init.Prescaler = 84 - 1;
    htim2.Init.Period = 1000 - 1;
    htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
    if (HAL_TIM_Base_Init(&htim2))
        Error_Handler();

    HAL_NVIC_SetPriority(TIM2_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(TIM2_IRQn);
}

void stepper_move(MotorDirection dir, uint32_t steps)
{
    if (button_pressed)
    {
        button_pressed = false;

        if (move_active)
        {
            return;
        }

        if (dir == DIR_RIGHT)
        {
            HAL_GPIO_WritePin(STEP_DIR_GPIO_Port, STEP_DIR_Pin, GPIO_PIN_SET);
        }
        else
        {
            HAL_GPIO_WritePin(STEP_DIR_GPIO_Port, STEP_DIR_Pin, GPIO_PIN_RESET);
        }

        /*
        # Teoretycznie:
        ` HAL_GPIO_WritePin(STEP_DIR_GPIO_Port, STEP_DIR_Pin, dir); ` zamiast tego if(dir==DIR_RIGHT){...} calego
            dir_right    to 1, dir_left       to 0
            GPIO_PIN_SET to 1, GPIO_PIN_RESET to 0

        można by bylo skipnac ifa i jakos minimalnie szybciej by mogło byc

        Wiadomo że jesli dodamy wiecej w przyszlosci to bedzie chujowo, ale daloby sie z tym pracowac jesli sie pobawimy troche pinami
        nie bede dawac perf(move->direction) bo to zbyt malo żeby cos nam dalo, wiec bez sensu cokolwiek zmieniac
        */

        steps_to_move = steps * 2; //! probs will overflow unless i don't know what i'm saying
        // cuz it's only increasing right
        move_active = true;

        if (HAL_TIM_Base_Start_IT(&htim2))
        {
            move_active = false;
            Error_Handler();
        }
    }
}

void TIM2_IRQHandler(void)
{
    if (__HAL_TIM_GET_FLAG(&htim2, TIM_FLAG_UPDATE) != RESET)
    {
        if (steps_to_move > 0)
        {
            HAL_GPIO_TogglePin(STEP_PULSE_GPIO_Port, STEP_PULSE_Pin);
            steps_to_move--;
        }
        else
        {

            HAL_GPIO_WritePin(STEP_PULSE_GPIO_Port, STEP_PULSE_Pin, GPIO_PIN_RESET);
            HAL_TIM_Base_Stop_IT(&htim2);
            move_active = false;
        }
    }

    __HAL_TIM_CLEAR_IT(&htim2, TIM_IT_UPDATE);
}