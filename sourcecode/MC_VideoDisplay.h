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
#define VideoPixelPerCharacter              8

#define VideoRows                           64
#define VideoCols                           (MemoryVideoSizeAddress / 64)
#define VideoLinePixelWidth                 VideoRows * VideoPixelPerCharacter

#if F000OrF800_Rom
#define VideoCharacterStartPos_x			11
#define VideoCharacterPerLine				48
#else 
#define VideoCharacterStartPos_x			12
#define VideoCharacterPerLine				49
#endif

#define MemoryVideoMapSize					(MemoryVideoSizeAddress * VideoPixelPerCharacter)			// Screen Video Ram
#define MemoryVideoChrMapSize				(256 * VideoPixelPerCharacter)	                            // Screen Character Set   

#define MemoryBitMapWidth                   ((VideoCharacterPerLine * VideoPixelPerCharacter))          // Width
#define MemoryBitMaprHeight                 ((VideoCols * VideoPixelPerCharacter))                      // Height

#define VideoDisplayPixelStart_x            (VideoCharacterStartPos_x * VideoPixelPerCharacter)
#define VideoDisplayPixelEnd_x              ((VideoCharacterPerLine * VideoPixelPerCharacter ) + VideoDisplayPixelStart_x)

#define VideoDisplaySize_x                 (VideoDisplayPixelEnd_x - VideoDisplayPixelStart_x)
#define VideoDisplaySize_y                 (MemoryBitMaprHeight)

#define VideoLeftOffSet                    10
#define VideoTopOffSet                     10

//-----------------------------------------------------------------------------
// struct
//-----------------------------------------------------------------------------
typedef struct ScreenImageInfo
{
	HDC                     hdc;
	HBITMAP                 Bitmap;
	HGDIOBJ                 SelectObj;
	RECT                    Rect;
	RECT                    DisplayRect;
	COLORREF                Colour;
} _ScreenImageInfo;

typedef struct DisplayInfo
{
	HWND                    Hwnd;
	HDC                     hdc;
	RECT                    Rect;
} _DisplayInfo;

//-----------------------------------------------------------------------------
// Name: class MC_VideoDisplay
// Desc: Application class
//-----------------------------------------------------------------------------
class MC_VideoDisplay
{
public:
	bool                m_Update;
	uint8_t             m_MemoryVideoCompare[MemoryVideoSizeAddress];
	DisplayInfo         m_Display;

private:
	ScreenImageInfo     m_ScreenImage;
	uint8_t             m_MemoryVideoMap[MemoryVideoMapSize];
	uint8_t             m_MemoryVideoChrRomMap[MemoryVideoChrMapSize];

protected:

	//-----------------------------------------------------------------------------

public:
	MC_VideoDisplay();
	virtual				~MC_VideoDisplay();
	void                Initialize();
	void                Destroy();
	void                Create();
	void                ReSizeDisplay();
	void                Forceupdate();
	void                CpuEmuVideoDisplay(uint16_t address, uint8_t value);
	void                CpuEmuRenderDisplay();
	void                CpuEmuLoadCharacterSetRom(std::string FileName);

private:
protected:

};
//-----------------------------------------------------------------------------
#endif // MC_VideoDisplay_H
