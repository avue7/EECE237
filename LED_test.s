; Assembly language to demonstrate LED and push_button
; operation on STM32F3 Discovery Kit
; All 8 LED's are turned on when the blue button is pushed.
; EECE 237 Spring 2016
 
	AREA   LED_TEST, CODE, READONLY
	ENTRY
	EXPORT   __main
__main  B  start
;         
RCC_BASE 			EQU   	0x40021000	
AHBENR_OFFSET		EQU  	0x14
AHBENR_GPIOAE 		EQU  	0x0220000
;
GPIOA_BASE			EQU		0x48000000
GPIOE_BASE			EQU		0X48001000 
MODER_OFFSET 		EQU		0x00			
IDR_OFFSET			EQU		0x10	
ODR_OFFSET			EQU		0x14
;            
start
	BL	config_rcc		;configure RCC for GPIOA and GPIOE
	BL	config_gpioa	;Configure push-button port.
	BL  config_gpioe	;Configure LED port
main_loop
	BL 	read_button		;R3=1 if the button is pressed.
	CMP	R3, #0		
	BLEQ	LED_off
	BLNE	LED_on
	B	main_loop		;Stay in the infinite loop
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;   
config_rcc				; Enable RCC GPIO Clock
	LDR   R0, =RCC_BASE ; load base address for RCC
	LDR   R1, =AHBENR_OFFSET ; load offset for AHBENR
	LDR   R2, =AHBENR_GPIOAE ; load value for GPIO PortE enable
	LDR   R3, [R0, R1] 		; Read current AHBENR value
	ORR   R2, R2, R3  	; Modify AHBENR value to enable E
	STR   R2, [R0, R1] 	; Write new AHBENR value to register
	BX	LR
;
config_gpioa	;Configure GPIOA[0] as an input pin.
	LDR    R0, =GPIOA_BASE ;
    LDR    R1, [R0, #MODER_OFFSET] ;Make PA[0] input pin
    BIC    R1, R1, #0X03 ;reset only [1:0] bits
    STR    R1, [R0, #MODER_OFFSET] ;All pins are input 
    BX     LR
;
config_gpioe	; Configure GPIO_E port (LED port)
	LDR		R0, =GPIOE_BASE		;
	LDR		R1, =0x55550000		;Assign GPIOE output port.
	STR		R1, [R0, #MODER_OFFSET]	;STORE MODER=0X10
	BX  	LR
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;           
read_button
     ldr  	r0, =GPIOA_BASE
     ldr  	r3, [r0, #IDR_OFFSET] ;read idr
     and  	r3, r3, #0001	; cmp r1, #0x00
     bx		lr
;
LED_on
	ldr		r0, =GPIOE_BASE
	ldr  	r1, =0x0000aa00
	str  	r1, [r0, #ODR_OFFSET]
	bx		lr
;
LED_off
	ldr		r0, =GPIOE_BASE
	ldr  	r1, =0x0000
	str  	r1, [r0, #ODR_OFFSET]
	bx		lr
	END 

