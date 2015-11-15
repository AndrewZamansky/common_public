;/*
;----------------------------------------------------------
; file : SWI_armcc.s
;----------------------------------------------------------
;*/
               AREA    |.text|, CODE, READONLY

                PRESERVE8
                THUMB

bkpt_asm   PROC
			EXPORT bkpt_asm
                ; do nothing for now
           ENDP

           END
