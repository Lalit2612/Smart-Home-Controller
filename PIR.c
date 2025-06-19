#include "PIR.h"

BaseType_t xHPTW2 = pdFALSE;

void PIR_In_Setup(){
	
	RCC->AHB1ENR |= 0x4;
	
	PIR_Port->MODER &= ~(0x3 << (PIR_Pin * 2));
	
	PIR_Port->OTYPER &= ~(1 << PIR_Pin);
	
	PIR_Port->PUPDR &= ~(0x3 << (PIR_Pin * 2));
	PIR_Port->PUPDR |= (0x2 << (PIR_Pin * 2));
	
}

void EXTI_Setup_PIR(){
	EXTI->RTSR |= (1<<PIR_Pin);
	EXTI->FTSR |= (1<<PIR_Pin);
	EXTI->IMR  |= (1<<PIR_Pin);
	NVIC_EnableIRQ(EXTI4_IRQn);
}

void EXTI4_IRQHandler(){
	
	if(EXTI->PR & (1<<PIR_Pin)){
		if(PIR_Port->IDR & (1<<PIR_Pin)){
				xTaskNotifyFromISR(pirHandle,1, eSetValueWithOverwrite, &xHPTW2);
		}
		else{
				xTaskNotifyFromISR(pirHandle,2, eSetValueWithOverwrite, &xHPTW2);
		}
	}
	EXTI->PR |= (1<<PIR_Pin);
	portYIELD_FROM_ISR(xHPTW2);
}
void PIRTask(void* param __attribute__((unused))){
	PIR_In_Setup();
	EXTI_Setup_PIR();
	uint32_t pirNotif;
	
	while(1){
		if(xTaskNotifyWait(0, 0, &pirNotif, pdMS_TO_TICKS(5))){
			if(pirNotif == 1){
				//turn the servo by 90 deg, to open the way
			}
			else if (pirNotif == 2){
				//turn the servo by -90 deg, to shut the way
			}
		}
	}
}
