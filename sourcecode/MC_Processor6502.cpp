/**********************************************************************************
* MIT License																	  *
*																				  *
* Copyright(c) 2017 Gianluca Ghettini											  *
* https://github.com/gianlucag/mos6502											  *
*																				  *
* Permission is hereby granted, free of charge, to any person obtaining a copy    *
* of this softwareand associated documentation files(the "Software"), to deal	  *
* in the Software without restriction, including without limitation the rights	  *
* to use, copy, modify, merge, publish, distribute, sublicense, and /or sell	  *
* copies of the Software, and to permit persons to whom the Software is			  *
* furnished to do so, subject to the following conditions :						  *
*																				  *
* The above copyright noticeand this permission notice shall be included in all   *
* copies or substantial portions of the Software.								  *
*																				  *
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR	  *
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,		  *
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE	  *
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER		  *	
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,   *
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE	  *
* SOFTWARE.																		  *
 **********************************************************************************/

 //-----------------------------------------------------------------------------
 // MC_Processor6502.cpp is base on mos6502.cpp
 //-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// File: MC_Processor6502.cpp: implementation of the MC_Processor6502 class.
//-----------------------------------------------------------------------------
#include "framework.h"
#include "MC_Processor6502.h"

using namespace std;

//*****************************************************************************  
// Public Code
//*****************************************************************************

//-----------------------------------------------------------------------------
// IMPLEMENT_DYNCREATE
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// message handlers
//-----------------------------------------------------------------------------

