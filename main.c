#include "stm32f4xx.h"
#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "task.h"
#include "queue.h"
#include "stdio.h"
#include "LDR.h"
#include "dht11.h"
#include "PIR.h"

TaskHandle_t pirHandle = NULL;
TaskHandle_t dhtHandle = NULL;
TaskHandle_t ldrHandle = NULL;

int main(){
	
	//xTaskCreate(controlTask, "Main Task", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY, NULL);
	xTaskCreate(dhtTask, "DHT Task", configMINIMAL_STACK_SIZE, NULL, 1, &dhtHandle);
	xTaskCreate(ldrTask, "LED Task", configMINIMAL_STACK_SIZE, NULL, 2, &ldrHandle);
	xTaskCreate(PIRTask,  "PIR Task",  configMINIMAL_STACK_SIZE, NULL, 3, &pirHandle);
	//xTaskCreate(doorTask, "Door Task", configMINIMAL_STACK_SIZE, NULL, 4, NULL);
	
	vTaskStartScheduler();
	
}
