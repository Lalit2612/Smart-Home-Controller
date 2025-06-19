#include "stdio.h"
#include "string.h"
#include "stm32f4xx.h"
#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "task.h"
#include "queue.h"

#define I2C_Port GPIOB
#define SCL_Pin  8
#define SDA_Pin  9
#define LCD_ADDR 0x27

void SCL_SDA_Setup(void);
void I2C_Setup(void);
void send_START_Addr(void);

