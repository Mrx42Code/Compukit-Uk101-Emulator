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
// File: MC_Hardware6502.h: interface for the MC_Hardware6502 class.
// Desc: Application interface for the MC_Hardware6502 class.
//-----------------------------------------------------------------------------
#ifndef MC_Hardware6502_H
#define MC_Hardware6502_H

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
#define BreakPointMemory                    true
#define BreakPointOpCode                    true

#define USEINIFILESETTING                   false 

#define CPU6502_TESTMODE                    false   

#define CPU6502_CLKREFSPEED				    (1000000 / 10)                      // Cpu 6502 Ref 1 mhz / 10th Sec
#define CPU6502_SPEED				        1750.0f                             // Cpu 6502 Speed 2 mhz on this pc

#define BasicSelectUk101OrOsi				false                               // Basic Select Comuukit uk101 or Osi

#define Basic5Test						    false                               // Basic 5 Test

#define cegmonukRomMod						false                               // cegmonuk Res Mod and save Rom

#define F000OrF800_Rom						false                               // Big Monitor Rom And Move Uart Address

#define HardWareHiResScreen				    false                               // Screen Size 48x48
#define HardWareMedResScreen			    false                               // Screen Size 48x32
#define HardWareLoResScreen			        true                                // Screen Size 48x16

#define MemoryMapAddress					0x0000								// 0000-FFFF Ram - Cpu Memory Map
#define MemoryMapSizeAddress				0x10000								// 64k
#define MemoryMapEndAddress				    (MemoryMapAddress + MemoryMapSizeAddress) - 1

#define MemoryRamAddress					0x0000								// 0000-7FFF Ram 
#define MemoryRamSizeAddress				0x8000								// 32k
#define MemoryRamEndAddress				    (MemoryRamAddress + MemoryRamSizeAddress) - 1
#define MemoryRamRWAddress					true								// R/W    

#define MemoryExtRomAddress					0x8000								// 8000 - 87FF Ext Rom
#define MemoryExtRomSizeAddress				0x0800								// 2k
#define MemoryExtRomEndAddress				(MemoryExtRomAddress + MemoryExtRomSizeAddress) - 1
#define MemoryExtRomRWAddress				false								// R

#define MemoryParPortAddress				0x8800								// 8800 - 8803  Parallel Port Controller
#define MemoryParPortSizeAddress			0x0004								// 4
#define MemoryParPortEndAddress				(MemoryParPortAddress + MemoryParPortSizeAddress) - 1
#define MemoryParPortRWAddress				true								// R/W

#define MemoryBasic5RomAddress				0x9000								// 9000 - 97FF Basic5 Rom
#define MemoryBasic5RomSizeAddress			0x0800								// 2k
#define MemoryBasic5RomEndAddress			(MemoryBasic5RomAddress + MemoryBasic5RomSizeAddress) - 1
#define MemoryBasic5RomRWAddress			false								// R/W

#define MemoryDiskRomAddress				0x9800								// 9800 - 9FFF Disk Rom / BasicX Rom
#define MemoryDiskRomSizeAddress			0x0800								// 2k
#define MemoryDiskRomEndAddress				(MemoryDiskRomAddress + MemoryDiskRomSizeAddress) - 1
#define MemoryDiskRomRWAddress				false								// R/W

#define MemoryBasicRomAddress				0xA000								// A000 - BFFF Basic Rom
#define MemoryBasicRomSizeAddress			0x2000								// 8k
#define MemoryBasicRomEndAddress			(MemoryBasicRomAddress + MemoryBasicRomSizeAddress) - 1
#define MemoryBasicRomRWAddress				false								// R

#define MemoryDiskAddress					0xC000								// C000 - C003 6820 Disk Controller
#define MemoryDiskSizeAddress				0x0004								// 4
#define MemoryDiskEndAddress			    (MemoryDiskAddress + MemoryDiskSizeAddress) - 1
#define MemoryDiskRWAddress					true								// R/W

