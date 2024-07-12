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
// File: EmulatorCompukitUK101_Desktop.h: interface for EmulatorCompukitUK101_Desktop.
// Desc: Application interface for the EmulatorCompukitUK101_Desktop.
//-----------------------------------------------------------------------------
#ifndef EmulatorCompukitUK101_Desktop_H
#define EmulatorCompukitUK101_Desktop_H

#pragma once

//-----------------------------------------------------------------------------
// include Windows standard Libs
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// include Project
//-----------------------------------------------------------------------------
#include "framework.h"
#include "resource.h"

//-----------------------------------------------------------------------------
// include Vendors
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// define
//-----------------------------------------------------------------------------
#define IDT_TIMERCyclesPerSec   1												// Cycles Per Second			
#define IDT_TIMERIrq            2												// IRQ
#define IDT_TIMERNmi            3												// NMI
#define IDT_TIMERDlg            4												// Dialog

#define MAX_LOADSTRING          100												// Max Load String

//-----------------------------------------------------------------------------
// struct
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Name: Functions Calls
//-----------------------------------------------------------------------------
// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);						// Register Window Class
BOOL                InitInstance(HINSTANCE, int);								// Init Instance
void                UpdateMenus(HWND hWnd);										// Update Menus
void                DebugControlPanelSetItems(HWND hWnd);						// Set Debug Control Panel Items
void                DebugControlPanelGetItems(HWND hWnd);						// Get Debug Control Panel Items
void                SetButtonChecked(HWND hWnd, int Button, int Mode);			// Set Button Checked
bool                AddConsole();												// Add Console
void                UpdateConsoleTitle();										// Update Console Title
std::wstring        ConvertHexUint16ToWstring(uint16_t Value);					// Convert Hex Uint16 To Wstring
std::wstring        ConvertHexUint8ToWstring(uint8_t Value);					// Convert Hex Uint8 To Wstring
uint16_t            ConvertHexLPWSTRTouint16(LPWSTR Value);						// Convert Hex LPWSTR To Uint16
uint8_t             ConvertHexLPWSTRTouint8(LPWSTR Value);						// Convert Hex LPWSTR To Uint8

//-----------------------------------------------------------------------------
#endif // EmulatorCompukitUK101_Desktop_H

