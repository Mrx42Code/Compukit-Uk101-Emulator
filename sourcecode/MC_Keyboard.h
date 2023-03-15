//*****************************************************************************
// MIT License
//
// Copyright(c) 2023 Mrx42Code
// https://github.com/Mrx42Code/Compukit-Uk101-Emulator 
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this softwareand associated documentation files(the "Software"), to deal
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
	uint8_t	            Keyin;
	uint8_t	            Row;
	uint8_t	            RowScanCode;
	uint8_t	            Col;
	uint8_t	            ColScanCode;
	uint8_t	            LShift;
	uint8_t	            RShift;
	uint8_t	            caplock;
	uint8_t	            ctrl;
	bool	            KeysDone;
	uint8_t	            CpuRow;
	uint8_t	            CpuCount;
} _KeyboardInputType;

//-----------------------------------------------------------------------------
// Name: class MC_Keyboard
// Desc: Application class
//-----------------------------------------------------------------------------
class MC_Keyboard
{
public:
	uint8_t				m_MemoryReadValue;
	uint8_t				m_MemoryWriteValue;

private:
	KeyboardInputType   m_MemoryKeyScan;

protected:

	//-----------------------------------------------------------------------------

public:
	MC_Keyboard();
	virtual				~MC_Keyboard();
	void                Initialize();
	void                Destroy();
	void                Create();
	uint8_t				MemoryRead();
	void				MemoryWrite(uint8_t MemoryWriteValue);
	bool				KeyboardMapKey(uint8_t KeyPress);

private:

protected:

};
//-----------------------------------------------------------------------------
#endif // MC_Keyboard_H