#define MemoryVideoAddress					0xD000								// D000 - D3FF Video Ram
#if HardWareHiResScreen
#define MemoryVideoSizeAddress				0x0C00								// (3k 0x0400 48x16) (2k 0x0800 48x32) (3k 0x0C00 48x48)
#endif
#if HardWareMedResScreen
#define MemoryVideoSizeAddress				0x0800								// (2k 0x0400 48x16) (2k 0x0800 48x32) (3k 0x0C00 48x48)
#endif
#if HardWareLoResScreen
#define MemoryVideoSizeAddress				0x0400								// (1k 0x0400 48x16) (2k 0x0800 48x32) (3k 0x0C00 48x48)
#endif
#define MemoryVideoEndAddress			    (MemoryVideoAddress + MemoryVideoSizeAddress) - 1
#define MemoryVideoRWAddress				true								// R/W

#define MemoryKeyboardAddress				0xDF00								// DF00 Keyboard  Controller
#define MemoryKeyboardSizeAddress			0x0001								// 1
#define MemoryKeyboardEndAddress		    (MemoryKeyboardAddress + MemoryKeyboardSizeAddress) - 1
#define MemoryKeyboardRWAddress				true								// R/W

#if F000OrF800_Rom
#define Memory6850Address					0xE000								// E000 - E001 6850  Controller
#define Memory6850SizeAddress				0x0002								// 2
#define Memory6850EndAddress				(Memory6850Address + Memory6850SizeAddress) - 1
#define Memory6850RWAddress					true								// R/W

#define MemoryMonitorRomAddress				0xF000								// F000 - FFFF Monitor Rom
#define MemoryMonitorRomSizeAddress			0x1000								// 4k
#define MemoryMonitorRomEndAddress			(MemoryMonitorRomAddress + MemoryMonitorRomSizeAddress) - 1
#define MemoryMonitorRomRWAddress			false								// R
#else
#define Memory6850Address					0xF000								// F000 - F001 6850  Controller
#define Memory6850SizeAddress				0x0002								// 2
#define Memory6850EndAddress				(Memory6850Address + Memory6850SizeAddress) - 1
#define Memory6850RWAddress					true								// R/W

#define MemoryMonitorRomAddress				0xF800								// F800 - FFFF Monitor Rom
#define MemoryMonitorRomSizeAddress			0x0800								// 4k
#define MemoryMonitorRomEndAddress			(MemoryMonitorRomAddress + MemoryMonitorRomSizeAddress) - 1
#define MemoryMonitorRomRWAddress			false								// R
#endif

#define CTRL_6850ADDR						Memory6850Address					// F000 6850 Controller Reg Ctrl 6850
#define DATA_6850ADDR						Memory6850Address + 1				// F001 6850 Controller Reg Data 6850

#define RDRF6850                            (1 << 0)
#define TDRE6850                            (1 << 1)

#define UARTBUFFER6850                      (1024 * 64) * 2

#define LOADSAVE_FILE_FILTER			    "All Files (*.*)\0*.*\0"
#define LOADHEX_FILE_FILTER			        "hex Files (*.hex*)\0*.hex\0"

//-----------------------------------------------------------------------------
// struct
//-----------------------------------------------------------------------------
typedef struct UartStatusBits
{
	bool                    RDRF : 1; // bit 0
	bool                    TDRE : 1;
	bool                    DCD : 1;
	bool                    CTS : 1;
	bool                    FE : 1;
	bool                    OVRN : 1;
	bool                    PE : 1;
	bool                    IRQ : 1;  // bit 7
} _UartStatusBits;

union UartStatusReg {
	struct UartStatusBits   bits;
	uint8_t                 byte;
};

typedef struct Uart6850InOut
{
	uint8_t                 CharData;
	char                    Buffer[UARTBUFFER6850];
	int                     ProcessedIndex;
	int                     Index;
} _Uart6850InOut;

typedef struct Uart6850
{
	union UartStatusReg     Registers_SR;
	Uart6850InOut           Input;
	Uart6850InOut           Output;
} _Uart6850;

typedef struct CpuThreadType
{
	bool				    Running = false;
	bool				    Quit = false;
	std::thread			    Thread;
} _CpuThreadType;

typedef struct CpuSpeedSettings
{
	int                     Speed;
	float                   SpeedUpDn;
	double                  AvrSpeed;
	double                  AvrBigSpeed;
	double                  CyclesPerSec;
} _CpuSpeedSettings;

typedef struct CpuDebugPanel {
	uint16_t                DumpStartAddress;
	uint16_t                DumpEndAddress;
	bool                    Update;
} CpuDebugPanel;

typedef struct IniFilevector {
	std::vector<std::string> LineData;
} _IniFilevector;

