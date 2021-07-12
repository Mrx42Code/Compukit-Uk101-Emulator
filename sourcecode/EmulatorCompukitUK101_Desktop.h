/**********************************************************************************
* MIT License																	  *
*																				  *
* Copyright(c) 2021 Mrx42Code                                                     *
* https://github.com/Mrx42Code/Compukit-Uk101-Emulator  				          *
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
// File: EmulatorCompukitUK101_Desktop.h: interface for EmulatorCompukitUK101_Desktop.
// Desc: Application interface for the EmulatorCompukitUK101_Desktop.
//-----------------------------------------------------------------------------
#ifndef EmulatorCompukitUK101_Desktop_H
    #define EmulatorCompukitUK101_Desktop_H

#pragma once

#include "framework.h"
#include "resource.h"

//-----------------------------------------------------------------------------
// Const
//-----------------------------------------------------------------------------
#define IDT_TIMERCyclesPerSec   1
#define IDT_TIMERIrq            2
#define IDT_TIMERNmi            3
#define IDT_TIMERDlg            4

#define MAX_LOADSTRING          100
//-----------------------------------------------------------------------------
// Struct
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Name: Desktop Calls
//-----------------------------------------------------------------------------
// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    DebugControlPanel(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
void                UpdateMenus(HWND hWnd);
void                DebugControlPanelSetItems(HWND hWnd);
void                DebugControlPanelGetItems(HWND hWnd);
void                SetButtonChecked(HWND hWnd, int Button, int Mode);
void                AddConsole();
void                UpdateConsoleTitle();
std::wstring        ConvertHexUint16ToWstring(uint16_t Value);
std::wstring        ConvertHexUint8ToWstring(uint8_t Value);
uint16_t            ConvertHexLPWSTRTouint16(LPWSTR Value);
uint8_t             ConvertHexLPWSTRTouint8(LPWSTR Value);
 
//-----------------------------------------------------------------------------
#endif // EmulatorCompukitUK101_Desktop_H

