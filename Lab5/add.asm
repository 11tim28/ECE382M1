		.ORIG x3000        
		LEA R6, USP
		LDW R6, R6, #0                  
		AND R1, R1, #0                                          
		ADD R1, R1, #10		; R1 = 10
		ADD R1, R1, #10		; R1 = 20								; S18(45)
		AND R3, R3, #0		; R3 = 0								; S18(67)
		LEA R0, Loc													; S18(89)
		LDW R0, R0, #0		; R0 = xC000							; S18(111)
Loop	LDB R2, R0, #0												; S18(152)	; S18(282) -> S18(323)[INT] 	; S18(22930)
		ADD R3, R2, R3												; S18(193)	; S18(22841)
		ADD R0, R0, #1												; S18(215)	; S18(22863)
		ADD R1, R1, #-1												; S18(237)	; S18(22885)
		BRp Loop													; S18(259)	; S18(22907)

		STW R3, R0, #0												; S18(25269)

		; ---------------------------------------------------S18(25310)---------------------------------------------------

		; Test protection exception
		JMP R3														; S18(25310)

		; Test page fault
		; LEA R0, PF
		; LDW R0, R0, #0
		; LDW R1, R0, #0

		; Test unaligned access exception
		; ADD R0, R0, #3
		; STW R1, R0, #0

		; Test unknown opcode
		; .FILL xA000


		HALT

Loc		.FILL xC000
PF		.FILL x5000
USP		.FILL xFE00
		.END

																	; END(25442)
; 	address mapping
; 	0x3000 -> 0x3200 (page 24 -> frame 25)
;	0xC000 -> 0x3800 (page 96 -> frame 28)
; 	0xFC00 -> 0xCA00 (page 126 -> frame 29)


		; Test protection exception
		; AND R0, R0, #0      
		; STW R3, R0, #0

		; Test unaligned access exception
		; ADD R0, R0, #3
		; STW R3, R0, #0

		; Test unknown opcode
		; .FILL xA000
