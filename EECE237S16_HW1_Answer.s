;EECE237 Spring 2016	Homework 1
;due on 2/28/2016
;The task is to keep the smallest number in the data array
;
;
 	area  hw1, code, readonly
	entry
	export  __main 
__main	b 	start
baseaddr	equ		0x20001000
index_increment	equ	4
start	
	mov r0, #0	;clear r0
	mov r1,	#8	;initialize the internation counter to 8.
	ldr	r2, =baseaddr
loop
	ldr	r4, [r2], #index_increment	;fetch the data and increment r2 address
	cmp	r0, r4		;compare r0 and r4
	bls	move_on	;if r0 is less than r4, move on. If not, replace  
	mov	r0, r4
move_on 
	subs	r1, r1, #1		;decrement the counter by one. Set PSR
	bne		loop
;	
	end 

********************************************************
Grading guideline
Keep in mind that no two programs are the same. They all have different ways of approaching the problem. This diversity is encouraged. Grading checks the validity of the program, not the line-by-line comparison with the answer.
***bls after cmp can be replaced by ble or blt.*****

1) Check if the program has the major elements of the structure such as area, entry, export, __main and end.  (3 points)
2) Check if the program has any syntax errors or typos that will cause build error. Check whether LDR instruction is used properly.   (2 points)
3) Check if the looping is handled properly. Initial count, decrement and ending the loop.  (3 points)
4) Check if the program works. (2 points)  Total 10 points.