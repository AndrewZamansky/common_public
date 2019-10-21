using NuEclipse:

1) flash device using "Nuvoton NuMicro ICP programming tool".
   IMPORTANT: press disconnect after finished!

2) reset by one of the options:
   a) physically reset board.
   b) select "Nuvoton NuMicro ICP programming tool"->options->"reset chip after programming"

3)
  in eclipse plugin "GDB Nuvoton Nu-Link Debugging"
  a) in "Main" select project and elf file.
  b) in "Debugger" select gdb client from the same toolchain that project was compiled with.
     ex. {...}/uCWorkspace/tools/windows/gcc/arm-none-eabi-4.9.3/bin/arm-none-eabi-gdb.exe
  c)
    i)  in "Startup" unselect Load executable to flash
    ii) unselect "continue"
