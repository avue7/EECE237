//EECE 237 
//This is the minimum codes in C to drive LED's.
//
//

#include "stm32f30x_gpio.h"
#include "main.h"
GPIO_InitTypeDef   GPIO_InitStructure;
void    IO_Init(void); //function prototype
void light_all_LED(void);
void pause_1second(void);
void turn_off_LED(void);


int main(void) 
{
 
	IO_Init();
		//int i;
		//for (i=0; i<=0x01000000;i++)
		//{
	while (!((GPIOA->IDR)&0x001));
	do{
			light_all_LED();
			pause_1second();
			turn_off_LED();
			pause_1second();
	}
	while(1);
	
}

void turn_off_LED()
{
		GPIOE->ODR = 0x0000;
		GPIO_Write(GPIOE, 0x0000);
}

void pause_1second()
{
	uint32_t i, time;
	time = 5500000 ;
	for (i=0; i<time; i++);
}

void light_all_LED()
{	
	GPIOE->ODR = 0xff00;
	GPIO_Write(GPIOE, 0xff00);
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~       
       
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




