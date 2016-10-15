//
//      EECE 237 F15
//      C LCD Test File
//
//includes for the project
#include "stm32F30x.h"
#include "STM32f3_discovery.h"
#include "stm32f30x_gpio.h"
#include  "stm32f30x_i2c.h"
#include "main.h"
#include  "stdio.h"
#include  "string.h"

RCC_ClocksTypeDef 			RCC_Clocks;
GPIO_InitTypeDef        GPIO_InitStructure;

void    IO_Init(void);
void    I2C_init(void);
void    I2C2_init(void);

void  LCD_write(int,int, char);
void  LCD_clear(void);
void  LCD_contrast(int);                // Contrast level = 1..50
void  LCD_backlight(int);               // Backlight level = 1..8
void Delay(uint32_t nTime);

unsigned  int   tick_count = 0;
char strDisp[20] ;  
volatile int     ButtonPressed = 0;
static __IO uint32_t TimingDelay;

char        message[16];
char        LCD_msg[] = "Hello, World! ";

int main(void) {

    int   i, j = 0;
    
    IO_Init();
           
    I2C2_init();  
    while(I2C_GetFlagStatus(I2C2, I2C_ISR_BUSY) != RESET);
	
		LCD_contrast(50);
		LCD_backlight(8);
		
		LCD_clear();
		
    for (i=0; i < strlen(LCD_msg); i++)
        LCD_write(0, i, LCD_msg[i]);                 
   
    while (!ButtonPressed);
    ButtonPressed = 0;
    
		LCD_clear();
		
    GPIOE->ODR = 0;

    while(1){
     
          sprintf(message, "Ticks = %5i", tick_count);

          for (i=0; i < strlen(message); i++)
              LCD_write(0, i, message[i]);				// Display number on 1st line of display
                    
          while (!ButtonPressed);
          ButtonPressed = 0;
          
          GPIOE->ODR &=  0x8000 >> j++;
					j %= 8;
          
          LCD_clear();

    }

}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~       
       
void IO_Init() {
  
		EXTI_InitTypeDef   EXTI_InitStructure;
		GPIO_InitTypeDef   GPIO_InitStructure;
		NVIC_InitTypeDef   NVIC_InitStructure;
			 
			
		/* SysTick end of count event each 1ms */
		RCC_GetClocksFreq(&RCC_Clocks);
		SysTick_Config(RCC_Clocks.HCLK_Frequency / 1000);


		/* GPIOE Periph clock enable */
		RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOE, ENABLE);

		/* Configure PE14 and PE15 in output pushpull mode */
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15 | GPIO_Pin_14 | GPIO_Pin_13 | GPIO_Pin_12
			| GPIO_Pin_11| GPIO_Pin_10| GPIO_Pin_9| GPIO_Pin_8;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;              
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;             //GPIO_PuPd_NOPULL
		GPIO_Init(GPIOE, &GPIO_InitStructure);

		/* Enable GPIOA clock */
		RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);

		/* Configure PA0 pin as input floating */
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
		GPIO_Init(GPIOA, &GPIO_InitStructure);

		/* Enable SYSCFG clock */
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

		/* Connect EXTI0 Line to PA0 pin */
		SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource0);

		/* Configure EXTI0 line */
		EXTI_InitStructure.EXTI_Line = EXTI_Line0;
		EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
		EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
		EXTI_InitStructure.EXTI_LineCmd = ENABLE;
		EXTI_Init(&EXTI_InitStructure);

		/* Enable and set EXTI0 Interrupt to the lowest priority */
		NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0F;
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0F;
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
		NVIC_Init(&NVIC_InitStructure);

}



void I2C2_init(void)
{
		GPIO_InitTypeDef GPIO_InitStructure;
		I2C_InitTypeDef  I2C_InitStructure;

		RCC_I2CCLKConfig(RCC_I2C2CLK_SYSCLK);  

		RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C2, ENABLE);  

		RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
		GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_4);
		GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_4);
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
		GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
		GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
		GPIO_Init(GPIOA, &GPIO_InitStructure);

		GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_10;
		GPIO_Init(GPIOA, &GPIO_InitStructure);

		I2C_DeInit(I2C2);
		I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;  
		I2C_InitStructure.I2C_AnalogFilter = I2C_AnalogFilter_Enable;
		I2C_InitStructure.I2C_DigitalFilter = 0x00;
		I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
		I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
		I2C_InitStructure.I2C_Timing = 0xC062121F; 

		I2C_Init(I2C2, &I2C_InitStructure);  
		I2C_Cmd(I2C2, ENABLE);
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

		Delay(20);
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
			
		Delay(20);
}         


void  LCD_clear() {
       
		I2C_TransferHandling(I2C2, 0x50 , 2, I2C_SoftEnd_Mode, I2C_Generate_Start_Write);

		while(I2C_GetFlagStatus(I2C2, I2C_ISR_TXIS) == RESET);

		I2C_SendData(I2C2, 0xFE);
					 
		while(I2C_GetFlagStatus(I2C2, I2C_ISR_TXIS) == RESET);

		I2C_SendData(I2C2, 0x51);

		Delay(20);
}         

/**
  * @brief  Inserts a delay time.
  * @param  nTime: specifies the delay time length, in milliseconds.
  * @retval None
  */
void Delay(uint32_t nTime)
{ 
  TimingDelay = nTime;

  while(TimingDelay != 0);
}

/**
  * @brief  Decrements the TimingDelay variable.
  * @param  None
  * @retval None
  */
void TimingDelay_Decrement(void)
{
  if (TimingDelay != 0x00)
  { 
    TimingDelay--;
  }
  
}


void SysTick_Handler(void)
{
     TimingDelay_Decrement();
     
     tick_count++;
}


void EXTI0_IRQHandler(void)
{

	if ((EXTI_GetITStatus(USER_BUTTON_EXTI_LINE) == SET)&&(STM_EVAL_PBGetState(BUTTON_USER) != RESET))
  {
		
		GPIOE->ODR ^= 0xFF00;
     
    ButtonPressed = 1;
		
    /* Clear the EXTI line 0 pending bit */
    EXTI_ClearITPendingBit(USER_BUTTON_EXTI_LINE);
  }    

}

