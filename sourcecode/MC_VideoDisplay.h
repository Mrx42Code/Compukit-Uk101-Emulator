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
// File: MC_VideoDisplay.h: interface for the MC_VideoDisplay class.
// Desc: Application interface for the MC_VideoDisplay class.
//-----------------------------------------------------------------------------
#ifndef MC_VideoDisplay_H
#define MC_VideoDisplay_H

#pragma once

//-----------------------------------------------------------------------------
// include Windows standard Libs
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// include Project
//-----------------------------------------------------------------------------
#include "framework.h"
#include "MC_Hardware6502.h"

//-----------------------------------------------------------------------------
// include Vendors
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// define
//-----------------------------------------------------------------------------
#define VideoPixelPerCharacter              8									// 8 pixels per character

#define VideoRows                           64									// 64 rows
#define VideoCols                           (MemoryVideoSizeAddress / 64)
#define VideoLinePixelWidth                 VideoRows * VideoPixelPerCharacter	// 64 * 8 = 512

#if F000OrF800_Rom
#define VideoCharacterStartPos_x			11									// 11
#define VideoCharacterPerLine				48									// 48	
#else 
#define VideoCharacterStartPos_x			12									// 12
#define VideoCharacterPerLine				49									// 49
#endif

#define MemoryVideoMapSize					(MemoryVideoSizeAddress * VideoPixelPerCharacter)			// Screen Video Ram
#define MemoryVideoChrMapSize				(256 * VideoPixelPerCharacter)	                            // Screen Character Set   

#define MemoryBitMapWidth                   ((VideoCharacterPerLine * VideoPixelPerCharacter))          // Width
#define MemoryBitMaprHeight                 ((VideoCols * VideoPixelPerCharacter))                      // Height

#define VideoDisplayPixelStart_x            (VideoCharacterStartPos_x * VideoPixelPerCharacter)			// Start Position
#define VideoDisplayPixelEnd_x              ((VideoCharacterPerLine * VideoPixelPerCharacter ) + VideoDisplayPixelStart_x)	// End Position

#define VideoDisplaySize_x                 (VideoDisplayPixelEnd_x - VideoDisplayPixelStart_x)			// Size x
#define VideoDisplaySize_y                 (MemoryBitMaprHeight)										// Size	y

#define VideoLeftOffSet                    10									// 10
#define VideoTopOffSet                     10									// 10

//-----------------------------------------------------------------------------
// struct
//-----------------------------------------------------------------------------
typedef struct ScreenImageInfo													// Screen Image Info
{
	HDC                 hdc;													// Device Context
	HBITMAP             Bitmap;													// Bitmap
	HGDIOBJ             SelectObj;												// Select Object
	RECT                Rect;													// Rect
	RECT                DisplayRect;											// Display Rect
	COLORREF            Colour;													// Colour
} _ScreenImageInfo;

typedef struct DisplayInfo														// Display Info
{
	HWND                Hwnd;													// Hwnd
	HDC                 hdc;													// Device Context
	RECT				Rect;													// Rect
} _DisplayInfo;

//-----------------------------------------------------------------------------
// Name: class MC_VideoDisplay
// Desc: Application class
//-----------------------------------------------------------------------------
class MC_VideoDisplay
{
public:
	bool                m_Update;												// Update
	uint8_t             m_MemoryVideoCompare[MemoryVideoSizeAddress];			// Memory Video Compare
	DisplayInfo         m_Display;													// Display

private:
	ScreenImageInfo     m_ScreenImage;											// Screen Image
	uint8_t             m_MemoryVideoMap[MemoryVideoMapSize];					// Memory Video
	uint8_t             m_MemoryVideoChrRomMap[MemoryVideoChrMapSize];			// Memory Video Character Set

protected:

	//--------------------------------------------------------------------------

public:
	MC_VideoDisplay();
	virtual				~MC_VideoDisplay();
	void                Initialize();											// Initialize
	void                Destroy();												// Destroy
	void                Create();												// Create
	void                ReSizeDisplay();										// ReSize Display
	void                Forceupdate();											// Force Update
	void                CpuEmuVideoDisplay(uint16_t address, uint8_t value);	// Cpu Emu Video Display
	void                CpuEmuRenderDisplay();									// Cpu Emu Render Display
	void                CpuEmuLoadCharacterSetRom(std::string FileName);		// Cpu Emu Load Character Set Rom

private:
protected:

};
//-----------------------------------------------------------------------------
#endif // MC_VideoDisplay_H
