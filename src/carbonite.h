/* MIT License
 *
 * Copyright (c) 2023 Abdurahman Elmawi
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * SPDX-License-Identifier: MIT
 */


/* Carbonite Virtual Machine
 * This is a virtual machine library uses a special architecture called the Dioxide Architecture, built by me
 * It is a RISC instruction set centered around manipulating registers and memory
 * 
 * Each instruction should look something like this:
 * INST (ARG1 ARG2)
 * 
 * Here is the instruction set:
 * 00: NUL (NULL NULL) [do nothing]
 * 01: BRC (pc NULL) [pc = pc]
 * 02: BRZ (reg pc) [reg == 0? pc = pc]
 * 03: SET (reg val) [reg = val]
 * 04: LOD (addr reg) [reg = ram]
 * 05: STR (reg addr) [ram = reg]
 * 06: ADD (reg1 reg2) [reg1 = reg1 + reg2]
 * 07: SUB (reg1 reg2) [reg1 = reg1 - reg2]
 * 07: TWC (reg NULL) [two's compliment of register]
 * 08: RSH (reg NULL) [reg >>]
 * 09: LSH (reg NULL) [reg <<]
 * 10: HLT (NULL NULL) [stop CPU]
 * 
 * Each instruction will take 9 bytes of data:
 * Byte 0: instruction
 * Byte 1-4: value 1
 * Byte 5-8: value 2
*/

#ifndef CARBONITE
#define CARBONITE

typedef struct VM {
    unsigned char* memory;
    unsigned int* registers;
    int num_registers;
    int reg_size;
    int memory_size;

    int pc;
    int terminate;
} VM;

int twos_compliment(int value)
{
    return value;
}

VM create_vm(int memory_size, int num_registers, int register_size)
{
    VM vm;
    vm.memory = (unsigned char*) malloc(memory_size);
    vm.registers = (unsigned int*) malloc(num_registers * register_size);
    vm.memory_size = memory_size;
    vm.num_registers = num_registers;
    vm.reg_size = register_size;
    vm.terminate = 0;
    printf("VM memory initialized (Memory: %d bytes, Registers: %d, Register Size: %d bytes)\n", memory_size, num_registers, register_size);
    return vm;
}

void load_program(VM *vm, unsigned char *program, int size)
{
    for(int i = 0; i < size; i++)
        vm->memory[i] = program[i];
    
    printf("VM program loaded (size: %d bytes)\n", size);
}

void dump_vm(VM *vm)
{
    printf("Dumping VM RAM/register contents:\n");
    printf("--- Registers ---\n");
    for(int i = 0; i < vm->num_registers; i++)
        printf("Register %d: %x\n", i, vm->registers[i]);
    printf("--- RAM ---\n");
    for(int i = 0; i < vm->memory_size; i++)
        printf("%x ", vm->memory[i]);
    printf("\n");
}

void run_cycle(VM *vm)
{
    int pc = vm->pc;
    unsigned char instruction = vm->memory[pc];
    int arg1 = 0;
    int arg2 = 0;
    //Get instruction and arguments from memory
    for(int i = 0; i < 4; i++)
    {
        int shift_size = (3 - i) * 8;
        arg1 += vm->memory[pc + i + 1] << shift_size;
        arg2 += vm->memory[pc + i + 5] << shift_size;
    }
    printf("%d (%d, %d)\n", instruction, arg1, arg2);
    int branched = 0;
    switch(instruction)
    {
        case 0: //NUL
            break;
        case 1: //BRC
            vm->pc = arg1 * 9;
            branched = 1;
            break;
        case 2: //BRZ
            if(vm->registers[arg1] == 0)
            {
                vm->pc = arg2 * 9;
                branched = 1;
            }
            break;
        case 3: //SET
            vm->registers[arg1] = arg2;
            break;
        case 4: //LOD
            vm->registers[arg2] = vm->memory[arg1];
            break;
        case 5: //STR
            vm->memory[arg2] = vm->registers[arg1];
            break;
        case 6: //ADD
            vm->registers[arg1] += vm->registers[arg2];
            break;
        case 7: //TWC TODO, for now it is SUB
            // vm->registers[arg1] = twos_compliment(vm->registers[arg1]);
            vm->registers[arg1] -= vm->registers[arg2];
            break;
        case 8: //RSH
            vm->registers[arg1] = vm->registers[arg1] >> 1;
            break;
        case 9: //LSH
            vm->registers[arg1] = vm->registers[arg1] << 1;
            break;
        case 10: //HLT
            vm->terminate = 1;
            break;
    }
    if(!branched)
        vm->pc+=9;
    if(vm->pc > vm->memory_size)
    {
        printf("Program counter has exceeded the boundaries of RAM. Terminating system.\nDumping VM:\n");
        dump_vm(vm);
        exit(1);
    }
}

#endif