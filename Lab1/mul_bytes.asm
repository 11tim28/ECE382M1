        .ORIG x3000 
        LEA     R0, I1      ;   R0 <- addr(I1)
        LDW     R1, R0, #0  ;   R1 <- mem[R0] = x3100
        LDB     R2, R1, #0  ;   R2 <- mem[R1] = mem[x3100]

        LEA     R0, I2      ;   R0 <- addr(I2)
        LDW     R1, R0, #0  ;   R1 <- mem[R0] = x3101
        LDB     R3, R1, #0  ;   R3 <- mem[R1] = mem[x3101]

        AND     R0, R0, #0  ;   R0 <- #0
        ADD     R1, R0, #1  ;   R1 <- x0001;
        ADD     R6, R0, #0  ;   set R6 = 0

EVA	AND	R4, R1, R3
        BRZ     NA
	ADD	R6, R6, R2

NA      LSHF    R2, R2, #1
        RSHFL   R3, R3, #1
        BRP     EVA

ST      LEA     R0, OADDR   
        LDW     R2, R0, #0  ;   R2 <- mem[R0] = 0x3102

        LEA     R0, OFADDR  ;
        LDW     R3, R0, #0  ;   R3 <- mem[R0] = 0x3103

        STB     R6, R2, #0
        RSHFL   R6, R6, #8
        BRP     STOF
        BR      DONE

STOF    STB     R1, R3, #0
        BR      DONE

DONE    HALT

I1      .FILL   x3100
I2      .FILL   x3101
OADDR   .FILL   x3102
OFADDR  .FILL   x3103
        .END

; R0: x0000
; R1: x0001 (mask)
; R2: mem[x3100]
; R3: mem[x3101]
; R4: mask eval
; R5: 
; R6: ans
; R7: 

