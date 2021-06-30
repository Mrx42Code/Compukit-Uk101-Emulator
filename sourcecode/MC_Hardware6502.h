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
// File: MC_Hardware6502.h: interface for the MC_Hardware6502 class.
// Desc: Application interface for the MC_Hardware6502 class.
//-----------------------------------------------------------------------------
#ifndef MC_Hardware6502_H
	#define MC_Hardware6502_H

#pragma once

//-----------------------------------------------------------------------------
// Const
//-----------------------------------------------------------------------------
#define CPU6502_SPEED				        3200                                // Cpu 6502 Speed 2 mhz on this pc

#define BasicSelectUk101OrOsi				false                               // Basic Select Comuukit uk101 or Osi

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
#define MemoryExtRomSizeAddress				0x0800								// 4k
#define MemoryExtRomEndAddress				(MemoryExtRomAddress + MemoryExtRomSizeAddress) - 1
#define MemoryExtRomRWAddress				false								// R

#define MemoryParPortAddress				0x8800								// 8800 - 8803  Parallel Port Controller
#define MemoryParPortSizeAddress			0x0004								// 4
#define MemoryParPortEndAddress				(MemoryParPortAddress + MemoryParPortSizeAddress) - 1
#define MemoryParPortRWAddress				true								// R/W

#define MemoryDiskRomAddress				0x9800								// 9800 - 9FFF Disk Rom
#define MemoryDiskRomSizeAddress			0x0004								// 4k
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
    #define MemoryVideoSizeAddress			0x0C00								// (3k 0x0400 48x16) (2k 0x0800 48x32) (3k 0x0C00 48x48)
#endif
#if HardWareMedResScreen
    #define MemoryVideoSizeAddress			0x0800								// (2k 0x0400 48x16) (2k 0x0800 48x32) (3k 0x0C00 48x48)
#endif
#if HardWareLoResScreen
    #define MemoryVideoSizeAddress			0x0400								// (1k 0x0400 48x16) (2k 0x0800 48x32) (3k 0x0C00 48x48)
#endif
#define MemoryVideoEndAddress			    (MemoryVideoAddress + MemoryVideoSizeAddress) - 1
#define MemoryVideoRWAddress				true								// R/W

#define MemoryKeyboardAddress				0xDF00								// DF00 Keyboard  Controller
#define MemoryKeyboardSizeAddress			0x0001								// 1
#define MemoryKeyboardEndAddress		    (MemoryKeyboardAddress + MemoryKeyboardSizeAddress) - 1
#define MemoryKeyboardRWAddress				true								// R/W

#if F000OrF800_Rom
    #define Memory6850Address				0xE000								// E000 - E001 6850  Controller
    #define Memory6850SizeAddress			0x0002								// 2
    #define Memory6850EndAddress		    (Memory6850Address + Memory6850SizeAddress) - 1
    #define Memory6850RWAddress				true								// R/W

    #define MemoryMonitorRomAddress			0xF000								// F000 - FFFF Monitor Rom
    #define MemoryMonitorRomSizeAddress		0x1000								// 4k
    #define MemoryMonitorRomEndAddress		(MemoryMonitorRomAddress + MemoryMonitorRomSizeAddress) - 1
    #define MemoryMonitorRomRWAddress		false								// R
#else
    #define Memory6850Address				0xF000								// F000 - F001 6850  Controller
    #define Memory6850SizeAddress			0x0002								// 2
    #define Memory6850EndAddress		    (Memory6850Address + Memory6850SizeAddress) - 1
    #define Memory6850RWAddress				true								// R/W

    #define MemoryMonitorRomAddress			0xF800								// F800 - FFFF Monitor Rom
    #define MemoryMonitorRomSizeAddress		0x0800								// 4k
    #define MemoryMonitorRomEndAddress		(MemoryMonitorRomAddress + MemoryMonitorRomSizeAddress) - 1
    #define MemoryMonitorRomRWAddress		false								// R
#endif

#define CTRL_6850ADDR						Memory6850Address					// F000 6850 Controller Reg Ctrl 6850
#define DATA_6850ADDR						Memory6850Address + 1				// F001 6850 Controller Reg Data 6850

#define RDRF6850                            (1 << 0)
#define TDRE6850                            (1 << 1)

#define UARTBUFFER6850                      (1024 * 64) * 2

#define LOADSAVE_FILE_FILTER			    "All Files (*.*)\0*.*\0"

