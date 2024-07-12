//*****************************************************************************
// MIT License
//
// Copyright(c) 2024 Mrx42Code
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
// File: MC_Keyboard.h: interface for the MC_Keyboard class.
// Desc: Application interface for the MC_Keyboard class.
//-----------------------------------------------------------------------------
#ifndef MC_Keyboard_H
#define MC_Keyboard_H

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

//-----------------------------------------------------------------------------
// struct
//-----------------------------------------------------------------------------
typedef struct KeyboardInputType
{
	uint8_t					Keyin;												// key Input
	uint8_t					Row;												// key Row
	uint8_t					RowScanCode;										// key Row Scan Code
	uint8_t					Col;												// key Col
	uint8_t					ColScanCode;										// key Col Scan Code
	uint8_t					LShift;												// key LShift
	uint8_t					RShift;												// key RShift
	uint8_t					caplock;											// key caplock
	uint8_t					ctrl;												// key ctrl
	bool					KeysDone;											// key done
	uint8_t					CpuRow;												// Cpu Row
	uint8_t					CpuCount;											// Cpu Count
} _KeyboardInputType;

//-----------------------------------------------------------------------------
// Name: class MC_Keyboard
// Desc: Application class
//-----------------------------------------------------------------------------
class MC_Keyboard
{
public:
	uint8_t					m_MemoryReadValue;									// Memory Read Value
	uint8_t					m_MemoryWriteValue;									// Memory Write Value

private:
	struct KeyboardInputType m_MemoryKeyScan;									// Memory Key Scan

protected:

	//-------------------------------------------------------------------------

public:
	MC_Keyboard();
	virtual				~MC_Keyboard();
	void                Initialize();
	void                Destroy();												// Destroy
	void                Create();												// Create
	uint8_t				MemoryRead();											// Memory Read
	void				MemoryWrite(uint8_t MemoryWriteValue);					// Memory Write
	bool				KeyboardMapKey(uint8_t KeyPress);						// Keyboard Map Key

private:

protected:

};
//-----------------------------------------------------------------------------
#endif // MC_Keyboard_H
