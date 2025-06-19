#include "LCD.h"

void SCL_SDA_Setup(){
	
	RCC->APB1ENR |= (1<<21);									//Enable the peripheral clock to I2C1
	
	I2C_Port->MODER &= ~(0x3<<(2*SCL_Pin));		//Alternate function mode for SCL
	I2C_Port->MODER |= (0x2<<(2*SCL_Pin));
	I2C_Port->OTYPER |= (1<<SCL_Pin);					//Open Drain Config for SCL Pin
	I2C_Port->PUPDR &= ~(0x3<<(2 * SCL_Pin));	//Pull Up Resistor config for SCL Pin
	I2C_Port->PUPDR |= (0x1<<(2 * SCL_Pin));
	
	I2C_Port->MODER &= ~(0x3<<(2*SDA_Pin));		//Alternate function mode for SDA
	I2C_Port->MODER |= (0x2<<(2*SDA_Pin));
	I2C_Port->OTYPER |= (1<<SDA_Pin);					//Open Drain Config for SDA Pin
	I2C_Port->PUPDR &= ~(0x3<<(2 * SDA_Pin));	//Pull Up Resistor config for SDA Pin
	I2C_Port->PUPDR |= (0x1<<(2 * SDA_Pin));

}

void I2C_Setup(){
	
	I2C1->CR2 &= ~(0x3F);										//The clock freq of I2C interface must be equal to APB1 frequency
	I2C1->CR2 |= 0x2A;
	
	I2C1->CCR &= ~0xFFFF;										//Configuring the SCL Clock frequency 
	I2C1->CCR |= 0x8023;					
	
	I2C1->TRISE |= 0x002B;									//TRISE = FREQ+1
	
	I2C1->CR1 |= 1;													//ENABLE the I2C Peripheral
}

void send_START_Addr(){
	
	I2C1->CR1 |= (1<<8);										//send the start condition
	
	while(!(I2C1->SR1 & 1));								//wait till start bit is detected by peripheral
	
	I2C1->DR = LCD_ADDR << 1;								//The LSB indicates a write from master to slave
	
	while(!(I2C1->SR1 & (1<<1)));						//Wait till addr is sent 
	
	volatile uint32_t temp;									//clear the address flag by reading sr1 followed by reading sr2
	temp = I2C1->SR1;
	temp = I2C1->SR2;
}

void LCDTask(){
	
	
}
