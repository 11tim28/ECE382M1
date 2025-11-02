    .ORIG x1000
    ; system space (page 0 ~ 23)
    .FILL x0004     ; page 0
    .FILL x0204
    .FILL x0404
    .FILL x0604
    .FILL x0804
    .FILL x0A04
    .FILL x0C04
    .FILL x0E04
    .FILL x1004
    .FILL x1204
    .FILL x1404
    .FILL x1604
    .FILL x1804
    .FILL x1A04
    .FILL x1C04
    .FILL x1E04
    .FILL x2004
    .FILL x2204
    .FILL x2404
    .FILL x2604
    .FILL x2804
    .FILL x2A04
    .FILL x2C04
    .FILL x2E04     ; page 23

    ; user space 
    ; page 24 ~ 32
    .FILL x320C     ; page 24 -> frame 25 (0x3000 -> 0x3200)
    .FILL x0008
    .FILL x0008
    .FILL x0008
    .FILL x0008     
    .FILL x0008
    .FILL x0008
    .FILL x0008
    .FILL x0008

    ; page 33 ~ 40
    .FILL x0008
    .FILL x0008
    .FILL x0008 
    .FILL x0008
    .FILL x0008
    .FILL x0008
    .FILL x0008
    .FILL x0008

    ; page 41 ~ 48
    .FILL x0008
    .FILL x0008
    .FILL x0008
    .FILL x0008
    .FILL x0008
    .FILL x0008     
    .FILL x0008
    .FILL x0008

    ; page 49 ~ 56
    .FILL x0008
    .FILL x0008
    .FILL x0008
    .FILL x0008
    .FILL x0008
    .FILL x0008
    .FILL x0008
    .FILL x0008

    ; page 57 ~ 64
    .FILL x0008 
    .FILL x0008
    .FILL x0008
    .FILL x0008
    .FILL x0008
    .FILL x0008
    .FILL x0008
    .FILL x0008

    ; page 65 ~ 72
    .FILL x0008
    .FILL x0008
    .FILL x0008
    .FILL x0008 
    .FILL x0008
    .FILL x0008
    .FILL x0008
    .FILL x0008

    ; page 73 ~ 80
    .FILL x0008
    .FILL x0008
    .FILL x0008
    .FILL x0008
    .FILL x0008
    .FILL x0008
    .FILL x0008 
    .FILL x0008

    ; page 81 ~ 88
    .FILL x0008
    .FILL x0008
    .FILL x0008
    .FILL x0008
    .FILL x0008
    .FILL x0008
    .FILL x0008
    .FILL x0008

    ; page 89 ~ 96
    .FILL x0008
    .FILL x0008 
    .FILL x0008
    .FILL x0008
    .FILL x0008
    .FILL x0008
    .FILL x0008
    .FILL x380C     ; page 96 -> frame 28 (0xC000 -> 0x3800)
    
    ; page 97 ~ 104
    .FILL x0008
    .FILL x0008
    .FILL x0008
    .FILL x0008
    .FILL x0008 
    .FILL x0008
    .FILL x0008
    .FILL x0008

    ; page 105 ~ 112
    .FILL x0008
    .FILL x0008
    .FILL x0008
    .FILL x0008
    .FILL x0008
    .FILL x0008
    .FILL x0008
    .FILL x0008 

    ; page 113 ~ 120
    .FILL x0008
    .FILL x0008
    .FILL x0008
    .FILL x0008
    .FILL x0008
    .FILL x0008
    .FILL x0008
    .FILL x0008

    ; page 121 ~ 127
    .FILL x0008
    .FILL x0008
    .FILL x0008 
    .FILL x0008
    .FILL x0008
    .FILL x3A0C     ; page 126 -> frame 29 (0xFC00 -> 0xCA00)
    .FILL x0008
    .END
