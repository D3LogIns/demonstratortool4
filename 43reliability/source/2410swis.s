;====================================================================
; File Name : 2410swis.s
; Function  : S3C2410 
; Program   : Shin, On Pil (SOP)
; Date      : March 20, 2002
; Version   : 0.0
; History
;   0.0 : Programming start (March 12, 2002) -> SOP
;====================================================================

;Check if tasm.exe(armasm -16 ...@ADS 1.0) is used.
    GBLL        THUMBCODE
        
    [ {CONFIG} = 16 
THUMBCODE SETL  {TRUE}
        CODE32
    |   
THUMBCODE SETL  {FALSE}
    ]

    MACRO
        MOV_PC_LR
        [ THUMBCODE
            bx      lr
        |
            mov     pc,lr
        ]
    MEND

    AREA |C$$code|, CODE, READONLY

        EXPORT  SWI_ISR
SWI_ISR
            movs pc,lr
        
    END