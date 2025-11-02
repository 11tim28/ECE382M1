		.ORIG x1200
		ADD R6, R6, #-2              								; S18(386)        
		STW R0, R6, #0												; S18(408)
		ADD R6, R6, #-2                      						; S18(449)
		STW R1, R6, #0												; S18(471)
		ADD R6, R6, #-2                      						; S18(512)
		STW R2, R6, #0												; S18(534)

		LEA R0, PT                          						; S18(575)
		LDW R0, R0, #0		; R0 = x1000							; S18(597)
		LEA R1, SIZE												; S18(638)
		LDW R1, R1, #0		; R1 = x0080 (128)
Loop	LDW R2, R0, #0  											; S18(701)	; S18(872)
		AND R2, R2, #-2 	; set R = 0								; S18(742)
		STW R2, R0, #0                    							; S18(764)
		ADD R0, R0, #2												; S18(805)
		ADD R1, R1, #-1												; S18(827)
		BRp Loop													; S18(849)


		LDW R2, R6, #0      										; S18(22588)      
		ADD R6, R6, #2
		LDW R1, R6, #0                       
		ADD R6, R6, #2
		LDW R0, R6, #0                       
		ADD R6, R6, #2
		RTI                 										; S18(22798)           

PT		.FILL x1000
SIZE	.FILL x0080
		.END
