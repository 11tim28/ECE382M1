		.ORIG x3000
		LEA R0, Tag
		LDW R0, R0, #0                                         
		AND R1, R1, #0                                         
		ADD R1, R1, #1		; R1 = 1                           
		STW R1, R0, #0		; m[x4000] = 1                     
		ADD R1, R1, #9		; R1 = 10
		ADD R1, R1, #10		; R1 = 20
		AND R3, R3, #0		; R3 = 0
		LEA R0, Loc
		LDW R0, R0, #0		; R0 = xC000
Loop	LDB R2, R0, #0
		ADD R3, R2, R3
		ADD R0, R0, #1
		ADD R1, R1, #-1
		BRp Loop

		STW R3, R0, #0

		; Test protection exception
		AND R0, R0, #0      
		STW R3, R0, #0

		; Test unaligned access exception
		; ADD R0, R0, #3
		; STW R3, R0, #0

		; Test unknown opcode
		; .FILL xA000
		HALT

Tag 	.FILL x4000
Loc		.FILL xC000
		.END
