#include "stdio.h"
#include "string.h"
#include "stm32f4xx.h"
#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "task.h"
#include "queue.h"

//Port and Pin Configuration for DHT11
#define DHT_Port 		GPIOA
#define DHT_Pin	 		5

#define MAX_PULSE_WIDTH 100
#define MIN_PULSE_WIDTH  20

extern uint8_t data[5];
extern uint8_t byteIndex;
extern uint8_t bitIndex;
extern TickType_t start;
extern uint32_t pulseWidth;
extern int dataReady;
extern QueueHandle_t pWQueue;
extern TaskHandle_t dhtHandle;
extern BaseType_t xHPTW1;

void timerSetup(void);
void DHT_Out_Setup(void);
void DHT_In_Setup(void);
void EXTI_Setup_DHT(void);
void TIM1_UP_TIM10_IRQHandler(void);
void EXTI9_5_IRQHandler(void);
void dhtTask (void*__attribute__((unused)));
void initializeDHT(void);
void DHT11_Delay(void);