typedef struct BreakPoint
{
	bool                SetFlag;
	bool                Found;
	uint16_t            Address;
} _BreakPoint;

//-----------------------------------------------------------------------------
// Name: class MC_Hardware6502
// Desc: Application class
//-----------------------------------------------------------------------------
class MC_Hardware6502
{

public:
	HANDLE              m_hConsole;
	CpuSpeedSettings    m_CpuSettings;
	bool                m_BasicSelectUk101OrOsi;
	bool                m_Cpu6502Run;
	bool                m_Cpu6502Step;
	bool                m_Disassembler6502;
	BreakPoint          m_BreakPointOpCode;
	BreakPoint          m_BreakPointMemory;
	uint8_t             m_MemoryMap[MemoryMapSizeAddress];
	bool                m_MemoryWriteOverride;
	HWND                m_App_Hwnd;
	CpuDebugPanel       m_CpuDebugPanel;
	IniFilevector       m_IniFileString;

private:
	Uart6850            m_Uart6850;
	CpuThreadType       mc_ThreadMain;
	CpuThreadType       mc_ThreadVideo;

protected:

	//-----------------------------------------------------------------------------

public:
	MC_Hardware6502();
	virtual				~MC_Hardware6502();
	void				Initialize();
	void				Destroy();
	void				Create();
	void                ReSizeDisplay();
	void                PrintStatus(bool Error, std::string Msg);
	void				KeyPress(uint8_t Key);

	void                CpuSetParameters();
	void                CpuSetBreakPointOpCode(bool Enable, uint16_t Address);
	void                CpuSetBreakPointMemory(bool Enable, uint16_t Address);
	void				CpuSetPC(uint16_t PC);

	void                CpuIRQ();
	void                CpuNMI();
	void                CpuInitializeAndReset();
	void                CpuReset();
	void                CpuStop();
	void                CpuRun();
	void                CpuStep();

	void                CpuCalCyclesPer10thSec();
	void                CpuCegmonukRomMod();
	void                CpuLoadFile();
	void                CpuSaveFile();
	void                CpuMemoryLoadFile();
	void                CpuMemoryMapDump();
	void                CpuMemoryMapDump(uint16_t StartAddress, uint16_t EndAddress);
	uint8_t             CpuMemoryMapRead(uint16_t address);
	void                CpuMemoryMapWrite(uint16_t address, uint8_t value);
	void                CpuPrintMemoryInfo();

private:
	void                CpuMemoryInit();
	void                Cpu6850Uartinit();
	uint8_t             CpuEmu6850UartRead(uint16_t address);
	void                CpuEmu6850UartWrite(uint16_t address, uint8_t value);
	uint8_t             CpuEmuKeyboard(uint16_t address, bool RW);
	void                CpuLoadIniFileRoms();
	void                CpuLoadRoms();

	void                MemoryLoad(uint16_t MemoryAddress, uint16_t MemorySize, std::string FileName);
	void                MemorySave(uint16_t MemoryAddress, uint16_t MemorySize, std::string FileName);
	void                MemoryLoadIntelFormat(uint16_t MemoryAddress, uint16_t MemorySize, std::string FileName);
	std::string         FilenameOpenDlg(const char* filter, HWND hwnd);
	std::string         FilenameSaveDlg(const char* filter, HWND hwnd);
	void                LoadUartData(std::string FileName);
	void                SaveUartData(std::string FileName);
	uint16_t            Hex2Dec(std::string s);
	void                PrintHexDump(const char* desc, void* addr, long len);
	void                PrintHexDump16Bit(const char* desc, void* addr, long len, long offset);
	void                IniFileDefault();
	std::string         IniFileHeaderAddress(std::string Header, uint16_t MemAddress, uint16_t MemSize);
	bool                IniFileRead(std::string FileName);
	bool                IniFileWrite(std::string FileName);
	bool                TestForBreakPointOpCode();
	void                TestForBreakPointMemory(uint16_t& address, uint8_t& data, bool ReadWrite);

	void				Thread_Create();
	void				Thread_Stop();
	void				Thread_CallBack_Main(int MultiThread_ID);
	void                Thread_CallBack_Video(int MultiThread_ID);

protected:

};
//-----------------------------------------------------------------------------
#endif // MC_Hardware6502_H
