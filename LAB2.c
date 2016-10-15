/**
 * Athit Vue, Section 01
 * Lab #2
 * @brief This is a program that lights up the LEDs and display
 * the message "Welcome to Chico!" in the LCD panel. This program
 * is a modified version of the the following two files combined: 
 * LED_in_C.c and LCD_in_C.c. First it checks to see if the user
 * blue button is pushed. If not, then it stays in that infinite 
 * loop until, the button is pushed. Once the button is pushed, 
 * It lights up all the LEDs, clears the message on the screen, 
 * and stays lit for one second. Then the LEDs turn off and a 
 * message "Welcome to Chico!" is displayed for another second.
 * It then repeats itself with lighting up all the LEDs and etc. 
 * in an infinite loop. 
 */

#include "stm32f30x_gpio.h"
#include  "stm32f30x_i2c.h"
#include "main.h"
#include  "stdio.h"
#include  "string.h"
//#include  "LCD.h"

/*** Function prototypes for GPIO ***/
GPIO_InitTypeDef   GPIO_InitStructure;
void IO_Init(void);
void light_all_LED(void);
void pause_1second(void);
void turn_off_LED(void);

/*** Function prototypes for LCD ***/
void  I2C2_init(void);
void	LCD_init(void);
void  LCD_write(int,int, char);
void  LCD_clear(void);
void  LCD_contrast(int);                // Contrast level = 1..50
void  LCD_backlight(int);               // Backlight level = 1..8
void 	display_on_screen(void);
void 	pause(void);
char  message[17];
char  LCD_msg[] = "Welcome to Chico!";

int main(void) 
{
	IO_Init(); 		// Initializing GPIOE and GPIOA peripheral blocks.
	I2C2_init(); 	// Initializing I2C2 peripheral block.
	LCD_init();		// Setting the contrast and brightness and clearing
								// the LCD. Kept it for future reference.
	
	while (!((GPIOA->IDR)&0x001)); // Waiting until the button is pushed.
	
	/* Decided to use do-while loop for infinite loop of needed pattern. */
	do{
			light_all_LED();			// Light-up all the LEDs.
			LCD_clear();					// Clear the LCD.
			pause_1second();			// Pause for 1 second.
			turn_off_LED();				// Turn off the LEDs.
			display_on_screen();	// Display the message on the LCD.
			pause_1second();			// Pause for 1 second.
	}
	while(1);		// This is an infinite do-while loop since the condition will always be true. 
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~       

void turn_off_LED()
{
		GPIOE->ODR = 0x0000;				// Set the ODR bit pattern. 
		GPIO_Write(GPIOE, 0x0000);	// Write bit pattern to GPIOE
}

void pause_1second()
{
	uint32_t i, time;
	time = 4900000 ;						// Best time that I've found that came close to 1 second.
	for (i=0; i<time; i++);			// For loop to do nothing and waste time incrementing each call.
}

void light_all_LED()
{	
	GPIOE->ODR = 0xff00;				// Set the bit pattern for 15:8 pins
	GPIO_Write(GPIOE, 0xff00);
}

void display_on_screen()
{
  int i;
	for (i=0; i < strlen(LCD_msg); i++) // For each letter, display it on the screen
  {
		LCD_write(0, i, LCD_msg[i]);  
	}
}

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
   
void IO_Init() {
 		/* GPIOE Periph clock enable */
		RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOE, ENABLE);
		
	/* Initialize GPIOE. Select pin[15:8] for LED's*/
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15 | GPIO_Pin_14 | GPIO_Pin_13 | GPIO_Pin_12
			| GPIO_Pin_11| GPIO_Pin_10| GPIO_Pin_9| GPIO_Pin_8;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;              
//		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
//		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;   //GPIO_PuPd_NOPULL
		GPIO_Init(GPIOE, &GPIO_InitStructure);
//
/* Initialize GPIOA	*/
		RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0; 
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;              
		GPIO_Init(GPIOA, &GPIO_InitStructure);
}

void pause (){
	uint32_t i, time;
	time=0x008000;
	for (i=0; i<time; i++);
}
