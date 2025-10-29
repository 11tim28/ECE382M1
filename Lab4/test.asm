    .ORIG x3000
    LEA R0, MEM
    LDW R0, R0, #0
    LDW R1, R0, #0
    LDW R2, R0, #1
    LDW R3, R0, #2
    ADD R4, R1, R2
    HALT

MEM .FILL 0xC000
    .END

