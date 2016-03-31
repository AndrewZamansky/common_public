
; <h> Stack Configuration
;   <o> Stack Size (in Bytes) <0x0-0xFFFFFFFF:8>
; </h>

Stack_Size      EQU     0x00000800

                AREA    STACK, NOINIT, READWRITE, ALIGN=3
				EXPORT  Top_Of_Stacks
				EXPORT  __initial_sp
Stack_Mem       SPACE   Stack_Size
Top_Of_Stacks
__initial_sp

; <h> Heap Configuration
;   <o>  Heap Size (in Bytes) <0x0-0xFFFFFFFF:8>
; </h>

Heap_Size       EQU     0x00001000

				EXPORT  __HEAP_START
				EXPORT  __heap_base
				EXPORT  __HEAP_END
				EXPORT  __heap_limit
                AREA    HEAP, NOINIT, READWRITE, ALIGN=3
__HEAP_START
__heap_base
Heap_Mem        SPACE   Heap_Size
__HEAP_END
__heap_limit

                PRESERVE8
                THUMB


; Vector Table Mapped to Address 0 at Reset

                AREA    RESET, DATA, READONLY
                EXPORT  __Vectors
                EXPORT  __Vectors_End
                EXPORT  __Vectors_Size
				IMPORT do_startup

__Vectors       DCD     Top_Of_Stacks              ; Top of Stack
                DCD     do_startup             	  ; Reset Handler
                DCD     NMI_Handler               ; NMI Handler
                DCD     HardFault_Handler         ; Hard Fault Handler
                DCD     MemManage_Handler         ; MPU Fault Handler
                DCD     BusFault_Handler          ; Bus Fault Handler
                DCD     UsageFault_Handler        ; Usage Fault Handler
                DCD     0                         ; Reserved
                DCD     0                         ; Reserved
                DCD     0                         ; Reserved
                DCD     0                         ; Reserved
                DCD     SVC_Handler               ; SVCall Handler
                DCD     DebugMon_Handler          ; Debug Monitor Handler
                DCD     0                         ; Reserved
                DCD     _dummy_trap      		  ; PendSV Handler
                DCD     _dummy_trap               ; SysTick Handler

                ; External Interrupts
                DCD     _dummy_trap            ;  0: Power On Interrupt
                DCD     _dummy_trap            ;  1: Watch Dog Interrupt
				DCD     _dummy_trap		  	  ;  2: Audio Process Unit Interrupt
                DCD     _dummy_trap            ;  3: GPIO Port F
				DCD     _dummy_trap		  ;  4: External GPIO Group 0
				DCD     _dummy_trap		  ;  5: External GPIO Group 1
				DCD     _dummy_trap	 	  ;  6: External GPIO Group 2
				DCD     _dummy_trap	      ;  7: External GPIO Group 3
                DCD     _dummy_trap           ;  8: SPIM Interrupt
                DCD     _dummy_trap           ;  9: USB Device 2.0 Interrupt
                DCD     _dummy_trap           ; 10: Timer 0 Interrupt
                DCD     _dummy_trap           ; 11: Timer 1 Interrupt
                DCD     _dummy_trap           ; 12: Timer 2 Interrupt
                DCD     _dummy_trap           ; 13: Timer 3 Interrupt
                DCD     _dummy_trap            ; 14: SD Host Interrupt
                DCD     _dummy_trap           ; 15: PWM0 Interrupt
                DCD     _dummy_trap           ; 16: PWM1 Interrupt
                DCD     _dummy_trap           ; 17: PWM2 Interrupt
                DCD     _dummy_trap           ; 18: PWM2 Interrupt
                DCD     _dummy_trap            ; 19: Real Time Clock Interrupt
                DCD     _dummy_trap           ; 20: SPI0 Interrupt
                DCD     _dummy_trap           ; 21: I2C1 Interrupt
                DCD     _dummy_trap           ; 22: I2C0 Interrupt
                DCD     _dummy_trap          ; 23: UART0 Interrupt
                DCD     _dummy_trap          ; 24: UART1 Interrupt
                DCD     _dummy_trap            ; 25: ADC  Interrupt
                DCD     _dummy_trap           ; 26: Window Watch Dog Timer Interrupt
                DCD     _dummy_trap           ; 27: USB Host 1.1 Interrupt
                DCD     _dummy_trap          ; 28: UART2 Interrupt
                DCD     _dummy_trap            ; 29: Low Voltage Detection Interrupt
                DCD     _dummy_trap           ; 30: SPI1 Interrupt

__Vectors_End

__Vectors_Size  EQU     __Vectors_End - __Vectors

               AREA    |.text|, CODE, READONLY

; Dummy Exception Handlers (infinite loops which can be modified)

NMI_Handler     PROC
                EXPORT  NMI_Handler               [WEAK]
                B       .
                ENDP
HardFault_Handler\
                PROC
                EXPORT  HardFault_Handler         [WEAK]
                B       .
                ENDP
MemManage_Handler\
                PROC
                EXPORT  MemManage_Handler         [WEAK]
                B       .
                ENDP
BusFault_Handler\
                PROC
                EXPORT  BusFault_Handler          [WEAK]
                B       .
                ENDP
UsageFault_Handler\
                PROC
                EXPORT  UsageFault_Handler        [WEAK]
                B       .
                ENDP
SVC_Handler     PROC
                EXPORT  SVC_Handler               [WEAK]
                B       .
                ENDP
DebugMon_Handler\
                PROC
                EXPORT  DebugMon_Handler          [WEAK]
                B       .
                ENDP

_dummy_trap     PROC
                B       .
                ENDP

                END

