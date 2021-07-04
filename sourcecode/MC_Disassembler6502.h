/**********************************************************************************
 * dcc6502.c -> Main module of:                                                   *
 * Disassembler and Cycle Counter for the 6502 microprocessor                     *
 *                                                                                *
 * This code is offered under the MIT License (MIT)                               *
 *                                                                                *
 * Copyright (c) 1998-2014 Tennessee Carmel-Veilleux <veilleux@tentech.ca>        *
 * https://github.com/tcarmelveilleux/dcc6502                                     *
 *                                                                                *
 * Permission is hereby granted, free of charge, to any person obtaining a copy   *
 * of this software and associated documentation files (the "Software"), to deal  *
 * in the Software without restriction, including without limitation the rights   *
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell      *
 * copies of the Software, and to permit persons to whom the Software is          *
 * furnished to do so, subject to the following conditions:                       *
 *                                                                                *
 * The above copyright notice and this permission notice shall be included in all *
 * copies or substantial portions of the Software.                                *
 *                                                                                *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR     *
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,       *
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE    *
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER         *
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,  *
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE  *
 * SOFTWARE.                                                                      *
 **********************************************************************************/

 //-----------------------------------------------------------------------------
// MC_Disassembler6502.h is base on dcc6502.c
 //-----------------------------------------------------------------------------
 
//-----------------------------------------------------------------------------
// File: MC_Disassembler6502.h: interface for the MC_Disassembler6502 class.
// Desc: Application interface for the MC_Disassembler6502 class.
//-----------------------------------------------------------------------------
#ifndef MC_Disassembler6502_H
    #define MC_Disassembler6502_H

#pragma once

//-----------------------------------------------------------------------------
// Const
//-----------------------------------------------------------------------------
#define VERSION_INFO        "v2.2"
#define NUMBER_OPCODES      151

/* Exceptions for cycle counting */
#define CYCLES_CROSS_PAGE_ADDS_ONE   (1 << 0)
#define CYCLES_BRANCH_TAKEN_ADDS_ONE (1 << 1)

/** Some compilers don't have EOK in errno.h */
#ifndef EOK
    #define EOK 0
#endif
//-----------------------------------------------------------------------------
// Struct
//-----------------------------------------------------------------------------
/* The 6502's 13 addressing modes */
typedef enum {
    IMMED = 0,                                      /* Immediate */
    ABSOL,                                          /* Absolute */
    ZEROP,                                          /* Zero Page */
    IMPLI,                                          /* Implied */
    INDIA,                                          /* Indirect Absolute */
    ABSIX,                                          /* Absolute indexed with X */
    ABSIY,                                          /* Absolute indexed with Y */
    ZEPIX,                                          /* Zero page indexed with X */
    ZEPIY,                                          /* Zero page indexed with Y */
    INDIN,                                          /* Indexed indirect (with X) */
    ININD,                                          /* Indirect indexed (with Y) */
    RELAT,                                          /* Relative */
    ACCUM                                           /* Accumulator */
} addressing_mode_e;

typedef struct opcode_s {
    uint8_t                 number;                 /* Number of the opcode */
    const char*             mnemonic;               /* Index in the name table */
    addressing_mode_e       addressing;             /* Addressing mode */
    unsigned int            cycles;                 /* Number of cycles */
    unsigned int            cycles_exceptions;      /* Mask of cycle-counting exceptions */
} opcode_t;

typedef struct options_s {
    char*                   filename;               /* Input filename */
    int                     cycle_counting;         /* 1 if we want cycle counting */
    int                     hex_output;             /* 1 if hex dump output is desired at beginning of line */
    unsigned long           max_num_bytes;
    uint16_t                org;                    /* Origin of addresses */
    long                    offset;                 /* File offset to start disassembly from */
} options_t;
//-----------------------------------------------------------------------------
// Name: class MC_Disassembler6502
// Desc: Application class
//-----------------------------------------------------------------------------
class MC_Disassembler6502
{

    public:

    protected:

    private:

        //-----------------------------------------------------------------------------

    public:
                            MC_Disassembler6502();
        virtual				~MC_Disassembler6502();
        void                DisassemblerLine(char* Buffer, size_t BufferSize, uint8_t* MemoryMap, uint16_t Address, uint16_t byte_count);
        int                 DisassemblerFile(int argc, char* argv[]);

    protected:
        void                MmitHeader(options_s* options, int fsize);
        char*               AppendCycle(char* input, size_t inputsize, uint8_t entry, uint16_t pc, uint16_t new_pc);
        void                AddNesStr(char* instr, size_t instrsize, const char* instr2);
        void                Disassemble(char* output, size_t outputsize, uint8_t* buffer, options_s* options, uint16_t* pc);
        void                Version();
        void                Usage();
        int                 StrArgToUlong(char* str, unsigned long* value);
        void                UsageAndExit(int exit_code, const char* message);
        void                ParseArgs(int argc, char* argv[], options_s* options);

    private:
};
//-----------------------------------------------------------------------------
#endif // MC_Disassembler6502_H
