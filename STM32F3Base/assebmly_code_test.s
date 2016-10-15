;EECE237 Spring 2016	Homework 1
;due on 2/28/2016
;The task is to keep the smallest number in the data array
;
;
 	area  hw1, code, readonly
	entry
	export  __main 
__main	b 	start
;
start
	mov r0,#24
	bl delay
	mov r1,#2
delay
	sub r0,r0, #2
	cmp r0,#0
	bne delay
	bx lr