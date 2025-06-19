#include "stm32f4xx.h"
#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "task.h"
#include "queue.h"

//Port and Pin Configuration for LDR Sensor
#define LDR_Port 		GPIOB
#define LDR_Pin  		0

extern TaskHandle_t ldrHandle;
void ldrSetup(void);
void ldrTask(void* __attribute__((unused)));
void ADC_IRQHandler(void);
