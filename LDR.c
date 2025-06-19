#include "LDR.h"

void ldrSetup(){
	RCC->APB2ENR |= (1 << 8); 											//ENABLE the ADC clock
	RCC->AHB1ENR |= 2; 															//ENABLE the GPIOB (LDR Port)
	LDR_Port->MODER &= ~(0x3 << (2 * LDR_Pin)); 		//Setting the LDR Pin in analog function mode 
	LDR_Port->MODER |= (0x3 << (2 * LDR_Pin));	
	
	ADC1->CR2 &= ~1;																	//DISABLE the ADC during init
	ADC1->CR2 |= (1<<1);															//Setting the CONT bit, the single channel will be converted continuously
	
	ADC1->SQR1 &= ~(0xF << 20);											//CLEAR the length field to indicate only 1 conversion
	ADC1->SQR3 &= ~(0x1F);
	ADC1->SQR3 |= 0x8;															//Select channel 8 for conversion
	
	ADC1->SMPR2 &= ~(0x7 << 24);										//56 cycles set in Sampling Time Register 2 for channel 8
	ADC1->SMPR2 |= (0x3 <<24);		

	ADC1->CR1 &= ~(0x1F);
	ADC1->CR1 |= 0x8;																//Select channel 8 to enable analog watchdog
	ADC1->CR1 |= (1 << 9);													//Enable analog watchdog for only channel 8
	ADC1->CR1 |= (1 << 23);													//ENABLE analog Watchdog globally
	ADC1->CR1 |= (1 << 6);													//Enable the Analog Watchdog Interrupt
	NVIC_EnableIRQ(ADC_IRQn);												//ENABLE the ADC interrupt in the NVIC
	
	ADC1->CR2 |= 1;																	//ENABLE the ADC
}

void ADC_IRQHandler(){
	if(ADC1->SR & ADC_SR_AWD){															//Check if the analog watchdog flag is set in ADC status register												
		ADC1->SR &= ~ADC_SR_AWD;															//Clear the flag 
		vTaskNotifyGiveFromISR(ldrHandle, 0);									//and send task notification to the ldrTask
	}
}
	
void ldrTask (void* param __attribute__((unused))){
	ldrSetup();																							//call the setup function once for initialization
	while(1){
		ulTaskNotifyTake(pdTRUE, portMAX_DELAY);							//Wait for a notification from the ADC ISR
		uint32_t adcVal = ADC1->DR;									
		if(adcVal > ADC1->HTR){																//Check if ADC Data register value is greater than higher threshold
			//Switch OFF the relay
		}
		else if(adcVal < ADC1->LTR){													//Check if the ADC Data register value is lesser than lower threshold
			//Switch ON the relay
		}		
	}
}