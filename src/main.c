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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "config.h"
#include "carbonite.h"
#include "file.h"

typedef char *prg_object;

FileDescriptor program;
FileDescriptor config;
int config_used = 0;

VM vm;

int string_is_equal(char* string1, char* string2)
{
    return (strcmp(string1, string2) == 0);
}

int main(int argc, char *argv[])
{
	// Read user input
	for (int i = 0; i < argc; i++)
	{
		char *arg = argv[i];
		if (string_is_equal(arg, "--config") || string_is_equal(arg, "-c"))
		{
			if(config_used == 0)
			{
			i++;
			if (i >= argc)
			{
				printf("A configuration file was not specified.\n");
				return EXIT_FAILURE;
			}
			config_used = 1;
			config = read_file(argv[i]);
			}
		} else
			program = read_file(arg);
	}

	// Import config file
	if (config_used)
	{
		printf("%s\n", config.string);
		fclose(config.file);
	}

	// Create virtual machine
	vm = create_vm(ALLOCATED_RAM, NUM_REGS, REGISTER_SIZE);
	VM *vm_ptr = &vm;
	
	/* Compile program into dioxide machine code

	 * The first step to compiling cvm code is to break down the sets of strings into a set of objects
	 * From there, we read each 3 objects as a single complete instruction
	 * We then decode it into 8bit chars that can then be inputted into the VM as machine code.
	 * 
	 * From there, the machine executes the code.
	*/
	prg_object *program_objects = malloc(0);
	int num_objects = 0;
	int object_size = 0;
	char buffer;
	int skip_object = 0;
	for(int i = 0; i <= program.length; i++)
	{
		buffer = program.string[i];

		if(skip_object == 0)
		{
			if(buffer == ' ' || buffer == '\n' || buffer == ';' || i == program.length || buffer == '#')
			{
				if(object_size > 0)
				{
					//Copy object into a buffer and then write a pointer to the buffer to the object list
					prg_object object_buffer = malloc(object_size);
					for(int j = 0; j < object_size; j++)
						object_buffer[j] = program.string[(i - object_size) + j];
					num_objects++;
					program_objects = realloc(program_objects, sizeof(prg_object[num_objects]));
					program_objects[num_objects - 1] = object_buffer;
					object_size = 0;
				}
			} else
				object_size++;
		}

		if(buffer == '#')
			skip_object = 1;
		if(buffer == '\n')
			skip_object = 0;
	}
	//Close files
	fclose(program.file);

	// Turn objects into actual machine code
	unsigned char m_program[num_objects * 3];
	for(int i = 0; i < num_objects; i+= 3)
	{
		char *instruction = program_objects[i];
		char *arg1 = program_objects[i + 1];
		char *arg2 = program_objects[i + 2];

		unsigned char m_instruction;
		unsigned int m_arg1_buff, m_arg2_buff;

		char is_valid_instruction = 1;

		if (string_is_equal(instruction, "NUL")) m_instruction = 0;
		else if (string_is_equal(instruction, "BRC")) m_instruction = 1;
		else if (string_is_equal(instruction, "BRZ")) m_instruction = 2;
		else if (string_is_equal(instruction, "SET")) m_instruction = 3;
		else if (string_is_equal(instruction, "LOD")) m_instruction = 4;
		else if (string_is_equal(instruction, "STR")) m_instruction = 5;
		else if (string_is_equal(instruction, "ADD")) m_instruction = 6;
		else if (string_is_equal(instruction, "SUB")) m_instruction = 7;
		else if (string_is_equal(instruction, "RSH")) m_instruction = 8;
		else if (string_is_equal(instruction, "LSH")) m_instruction = 9;
		else if (string_is_equal(instruction, "HLT")) m_instruction = 10;
		else
			is_valid_instruction = 0;

		if(is_valid_instruction)
		{
			m_arg1_buff = atoi(arg1);
			m_arg2_buff = atoi(arg2);

			int input_arg_size = sizeof(m_arg1_buff);

			unsigned char m_arg1[input_arg_size];
			unsigned char m_arg2[input_arg_size];
			// Convert multi-byte arguments into an array of chars so that they can be placed in VM ram
			for(int j = 0; j < input_arg_size; j++)
			{
				int shift_size = (input_arg_size - (j + 1)) * 8;
				m_arg1[j] = m_arg1_buff >> shift_size;
				m_arg2[j] = m_arg2_buff >> shift_size;
			}

			//Store the complete instruction in the program buffer
			int program_index = i * 3;
			m_program[program_index] = m_instruction;
			for(int j = 0; j < input_arg_size; j++)
			{
				m_program[program_index + j + 1] = m_arg1[j];
				m_program[program_index + j + 5] = m_arg2[j];
			}
		}
	}

	//Load program into virtual machine
	load_program(vm_ptr, m_program, num_objects * 3);

	//Run program
	while(!vm.terminate)
	{
		run_cycle(vm_ptr);
	}
	// dump_vm(vm_ptr);


	return EXIT_SUCCESS;
}
