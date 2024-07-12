//*****************************************************************************
// MIT License
//
// Copyright(c) 2017 Gianluca Ghettini
// https://github.com/gianlucag/mos6502
//
// Disassembler for the 6502 microprocessor
// Copyright (c) 1998-2014 Tennessee Carmel-Veilleux <veilleux@tentech.ca>
// https://github.com/tcarmelveilleux/dcc6502
//
// Copyright(c) 2043 Mrx42Code
// https://github.com/Mrx42Code/Compukit-Uk101-Emulator
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files(the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and /or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions :
//
// The above copyright noticeand this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//*****************************************************************************

//-----------------------------------------------------------------------------
// MC_Processor6502.h is base on mos6502.h & Disassembler dcc6502.h
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// File: MC_Processor6502.h: interface for the MC_Processor6502 class.
// Desc: Application interface for the MC_Processor6502 class.
//-----------------------------------------------------------------------------
#ifndef MC_Processor6502_H
#define MC_Processor6502_H

#pragma once

//-----------------------------------------------------------------------------
// include Windows standard Libs
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// include Project
//-----------------------------------------------------------------------------
#include "framework.h"

//-----------------------------------------------------------------------------
// include Vendors
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// define
//-----------------------------------------------------------------------------
#define CrashDumpSize		32													// 32 entries
#define DebugLineLen		512													// 512 chars
#define DebugSubLineLen		DebugLineLen / 2									// 256 chars

#define FLAGNEGATIVE		0x80												// 7th bit
#define FLAGOVERFLOW		0x40												// 6th bit
#define FLAGCONSTANT		0x20												// 5th bit
#define FLAGBREAK			0x10												// 4th bit
#define FLAGDECIMAL			0x08												// 3rd bit
#define FLAGINTERRUPT		0x04												// 2nd bit
#define FLAGZERO			0x02												// 1st bit
#define FLAGCARRY			0x01												// 0th bit

#define SET_NEGATIVE(x) (x ? (m_registers.status |= FLAGNEGATIVE) : (m_registers.status &= (~FLAGNEGATIVE)) )	// SET NEGATIVE
#define SET_OVERFLOW(x) (x ? (m_registers.status |= FLAGOVERFLOW) : (m_registers.status &= (~FLAGOVERFLOW)) )	// SET OVERFLOW
#define SET_CONSTANT(x) (x ? (m_registers.status |= FLAGCONSTANT) : (m_registers.status &= (~FLAGCONSTANT)) )	// SET CONSTANT
#define SET_BREAK(x) (x ? (m_registers.status |= FLAGBREAK) : (m_registers.status &= (~FLAGBREAK)) )			// SET BREAK
#define SET_DECIMAL(x) (x ? (m_registers.status |= FLAGDECIMAL) : (m_registers.status &= (~FLAGDECIMAL)) )		// SET DECIMAL
#define SET_INTERRUPT(x) (x ? (m_registers.status |= FLAGINTERRUPT) : (m_registers.status &= (~FLAGINTERRUPT)) )// SET INTERRUPT
#define SET_ZERO(x) (x ? (m_registers.status |= FLAGZERO) : (m_registers.status &= (~FLAGZERO)) )				// SET ZERO
#define SET_CARRY(x) (x ? (m_registers.status |= FLAGCARRY) : (m_registers.status &= (~FLAGCARRY)) )			// SET CARRY

#define IF_NEGATIVE() ((m_registers.status & FLAGNEGATIVE) ? true : false)		// IF NEGATIVE
#define IF_OVERFLOW() ((m_registers.status & FLAGOVERFLOW) ? true : false)		// IF OVERFLOW
#define IF_CONSTANT() ((m_registers.status & FLAGCONSTANT) ? true : false)		// IF CONSTANT
#define IF_BREAK() ((m_registers.status & FLAGBREAK) ? true : false)			// IF BREAK		
#define IF_DECIMAL() ((m_registers.status & FLAGDECIMAL) ? true : false)		// IF DECIMAL
#define IF_INTERRUPT() ((m_registers.status & FLAGINTERRUPT) ? true : false)	// IF INTERRUPT
#define IF_ZERO() ((m_registers.status & FLAGZERO) ? true : false)				// IF ZERO
#define IF_CARRY() ((m_registers.status & FLAGCARRY) ? true : false)			// IF CARRY

#define SHOW(T,V) do { T x = V; PrintBits(#T, #V, (unsigned char*) &x, sizeof(x)); } while(0) // SHOW

/* Helper macros for disassemble() function */
#define HIGH_PART(val) (((val) >> 8) & 0xFFu)									// extract high byte
#define LOW_PART(val) ((val) & 0xFFu)											// extract low byte
#define LOAD_WORD(buffer, current_pc) ((uint16_t)buffer[(current_pc) + 1] | (((uint16_t)buffer[(current_pc) + 2]) << 8)) // load word

//-----------------------------------------------------------------------------
// struct
//-----------------------------------------------------------------------------
typedef struct Cycles6502Info
{
	bool					CanHaveExCycles;									// true if instruction can have extra cycles
	uint8_t					Cycles;												// number of cycles
	uint8_t					ExCycles;											// number of extra cycles
	uint8_t					TotalCycles;										// total number of cycles
} _Cycles6502Info;

typedef struct Registers6502
{
	uint8_t					A;													// accumulator
	uint8_t					X;													// Register X-index
	uint8_t					Y;													// Register Y-index
	uint8_t					sp;													// stack pointer
	uint16_t				pc;													// program counter
	uint8_t					status;												// status register
	bool					IllegalOpcode;										// Illegal Op code Flag;
} _Registers6502;

enum Flags {
	CarryFlag = 0,																// CarryFlag
	ZeroFlag = 1,																// ZeroFlag
	InterruptDisableFlag = 2,													// InterruptDisableFlag
	DecimalFlag = 3,															// DecimalFlag
	BreakFlag = 4,																// BreakFlag
	UnknownFlag = 5,															// UnknownFlag
	OverflowFlag = 6,															// OverflowFlag
	NegativeFlag = 7															// NegativeFlag
};

enum {
	IMMED = 0,																	// Immediate
	ABSOL,																		// Absolute
	ZEROP,																		// Zero Page
	IMPLI,																		// Implied
	INDIA,																		// Indirect Absolute
	ABSIX,																		// Absolute indexed with X
	ABSIY,																		// Absolute indexed with Y
	ZEPIX,																		// Zero page indexed with X
	ZEPIY,																		// Zero page indexed with Y
	INDIN,																		// Indexed indirect (with X)
	ININD,																		// Indirect indexed (with Y)
	RELAT,																		// Relative
	ACCUM,																		// Accumulator
	ILLEGAL
};

//-----------------------------------------------------------------------------
// Name: class MC_Processor6502
// Desc: Application class
//-----------------------------------------------------------------------------
class MC_Processor6502
{

public:
	enum class			CycleMethod { INST_COUNT, CYCLE_COUNT };				// CycleMethod

	typedef void		(MC_Processor6502::* CodeExec)(uint16_t);				// CodeExec
	typedef uint16_t(MC_Processor6502::* AddrExec)();							// AddrExec

	typedef struct Instr														// Instruction 
	{
		AddrExec				AddrMode;										// Addressing Mode
		CodeExec				Code;											// Code execution
		uint8_t					Cycles;											// Number of cycles
		bool					CanHaveExCycles;								// true if instruction can have extra cycles
		const char*				Mnemonic;										// Mnemonic
		uint8_t					AddressingMode;									// Addressing Mode
	} _Instr;

	typedef struct DebugInfo6502												// Debug info
	{
		uint16_t				pc;												// program counter
		struct Registers6502	Registers;										// registers
		uint8_t					TotalCycles;									// total number of cycles
		uint8_t					ExCycles;										// number of extra cycles
		bool					Updated;										// true if debug info has been updated
	} _DebugInfo6502;

	typedef struct CrashDump6502												// crash dump
	{
		struct DebugInfo6502	Info[CrashDumpSize];							// crash dump debug array
		uint16_t				Index;											// index

	} _CrashDump6502;

	typedef struct Instruction													// Instruction
	{
		uint8_t					OpCode;											// opcode
		Instr					instr;											// instruction
	} _Instruction;

	struct CrashDump6502		m_CrashDump;									// crash dump
	struct DebugInfo6502		m_Debug;										// debug info
	uint64_t					m_TotalCyclesPerSec;							// total cycles per second

private:
	struct Instr				m_InstrTbl[256];								// instruction table
	struct Instruction			m_Instruction;									// instruction
	struct Registers6502		m_registers;									// registers
	struct Cycles6502Info		m_Clock;										// clock

	// IRQ, reset, NMI vectors
	static const uint16_t		s_irqVectorH = 0xFFFF;							// IRQ vector Hi
	static const uint16_t		s_irqVectorL = 0xFFFE;							// IRQ vector Lo
	static const uint16_t		s_rstVectorH = 0xFFFD;							// Reset vector Hi
	static const uint16_t		s_rstVectorL = 0xFFFC;							// Reset vector Lo
	static const uint16_t		s_nmiVectorH = 0xFFFB;							// NMI vector Hi
	static const uint16_t		s_nmiVectorL = 0xFFFA;							// NMI vector Lo

	// read/write callbacks
	typedef void (*BusWrite)(uint16_t, uint8_t);								// BusWrite callback
	typedef uint8_t(*BusRead)(uint16_t);										// BusRead callback
	BusRead						MemoryRead;										// MemoryRead callback
	BusWrite					MemoryWrite;									// MemoryWrite callback

protected:

	//-----------------------------------------------------------------------------

public:
	MC_Processor6502();
	MC_Processor6502(BusRead r, BusWrite w);									// constructor with	BusWrite callback & BusRead callback
	virtual				~MC_Processor6502();
	void				NMI();													// NMI
	void				IRQ();													// IRQ
	void				Reset();												// Reset
	uint16_t			GetPC();												// Get PC programe counter
	void				SetPC(uint16_t PC);										// Set PC programe counter
	struct Registers6502 GetRegisters();										// Get Registers
	void				SetRegisters(Registers6502 Registers);					// Set Registers
	bool				RunOneOp();												// Run One Op
	void				RunCode(int32_t cycles, uint64_t& cycleCount, CycleMethod cycleMethod = CycleMethod::CYCLE_COUNT);// Run Code
	void				DebugInfo(uint8_t* MemoryMap);							// Debug Info		
	void				DebugCrashInfo(uint8_t* MemoryMap);						// Debug Crash Info

private:
	void				Initialize();											// Initialize
	uint8_t				Exec(Instr& instr);										// Execute instruction
	// addressing modes
	uint16_t			Addr_ACC();												// ACCUMULATOR				
	uint16_t			Addr_IMM();												// IMMEDIATE
	uint16_t			Addr_ABS();												// ABSOLUTE
	uint16_t			Addr_ZER();												// ZERO PAGE
	uint16_t			Addr_ZEX();												// INDEXED-X ZERO PAGE
	uint16_t			Addr_ZEY();												// INDEXED-Y ZERO PAGE
	uint16_t			Addr_ABX();												// INDEXED-X ABSOLUTE
	uint16_t			Addr_ABY();												// INDEXED-Y ABSOLUTE
	uint16_t			Addr_IMP();												// IMPLIED
	uint16_t			Addr_REL();												// RELATIVE
	uint16_t			Addr_INX();												// INDEXED-X INDIRECT
	uint16_t			Addr_INY();												// INDEXED-Y INDIRECT
	uint16_t			Addr_ABI(); 											// ABSOLUTE INDIRECT
	// opcodes (grouped as per datasheet)
	void				Op_ADC(uint16_t src);									// ADD WITH CARRY
	void				Op_AND(uint16_t src);									// AND
	void				Op_ASL(uint16_t src);									// ARITHMETIC SHIFT LEFT
	void				Op_ASL_ACC(uint16_t src);								// ARITHMETIC SHIFT LEFT ACCUMULATOR
	void				Op_BCC(uint16_t src);									// BRANCH ON CARRY CLEAR
	void				Op_BCS(uint16_t src);									// BRANCH ON CARRY SET
	void				Op_BEQ(uint16_t src);									// BRANCH ON EQUAL
	void				Op_BIT(uint16_t src);									// BIT
	void				Op_BMI(uint16_t src);									// BRANCH ON MINUS
	void				Op_BNE(uint16_t src);									// BRANCH ON NOT EQUAL
	void				Op_BPL(uint16_t src);									// BRANCH ON PLUS
	void				Op_BRK(uint16_t src);									// BREAK
	void				Op_BVC(uint16_t src);									// BRANCH ON OVERFLOW CLEAR
	void				Op_BVS(uint16_t src);									// BRANCH ON OVERFLOW SET
	void				Op_CLC(uint16_t src);									// CLEAR CARRY
	void				Op_CLD(uint16_t src);									// CLEAR DECIMAL
	void				Op_CLI(uint16_t src);									// CLEAR INTERRUPT
	void				Op_CLV(uint16_t src);									// CLEAR OVERFLOW
	void				Op_CMP(uint16_t src);									// COMPARE ACCUMULATOR
	void				Op_CPX(uint16_t src);									// COMPARE X Register
	void				Op_CPY(uint16_t src);									// COMPARE Y Register
	void				Op_DEC(uint16_t src);									// DECREMENT ACCUMULATOR
	void				Op_DEX(uint16_t src);									// DECREMENT X Register
	void				Op_DEY(uint16_t src);									// DECREMENT Y Register
	void				Op_EOR(uint16_t src);									// EXCLUSIVE OR ACCUMULATOR
	void				Op_INC(uint16_t src);									// INCREMENT ACCUMULATOR
	void				Op_INX(uint16_t src);									// INCREMENT X Register
	void				Op_INY(uint16_t src);									// INCREMENT Y Register
	void				Op_JMP(uint16_t src);									// JUMP
	void				Op_JSR(uint16_t src);									// JUMP SUBROUTINE
	void				Op_LDA(uint16_t src);									// LOAD ACCUMULATOR
	void				Op_LDX(uint16_t src);									// LOAD X Register
	void				Op_LDY(uint16_t src);									// LOAD Y Register
	void				Op_LSR(uint16_t src);									// LOGICAL SHIFT RIGHT
	void				Op_LSR_ACC(uint16_t src);								// LOGICAL SHIFT RIGHT ACCUMULATOR
	void				Op_NOP(uint16_t src);									// NO OPERATION
	void				Op_ORA(uint16_t src);									// OR WITH ACCUMULATOR
	void				Op_PHA(uint16_t src);									// PUSH ACCUMULATOR
	void				Op_PHP(uint16_t src);									// PUSH STATUS
	void				Op_PLA(uint16_t src);									// POP ACCUMULATOR
	void				Op_PLP(uint16_t src);									// POP STATUS
	void				Op_ROL(uint16_t src);									// ROTATE LEFT
	void				Op_ROL_ACC(uint16_t src);								// ROTATE LEFT ACCUMULATOR
	void				Op_ROR(uint16_t src);									// ROTATE RIGHT
	void				Op_ROR_ACC(uint16_t src);								// ROTATE RIGHT ACCUMULATOR
	void				Op_RTI(uint16_t src);									// RETURN FROM INTERRUPT
	void				Op_RTS(uint16_t src);									// RETURN FROM SUBROUTINE
	void				Op_SBC(uint16_t src);									// SUBTRACT WITH CARRY
	void				Op_SEC(uint16_t src);									// SET CARRY
	void				Op_SED(uint16_t src);									// SET DECIMAL
	void				Op_SEI(uint16_t src);									// SET INTERRUPT
	void				Op_STA(uint16_t src);									// STORE ACCUMULATOR
	void				Op_STX(uint16_t src);									// STORE X Register
	void				Op_STY(uint16_t src);									// STORE Y Register
	void				Op_TAX(uint16_t src);									// TRANSFER ACCUMULATOR TO X
	void				Op_TAY(uint16_t src);									// TRANSFER ACCUMULATOR TO Y
	void				Op_TSX(uint16_t src);									// TRANSFER STACK POINTER TO X
	void				Op_TXA(uint16_t src);									// TRANSFER X TO ACCUMULATOR
	void				Op_TXS(uint16_t src);									// TRANSFER X TO STACK POINTER
	void				Op_TYA(uint16_t src);									// TRANSFER Y TO ACCUMULATOR
	void				Op_ILLEGAL(uint16_t src);								// ILLEGAL OPCODE
	// stack operations
	inline void			StackPush(uint8_t byte);								// push a byte onto the stack
	inline uint8_t		StackPop();												// pop a byte off the stack

	void				Disassemble(char* output, size_t outputsize, uint8_t* buffer, uint16_t* pc); // disassemble a 6502 instruction
	void				PrintByteAsBits(char val);								// print a byte as bits
	void				PrintBits(const char* ty, const char* val, unsigned char* bytes, size_t num_bytes); // print bits
	void				PrintHexDump16Bit(const char* desc, void* addr, long len, long offset); // print a hex dump

protected:

};
//-----------------------------------------------------------------------------
#endif // MC_Processor6502_H