//-Public----------------------------------------------------------------------
// Name: MC_Processor6502()
// Desc: MC_Processor6502 class
//-----------------------------------------------------------------------------
MC_Processor6502::MC_Processor6502()
{
	memset(&m_registers, 0, sizeof(m_registers));
	memset(&m_CrashDump, 0, sizeof(m_CrashDump));
	memset(&m_InstrTable, 0, sizeof(m_InstrTable));
	memset(&m_DebugInstr, 0, sizeof(m_DebugInstr));
	MemoryRead = nullptr;
	MemoryWrite = nullptr;
	m_TotalCyclesPerSec = 0;
}
//-Public----------------------------------------------------------------------
// Name: mos6502(BusRead r, BusWrite w)
// Desc: MC_Processor6502 class
//-----------------------------------------------------------------------------
MC_Processor6502::MC_Processor6502(BusRead r, BusWrite w)
{
	Instr instr;

	memset(&m_registers, 0, sizeof(m_registers));
	memset(&m_CrashDump, 0, sizeof(m_CrashDump));
	memset(&m_InstrTable, 0, sizeof(m_InstrTable));
	memset(&m_DebugInstr, 0, sizeof(m_DebugInstr));
	memset(&instr, 0, sizeof(instr));
	MemoryRead = nullptr;
	MemoryWrite = nullptr;
	m_TotalCyclesPerSec = 0;
	MemoryWrite = (BusWrite)w;
	MemoryRead = (BusRead)r;
	// fill jump table with ILLEGALs
	instr.addr = &MC_Processor6502::Addr_IMP;
	instr.code = &MC_Processor6502::Op_ILLEGAL;
	instr.cycles = 0;
	for (int i = 0; i < 256; i++) {
		m_InstrTable[i] = instr;
	}
	// insert opcodes
	instr.addr = &MC_Processor6502::Addr_IMM;
	instr.code = &MC_Processor6502::Op_ADC;
	instr.cycles = 2;
	m_InstrTable[0x69] = instr;
	instr.addr = &MC_Processor6502::Addr_ABS;
	instr.code = &MC_Processor6502::Op_ADC;
	instr.cycles = 4;
	m_InstrTable[0x6D] = instr;
	instr.addr = &MC_Processor6502::Addr_ZER;
	instr.code = &MC_Processor6502::Op_ADC;
	instr.cycles = 3;
	m_InstrTable[0x65] = instr;
	instr.addr = &MC_Processor6502::Addr_INX;
	instr.code = &MC_Processor6502::Op_ADC;
	instr.cycles = 6;
	m_InstrTable[0x61] = instr;
	instr.addr = &MC_Processor6502::Addr_INY;
	instr.code = &MC_Processor6502::Op_ADC;
	instr.cycles = 6;
	m_InstrTable[0x71] = instr;
	instr.addr = &MC_Processor6502::Addr_ZEX;
	instr.code = &MC_Processor6502::Op_ADC;
	instr.cycles = 4;
	m_InstrTable[0x75] = instr;
	instr.addr = &MC_Processor6502::Addr_ABX;
	instr.code = &MC_Processor6502::Op_ADC;
	instr.cycles = 4;
	m_InstrTable[0x7D] = instr;
	instr.addr = &MC_Processor6502::Addr_ABY;
	instr.code = &MC_Processor6502::Op_ADC;
	instr.cycles = 4;
	m_InstrTable[0x79] = instr;

	instr.addr = &MC_Processor6502::Addr_IMM;
	instr.code = &MC_Processor6502::Op_AND;
	instr.cycles = 2;
	m_InstrTable[0x29] = instr;
	instr.addr = &MC_Processor6502::Addr_ABS;
	instr.code = &MC_Processor6502::Op_AND;
	instr.cycles = 4;
	m_InstrTable[0x2D] = instr;
	instr.addr = &MC_Processor6502::Addr_ZER;
	instr.code = &MC_Processor6502::Op_AND;
	instr.cycles = 3;
	m_InstrTable[0x25] = instr;
	instr.addr = &MC_Processor6502::Addr_INX;
	instr.code = &MC_Processor6502::Op_AND;
	instr.cycles = 6;
	m_InstrTable[0x21] = instr;
	instr.addr = &MC_Processor6502::Addr_INY;
	instr.code = &MC_Processor6502::Op_AND;
	instr.cycles = 5;
	m_InstrTable[0x31] = instr;
	instr.addr = &MC_Processor6502::Addr_ZEX;
	instr.code = &MC_Processor6502::Op_AND;
	instr.cycles = 4;
	m_InstrTable[0x35] = instr;
	instr.addr = &MC_Processor6502::Addr_ABX;
	instr.code = &MC_Processor6502::Op_AND;
	instr.cycles = 4;
	m_InstrTable[0x3D] = instr;
	instr.addr = &MC_Processor6502::Addr_ABY;
	instr.code = &MC_Processor6502::Op_AND;
	instr.cycles = 4;
	m_InstrTable[0x39] = instr;

	instr.addr = &MC_Processor6502::Addr_ABS;
	instr.code = &MC_Processor6502::Op_ASL;
	instr.cycles = 6;
	m_InstrTable[0x0E] = instr;
	instr.addr = &MC_Processor6502::Addr_ZER;
	instr.code = &MC_Processor6502::Op_ASL;
	instr.cycles = 5;
	m_InstrTable[0x06] = instr;
	instr.addr = &MC_Processor6502::Addr_ACC;
	instr.code = &MC_Processor6502::Op_ASL_ACC;
	instr.cycles = 2;
	m_InstrTable[0x0A] = instr;
	instr.addr = &MC_Processor6502::Addr_ZEX;
	instr.code = &MC_Processor6502::Op_ASL;
	instr.cycles = 6;
	m_InstrTable[0x16] = instr;
	instr.addr = &MC_Processor6502::Addr_ABX;
	instr.code = &MC_Processor6502::Op_ASL;
	instr.cycles = 7;
	m_InstrTable[0x1E] = instr;

	instr.addr = &MC_Processor6502::Addr_REL;
	instr.code = &MC_Processor6502::Op_BCC;
	instr.cycles = 2;
	m_InstrTable[0x90] = instr;

	instr.addr = &MC_Processor6502::Addr_REL;
	instr.code = &MC_Processor6502::Op_BCS;
	instr.cycles = 2;
	m_InstrTable[0xB0] = instr;

	instr.addr = &MC_Processor6502::Addr_REL;
	instr.code = &MC_Processor6502::Op_BEQ;
	instr.cycles = 2;
	m_InstrTable[0xF0] = instr;

	instr.addr = &MC_Processor6502::Addr_ABS;
	instr.code = &MC_Processor6502::Op_BIT;
	instr.cycles = 4;
	m_InstrTable[0x2C] = instr;
	instr.addr = &MC_Processor6502::Addr_ZER;
	instr.code = &MC_Processor6502::Op_BIT;
	instr.cycles = 3;
	m_InstrTable[0x24] = instr;

	instr.addr = &MC_Processor6502::Addr_REL;
	instr.code = &MC_Processor6502::Op_BMI;
	instr.cycles = 2;
	m_InstrTable[0x30] = instr;

	instr.addr = &MC_Processor6502::Addr_REL;
	instr.code = &MC_Processor6502::Op_BNE;
	instr.cycles = 2;
	m_InstrTable[0xD0] = instr;

	instr.addr = &MC_Processor6502::Addr_REL;
	instr.code = &MC_Processor6502::Op_BPL;
	instr.cycles = 2;
	m_InstrTable[0x10] = instr;

	instr.addr = &MC_Processor6502::Addr_IMP;
	instr.code = &MC_Processor6502::Op_BRK;
	instr.cycles = 7;
	m_InstrTable[0x00] = instr;

	instr.addr = &MC_Processor6502::Addr_REL;
	instr.code = &MC_Processor6502::Op_BVC;
	instr.cycles = 2;
	m_InstrTable[0x50] = instr;

	instr.addr = &MC_Processor6502::Addr_REL;
	instr.code = &MC_Processor6502::Op_BVS;
	instr.cycles = 2;
	m_InstrTable[0x70] = instr;

	instr.addr = &MC_Processor6502::Addr_IMP;
	instr.code = &MC_Processor6502::Op_CLC;
	instr.cycles = 2;
	m_InstrTable[0x18] = instr;

	instr.addr = &MC_Processor6502::Addr_IMP;
	instr.code = &MC_Processor6502::Op_CLD;
	instr.cycles = 2;
	m_InstrTable[0xD8] = instr;

	instr.addr = &MC_Processor6502::Addr_IMP;
	instr.code = &MC_Processor6502::Op_CLI;
	instr.cycles = 2;
	m_InstrTable[0x58] = instr;

	instr.addr = &MC_Processor6502::Addr_IMP;
	instr.code = &MC_Processor6502::Op_CLV;
	instr.cycles = 2;
	m_InstrTable[0xB8] = instr;

	instr.addr = &MC_Processor6502::Addr_IMM;
	instr.code = &MC_Processor6502::Op_CMP;
	instr.cycles = 2;
	m_InstrTable[0xC9] = instr;
	instr.addr = &MC_Processor6502::Addr_ABS;
	instr.code = &MC_Processor6502::Op_CMP;
	instr.cycles = 4;
	m_InstrTable[0xCD] = instr;
	instr.addr = &MC_Processor6502::Addr_ZER;
	instr.code = &MC_Processor6502::Op_CMP;
	instr.cycles = 3;
	m_InstrTable[0xC5] = instr;
	instr.addr = &MC_Processor6502::Addr_INX;
	instr.code = &MC_Processor6502::Op_CMP;
	instr.cycles = 6;
	m_InstrTable[0xC1] = instr;
	instr.addr = &MC_Processor6502::Addr_INY;
	instr.code = &MC_Processor6502::Op_CMP;
	instr.cycles = 3;
	m_InstrTable[0xD1] = instr;
	instr.addr = &MC_Processor6502::Addr_ZEX;
	instr.code = &MC_Processor6502::Op_CMP;
	instr.cycles = 4;
	m_InstrTable[0xD5] = instr;
	instr.addr = &MC_Processor6502::Addr_ABX;
	instr.code = &MC_Processor6502::Op_CMP;
	instr.cycles = 4;
	m_InstrTable[0xDD] = instr;
	instr.addr = &MC_Processor6502::Addr_ABY;
	instr.code = &MC_Processor6502::Op_CMP;
	instr.cycles = 4;
	m_InstrTable[0xD9] = instr;

	instr.addr = &MC_Processor6502::Addr_IMM;
	instr.code = &MC_Processor6502::Op_CPX;
	instr.cycles = 2;
	m_InstrTable[0xE0] = instr;
	instr.addr = &MC_Processor6502::Addr_ABS;
	instr.code = &MC_Processor6502::Op_CPX;
	instr.cycles = 4;
	m_InstrTable[0xEC] = instr;
	instr.addr = &MC_Processor6502::Addr_ZER;
	instr.code = &MC_Processor6502::Op_CPX;
	instr.cycles = 3;
	m_InstrTable[0xE4] = instr;

	instr.addr = &MC_Processor6502::Addr_IMM;
	instr.code = &MC_Processor6502::Op_CPY;
	instr.cycles = 2;
	m_InstrTable[0xC0] = instr;
	instr.addr = &MC_Processor6502::Addr_ABS;
	instr.code = &MC_Processor6502::Op_CPY;
	instr.cycles = 4;
	m_InstrTable[0xCC] = instr;
	instr.addr = &MC_Processor6502::Addr_ZER;
	instr.code = &MC_Processor6502::Op_CPY;
	instr.cycles = 3;
	m_InstrTable[0xC4] = instr;

	instr.addr = &MC_Processor6502::Addr_ABS;
	instr.code = &MC_Processor6502::Op_DEC;
	instr.cycles = 6;
	m_InstrTable[0xCE] = instr;
	instr.addr = &MC_Processor6502::Addr_ZER;
	instr.code = &MC_Processor6502::Op_DEC;
	instr.cycles = 5;
	m_InstrTable[0xC6] = instr;
	instr.addr = &MC_Processor6502::Addr_ZEX;
	instr.code = &MC_Processor6502::Op_DEC;
	instr.cycles = 6;
	m_InstrTable[0xD6] = instr;
	instr.addr = &MC_Processor6502::Addr_ABX;
	instr.code = &MC_Processor6502::Op_DEC;
	instr.cycles = 7;
	m_InstrTable[0xDE] = instr;

	instr.addr = &MC_Processor6502::Addr_IMP;
	instr.code = &MC_Processor6502::Op_DEX;
	instr.cycles = 2;
	m_InstrTable[0xCA] = instr;

	instr.addr = &MC_Processor6502::Addr_IMP;
	instr.code = &MC_Processor6502::Op_DEY;
	instr.cycles = 2;
	m_InstrTable[0x88] = instr;

	instr.addr = &MC_Processor6502::Addr_IMM;
	instr.code = &MC_Processor6502::Op_EOR;
	instr.cycles = 2;
	m_InstrTable[0x49] = instr;
	instr.addr = &MC_Processor6502::Addr_ABS;
	instr.code = &MC_Processor6502::Op_EOR;
	instr.cycles = 4;
	m_InstrTable[0x4D] = instr;
	instr.addr = &MC_Processor6502::Addr_ZER;
	instr.code = &MC_Processor6502::Op_EOR;
	instr.cycles = 3;
	m_InstrTable[0x45] = instr;
	instr.addr = &MC_Processor6502::Addr_INX;
	instr.code = &MC_Processor6502::Op_EOR;
	instr.cycles = 6;
	m_InstrTable[0x41] = instr;
	instr.addr = &MC_Processor6502::Addr_INY;
	instr.code = &MC_Processor6502::Op_EOR;
	instr.cycles = 5;
	m_InstrTable[0x51] = instr;
	instr.addr = &MC_Processor6502::Addr_ZEX;
	instr.code = &MC_Processor6502::Op_EOR;
	instr.cycles = 4;
	m_InstrTable[0x55] = instr;
	instr.addr = &MC_Processor6502::Addr_ABX;
	instr.code = &MC_Processor6502::Op_EOR;
	instr.cycles = 4;
	m_InstrTable[0x5D] = instr;
	instr.addr = &MC_Processor6502::Addr_ABY;
	instr.code = &MC_Processor6502::Op_EOR;
	instr.cycles = 4;
	m_InstrTable[0x59] = instr;

	instr.addr = &MC_Processor6502::Addr_ABS;
	instr.code = &MC_Processor6502::Op_INC;
	instr.cycles = 6;
	m_InstrTable[0xEE] = instr;
	instr.addr = &MC_Processor6502::Addr_ZER;
	instr.code = &MC_Processor6502::Op_INC;
	instr.cycles = 5;
	m_InstrTable[0xE6] = instr;
	instr.addr = &MC_Processor6502::Addr_ZEX;
	instr.code = &MC_Processor6502::Op_INC;
	instr.cycles = 6;
	m_InstrTable[0xF6] = instr;
	instr.addr = &MC_Processor6502::Addr_ABX;
	instr.code = &MC_Processor6502::Op_INC;
	instr.cycles = 7;
	m_InstrTable[0xFE] = instr;

	instr.addr = &MC_Processor6502::Addr_IMP;
	instr.code = &MC_Processor6502::Op_INX;
	instr.cycles = 2;
	m_InstrTable[0xE8] = instr;

	instr.addr = &MC_Processor6502::Addr_IMP;
	instr.code = &MC_Processor6502::Op_INY;
	instr.cycles = 2;
	m_InstrTable[0xC8] = instr;

	instr.addr = &MC_Processor6502::Addr_ABS;
	instr.code = &MC_Processor6502::Op_JMP;
	instr.cycles = 3;
	m_InstrTable[0x4C] = instr;
	instr.addr = &MC_Processor6502::Addr_ABI;
	instr.code = &MC_Processor6502::Op_JMP;
	instr.cycles = 5;
	m_InstrTable[0x6C] = instr;

	instr.addr = &MC_Processor6502::Addr_ABS;
	instr.code = &MC_Processor6502::Op_JSR;
	instr.cycles = 6;
	m_InstrTable[0x20] = instr;

	instr.addr = &MC_Processor6502::Addr_IMM;
	instr.code = &MC_Processor6502::Op_LDA;
	instr.cycles = 2;
	m_InstrTable[0xA9] = instr;
	instr.addr = &MC_Processor6502::Addr_ABS;
	instr.code = &MC_Processor6502::Op_LDA;
	instr.cycles = 4;
	m_InstrTable[0xAD] = instr;
	instr.addr = &MC_Processor6502::Addr_ZER;
	instr.code = &MC_Processor6502::Op_LDA;
	instr.cycles = 3;
	m_InstrTable[0xA5] = instr;
	instr.addr = &MC_Processor6502::Addr_INX;
	instr.code = &MC_Processor6502::Op_LDA;
	instr.cycles = 6;
	m_InstrTable[0xA1] = instr;
	instr.addr = &MC_Processor6502::Addr_INY;
	instr.code = &MC_Processor6502::Op_LDA;
	instr.cycles = 5;
	m_InstrTable[0xB1] = instr;
	instr.addr = &MC_Processor6502::Addr_ZEX;
	instr.code = &MC_Processor6502::Op_LDA;
	instr.cycles = 4;
	m_InstrTable[0xB5] = instr;
	instr.addr = &MC_Processor6502::Addr_ABX;
	instr.code = &MC_Processor6502::Op_LDA;
	instr.cycles = 4;
	m_InstrTable[0xBD] = instr;
	instr.addr = &MC_Processor6502::Addr_ABY;
	instr.code = &MC_Processor6502::Op_LDA;
	instr.cycles = 4;
	m_InstrTable[0xB9] = instr;

	instr.addr = &MC_Processor6502::Addr_IMM;
	instr.code = &MC_Processor6502::Op_LDX;
	instr.cycles = 2;
	m_InstrTable[0xA2] = instr;
	instr.addr = &MC_Processor6502::Addr_ABS;
	instr.code = &MC_Processor6502::Op_LDX;
	instr.cycles = 4;
	m_InstrTable[0xAE] = instr;
	instr.addr = &MC_Processor6502::Addr_ZER;
	instr.code = &MC_Processor6502::Op_LDX;
	instr.cycles = 3;
	m_InstrTable[0xA6] = instr;
	instr.addr = &MC_Processor6502::Addr_ABY;
	instr.code = &MC_Processor6502::Op_LDX;
	instr.cycles = 4;
	m_InstrTable[0xBE] = instr;
	instr.addr = &MC_Processor6502::Addr_ZEY;
	instr.code = &MC_Processor6502::Op_LDX;
	instr.cycles = 4;
	m_InstrTable[0xB6] = instr;

	instr.addr = &MC_Processor6502::Addr_IMM;
	instr.code = &MC_Processor6502::Op_LDY;
	instr.cycles = 2;
	m_InstrTable[0xA0] = instr;
	instr.addr = &MC_Processor6502::Addr_ABS;
	instr.code = &MC_Processor6502::Op_LDY;
	instr.cycles = 4;
	m_InstrTable[0xAC] = instr;
	instr.addr = &MC_Processor6502::Addr_ZER;
	instr.code = &MC_Processor6502::Op_LDY;
	instr.cycles = 3;
	m_InstrTable[0xA4] = instr;
	instr.addr = &MC_Processor6502::Addr_ZEX;
	instr.code = &MC_Processor6502::Op_LDY;
	instr.cycles = 4;
	m_InstrTable[0xB4] = instr;
	instr.addr = &MC_Processor6502::Addr_ABX;
	instr.code = &MC_Processor6502::Op_LDY;
	instr.cycles = 4;
	m_InstrTable[0xBC] = instr;

	instr.addr = &MC_Processor6502::Addr_ABS;
	instr.code = &MC_Processor6502::Op_LSR;
	instr.cycles = 6;
	m_InstrTable[0x4E] = instr;
	instr.addr = &MC_Processor6502::Addr_ZER;
	instr.code = &MC_Processor6502::Op_LSR;
	instr.cycles = 5;
	m_InstrTable[0x46] = instr;
	instr.addr = &MC_Processor6502::Addr_ACC;
	instr.code = &MC_Processor6502::Op_LSR_ACC;
	instr.cycles = 2;
	m_InstrTable[0x4A] = instr;
	instr.addr = &MC_Processor6502::Addr_ZEX;
	instr.code = &MC_Processor6502::Op_LSR;
	instr.cycles = 6;
	m_InstrTable[0x56] = instr;
	instr.addr = &MC_Processor6502::Addr_ABX;
	instr.code = &MC_Processor6502::Op_LSR;
	instr.cycles = 7;
	m_InstrTable[0x5E] = instr;

	instr.addr = &MC_Processor6502::Addr_IMP;
	instr.code = &MC_Processor6502::Op_NOP;
	instr.cycles = 2;
	m_InstrTable[0xEA] = instr;

	instr.addr = &MC_Processor6502::Addr_IMM;
	instr.code = &MC_Processor6502::Op_ORA;
	instr.cycles = 2;
	m_InstrTable[0x09] = instr;
	instr.addr = &MC_Processor6502::Addr_ABS;
	instr.code = &MC_Processor6502::Op_ORA;
	instr.cycles = 4;
	m_InstrTable[0x0D] = instr;
	instr.addr = &MC_Processor6502::Addr_ZER;
	instr.code = &MC_Processor6502::Op_ORA;
	instr.cycles = 3;
	m_InstrTable[0x05] = instr;
	instr.addr = &MC_Processor6502::Addr_INX;
	instr.code = &MC_Processor6502::Op_ORA;
	instr.cycles = 6;
	m_InstrTable[0x01] = instr;
	instr.addr = &MC_Processor6502::Addr_INY;
	instr.code = &MC_Processor6502::Op_ORA;
	instr.cycles = 5;
	m_InstrTable[0x11] = instr;
	instr.addr = &MC_Processor6502::Addr_ZEX;
	instr.code = &MC_Processor6502::Op_ORA;
	instr.cycles = 4;
	m_InstrTable[0x15] = instr;
	instr.addr = &MC_Processor6502::Addr_ABX;
	instr.code = &MC_Processor6502::Op_ORA;
	instr.cycles = 4;
	m_InstrTable[0x1D] = instr;
	instr.addr = &MC_Processor6502::Addr_ABY;
	instr.code = &MC_Processor6502::Op_ORA;
	instr.cycles = 4;
	m_InstrTable[0x19] = instr;

	instr.addr = &MC_Processor6502::Addr_IMP;
	instr.code = &MC_Processor6502::Op_PHA;
	instr.cycles = 3;
	m_InstrTable[0x48] = instr;

	instr.addr = &MC_Processor6502::Addr_IMP;
	instr.code = &MC_Processor6502::Op_PHP;
	instr.cycles = 3;
	m_InstrTable[0x08] = instr;

	instr.addr = &MC_Processor6502::Addr_IMP;
	instr.code = &MC_Processor6502::Op_PLA;
	instr.cycles = 4;
	m_InstrTable[0x68] = instr;

	instr.addr = &MC_Processor6502::Addr_IMP;
	instr.code = &MC_Processor6502::Op_PLP;
	instr.cycles = 4;
	m_InstrTable[0x28] = instr;

	instr.addr = &MC_Processor6502::Addr_ABS;
	instr.code = &MC_Processor6502::Op_ROL;
	instr.cycles = 6;
	m_InstrTable[0x2E] = instr;
	instr.addr = &MC_Processor6502::Addr_ZER;
	instr.code = &MC_Processor6502::Op_ROL;
	instr.cycles = 5;
	m_InstrTable[0x26] = instr;
	instr.addr = &MC_Processor6502::Addr_ACC;
	instr.code = &MC_Processor6502::Op_ROL_ACC;
	instr.cycles = 2;
	m_InstrTable[0x2A] = instr;
	instr.addr = &MC_Processor6502::Addr_ZEX;
	instr.code = &MC_Processor6502::Op_ROL;
	instr.cycles = 6;
	m_InstrTable[0x36] = instr;
	instr.addr = &MC_Processor6502::Addr_ABX;
	instr.code = &MC_Processor6502::Op_ROL;
	instr.cycles = 7;
	m_InstrTable[0x3E] = instr;

	instr.addr = &MC_Processor6502::Addr_ABS;
	instr.code = &MC_Processor6502::Op_ROR;
	instr.cycles = 6;
	m_InstrTable[0x6E] = instr;
	instr.addr = &MC_Processor6502::Addr_ZER;
	instr.code = &MC_Processor6502::Op_ROR;
	instr.cycles = 5;
	m_InstrTable[0x66] = instr;
	instr.addr = &MC_Processor6502::Addr_ACC;
	instr.code = &MC_Processor6502::Op_ROR_ACC;
	instr.cycles = 2;
	m_InstrTable[0x6A] = instr;
	instr.addr = &MC_Processor6502::Addr_ZEX;
	instr.code = &MC_Processor6502::Op_ROR;
	instr.cycles = 6;
	m_InstrTable[0x76] = instr;
	instr.addr = &MC_Processor6502::Addr_ABX;
	instr.code = &MC_Processor6502::Op_ROR;
	instr.cycles = 7;
	m_InstrTable[0x7E] = instr;

	instr.addr = &MC_Processor6502::Addr_IMP;
	instr.code = &MC_Processor6502::Op_RTI;
	instr.cycles = 6;
	m_InstrTable[0x40] = instr;

	instr.addr = &MC_Processor6502::Addr_IMP;
	instr.code = &MC_Processor6502::Op_RTS;
	instr.cycles = 6;
	m_InstrTable[0x60] = instr;

	instr.addr = &MC_Processor6502::Addr_IMM;
	instr.code = &MC_Processor6502::Op_SBC;
	instr.cycles = 2;
	m_InstrTable[0xE9] = instr;
	instr.addr = &MC_Processor6502::Addr_ABS;
	instr.code = &MC_Processor6502::Op_SBC;
	instr.cycles = 4;
	m_InstrTable[0xED] = instr;
	instr.addr = &MC_Processor6502::Addr_ZER;
	instr.code = &MC_Processor6502::Op_SBC;
	instr.cycles = 3;
	m_InstrTable[0xE5] = instr;
	instr.addr = &MC_Processor6502::Addr_INX;
	instr.code = &MC_Processor6502::Op_SBC;
	instr.cycles = 6;
	m_InstrTable[0xE1] = instr;
	instr.addr = &MC_Processor6502::Addr_INY;
	instr.code = &MC_Processor6502::Op_SBC;
	instr.cycles = 5;
	m_InstrTable[0xF1] = instr;
	instr.addr = &MC_Processor6502::Addr_ZEX;
	instr.code = &MC_Processor6502::Op_SBC;
	instr.cycles = 4;
	m_InstrTable[0xF5] = instr;
	instr.addr = &MC_Processor6502::Addr_ABX;
	instr.code = &MC_Processor6502::Op_SBC;
	instr.cycles = 4;
	m_InstrTable[0xFD] = instr;
	instr.addr = &MC_Processor6502::Addr_ABY;
	instr.code = &MC_Processor6502::Op_SBC;
	instr.cycles = 4;
	m_InstrTable[0xF9] = instr;

	instr.addr = &MC_Processor6502::Addr_IMP;
	instr.code = &MC_Processor6502::Op_SEC;
	instr.cycles = 2;
	m_InstrTable[0x38] = instr;

	instr.addr = &MC_Processor6502::Addr_IMP;
	instr.code = &MC_Processor6502::Op_SED;
	instr.cycles = 2;
	m_InstrTable[0xF8] = instr;

	instr.addr = &MC_Processor6502::Addr_IMP;
	instr.code = &MC_Processor6502::Op_SEI;
	instr.cycles = 2;
	m_InstrTable[0x78] = instr;

	instr.addr = &MC_Processor6502::Addr_ABS;
	instr.code = &MC_Processor6502::Op_STA;
	instr.cycles = 4;
	m_InstrTable[0x8D] = instr;
	instr.addr = &MC_Processor6502::Addr_ZER;
	instr.code = &MC_Processor6502::Op_STA;
	instr.cycles = 3;
	m_InstrTable[0x85] = instr;
	instr.addr = &MC_Processor6502::Addr_INX;
	instr.code = &MC_Processor6502::Op_STA;
	instr.cycles = 6;
	m_InstrTable[0x81] = instr;
	instr.addr = &MC_Processor6502::Addr_INY;
	instr.code = &MC_Processor6502::Op_STA;
	instr.cycles = 6;
	m_InstrTable[0x91] = instr;
	instr.addr = &MC_Processor6502::Addr_ZEX;
	instr.code = &MC_Processor6502::Op_STA;
	instr.cycles = 4;
	m_InstrTable[0x95] = instr;
	instr.addr = &MC_Processor6502::Addr_ABX;
	instr.code = &MC_Processor6502::Op_STA;
	instr.cycles = 5;
	m_InstrTable[0x9D] = instr;
	instr.addr = &MC_Processor6502::Addr_ABY;
	instr.code = &MC_Processor6502::Op_STA;
	instr.cycles = 5;
	m_InstrTable[0x99] = instr;

	instr.addr = &MC_Processor6502::Addr_ABS;
	instr.code = &MC_Processor6502::Op_STX;
	instr.cycles = 4;
	m_InstrTable[0x8E] = instr;
	instr.addr = &MC_Processor6502::Addr_ZER;
	instr.code = &MC_Processor6502::Op_STX;
	instr.cycles = 3;
	m_InstrTable[0x86] = instr;
	instr.addr = &MC_Processor6502::Addr_ZEY;
	instr.code = &MC_Processor6502::Op_STX;
	instr.cycles = 4;
	m_InstrTable[0x96] = instr;

	instr.addr = &MC_Processor6502::Addr_ABS;
	instr.code = &MC_Processor6502::Op_STY;
	instr.cycles = 4;
	m_InstrTable[0x8C] = instr;
	instr.addr = &MC_Processor6502::Addr_ZER;
	instr.code = &MC_Processor6502::Op_STY;
	instr.cycles = 3;
	m_InstrTable[0x84] = instr;
	instr.addr = &MC_Processor6502::Addr_ZEX;
	instr.code = &MC_Processor6502::Op_STY;
	instr.cycles = 4;
	m_InstrTable[0x94] = instr;

	instr.addr = &MC_Processor6502::Addr_IMP;
	instr.code = &MC_Processor6502::Op_TAX;
	instr.cycles = 2;
	m_InstrTable[0xAA] = instr;

	instr.addr = &MC_Processor6502::Addr_IMP;
	instr.code = &MC_Processor6502::Op_TAY;
	instr.cycles = 2;
	m_InstrTable[0xA8] = instr;

	instr.addr = &MC_Processor6502::Addr_IMP;
	instr.code = &MC_Processor6502::Op_TSX;
	instr.cycles = 2;
	m_InstrTable[0xBA] = instr;

	instr.addr = &MC_Processor6502::Addr_IMP;
	instr.code = &MC_Processor6502::Op_TXA;
	instr.cycles = 2;
	m_InstrTable[0x8A] = instr;

	instr.addr = &MC_Processor6502::Addr_IMP;
	instr.code = &MC_Processor6502::Op_TXS;
	instr.cycles = 2;
	m_InstrTable[0x9A] = instr;

	instr.addr = &MC_Processor6502::Addr_IMP;
	instr.code = &MC_Processor6502::Op_TYA;
	instr.cycles = 2;
	m_InstrTable[0x98] = instr;

	return;
}
//-Public----------------------------------------------------------------------
// Name: ~MC_Processor6502()
// Desc: ~MC_Processor6502 Destruction class
//-----------------------------------------------------------------------------
MC_Processor6502::~MC_Processor6502()
{

}
//-Public----------------------------------------------------------------------
// Name: NMI()
//-----------------------------------------------------------------------------
void MC_Processor6502::NMI()
{
	SET_BREAK(0);
	StackPush((m_registers.pc >> 8) & 0xFF);
	StackPush(m_registers.pc & 0xFF);
	StackPush(m_registers.status);
	SET_INTERRUPT(1);
	m_registers.pc = (MemoryRead(s_nmiVectorH) << 8) + MemoryRead(s_nmiVectorL);
	return;
}
//-Public----------------------------------------------------------------------
// Name: IRQ()
//-----------------------------------------------------------------------------
void MC_Processor6502::IRQ()
{
	if (!IF_INTERRUPT()) {
		SET_BREAK(0);
		StackPush((m_registers.pc >> 8) & 0xFF);
		StackPush(m_registers.pc & 0xFF);
		StackPush(m_registers.status);
		SET_INTERRUPT(1);
		m_registers.pc = (MemoryRead(s_irqVectorH) << 8) + MemoryRead(s_irqVectorL);
	}
	return;
}
//-Public----------------------------------------------------------------------
// Name: Reset()
//-----------------------------------------------------------------------------
void MC_Processor6502::Reset()
{
	m_registers.A = 0x00;
	m_registers.Y = 0x00;
	m_registers.X = 0x00;
	m_registers.pc = (MemoryRead(s_rstVectorH) << 8) + MemoryRead(s_rstVectorL);	// load PC from reset vector
	m_registers.sp = 0xFD;
	m_registers.status |= FLAGCONSTANT;
	m_registers.IllegalOpcode = false;
	return;
}
//-Public----------------------------------------------------------------------
// Name: SetPC(uint16_t PC)
//-----------------------------------------------------------------------------
void MC_Processor6502::SetPC(uint16_t PC)
{
	m_registers.pc = PC;
}
//-Public----------------------------------------------------------------------
// Name: RunOneOp()
//-----------------------------------------------------------------------------
bool MC_Processor6502::RunOneOp()
{
	uint8_t opcode;
	Instr instr;

	if (!m_registers.IllegalOpcode) {
		m_DebugInstr.pc = m_registers.pc;
		opcode = MemoryRead(m_registers.pc++);									// fetch
		m_DebugInstr.opcode = opcode;
		m_DebugInstr.Updated = true;
		instr = m_InstrTable[opcode];											// decode
		m_TotalCyclesPerSec += instr.cycles;
		Exec(instr);															// execute
		m_CrashDump.DebugInstr[m_CrashDump.Index] = m_DebugInstr;
		m_CrashDump.Registers[m_CrashDump.Index] = m_registers;
		m_CrashDump.Index++;
		if (m_CrashDump.Index >= CrashDumpSize) {
			m_CrashDump.Index = 0;
		}
	} else {
		printf("IllegalOpcode\r\n");
		m_registers.IllegalOpcode = false;
	}
	return m_registers.IllegalOpcode;
}
//-Public----------------------------------------------------------------------
// Name: RunCode(int32_t cyclesRemaining, uint64_t& cycleCount, CycleMethod cycleMethod)
//-----------------------------------------------------------------------------
void MC_Processor6502::RunCode(int32_t cyclesRemaining, uint64_t& cycleCount, CycleMethod cycleMethod)
{
	uint8_t opcode;
	Instr instr;

	while (cyclesRemaining > 0 && !m_registers.IllegalOpcode) {
		opcode = MemoryRead(m_registers.pc++);									// fetch
		instr = m_InstrTable[opcode];											// decode
		Exec(instr);															// execute
		cycleCount += instr.cycles;
		cyclesRemaining -= cycleMethod == CYCLE_COUNT ? instr.cycles : 1;		// cycleMethod == INST_COUNT
	}
}
//-Public----------------------------------------------------------------------
// MC_Processor6502::GetRegisters()
//-----------------------------------------------------------------------------
Registers6502 MC_Processor6502::GetRegisters()
{
	return m_registers;
}












