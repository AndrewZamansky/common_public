
/********* next function is called externally *********/
.global bkpt_asm
bkpt_asm:
	//push {r4,lr}
	hlt #0xF000
	//pop {r4,pc}
	ret
