# Carbonite Virtual Machine
This is a virtual machine library uses a special architecture called the Dioxide Architecture, built by me.
 * It is a RISC instruction set centered around manipulating registers and memory


# Instruction Set
Instruction format: `INST (ARG1 ARG2)`

Here is the instruction set:
 * 00: NUL (NULL NULL) [do nothing]
 * 01: BRC (pc NULL) [pc = pc]
 * 02: BRZ (reg pc) [reg == 0? pc = pc]
 * 03: HLT (NULL NULL) [stop CPU]
 * 04: SET(reg num) [reg = num]
 * 05: ADD (reg1 reg2) [reg1 = reg1 + reg2]
 * 06: SUB (reg1 reg2) [reg1 = reg1 - reg2]
 * 06: TWC (reg NULL) [two's compliment of register]
 * 07: RSH (reg NULL) [reg >>]
 * 08: LSH (reg NULL) [reg <<]
 * 09: LDCHAR (addr reg) [reg = ram] (1 byte)
 * 10: LDSHRT (addr reg) [reg = ram] (2 bytes)
 * 11: LODINT (addr reg) [reg = ram] (4 bytes)
 * 12: STCHAR (reg addr) [ram = reg] (1 byte)
 * 13: STSHRT (reg addr) [ram = reg] (2 byte)
 * 14: STRINT (reg addr) [ram = reg] (4 byte)

Each instruction will take 9 bytes of data:
 * Byte 0: instruction
 * Byte 1-4: value 1
 * Byte 5-8: value 2

# Memory Structure

 * 64KiB are program code. Your programs cannot be larger than 64KiB.
 * The program code spans from 0x0000 to 0xFFFF
 * The memory pointer for the devices is 0x10000 -> 0x1FFFF
 * The beginning of working RAM is at 0x11000
 * [64KiB (Program Code)]
 * [4KiB (Device I/O)]
 * [Rest (Working RAM)]
 * 
 * Your VM config MUST have at least 66 kilobytes of ram.

# Device Memory Map
 * [2 bytes (identifier)]
 * [4 bytes (device map size)]
 * Device map data entry
    * 2 bytes (data type)
    * 4 bytes (data size)
    * x bytes (data)