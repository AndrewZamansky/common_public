;/*
;----------------------------------------------------------
; file : startup_armcc.s
;----------------------------------------------------------
;*/
               AREA    |.text|, CODE, READONLY

                PRESERVE8
                THUMB

do_startup   PROC
                EXPORT  do_startup             [WEAK]
                IMPORT  System_prerun_init
                IMPORT  __main
                cpsid i ;/*disable interrupts*/
                LDR     R0, =System_prerun_init
                BLX     R0
                LDR     R0, =__main
                BX      R0
                ENDP

                END
