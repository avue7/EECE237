; Athit Vue, Student_ID#003978235
; EECE_237 Section 1
; Lab #1
; DESCRIPTION: This program turns on the LEDs in three patterns 
; on STM32 F3 board. At startup, all the LEDs should light up. When 
; the blue button is pressed, the pattern changes so that only the 
; odd numbered LEDs are on. Then after one second, the pattern 
; changes again so that only the even numbered LEDs are on. After
; a second, the pattern changes back to all the LEDs being turned on
; again. This cycle shall continue in an infinite loop.
 
	AREA   EECE237_S16_Lab1, CODE, READONLY
	ENTRY
	EXPORT   __main
__main  B  start
;Setting up Alias for RCC
RCC_BASE 			EQU   	0x40021000	
AHBENR_OFFSET		EQU  	0x14
AHBENR_GPIOAE 		EQU  	0x0220000
;Setting up Alias for GPIOA and GPIOE
GPIOA_BASE			EQU		0x48000000
GPIOE_BASE			EQU		0X48001000 
MODER_OFFSET 		EQU		0x00			
IDR_OFFSET			EQU		0x10	
ODR_OFFSET			EQU		0x14
;            
start
	BL	config_rcc		;Configure RCC for GPIOA and GPIOE.
	BL	config_gpioa	;Configure push-button port.
	BL  config_gpioe	;Configure LED port.
	BL 	LED_on_all 		;Configure all the LEDs to turn on
						; at startup. 
main_loop
	BL 	 read_button	;R3 = 1 if the button is pressed.
	CMP	 R3, #0			
	BLEQ LED_on_all		;If R3 = 0, button is not pressed, so all LED 
							;stays on. Save return address in LR. 
	BNE infinite_loop	;If R3 != 0, branch to infinite_loop
	B	main_loop		;Infinitely loop and wait until button
							;button is pressed.
infinite_loop			
	;At this point, blue button was pressed and R3 remains equals to 1.	
						;Hex(0x00a0 0000)for r5-r7
	mov r5, #8781824	;Move some number to r5 for delay.
	mov r6, #8388608	;Move some number to r6 for delay.
	mov r7, #8650752	;Move some number to r7 for delay.
	
	BL 	LED_on_odd		;First, branch to LED_on_odd and save
							;the return address in LR (Link Register)
	BL 	delay_1			;Second, branch to delay_1 and save the
							;return address in LR (Link Register)
	BL 	LED_on_even		;Third, branch to LED_on_even and save
							;the return address in LR.
	BL 	delay_2			;Delay for about 1 second.
	BL	LED_on_all		;Turn on all LEDs again.
	BL	delay_3			;Delay for another second.
	B	infinite_loop	;Branch to infinite_loop. Stay in infinite loop.
						;This cycle will run infinitely.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;   
config_rcc				;Enable RCC GPIO Clock.
	LDR   R0, =RCC_BASE 	 ;Load base address for RCC
	LDR   R1, =AHBENR_OFFSET ;Load offset for AHBENR
	LDR   R2, =AHBENR_GPIOAE ;Load value for GPIO PortE enable
	LDR   R3, [R0, R1] 		 ;Read current AHBENR value
	ORR   R2, R2, R3  		 ;Modify AHBENR value to enable E
	STR   R2, [R0, R1] 		 ;Write new AHBENR value to register
	BX	  LR
;
config_gpioa			;Configure GPIOA[0] as an input pin. Must
							;configure this for button to work.
	LDR	  R0, =GPIOA_BASE		  ;Load GPIOA base address to R0.
    LDR   R1, [R0, #MODER_OFFSET] ;Make PA[0] input pin.
    BIC   R1, R1, #0X03 		  ;reset only [1:0] bits.
    STR   R1, [R0, #MODER_OFFSET] ;All pins are input.
    BX	  LR					  ;Branch and exchange. Branch to an
									;address value stored in LR and set
									;the execution state of the processor
									;based on bit 0 of LR.
;
config_gpioe					;Configure GPIO_E port (LED port)
	LDR	  R0, =GPIOE_BASE		
	LDR	  R1, =0x55550000	  	  ;Assign GPIOE output port
								    ;making all upper 16-bit pins output.
	STR	  R1, [R0, #MODER_OFFSET] ;STORE MODER=0X10.
	BX    LR
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;           
read_button
	mov	  r3, #0				;Making sure r3 starts at 0.
	ldr   r0, =GPIOA_BASE		;Load GPIOA base.
    ldr   r3, [r0, #IDR_OFFSET] ;Read the IDR.
    and   r3, r3, #0001		  	;Cmp r1, #0x00
    bx	  lr					;Branch and exchange. Branch back to the address
									;store in LR.
;
LED_on_all
	ldr	  r0, =GPIOE_BASE		;Load GPIOE base.
	ldr   r1, =0x0000FF00		;Set all pins to light up.
	str   r1, [r0, #ODR_OFFSET]
	bx	  lr					;Branch and exchange. Branch back to the address
									;stored in LR.
;
LED_on_odd	;Using LDx to reference to where LED should lit up since it's 
			;clearly marked on the board.
			;Reference: page 34, figure 14 of STM32F3 User_Manual
								; pins: PE 15 = LD6(green)
								;		PE 13 = LD10(red)
								;		PE 11 = LD7(green)
								;		PE 09 = LD3(red)
	ldr	  r0, =GPIOE_BASE
	ldr   r1, =0x0000aa00   ;Set top modeR15,modeR13,modeR11,modeR9 to 
								;1 (true) for output.
	str   r1, [r0, #ODR_OFFSET]
	bx	  lr				;Branch and exchange. Branch back to the address stored in LR.
;
LED_on_even	;On the board (reference: page 34, figure 14 of STM32F3 User_Manual)
								; pins: PE 08 = LD4(blue)
								;		PE 10 = LD5(orange)
								;		PE 12 = LD9(blue)
								;		PE 14 = LD8(orange)
	ldr	  r0, =GPIOE_BASE
	ldr   r1, =0x00005500 	;Read from right->left, first two byte are set to input
								  ;top modeR08,modeR10,modeR12,and moder14 of the output
								    ;pins are set to 1 for output.
	str   r1, [r0, #ODR_OFFSET]
	bx	  lr				;Branch and exchange. Branch back to the address stored in LR.
;
delay_1				;Delay_1 loop to delay for some time. Goal = 1 second.
	sub   r5, r5, #1	;Decrement the value in R5.
	cmp   r5, #0		;Compare value in R5 to 0.
	bne	  delay_1		;If R5 does not equal 0 then branch to loop again until R5=0.
	bx    lr			;Branch and exchange. Branch back to the address stored in LR.
;
delay_2				;Delay_2 loop to delay for some time. Goal = 1 second.
	sub   r6, r6, #1	;Decrement the value in R6 by 1.
	cmp   r6, #0		;Compare value in R6 to 0.
	bne	  delay_2		;If R6 does not equal 0 then branch to loop again until R6=0.
	bx    lr			;Branch back to the address stored in LR.
;
delay_3				;Delay_3 loop to delay for some time. Goal = 1 second.
	sub  r7, r7, #1		;Decrement the value in R7 by 1.
	cmp   r7, #0		;Compare value in R7 to 0.
	bne	  delay_3		;If R7 does not equal 0 then branch to loop again until R7=0.
	bx    lr			;Branch back to the address stored in LR.