static const uint8_t m_KeyboardOutTable[8] = { 0xFE, 0xFD, 0xFB, 0xF7, 0xEF, 0xDF, 0xBF, 0x7F };

// chr, row, col, Lshift, Rshift, caplock, ctrl
static const uint8_t m_KeyboardCodeTable[] = {
        '1', 7, 7, 0, 0, 0, 0,  '2', 7, 6, 0, 0, 0, 0,  '3', 7, 5, 0, 0, 0, 0,  '4', 7, 4, 0, 0, 0, 0,  '5', 7, 3, 0, 0, 0, 0,  '6', 7, 2, 0, 0, 0, 0,  '7', 7, 1, 0, 0, 0, 0,  
        '!', 7, 7, 0, 1, 0, 0,  '"', 7, 6, 0, 1, 0, 0,  '#', 7, 5, 0, 1, 0, 0,  '$', 7, 4, 0, 1, 0, 0,  '%', 7, 3, 0, 1, 0, 0,  '&', 7, 2, 0, 1, 0, 0, 0x27, 7, 1, 0, 1, 0, 0, // '

        '8', 6, 7, 0, 0, 0, 0,  '9', 6, 6, 0, 0, 0, 0,  '0', 6, 5, 0, 0, 0, 0,  ':', 6, 4, 0, 0, 0, 0,  '-', 6, 3, 0, 0, 0, 0, 0x08, 6, 2, 0, 0, 0, 0, // "Del"
        '(', 6, 7, 0, 1, 0, 0,  ')', 6, 6, 0, 1, 0, 0,  '*', 6, 4, 0, 1, 0, 0,  '=', 6, 3, 0, 1, 0, 0,  '.', 5, 7, 0, 0, 0, 0,
        
        'L', 5, 6, 0, 0, 1, 0,  'O', 5, 5, 0, 0, 1, 0, 0x09, 5, 4, 0, 0, 1, 0, 0x0D, 5, 3, 0, 0, 0, 0, // CR
        'l', 5, 6, 0, 0, 0, 0,  'o', 5, 5, 0, 0, 0, 0,
        '>', 5, 7, 0, 1, 0, 0, 0x5C, 5, 6, 0, 1, 1, 0, // Backward Slash

        'W', 4, 7, 0, 0, 1, 0,  'E', 4, 6, 0, 0, 1, 0,  'R', 4, 5, 0, 0, 1, 0,  'T', 4, 4, 0, 0, 1, 0,  'Y', 4, 3, 0, 0, 1, 0,  'U', 4, 2, 0, 0, 1, 0,  'I', 4, 1, 0, 0, 1, 0,
        'w', 4, 7, 0, 0, 0, 0,  'e', 4, 6, 0, 0, 0, 0,  'r', 4, 5, 0, 0, 0, 0,  't', 4, 4, 0, 0, 0, 0,  'y', 4, 3, 0, 0, 0, 0,  'u', 4, 2, 0, 0, 0, 0,  'i', 4, 1, 0, 0, 0, 0,

        'S', 3, 7, 0, 0, 1, 0,  'D', 3, 6, 0, 0, 1, 0,  'F', 3, 5, 0, 0, 1, 0,  'G', 3, 4, 0, 0, 1, 0,  'H', 3, 3, 0, 0, 1, 0,  'J', 3, 2, 0, 0, 1, 0,  'K', 3, 1, 0, 0, 1, 0,
        's', 3, 7, 0, 0, 0, 0,  'd', 3, 6, 0, 0, 0, 0,  'f', 3, 5, 0, 0, 0, 0,  'g', 3, 4, 0, 0, 0, 0,  'h', 3, 3, 0, 0, 0, 0,  'j', 3, 2, 0, 0, 0, 0,  'k', 3, 1, 0, 0, 0, 0,
        0x0F, 3, 2, 0, 0, 0, 1, '[', 3, 1, 1, 0, 1, 0,

        'X', 2, 7, 0, 0, 1, 0,  'C', 2, 6, 0, 0, 1, 0,  'V', 2, 5, 0, 0, 1, 0,  'B', 2, 4, 0, 0, 1, 0,  'N', 2, 3, 0, 0, 1, 0,  'M', 2, 2, 0, 0, 1, 0,  ',', 2, 1, 0, 0, 0, 0,
        'x', 2, 7, 0, 0, 0, 0,  'c', 2, 6, 0, 0, 0, 0,  'v', 2, 5, 0, 0, 0, 0,  'b', 2, 4, 0, 0, 0, 0,  'n', 2, 3, 0, 0, 0, 0,  'm', 2, 2, 0, 0, 0, 0,  
        0x03, 2, 6, 0, 0, 0, 1, ']', 2, 2, 1, 0, 1, 0,  '<', 2, 1, 0, 1, 0, 0,

        'Q', 1, 7, 0, 0, 1, 0,  'A', 1, 6, 0, 0, 1, 0,  'Z', 1, 5, 0, 0, 1, 0,  ' ', 1, 4, 0, 0, 0, 0,  '/', 1, 3, 0, 0, 0, 0,  ';', 1, 2, 0, 0, 0, 0,  'P', 1, 1, 0, 0, 1, 0,
        'q', 1, 7, 0, 0, 0, 0,  'a', 1, 6, 0, 0, 0, 0,  'z', 1, 5, 0, 0, 0, 0,  'p', 1, 1, 0, 0, 0, 0,  '?', 1, 3, 0, 1, 0, 0,  '+', 1, 2, 0, 1, 0, 0,  '@', 1, 1, 1, 0, 1, 0
};

