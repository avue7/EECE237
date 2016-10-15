; Athit Vue	
; EECE 237
; section 01
; Homework #1
; Description: This program searches data stored in the mememory from 
; 0x20001000 to 0x2000101C and stores the smallest of the 8 data sets to 
; memory location 0x20002000.
 
	area  Homework_1, code, readonly
	entry
	export  __main 
__main

start
baseaddr 		equ 0x20001000		;using post-offset indirect addressing to
									; fetch data in memory
index_increment equ	4				;for each time decrement by 4
str_lowest_addr equ 0x20002000		;store the smallest number to this address
		
		mov r0,#0					;making sure r0 starts at 0
		mov r1,#0
		mov r4,#0
		mov r1,#8				    ;we have 8 data, use r1 as a counter
		ldr r2,=baseaddr			;load r2 with starting address	
		ldr r0,[r2],#index_increment;fecth the data, load it to r0
									; and get next data
		sub r1,r1,#1				;decrement the counter by one
loop									
		ldr r4,[r2],#index_increment;load the next data into r4 and 
									;go to next data
		cmp r0,r4					;compare r0 and r4
		blt move_on					;if r0 is less then r4, move on
		mov r0,r4					;if not then store r4 data in r0
		
move_on
		sub r1,r1, #1				;decrement the counter by one(condition for bne)
		bne loop					;repeat process until counter is 0:
									; condition is if r1 != 0;
		ldr r10,=str_lowest_addr	;assign r10 to address 0x20002000
		str r0,[r10]				;store the smallest to the address r10 is
		b stop
stop
		END