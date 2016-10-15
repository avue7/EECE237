/*
*      EECE 237 
*      LCD drive in C
* This is a test program to write a short message
* on an LCD panel with minimum library functions
*
*/

#include "stm32f30x_gpio.h"
#include  "stm32f30x_i2c.h"
#include "main.h"
#include  "stdio.h"
#include  "string.h"
//#include  "LCD.h"
GPIO_InitTypeDef        GPIO_InitStructure;

//void    I2C_init(void);
void    I2C2_init(void);
void	LCD_init(void);
void  LCD_write(int,int, char);
void  LCD_clear(void);
void  LCD_contrast(int);                // Contrast level = 1..50
void  LCD_backlight(int);               // Backlight level = 1..8

void 	pause(void);
char   message[16];
char   LCD_msg[] = "Wachabanga!";

int main(void) {
    int   i, j = 0;
   
    I2C2_init();  
 //   while(I2C_GetFlagStatus(I2C2, I2C_ISR_BUSY) != RESET);
		LCD_init();
		
    for (i=0; i < strlen(LCD_msg); i++)
        LCD_write(0, i, LCD_msg[i]);                 

}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~       

void I2C2_init(void)
{
		GPIO_InitTypeDef GPIO_InitStructure;
		I2C_InitTypeDef  I2C_InitStructure;
/*  initialize RCC block for I2C2 and GPIOA */
		RCC_I2CCLKConfig(RCC_I2C2CLK_SYSCLK);  
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C2, ENABLE);  
		RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
	/* initialize GPIOA[10:9] for I2C alternate function  */
		GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_4);
		GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_4);
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
		GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
		GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
		GPIO_Init(GPIOA, &GPIO_InitStructure);
		GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_10;
//
		GPIO_Init(GPIOA, &GPIO_InitStructure);
//
	/* initializ I2C2   */
		I2C_DeInit(I2C2);
		I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;  
		I2C_InitStructure.I2C_AnalogFilter = I2C_AnalogFilter_Enable;
		I2C_InitStructure.I2C_DigitalFilter = 0x00;
		I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
		I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
		I2C_InitStructure.I2C_Timing = 0xC062121F; 
//
		I2C_Init(I2C2, &I2C_InitStructure);  
		I2C_Cmd(I2C2, ENABLE);
}

void LCD_init()
{
	LCD_contrast(50);
	LCD_backlight(8);
	LCD_clear();
}

void  LCD_write(int row, int col, char data) {
          
		// Move to sepcified row, col
		I2C_TransferHandling(I2C2, 0x50 , 3, I2C_SoftEnd_Mode, I2C_Generate_Start_Write);
		while(I2C_GetFlagStatus(I2C2, I2C_ISR_TXIS) == RESET);
		I2C_SendData(I2C2, 0xFE);
		while(I2C_GetFlagStatus(I2C2, I2C_ISR_TXIS) == RESET);
		I2C_SendData(I2C2, 0x45);
		while(I2C_GetFlagStatus(I2C2, I2C_ISR_TXIS) == RESET);        
		if (!row)               // if row == 0
				I2C_SendData(I2C2, col);
		 else                  // else row asumed to be 1
				I2C_SendData(I2C2, (0x40 + col));       
		I2C_TransferHandling(I2C2, 0x50 , 1, I2C_SoftEnd_Mode, I2C_Generate_Start_Write);
		while(I2C_GetFlagStatus(I2C2, I2C_ISR_TXIS) == RESET);                        
		I2C_SendData(I2C2, data);
}         
//
//      Set LCD Contrast - Level should be 1..50 (Seems to work best if > 35)
//
void  LCD_contrast(int level) {
     
		I2C_TransferHandling(I2C2, 0x50 , 3, I2C_SoftEnd_Mode, I2C_Generate_Start_Write);
		while(I2C_GetFlagStatus(I2C2, I2C_ISR_TXIS) == RESET);
		I2C_SendData(I2C2, 0xFE);
		while(I2C_GetFlagStatus(I2C2, I2C_ISR_TXIS) == RESET);
		I2C_SendData(I2C2, 0x52);
		while(I2C_GetFlagStatus(I2C2, I2C_ISR_TXIS) == RESET);
		I2C_SendData(I2C2, level); 
		pause();
}         
//
//      Set LCD Backlight - Level should be 1..8 (Seems to work best if > 1)
//
void  LCD_backlight(int level) {
		I2C_TransferHandling(I2C2, 0x50 , 3, I2C_SoftEnd_Mode, I2C_Generate_Start_Write);
		while(I2C_GetFlagStatus(I2C2, I2C_ISR_TXIS) == RESET);
		I2C_SendData(I2C2, 0xFE);
		while(I2C_GetFlagStatus(I2C2, I2C_ISR_TXIS) == RESET);
		I2C_SendData(I2C2, 0x53);
		while(I2C_GetFlagStatus(I2C2, I2C_ISR_TXIS) == RESET);
		I2C_SendData(I2C2, level);
		pause();
}         

void  LCD_clear() {
		I2C_TransferHandling(I2C2, 0x50 , 2, I2C_SoftEnd_Mode, I2C_Generate_Start_Write);
		while(I2C_GetFlagStatus(I2C2, I2C_ISR_TXIS) == RESET);
		I2C_SendData(I2C2, 0xFE);
		while(I2C_GetFlagStatus(I2C2, I2C_ISR_TXIS) == RESET);
		I2C_SendData(I2C2, 0x51);
		pause();
}         

void pause (){
	uint32_t i, time;
	time=0x008000;
	for (i=0; i<time; i++);
}
