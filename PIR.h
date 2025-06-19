#include "stm32f4xx.h"
#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "task.h"
#include "queue.h"

//Port and Pin Configuration for PIR Sensor
#define PIR_Port    GPIOC
#define PIR_Pin     4

extern TaskHandle_t pirHandle;
void PIRTask(void* param __attribute__((unused)));
void PIR_In_Setup(void);
void EXTI_Setup_PIR(void);
void EXTI4_IRQHandler(void);