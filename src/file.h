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

#include <stdlib.h>
#include <unistd.h>

#include "vm/carbonite.h"

typedef struct FileDescriptor
{
    long length;
    char *string;
    FILE *file;
} FileDescriptor;

FileDescriptor read_file(char *file_name)
{
    FileDescriptor object;
    char *buffer;
    if (access(file_name, F_OK) == 0)
    {
        FILE *fp = fopen(file_name, "r");

        // Find file size
        fseek(fp, 0L, SEEK_END);
        long file_size = ftell(fp);
        rewind(fp);

        buffer = malloc(file_size);
        // while (!feof(fp))
        fread(buffer, file_size, 1, fp);

        //Set object properties
        object.string = buffer;
        object.length = file_size;
        object.file = fp;
    }
    else
    {
        printf("File does not exist\n");
        return object;
    }

    return object;
}