//*****************************************************************************  
// Protected Code
//*****************************************************************************

//-Protected-------------------------------------------------------------------
// Name: Addr_ACC()
//-----------------------------------------------------------------------------
uint16_t MC_Processor6502::Addr_ACC()
{
	return 0;																	// not used
}
//-Protected-------------------------------------------------------------------
// Name: Addr_IMM()
//-----------------------------------------------------------------------------
uint16_t MC_Processor6502::Addr_IMM()
{
	return m_registers.pc++;
}
//-Protected-------------------------------------------------------------------
// Name: Addr_ABS()
//-----------------------------------------------------------------------------
uint16_t MC_Processor6502::Addr_ABS()
{
	uint16_t addrL;
	uint16_t addrH;
	uint16_t addr;

	addrL = MemoryRead(m_registers.pc++);
	addrH = MemoryRead(m_registers.pc++);
	addr = addrL + (addrH << 8);
	return addr;
}
//-Protected-------------------------------------------------------------------
// Name: Addr_ZER()
//-----------------------------------------------------------------------------
uint16_t MC_Processor6502::Addr_ZER()
{
	return MemoryRead(m_registers.pc++);
}
//-Protected-------------------------------------------------------------------
// Name: Addr_IMP()
//-----------------------------------------------------------------------------
uint16_t MC_Processor6502::Addr_IMP()
{
	return 0;																	// not used
}
//-Protected-------------------------------------------------------------------
// Name: Addr_REL()
//-----------------------------------------------------------------------------
uint16_t MC_Processor6502::Addr_REL()
{
	uint16_t offset;
	uint16_t addr;

	offset = (uint16_t)MemoryRead(m_registers.pc++);
	if (offset & 0x80) 
		offset |= 0xFF00;

	addr = m_registers.pc + (int16_t)offset;
	return addr;
}
//-Protected-------------------------------------------------------------------
// Name: Addr_ABI()
//-----------------------------------------------------------------------------
uint16_t MC_Processor6502::Addr_ABI()
{
	uint16_t addrL;
	uint16_t addrH;
	uint16_t effL;
	uint16_t effH;
	uint16_t abs;
	uint16_t addr;

	addrL = MemoryRead(m_registers.pc++);
	addrH = MemoryRead(m_registers.pc++);
	abs = (addrH << 8) | addrL;
	effL = MemoryRead(abs);
#ifndef CMOS_INDIRECT_JMP_FIX
	effH = MemoryRead((abs & 0xFF00) + ((abs + 1) & 0x00FF));
#else
	effH = Read(abs + 1);
#endif
	addr = effL + 0x100 * effH;
	return addr;
}
//-Protected-------------------------------------------------------------------
// Name: Addr_ZEX()
//-----------------------------------------------------------------------------
uint16_t MC_Processor6502::Addr_ZEX()
{
	uint16_t addr = (MemoryRead(m_registers.pc++) + m_registers.X) % 256;
	return addr;
}
//-Protected-------------------------------------------------------------------
// Name: Addr_ZEY()
//-----------------------------------------------------------------------------
uint16_t MC_Processor6502::Addr_ZEY()
{
	uint16_t addr = (MemoryRead(m_registers.pc++) + m_registers.Y) % 256;
	return addr;
}
//-Protected-------------------------------------------------------------------
// Name: Addr_ABX()
//-----------------------------------------------------------------------------
uint16_t MC_Processor6502::Addr_ABX()
{
	uint16_t addr;
	uint16_t addrL;
	uint16_t addrH;

	addrL = MemoryRead(m_registers.pc++);
	addrH = MemoryRead(m_registers.pc++);
	addr = addrL + (addrH << 8) + m_registers.X;
	return addr;
}
//-Protected-------------------------------------------------------------------
// Name: Addr_ABY()
//-----------------------------------------------------------------------------
uint16_t MC_Processor6502::Addr_ABY()
{
	uint16_t addr;
	uint16_t addrL;
	uint16_t addrH;

	addrL = MemoryRead(m_registers.pc++);
	addrH = MemoryRead(m_registers.pc++);
	addr = addrL + (addrH << 8) + m_registers.Y;
	return addr;
}
//-Protected-------------------------------------------------------------------
// Name: Addr_INX()
//-----------------------------------------------------------------------------
uint16_t MC_Processor6502::Addr_INX()
{
	uint16_t zeroL;
	uint16_t zeroH;
	uint16_t addr;

	zeroL = (MemoryRead(m_registers.pc++) + m_registers.X) % 256;
	zeroH = (zeroL + 1) % 256;
	addr = MemoryRead(zeroL) + (MemoryRead(zeroH) << 8);
	return addr;
}
//-Protected-------------------------------------------------------------------
// Name: Addr_INY()
//-----------------------------------------------------------------------------
uint16_t MC_Processor6502::Addr_INY()
{
	uint16_t zeroL;
	uint16_t zeroH;
	uint16_t addr;

	zeroL = MemoryRead(m_registers.pc++);
	zeroH = (zeroL + 1) % 256;
	addr = MemoryRead(zeroL) + (MemoryRead(zeroH) << 8) + m_registers.Y;
	return addr;
}
//-Protected-------------------------------------------------------------------
// Name: StackPush(uint8_t byte)
//-----------------------------------------------------------------------------
void MC_Processor6502::StackPush(uint8_t byte)
{
	MemoryWrite(0x0100 + m_registers.sp, byte);
	if (m_registers.sp == 0x00)
		m_registers.sp = 0xFF;

	else m_registers.sp--;
}
//-Protected-------------------------------------------------------------------
// Name: StackPop()
//-----------------------------------------------------------------------------
uint8_t MC_Processor6502::StackPop()
{
	if (m_registers.sp == 0xFF)
		m_registers.sp = 0x00;
	else
		m_registers.sp++;

	return MemoryRead(0x0100 + m_registers.sp);
}
//-Protected-------------------------------------------------------------------
// Name: Exec(Instr i)
//-----------------------------------------------------------------------------
void MC_Processor6502::Exec(Instr i)
{
	uint16_t src = (this->*i.addr)();
	m_DebugInstr.address = src;
	m_DebugInstr.Cpu = i;
	(this->*i.code)(src);
}
//-Protected-------------------------------------------------------------------
// Name: Op_ILLEGAL(uint16_t src)
//-----------------------------------------------------------------------------
void MC_Processor6502::Op_ILLEGAL(uint16_t src)
{
	m_registers.IllegalOpcode = true;
}
//-Protected-------------------------------------------------------------------
// Name: Op_ADC(uint16_t src)
//-----------------------------------------------------------------------------
void MC_Processor6502::Op_ADC(uint16_t src)
{
	uint8_t m = MemoryRead(src);
	unsigned int tmp = m + m_registers.A + (IF_CARRY() ? 1 : 0);
	SET_ZERO(!(tmp & 0xFF));
	if (IF_DECIMAL()) {
		if (((m_registers.A & 0xF) + (m & 0xF) + (IF_CARRY() ? 1 : 0)) > 9)
			tmp += 6;

		SET_NEGATIVE(tmp & 0x80);
		SET_OVERFLOW(!((m_registers.A ^ m) & 0x80) && ((m_registers.A ^ tmp) & 0x80));
		if (tmp > 0x99) {
			tmp += 96;
		}
		SET_CARRY(tmp > 0x99);
	} else {
		SET_NEGATIVE(tmp & 0x80);
		SET_OVERFLOW(!((m_registers.A ^ m) & 0x80) && ((m_registers.A ^ tmp) & 0x80));
		SET_CARRY(tmp > 0xFF);
	}
	m_registers.A = tmp & 0xFF;
	return;
}
//-Protected-------------------------------------------------------------------
// Name: Op_AND(uint16_t src)
//-----------------------------------------------------------------------------
void MC_Processor6502::Op_AND(uint16_t src)
{
	uint8_t m = MemoryRead(src);
	uint8_t res = m & m_registers.A;
	SET_NEGATIVE(res & 0x80);
	SET_ZERO(!res);
	m_registers.A = res;
	return;
}
//-Protected-------------------------------------------------------------------
// Name: Op_ASL(uint16_t src)
//-----------------------------------------------------------------------------
void MC_Processor6502::Op_ASL(uint16_t src)
{
	uint8_t m = MemoryRead(src);
	SET_CARRY(m & 0x80);
	m <<= 1;
	m &= 0xFF;
	SET_NEGATIVE(m & 0x80);
	SET_ZERO(!m);
	MemoryWrite(src, m);
	return;
}
//-Protected-------------------------------------------------------------------
// Name: Op_ASL_ACC(uint16_t src)
//-----------------------------------------------------------------------------
void MC_Processor6502::Op_ASL_ACC(uint16_t src)
{
	uint8_t m = m_registers.A;
	SET_CARRY(m & 0x80);
	m <<= 1;
	m &= 0xFF;
	SET_NEGATIVE(m & 0x80);
	SET_ZERO(!m);
	m_registers.A = m;
	return;
}
//-Protected-------------------------------------------------------------------
// Name: Op_BCC(uint16_t src)
//-----------------------------------------------------------------------------
void MC_Processor6502::Op_BCC(uint16_t src)
{
	if (!IF_CARRY()) {
		m_registers.pc = src;
	}
	return;
}
//-Protected-------------------------------------------------------------------
// Name: Op_BCS(uint16_t src)
//-----------------------------------------------------------------------------
void MC_Processor6502::Op_BCS(uint16_t src)
{
	if (IF_CARRY())	{
		m_registers.pc = src;
	}
	return;
}
//-Protected-------------------------------------------------------------------
// Name: Op_BEQ(uint16_t src)
//-----------------------------------------------------------------------------
void MC_Processor6502::Op_BEQ(uint16_t src)
{
	if (IF_ZERO()) {
		m_registers.pc = src;
	}
	return;
}
//-Protected-------------------------------------------------------------------
// Name: Op_BIT(uint16_t src)
//-----------------------------------------------------------------------------
void MC_Processor6502::Op_BIT(uint16_t src)
{
	uint8_t m = MemoryRead(src);
	uint8_t res = m & m_registers.A;
	SET_NEGATIVE(res & 0x80);
	m_registers.status = (m_registers.status & 0x3F) | (uint8_t)(m & 0xC0);
	SET_ZERO(!res);
	return;
}
//-Protected-------------------------------------------------------------------
// Name: Op_BMI(uint16_t src)
//-----------------------------------------------------------------------------
void MC_Processor6502::Op_BMI(uint16_t src)
{
	if (IF_NEGATIVE()) {
		m_registers.pc = src;
	}
	return;
}
//-Protected-------------------------------------------------------------------
// Name Op_BNE(uint16_t src)
//-----------------------------------------------------------------------------
void MC_Processor6502::Op_BNE(uint16_t src)
{
	if (!IF_ZERO())	{
		m_registers.pc = src;
	}
	return;
}
//-Protected-------------------------------------------------------------------
// Name: Op_BPL(uint16_t src)
//-----------------------------------------------------------------------------
void MC_Processor6502::Op_BPL(uint16_t src)
{
	if (!IF_NEGATIVE())	{
		m_registers.pc = src;
	}
	return;
}
//-Protected-------------------------------------------------------------------
// Name: Op_BRK(uint16_t src)
//-----------------------------------------------------------------------------
void MC_Processor6502::Op_BRK(uint16_t src)
{
	m_registers.pc++;
	StackPush((m_registers.pc >> 8) & 0xFF);
	StackPush(m_registers.pc & 0xFF);
	StackPush(m_registers.status | FLAGBREAK);
	SET_INTERRUPT(1);
	m_registers.pc = (MemoryRead(s_irqVectorH) << 8) + MemoryRead(s_irqVectorL);
	return;
}
//-Protected-------------------------------------------------------------------
// Name: Op_BVC(uint16_t src)
//-----------------------------------------------------------------------------
void MC_Processor6502::Op_BVC(uint16_t src)
{
	if (!IF_OVERFLOW()) {
		m_registers.pc = src;
	}
	return;
}
//-Protected-------------------------------------------------------------------
// Name: Op_BVS(uint16_t src)
//-----------------------------------------------------------------------------
void MC_Processor6502::Op_BVS(uint16_t src)
{
	if (IF_OVERFLOW()) {
		m_registers.pc = src;
	}
	return;
}
//-Protected-------------------------------------------------------------------
// Name: Op_CLC(uint16_t src)
//-----------------------------------------------------------------------------
void MC_Processor6502::Op_CLC(uint16_t src)
{
	SET_CARRY(0);
	return;
}
//-Protected-------------------------------------------------------------------
// Name: Op_CLD(uint16_t src)
//-----------------------------------------------------------------------------
void MC_Processor6502::Op_CLD(uint16_t src)
{
	SET_DECIMAL(0);
	return;
}
//-Protected-------------------------------------------------------------------
// Name: Op_CLI(uint16_t src)
//-----------------------------------------------------------------------------
void MC_Processor6502::Op_CLI(uint16_t src)
{
	SET_INTERRUPT(0);
	return;
}
//-Protected-------------------------------------------------------------------
// Name: Op_CLV(uint16_t src)
//-----------------------------------------------------------------------------
void MC_Processor6502::Op_CLV(uint16_t src)
{
	SET_OVERFLOW(0);
	return;
}
//-Protected-------------------------------------------------------------------
// Name: Op_CMP(uint16_t src)
//-----------------------------------------------------------------------------
void MC_Processor6502::Op_CMP(uint16_t src)
{
	unsigned int tmp = m_registers.A - MemoryRead(src);
	SET_CARRY(tmp < 0x100);
	SET_NEGATIVE(tmp & 0x80);
	SET_ZERO(!(tmp & 0xFF));
	return;
}
//-Protected-------------------------------------------------------------------
// Name: Op_CPX(uint16_t src)
//-----------------------------------------------------------------------------
void MC_Processor6502::Op_CPX(uint16_t src)
{
	unsigned int tmp = m_registers.X - MemoryRead(src);
	SET_CARRY(tmp < 0x100);
	SET_NEGATIVE(tmp & 0x80);
	SET_ZERO(!(tmp & 0xFF));
	return;
}
//-Protected-------------------------------------------------------------------
// Name: Op_CPY(uint16_t src)
//-----------------------------------------------------------------------------
void MC_Processor6502::Op_CPY(uint16_t src)
{
	unsigned int tmp = m_registers.Y - MemoryRead(src);
	SET_CARRY(tmp < 0x100);
	SET_NEGATIVE(tmp & 0x80);
	SET_ZERO(!(tmp & 0xFF));
	return;
}
//-Protected-------------------------------------------------------------------
// Name: Op_DEC(uint16_t src)
//-----------------------------------------------------------------------------
void MC_Processor6502::Op_DEC(uint16_t src)
{
	uint8_t m = MemoryRead(src);
	m = (m - 1) % 256;
	SET_NEGATIVE(m & 0x80);
	SET_ZERO(!m);
	MemoryWrite(src, m);
	return;
}
//-Protected-------------------------------------------------------------------
// Name: Op_DEX(uint16_t src)
//-----------------------------------------------------------------------------
void MC_Processor6502::Op_DEX(uint16_t src)
{
	uint8_t m = m_registers.X;
	m = (m - 1) % 256;
	SET_NEGATIVE(m & 0x80);
	SET_ZERO(!m);
	m_registers.X = m;
	return;
}
//-Protected-------------------------------------------------------------------
// Name: Op_DEY(uint16_t src)
//-----------------------------------------------------------------------------
void MC_Processor6502::Op_DEY(uint16_t src)
{
	uint8_t m = m_registers.Y;
	m = (m - 1) % 256;
	SET_NEGATIVE(m & 0x80);
	SET_ZERO(!m);
	m_registers.Y = m;
	return;
}
//-Protected-------------------------------------------------------------------
// Name: Op_EOR(uint16_t src)
//-----------------------------------------------------------------------------
void MC_Processor6502::Op_EOR(uint16_t src)
{
	uint8_t m = MemoryRead(src);
	m = m_registers.A ^ m;
	SET_NEGATIVE(m & 0x80);
	SET_ZERO(!m);
	m_registers.A = m;
}
//-Protected-------------------------------------------------------------------
// Name: Op_INC(uint16_t src)
//-----------------------------------------------------------------------------
void MC_Processor6502::Op_INC(uint16_t src)
{
	uint8_t m = MemoryRead(src);
	m = (m + 1) % 256;
	SET_NEGATIVE(m & 0x80);
	SET_ZERO(!m);
	MemoryWrite(src, m);
}
//-Protected-------------------------------------------------------------------
// Name: Op_INX(uint16_t src)
//-----------------------------------------------------------------------------
void MC_Processor6502::Op_INX(uint16_t src)
{
	uint8_t m = m_registers.X;
	m = (m + 1) % 256;
	SET_NEGATIVE(m & 0x80);
	SET_ZERO(!m);
	m_registers.X = m;
}
//-Protected-------------------------------------------------------------------
// Name: Op_INY(uint16_t src)
//-----------------------------------------------------------------------------
void MC_Processor6502::Op_INY(uint16_t src)
{
	uint8_t m = m_registers.Y;
	m = (m + 1) % 256;
	SET_NEGATIVE(m & 0x80);
	SET_ZERO(!m);
	m_registers.Y = m;
}
//-Protected-------------------------------------------------------------------
// Name: Op_JMP(uint16_t src)
//-----------------------------------------------------------------------------
void MC_Processor6502::Op_JMP(uint16_t src)
{
	m_registers.pc = src;
}
//-Protected-------------------------------------------------------------------
// Name: Op_JSR(uint16_t src)
//-----------------------------------------------------------------------------
void MC_Processor6502::Op_JSR(uint16_t src)
{
	m_registers.pc--;
	StackPush((m_registers.pc >> 8) & 0xFF);
	StackPush(m_registers.pc & 0xFF);
	m_registers.pc = src;
}
//-Protected-------------------------------------------------------------------
// Name: Op_LDA(uint16_t src)
//-----------------------------------------------------------------------------
void MC_Processor6502::Op_LDA(uint16_t src)
{
	uint8_t m = MemoryRead(src);
	SET_NEGATIVE(m & 0x80);
	SET_ZERO(!m);
	m_registers.A = m;
}
//-Protected-------------------------------------------------------------------
// Name: Op_LDX(uint16_t src)
//-----------------------------------------------------------------------------
void MC_Processor6502::Op_LDX(uint16_t src)
{
	uint8_t m = MemoryRead(src);
	SET_NEGATIVE(m & 0x80);
	SET_ZERO(!m);
	m_registers.X = m;
}
//-Protected-------------------------------------------------------------------
// Name: Op_LDY(uint16_t src)
//-----------------------------------------------------------------------------
void MC_Processor6502::Op_LDY(uint16_t src)
{
	uint8_t m = MemoryRead(src);
	SET_NEGATIVE(m & 0x80);
	SET_ZERO(!m);
	m_registers.Y = m;
}
//-Protected-------------------------------------------------------------------
// Name: Op_LSR(uint16_t src)
//-----------------------------------------------------------------------------
void MC_Processor6502::Op_LSR(uint16_t src)
{
	uint8_t m = MemoryRead(src);
	SET_CARRY(m & 0x01);
	m >>= 1;
	SET_NEGATIVE(0);
	SET_ZERO(!m);
	MemoryWrite(src, m);
}
//-Protected-------------------------------------------------------------------
// Name: Op_LSR_ACC(uint16_t src)
//-----------------------------------------------------------------------------
void MC_Processor6502::Op_LSR_ACC(uint16_t src)
{
	uint8_t m = m_registers.A;
	SET_CARRY(m & 0x01);
	m >>= 1;
	SET_NEGATIVE(0);
	SET_ZERO(!m);
	m_registers.A = m;
}
//-Protected-------------------------------------------------------------------
// Name: Op_NOP(uint16_t src)
//-----------------------------------------------------------------------------
void MC_Processor6502::Op_NOP(uint16_t src)
{
	return;
}
//-Protected-------------------------------------------------------------------
// Name: Op_ORA(uint16_t src)
//-----------------------------------------------------------------------------
void MC_Processor6502::Op_ORA(uint16_t src)
{
	uint8_t m = MemoryRead(src);
	m = m_registers.A | m;
	SET_NEGATIVE(m & 0x80);
	SET_ZERO(!m);
	m_registers.A = m;
}
//-Protected-------------------------------------------------------------------
// Name: Op_PHA(uint16_t src)
//-----------------------------------------------------------------------------
void MC_Processor6502::Op_PHA(uint16_t src)
{
	StackPush(m_registers.A);
	return;
}
//-Protected-------------------------------------------------------------------
// Name: Op_PHP(uint16_t src)
//-----------------------------------------------------------------------------
void MC_Processor6502::Op_PHP(uint16_t src)
{
	StackPush(m_registers.status | FLAGBREAK);
	return;
}
//-Protected-------------------------------------------------------------------
// Name: Op_PLA(uint16_t src)
//-----------------------------------------------------------------------------
void MC_Processor6502::Op_PLA(uint16_t src)
{
	m_registers.A = StackPop();
	SET_NEGATIVE(m_registers.A & 0x80);
	SET_ZERO(!m_registers.A);
	return;
}
//-Protected-------------------------------------------------------------------
// Name: Op_PLP(uint16_t src)
//-----------------------------------------------------------------------------
void MC_Processor6502::Op_PLP(uint16_t src)
{
	m_registers.status = StackPop();
	SET_CONSTANT(1);
	return;
}
//-Protected-------------------------------------------------------------------
// Name: Op_ROL(uint16_t src)
//-----------------------------------------------------------------------------
void MC_Processor6502::Op_ROL(uint16_t src)
{
	uint16_t m = MemoryRead(src);
	m <<= 1;
	if (IF_CARRY())
		m |= 0x01;

	SET_CARRY(m > 0xFF);
	m &= 0xFF;
	SET_NEGATIVE(m & 0x80);
	SET_ZERO(!m);
	MemoryWrite(src, (uint8_t)m);
	return;
}
//-Protected-------------------------------------------------------------------
// Name: Op_ROL_ACC(uint16_t src)
//-----------------------------------------------------------------------------
void MC_Processor6502::Op_ROL_ACC(uint16_t src)
{
	uint16_t m = m_registers.A;
	m <<= 1;
	if (IF_CARRY())
		m |= 0x01;

	SET_CARRY(m > 0xFF);
	m &= 0xFF;
	SET_NEGATIVE(m & 0x80);
	SET_ZERO(!m);
	m_registers.A = (uint8_t)m;
	return;
}
//-Protected-------------------------------------------------------------------
// Name: Op_ROR(uint16_t src)
//-----------------------------------------------------------------------------
void MC_Processor6502::Op_ROR(uint16_t src)
{
	uint16_t m = MemoryRead(src);
	if (IF_CARRY())
		m |= 0x100;

	SET_CARRY(m & 0x01);
	m >>= 1;
	m &= 0xFF;
	SET_NEGATIVE(m & 0x80);
	SET_ZERO(!m);
	MemoryWrite(src, (uint8_t)m);
	return;
}
//-Protected-------------------------------------------------------------------
// Name: Op_ROR_ACC(uint16_t src)
//-----------------------------------------------------------------------------
void MC_Processor6502::Op_ROR_ACC(uint16_t src)
{
	uint16_t m = m_registers.A;
	if (IF_CARRY())
		m |= 0x100;

	SET_CARRY(m & 0x01);
	m >>= 1;
	m &= 0xFF;
	SET_NEGATIVE(m & 0x80);
	SET_ZERO(!m);
	m_registers.A = (uint8_t)m;
	return;
}
//-Protected-------------------------------------------------------------------
// Name: Op_RTI(uint16_t src)
//-----------------------------------------------------------------------------
void MC_Processor6502::Op_RTI(uint16_t src)
{
	uint8_t lo, hi;

	m_registers.status = StackPop();
	lo = StackPop();
	hi = StackPop();
	m_registers.pc = (hi << 8) | lo;
	return;
}
//-Protected-------------------------------------------------------------------
// Name: Op_RTS(uint16_t src)
//-----------------------------------------------------------------------------
void MC_Processor6502::Op_RTS(uint16_t src)
{
	uint8_t lo, hi;

	lo = StackPop();
	hi = StackPop();
	m_registers.pc = ((hi << 8) | lo) + 1;
	return;
}
//-Protected-------------------------------------------------------------------
// Name: Op_SBC(uint16_t src)
//-----------------------------------------------------------------------------
void MC_Processor6502::Op_SBC(uint16_t src)
{
	uint8_t m = MemoryRead(src);
	unsigned int tmp = m_registers.A - m - (IF_CARRY() ? 0 : 1);
	SET_NEGATIVE(tmp & 0x80);
	SET_ZERO(!(tmp & 0xFF));
	SET_OVERFLOW(((m_registers.A ^ tmp) & 0x80) && ((m_registers.A ^ m) & 0x80));
	if (IF_DECIMAL()) {
		if (((m_registers.A & 0x0F) - (IF_CARRY() ? 0 : 1)) < (m & 0x0F))
			tmp -= 6;

		if (tmp > 0x99)	{
			tmp -= 0x60;
		}
	}
	SET_CARRY(tmp < 0x100);
	m_registers.A = (tmp & 0xFF);
	return;
}
//-Protected-------------------------------------------------------------------
// Name: Op_SEC(uint16_t src)
//-----------------------------------------------------------------------------
void MC_Processor6502::Op_SEC(uint16_t src)
{
	SET_CARRY(1);
	return;
}
//-Protected-------------------------------------------------------------------
// Name: Op_SED(uint16_t src)
//-----------------------------------------------------------------------------
void MC_Processor6502::Op_SED(uint16_t src)
{
	SET_DECIMAL(1);
	return;
}
//-Protected-------------------------------------------------------------------
// Name: Op_SEI(uint16_t src)
//-----------------------------------------------------------------------------
void MC_Processor6502::Op_SEI(uint16_t src)
{
	SET_INTERRUPT(1);
	return;
}
//-Protected-------------------------------------------------------------------
// Name: Op_STA(uint16_t src)
//-----------------------------------------------------------------------------
void MC_Processor6502::Op_STA(uint16_t src)
{
	MemoryWrite(src, m_registers.A);
	return;
}
//-Protected-------------------------------------------------------------------
// Name: Op_STX(uint16_t src)
//-----------------------------------------------------------------------------
void MC_Processor6502::Op_STX(uint16_t src)
{
	MemoryWrite(src, m_registers.X);
	return;
}
//-Protected-------------------------------------------------------------------
// Name: Op_STY(uint16_t src)
//-----------------------------------------------------------------------------
void MC_Processor6502::Op_STY(uint16_t src)
{
	MemoryWrite(src, m_registers.Y);
	return;
}
//-Protected-------------------------------------------------------------------
// Name: Op_TAX(uint16_t src)
//-----------------------------------------------------------------------------
void MC_Processor6502::Op_TAX(uint16_t src)
{
	uint8_t m = m_registers.A;
	SET_NEGATIVE(m & 0x80);
	SET_ZERO(!m);
	m_registers.X = m;
	return;
}
//-Protected-------------------------------------------------------------------
// Name: Op_TAY(uint16_t src)
//-----------------------------------------------------------------------------
void MC_Processor6502::Op_TAY(uint16_t src)
{
	uint8_t m = m_registers.A;
	SET_NEGATIVE(m & 0x80);
	SET_ZERO(!m);
	m_registers.Y = m;
	return;
}
//-Protected-------------------------------------------------------------------
// Name: Op_TSX(uint16_t src)
//-----------------------------------------------------------------------------
void MC_Processor6502::Op_TSX(uint16_t src)
{
	uint8_t m = m_registers.sp;
	SET_NEGATIVE(m & 0x80);
	SET_ZERO(!m);
	m_registers.X = m;
	return;
}
//-Protected-------------------------------------------------------------------
// Name: Op_TXA(uint16_t src)
//-----------------------------------------------------------------------------
void MC_Processor6502::Op_TXA(uint16_t src)
{
	uint8_t m = m_registers.X;
	SET_NEGATIVE(m & 0x80);
	SET_ZERO(!m);
	m_registers.A = m;
	return;
}
//-Protected-------------------------------------------------------------------
// Name: Op_TXS(uint16_t src)
//-----------------------------------------------------------------------------
void MC_Processor6502::Op_TXS(uint16_t src)
{
	m_registers.sp = m_registers.X;
	return;
}
//-Protected-------------------------------------------------------------------
// Name: Op_TYA(uint16_t src)
//-----------------------------------------------------------------------------
void MC_Processor6502::Op_TYA(uint16_t src)
{
	uint8_t m = m_registers.Y;
	SET_NEGATIVE(m & 0x80);
	SET_ZERO(!m);
	m_registers.A = m;
	return;
}
