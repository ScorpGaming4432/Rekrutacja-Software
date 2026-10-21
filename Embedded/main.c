#include "main.h"
#include "stepper_driver.h"
#include <stdbool.h>
#include <stdio.h>

bool button_pressed = false;

UART_HandleTypeDef huart4;

#define UART_TX_BUFFER_SIZE 16
char tx_buffer[UART_TX_BUFFER_SIZE];

void Error_Handler() {
    for(;;); // chyba że printf("\a"); a potem loop
}

void uart_init(void)
{
    huart4.Instance = UART4;
    huart4.Init.BaudRate = 115200;
    huart4.Init.WordLength = UART_WORDLENGTH_8B;
    huart4.Init.StopBits = UART_STOPBITS_1;
    huart4.Init.Parity = UART_PARITY_NONE;
    huart4.Init.Mode = UART_MODE_TX_RX;
    huart4.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    huart4.Init.OverSampling = UART_OVERSAMPLING_16;

    if (HAL_UART_Init(&huart4) != HAL_OK)
    {
        Error_Handler();
    }
}

void button_init()
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    __HAL_RCC_GPIOA_CLK_ENABLE();

    GPIO_InitStruct.Pin = USER_BUTTON_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(USER_BUTTON_GPIO_Port, &GPIO_InitStruct);

    HAL_NVIC_SetPriority(EXTI0_IRQn, 2, 0);
    HAL_NVIC_EnableIRQ(EXTI0_IRQn);
}

void EXTI0_IRQHandler()
{
    HAL_GPIO_EXTI_IRQHandler(USER_BUTTON_Pin);
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    if (GPIO_Pin == USER_BUTTON_Pin)
    {
        button_pressed = true;
    }
}

int main()
{
    HAL_Init();
    SystemClock_Config();

    stepper_init();
    uart_init();
    button_init();

    sprintf(tx_buffer, "Motor initialized, press button to move.\r\n");
    HAL_UART_Transmit(&huart4, (uint8_t*)tx_buffer, strlen(tx_buffer), 100);

    while (1)
    {
        stepper_move(DIR_RIGHT, 500);

        HAL_Delay(10);
    }

    return 0;
}