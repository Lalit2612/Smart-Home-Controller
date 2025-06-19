#include "dht11.h"
#include "queue.h"

uint32_t pulseWidth = 0;
uint8_t bitIndex = 0;
uint8_t byteIndex = 0;
int dataReady = 0;
uint8_t data[5] = {0};

QueueHandle_t pWQueue;
BaseType_t xHPTW1 = pdFALSE;

void DHT11_Delay(){
	RCC->APB2ENR |= 1;
	TIM1->CNT = 0;
	TIM1->PSC = 83;
	TIM1->CR1 |= 0x9;
	while(TIM1->CNT != 30);
	TIM1->CNT = 0;
}

void timerSetup(){
	RCC->APB2ENR |= 1;
	TIM1->CNT = 0;
	TIM1->PSC = 83;
	TIM1->DIER |= TIM_DIER_UIE;
	NVIC_EnableIRQ(TIM1_UP_TIM10_IRQn);
}

void DHT_Out_Setup(){
	
	RCC->AHB1ENR |= 1;
	
	DHT_Port->MODER &= ~(0x3 << (DHT_Pin * 2));
	DHT_Port->MODER |= (1 << (DHT_Pin * 2));
	
	DHT_Port->OTYPER &= ~(1 << DHT_Pin);
	
	DHT_Port->ODR &= ~(1 << DHT_Pin);
	
}

void DHT_In_Setup(){
	
	RCC->AHB1ENR |= 1;
	
	DHT_Port->MODER &= ~(0x3 << (DHT_Pin * 2));
	
	DHT_Port->OTYPER &= ~(1 << DHT_Pin);
	
	DHT_Port->PUPDR &= ~(0x3 << (DHT_Pin * 2));
	DHT_Port->PUPDR |= (0x2 << (DHT_Pin * 2));
		
}
void EXTI_Setup_DHT(){
	EXTI->RTSR |= (1<<DHT_Pin);
	EXTI->FTSR |= (1<<DHT_Pin);
	EXTI->IMR  |= (1<<DHT_Pin);
	NVIC_EnableIRQ(EXTI9_5_IRQn);
}


void TIM1_UP_TIM10_IRQHandler(){
	
	if(TIM1->SR & TIM_SR_UIF){
			TIM1->SR &= ~TIM_SR_UIF;
			TIM1->CR1 &= ~TIM_CR1_CEN;
			xTaskNotifyFromISR(dhtHandle, 1, eSetValueWithOverwrite, 0);
	}
}

void EXTI9_5_IRQHandler(){
		
		if(EXTI->PR & (1<<DHT_Pin)){												//If the pending bit is enabled for the DHT Pin
			if((DHT_Port->IDR & (1<<DHT_Pin))){								//If input pin reads 1
				timerSetup();	
				TIM1->CR1 |= 1;																	//Enable the timer
			}
			else{
				pulseWidth = TIM1->CNT;													//Store the Timer count as pulse width
				if(TIM1->CNT > MAX_PULSE_WIDTH || TIM1->CNT < MIN_PULSE_WIDTH){					//If pulse width exceeds 100 or preceeds 20
					TIM1->CR1 &= ~TIM_CR1_CEN;										//Disable the timer and 																
					xTaskNotifyFromISR(dhtHandle, 2, eSetValueWithOverwrite, 0); //Notify the dhtTask about this error
				}
				else{
					if(xQueueSendFromISR(pWQueue, &pulseWidth, &xHPTW1) != pdTRUE){
						portYIELD_FROM_ISR(xHPTW1);
					}
				}
			}
			EXTI->PR |= (1<<DHT_Pin);													//Clear the pending bit
			TIM1->CNT = 0;																		//Count is reinitialized
		}
}

void initializeDHT(){
	
	DHT_Out_Setup();														//Set the DHT Pin in output mode
	DHT_Port->ODR &= ~(1 << DHT_Pin);						//SET pin in low state for 18 msec
	vTaskDelay(18/portTICK_PERIOD_MS);					
	DHT_Port->ODR |= 1 << DHT_Pin;							//Set pin in high state for 30 usec
	DHT11_Delay();
	DHT_In_Setup();															//SET DHT Pin in input mode
	EXTI_Setup_DHT();
}

void dhtTask (void* param __attribute__((unused))){
	
	uint32_t notifVal;													//Variable to receive the value of notification
	int retry_count = 0;												//Counter for timeout retries
	pWQueue = xQueueCreate(45, sizeof(uint32_t));
	
	initializeDHT();														//Call the initialization function once
	
	while(dataReady != 1){											
		if(xTaskNotifyWait(0, 0, &notifVal, pdMS_TO_TICKS(5))){
			if(notifVal == 1){											//Check if notification value is 1
				if(retry_count != 5){									//Retry for 5 counts
					printf("\r\nTimeout Occured, Retrying attempt %d of 5", retry_count+1);
					retry_count++;
					bitIndex = 0;
					byteIndex = 0;
					memset(data, 0, sizeof(data));			//Reset the data
					initializeDHT();											//Reinitialization 
				}
				else{																	//Max Retries limit reached
					printf("\r\nMax Retry Limit Reached");
					retry_count=0;
					//send the average of last 3 temperature and humidity values to display task
					break;															//Break out of loop
				}
			}
			else if(notifVal == 2){									//If a pulse width timeout has occured
					bitIndex = 0;
					data[byteIndex]=0;
			}
		}
		else if (dataReady == 1){								//If the data is ready, place it onto the queue and break out of the loop
			//send data to display task
			dataReady = 0;
			break;
		}
		if(xQueueReceive(pWQueue, &pulseWidth, pdMS_TO_TICKS(5))){		//Queue contains some values 
			if(pulseWidth > 70){											//If pulse width is >70 usec, update the byte value	
				data[byteIndex] |= (1<<(7-bitIndex));
			}	
			if(bitIndex == 8){											//If bitIndex is 8, move to the next byte
				bitIndex = 0;
				byteIndex++;
			}
			if(byteIndex == 5){											//If byteIndex is equal to 5, verify the checksum		
				if(data[4] == data[0]+data[1]+data[2]+data[3]){
					dataReady = 1;
					retry_count = 0;
				}
			}
			bitIndex++;
			pulseWidth = 0;
		}
		else{																													//Queue is empty
			//send the average of last three values of temperature and humidity to display task
		}
	}
}