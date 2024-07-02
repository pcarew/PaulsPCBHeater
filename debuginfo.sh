avr-objdump.exe -C -t *.elf |grep "\*ABS\*"  > symboltable.sym
avr-objdump.exe -C -t *.elf |grep "\.debug" |sort >> symboltable.sym
avr-objdump.exe -C -t *.elf |grep "\.comment" |sort >> symboltable.sym
avr-objdump.exe -C -t *.elf |grep "\.data" |sort >> symboltable.sym
avr-objdump.exe -C -t *.elf |grep "\.bss" |sort >> symboltable.sym
avr-objdump.exe -C -t *.elf |grep "\.text" |sort >> symboltable.sym
avr-objdump.exe -d *.elf > dissassm.asm