//-----------------------------------------------------------------------------
// Struct
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

typedef struct KeyInputType
{
    uint8_t	                Keyin;
    uint8_t	                Row;
    uint8_t	                RowScanCode;
    uint8_t	                Col;
    uint8_t	                ColScanCode;
    uint8_t	                LShift;
    uint8_t	                RShift;
    uint8_t	                caplock;
    uint8_t	                ctrl;
    bool	                KeysDone;
    uint8_t	                CpuRow;
    uint8_t	                CpuCount;
} _KeyInputType;

typedef struct ThreadType
{
    std::thread			    Thread;
    bool				    Running;
    bool				    Quit;
} _ThreadType;

typedef struct CpuSpeedSettings
{
    uint16_t                Speed;
    float                   SpeedUpDn;
    double                  AvrSpeed;
    double                  AvrBigSpeed;
    double                  CyclesPerSec;
} _CpuSpeedSettings;

//-----------------------------------------------------------------------------
// Name: class MC_Hardware6502
// Desc: Application class
//-----------------------------------------------------------------------------
class MC_Hardware6502
{

	public:
        CpuSpeedSettings    m_CpuSettings;
        bool                m_BasicSelectUk101OrOsi;
        bool                m_Disassembler6502;
        bool                m_Cpu6502Run;
        uint8_t             m_MemoryMap[MemoryMapSizeAddress];
        bool                m_MemoryWriteOverride;
        HWND                m_App_Hwnd;

    protected:
        KeyInputType        m_MemoryKeyScan;
        Uart6850            m_Uart6850;
        ThreadType          mc_ThreadMain;
        ThreadType          mc_ThreadVideo;

	private:

		//-----------------------------------------------------------------------------

	public:
                            MC_Hardware6502();
		virtual				~MC_Hardware6502();
		void				Initialize();
		void				Destroy();
		void				Create();
        void                ReSizeDisplay();
        void                KeyboardMapKey(uint8_t& KeyPress);

        void                CpuCalCyclesPerSec();
        void                CpuIRQ();
        void                CpuNMI();
        void                CpuReset();
        void                CpuInitializeAndReset();
        void                CpuCegmonukRomMod();
        void                CpuLoadFile();
        void                CpuSaveFile();
        void                CpuMemoryMapDump();
        uint8_t             CpuMemoryMapRead(uint16_t address);
        void                CpuMemoryMapWrite(uint16_t address, uint8_t value);

	protected:
        void                CpuMemoryInit();
        void                Cpu6850Uartinit();
        uint8_t             CpuEmu6850UartRead(uint16_t address);
        void                CpuEmu6850UartWrite(uint16_t address, uint8_t value);
        uint8_t             CpuEmuKeyboard(uint16_t address, bool RW);
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
        void                PrintByteAsBits(char val);
        void                PrintBits(const char* ty, const char* val, unsigned char* bytes, size_t num_bytes);
        void                DebugInfo();
        void                DebugCrashInfo();

        void				Thread_Create();
		void				Thread_Stop();
		void				Thread_CallBack_Main(int MultiThread_ID);
        void                Thread_CallBack_Video(int MultiThread_ID);
        void                Thread_Main();
        void                Thread_Video();

    private:

};
//-----------------------------------------------------------------------------
#endif // MC_Hardware6502_H
