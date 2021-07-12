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
// File: MC_VideoDisplay.cpp: implementation of the MC_VideoDisplay class.
//-----------------------------------------------------------------------------
#include "MC_VideoDisplay.h"

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
// Name: MC_VideoDisplay()
// Desc: MC_VideoDisplay class
//-----------------------------------------------------------------------------
MC_VideoDisplay::MC_VideoDisplay()
{
    Initialize();
}
//-Public----------------------------------------------------------------------
// Name: ~MC_VideoDisplay()
// Desc: ~MC_VideoDisplay Destruction class
//-----------------------------------------------------------------------------
MC_VideoDisplay::~MC_VideoDisplay()
{

}
//-Public----------------------------------------------------------------------
// Name: Initialize()
//-----------------------------------------------------------------------------
void MC_VideoDisplay::Initialize()
{
    memset(&m_MemoryVideoCompare, 0x00, sizeof(m_MemoryVideoCompare));
    memset(&m_MemoryVideoMap, 0x00, sizeof(m_MemoryVideoMap));
    memset(&m_MemoryVideoChrRomMap, 0xFF, sizeof(m_MemoryVideoChrRomMap));
    memset(&m_ScreenImage, 0, sizeof(m_ScreenImage));
    memset(&m_Display, 0, sizeof(m_Display));
    m_Update = false;
    m_Display.Hwnd = nullptr;
    m_Display.hdc = nullptr;
    m_Display.Rect = { 0, 0, 0, 0 };
    m_ScreenImage.hdc = nullptr;
    m_ScreenImage.Bitmap = nullptr;
    m_ScreenImage.SelectObj = nullptr;
    m_ScreenImage.Rect = {0, 0, 0, 0};
    m_ScreenImage.DisplayRect = { 0, 0, 0, 0 };
    m_ScreenImage.Colour = RGB(0, 255, 0);
}
//-Public----------------------------------------------------------------------
// Name: Destroy()
//-----------------------------------------------------------------------------
void MC_VideoDisplay::Destroy()
{
    SelectObject(m_ScreenImage.hdc, m_ScreenImage.SelectObj);
    ReleaseDC(m_Display.Hwnd, m_ScreenImage.hdc);
    DeleteObject(m_ScreenImage.Bitmap);
    ReleaseDC(m_Display.Hwnd, m_Display.hdc);
}
//-Public----------------------------------------------------------------------
// Name: Create()
//-----------------------------------------------------------------------------
void MC_VideoDisplay::Create()
{
    RECT Video_Rect  = { 0, 0, VideoDisplaySize_x, VideoDisplaySize_y };

    m_Display.hdc = GetDC(m_Display.Hwnd);
    m_ScreenImage.hdc = CreateCompatibleDC(m_Display.hdc);
    m_ScreenImage.Bitmap = CreateCompatibleBitmap(m_Display.hdc, MemoryBitMapWidth, MemoryBitMaprHeight);
    m_ScreenImage.Rect = {0, 0, MemoryBitMapWidth, MemoryBitMaprHeight};
    m_ScreenImage.DisplayRect.left = Video_Rect.left + VideoLeftOffSet;
    m_ScreenImage.DisplayRect.top = Video_Rect.top + VideoTopOffSet;
    m_ScreenImage.DisplayRect.right = Video_Rect.right + VideoLeftOffSet;
    m_ScreenImage.DisplayRect.bottom = Video_Rect.bottom + VideoTopOffSet;
    m_ScreenImage.SelectObj = SelectObject(m_ScreenImage.hdc, m_ScreenImage.Bitmap);
}
//-Public----------------------------------------------------------------------
// Name: ReSizeDisplay()
//-----------------------------------------------------------------------------
void MC_VideoDisplay::ReSizeDisplay()
{
    GetClientRect(m_Display.Hwnd, &m_Display.Rect);
    m_Display.hdc = GetDC(m_Display.Hwnd);
    BitBlt(m_Display.hdc, 0, 0, m_Display.Rect.right, m_Display.Rect.bottom, 0, 0, 0, BLACKNESS);
}
//-Public----------------------------------------------------------------------
// Name: Forceupdate()
//-----------------------------------------------------------------------------
void MC_VideoDisplay::Forceupdate()
{
    memset(&m_MemoryVideoCompare, 0xAA, sizeof(m_MemoryVideoCompare));
    m_Update = true;
}
//-Public----------------------------------------------------------------------
// Name: CpuEmuVideoDisplay(uint16_t address, uint8_t value)
//-----------------------------------------------------------------------------
void MC_VideoDisplay::CpuEmuVideoDisplay(uint16_t address, uint8_t value)
{
    uint16_t VideoRamAddress = (address - MemoryVideoAddress);
    uint16_t VideoAddress;
    uint16_t VideoChrAddress;
    uint16_t Tmpvalue = value;
    uint16_t TmpAddress = VideoRamAddress;
    uint16_t ChrAddress;

    if (VideoRamAddress >= 0 && VideoRamAddress < MemoryVideoSizeAddress) {
        for (ChrAddress = 0; ChrAddress < 8; ChrAddress++) {
            VideoChrAddress = (Tmpvalue << 3) + ChrAddress;
            VideoAddress = (ChrAddress << 6) + (TmpAddress & 0x003F) + ((TmpAddress >> 6) * VideoLinePixelWidth);
            if (VideoChrAddress >= 0 && VideoChrAddress < MemoryVideoChrMapSize && VideoAddress >= 0 && VideoAddress < MemoryVideoMapSize) {
                m_MemoryVideoMap[VideoAddress] = m_MemoryVideoChrRomMap[VideoChrAddress];;
            }
        }
        m_Update = true;
    }
}
//-Public----------------------------------------------------------------------
// Name: CpuEmuRenderDisplay()
//-----------------------------------------------------------------------------
void MC_VideoDisplay::CpuEmuRenderDisplay()
{
    uint32_t VideoRamAddress;
    uint8_t VideoByte;
    uint8_t Bits;
    uint8_t DataBits;
    uint32_t FlipVideoRamAddress;
    uint16_t x;
    uint16_t y;

    GetClientRect(m_Display.Hwnd, &m_Display.Rect);
    m_Display.hdc = GetDC(m_Display.Hwnd);
    BitBlt(m_ScreenImage.hdc, 0, 0, MemoryBitMapWidth, MemoryBitMaprHeight, 0, 0, 0, BLACKNESS);
    for (VideoRamAddress = 0; VideoRamAddress < MemoryVideoMapSize; VideoRamAddress++) {
        FlipVideoRamAddress = (VideoRamAddress & 0xFFC0) + (VideoRamAddress & 0x003F);
        VideoByte = m_MemoryVideoMap[FlipVideoRamAddress];
        for (Bits = 0; Bits < 8; Bits++) {
            DataBits = ((VideoByte >> (7 - Bits)) & 0x01);
            if (DataBits) {
                x = ((VideoRamAddress & 0x003F) << 3) + Bits;
                y = (VideoRamAddress & 0xFFC0) >> 6;
                if (x >= VideoDisplayPixelStart_x && x < VideoDisplayPixelEnd_x) {
                    SetPixel(m_ScreenImage.hdc, x - VideoDisplayPixelStart_x, y, m_ScreenImage.Colour);
                }
            }
        }
    }
    StretchBlt(m_Display.hdc, m_Display.Rect.left, m_Display.Rect.top + 5 , m_Display.Rect.right + 40, m_Display.Rect.bottom + 5, m_ScreenImage.hdc, 0, 0, m_ScreenImage.DisplayRect.right, m_ScreenImage.DisplayRect.bottom, SRCCOPY);
}
//-Public----------------------------------------------------------------------
// Name:  CpuEmuLoadCharacterSetRom(std::string FileName)
//-----------------------------------------------------------------------------
void MC_VideoDisplay::CpuEmuLoadCharacterSetRom(std::string FileName)
{
    streampos size;
    uint8_t* memblock;
    uint16_t FileSize = 0;

    ifstream file(FileName, ios::in | ios::binary | ios::ate);
    if (file.is_open()) {
        size = file.tellg();
        memblock = new uint8_t[(uint16_t)size];
        file.seekg(0, ios::beg);
        file.read((char*)memblock, size);
        file.close();
        FileSize = (uint16_t)size;
        if (FileSize <= MemoryVideoChrMapSize) {
            memcpy(&m_MemoryVideoChrRomMap, memblock, FileSize);
        } else {
            printf("file to Big for CharacterSetRom File %s\r\n", FileName.c_str());
        }
        delete[] memblock;
    } else {
        printf("Unable to open file %s\r\n", FileName.c_str());
    }
}










//*****************************************************************************  
// Protected Code
//*****************************************************************************

//-Protected-------------------------------------------------------------------
// Name: 
//-----------------------------------------------------------------------------
