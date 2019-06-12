using: eclipse + OpenOCD + "ST-Link V2":

1) install OpenOCD (version tested: 0.10.0)
2) in eclipse install "GDB Hardware Debugging" plugin.

3)
  in eclipse plugin "GDB Hardware Debugging"
  a) in "Main" select project and elf file.
  b) in "Debugger" select gdb client from the same toolchain that project was compiled with.
     ex. {...}/uCWorkspace/tools/windows/gcc/arm-none-eabi-4.9.3/bin/arm-none-eabi-gdb.exe
	 select IP address  :localhost
	 select Port number :3333
  c)
    i)  in "Initialization Commands" add :
		monitor init
		monitor arm semihosting enable
		monitor arm semihosting_fileio disable
		monitor reset halt
    
	ii) 
		in map file find address of 'start_with_semihosting'.
		put this address in "Set program counter at" field

4) run openOCD server :
   [path_to_openOCD_bin]/openocd -f [path_to_common_public]/c/hw_drivers/arch/ARM/soc/stm32/openocd.cfg