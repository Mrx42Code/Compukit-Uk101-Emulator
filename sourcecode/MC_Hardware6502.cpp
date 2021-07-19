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
// File: MC_Hardware6502.cpp: implementation of the MC_Hardware6502 class.
//-----------------------------------------------------------------------------
#include "MC_Hardware6502.h"
#include "MC_VideoDisplay.h"

using namespace std;

//*****************************************************************************  
// Public Code
//*****************************************************************************
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
        0x0A, 3, 2, 0, 0, 1, 1, '[', 3, 1, 1, 0, 1, 0,

        'X', 2, 7, 0, 0, 1, 0,  'C', 2, 6, 0, 0, 1, 0,  'V', 2, 5, 0, 0, 1, 0,  'B', 2, 4, 0, 0, 1, 0,  'N', 2, 3, 0, 0, 1, 0,  'M', 2, 2, 0, 0, 1, 0,  ',', 2, 1, 0, 0, 0, 0,
        'x', 2, 7, 0, 0, 0, 0,  'c', 2, 6, 0, 0, 0, 0,  'v', 2, 5, 0, 0, 0, 0,  'b', 2, 4, 0, 0, 0, 0,  'n', 2, 3, 0, 0, 0, 0,  'm', 2, 2, 0, 0, 0, 0,
        0x03, 2, 6, 0, 0, 0, 1, ']', 2, 2, 1, 0, 1, 0,  '<', 2, 1, 0, 1, 0, 0,

        'Q', 1, 7, 0, 0, 1, 0,  'A', 1, 6, 0, 0, 1, 0,  'Z', 1, 5, 0, 0, 1, 0,  ' ', 1, 4, 0, 0, 0, 0,  '/', 1, 3, 0, 0, 0, 0,  ';', 1, 2, 0, 0, 0, 0,  'P', 1, 1, 0, 0, 1, 0,
        'q', 1, 7, 0, 0, 0, 0,  'a', 1, 6, 0, 0, 0, 0,  'z', 1, 5, 0, 0, 0, 0,  'p', 1, 1, 0, 0, 0, 0,  '?', 1, 3, 0, 1, 0, 0,  '+', 1, 2, 0, 1, 0, 0,  '@', 1, 1, 1, 0, 1, 0,

        0x7B, 0, 1, 1, 0, 0, 0,  0x7D, 0, 2, 1, 0, 0, 0
};
//-----------------------------------------------------------------------------
// IMPLEMENT_DYNCREATE
//-----------------------------------------------------------------------------
MC_Hardware6502 mc_Hardware6502;
MC_VideoDisplay mc_VideoDisplay;

uint8_t CpuMemoryRead(uint16_t address);
void CpuMemoryWrite(uint16_t address, uint8_t value);
MC_Processor6502 mc_Processor6502(CpuMemoryRead, CpuMemoryWrite);

//-----------------------------------------------------------------------------
// Name: CpuMemoryRead(uint16_t address)
//-----------------------------------------------------------------------------
uint8_t CpuMemoryRead(uint16_t address)
{
    return mc_Hardware6502.CpuMemoryMapRead(address);
}
//-----------------------------------------------------------------------------
// Name: CpuMemoryWrite(uint16_t address, uint8_t value)
//-----------------------------------------------------------------------------
void CpuMemoryWrite(uint16_t address, uint8_t value)
{
    mc_Hardware6502.CpuMemoryMapWrite(address, value);
}

//-----------------------------------------------------------------------------
// message handlers
//-----------------------------------------------------------------------------

//-Public----------------------------------------------------------------------
// Name: MC_Hardware6502()
// Desc: MC_Hardware6502 class
//-----------------------------------------------------------------------------
MC_Hardware6502::MC_Hardware6502()
{
    m_hConsole = nullptr;
    mcp_Processor6502 = nullptr;
    memset(&m_CpuSettings, 0x00, sizeof(m_CpuSettings));
    m_CpuSettings.SpeedUpDn = CPU6502_SPEED;
    m_CpuSettings.Speed = 2;
    m_Cpu6502Run = false;
    m_Cpu6502Step = false;
    m_Disassembler6502 = false;
    memset(&m_BreakPointOpCode, 0, sizeof(m_BreakPointOpCode));
    memset(&m_BreakPointMemory, 0, sizeof(m_BreakPointMemory));
    m_BasicSelectUk101OrOsi = BasicSelectUk101OrOsi;
    m_App_Hwnd = nullptr;
    m_CpuDebugPanel.DumpStartAddress = 0x0000;
    m_CpuDebugPanel.DumpEndAddress = 0xFFFF;
    m_CpuDebugPanel.Update = false;
    CpuMemoryInit();
    mc_ThreadMain.Running = false;
    mc_ThreadMain.Quit = false;
    mc_ThreadVideo.Running = false;
    mc_ThreadVideo.Quit = false;
 }
//-Public----------------------------------------------------------------------
// Name: ~MC_Hardware6502()
// Desc: ~MC_Hardware6502 Destruction class
//-----------------------------------------------------------------------------
MC_Hardware6502::~MC_Hardware6502()
{
    Thread_Stop();
}
//-Public----------------------------------------------------------------------
// Name: Initialize()
//-----------------------------------------------------------------------------
void MC_Hardware6502::Initialize()
{
    PrintStatus(false, "Hardware Initialize");
    mcp_Processor6502 = &mc_Processor6502;
    mc_ThreadMain.Running = false;
    mc_ThreadMain.Quit = false;
    mc_ThreadVideo.Running = false;
    mc_ThreadVideo.Quit = false;
    mc_VideoDisplay.Initialize();
}
//-Public----------------------------------------------------------------------
// Name: Destroy()
//-----------------------------------------------------------------------------
void MC_Hardware6502::Destroy()
{
    PrintStatus(false, "Hardware Destroy");
    Thread_Stop();
    mc_VideoDisplay.Destroy();
    Sleep(1000);
}
//-Public----------------------------------------------------------------------
// Name: Create()
//-----------------------------------------------------------------------------
void MC_Hardware6502::Create()
{
    PrintStatus(false, "Hardware Create");
    mc_VideoDisplay.Create();
    mc_VideoDisplay.m_Display.Hwnd = m_App_Hwnd;
    m_Disassembler6502 = false;
#if USEINIFILESETTING
    if (IniFileRead("Compukit_UK101.ini")) {
        IniFileDefault();
        IniFileWrite("Compukit_UK101.ini");
    }
#endif
    CpuSetParameters();
    CpuInitializeAndReset();
    Thread_Create();
}
//-Public----------------------------------------------------------------------
// Name: ReSizeDisplay()
//-----------------------------------------------------------------------------
void MC_Hardware6502::ReSizeDisplay()
{
    mc_VideoDisplay.Forceupdate();
    mc_VideoDisplay.ReSizeDisplay();
}
//-Public----------------------------------------------------------------------
// Name: KeyboardMapKey(uint8_t& KeyPress)
//-----------------------------------------------------------------------------
void MC_Hardware6502::KeyboardMapKey(uint8_t& KeyPress)
{
    int i;
    int len = sizeof(m_KeyboardCodeTable);
    int index;
    uint8_t Keycp;
    char Text[2];
    Text[1] = 0;
    bool KeyFound = false;

    if (KeyPress) {
        for (i = 0; i < len; i++) {
            index = (i * 7) + 0;
            if (index < len) {
                Keycp = m_KeyboardCodeTable[index];
                if (Keycp && Keycp == KeyPress) {
                    memset(&m_MemoryKeyScan, 0x00, sizeof(m_MemoryKeyScan));
                    m_MemoryKeyScan.Keyin = KeyPress;
                    m_MemoryKeyScan.Row = m_KeyboardCodeTable[index + 1];
                    m_MemoryKeyScan.Col = m_KeyboardCodeTable[index + 2];
                    m_MemoryKeyScan.LShift = m_KeyboardCodeTable[index + 3];
                    m_MemoryKeyScan.RShift = m_KeyboardCodeTable[index + 4];
                    m_MemoryKeyScan.caplock = m_KeyboardCodeTable[index + 5];
                    m_MemoryKeyScan.ctrl = m_KeyboardCodeTable[index + 6];
                    m_MemoryKeyScan.RowScanCode = m_KeyboardOutTable[(uint8_t)m_MemoryKeyScan.Row];
                    m_MemoryKeyScan.ColScanCode = m_KeyboardOutTable[(uint8_t)m_MemoryKeyScan.Col];
                    m_MemoryKeyScan.CpuCount = 0;
                    m_MemoryKeyScan.KeysDone = false;
                    m_MemoryKeyScan.CpuRow = 0xFF;
                    KeyFound = true;
                    Text[0] = Keycp;
                }
            }
        }
    }
}
//-Public----------------------------------------------------------------------
// Name: PrintStatus(bool Error, std::string Msg)
//-----------------------------------------------------------------------------
void MC_Hardware6502::PrintStatus(bool Error, std::string Msg)
{
    if (Error) {
        printf("\u001b[31;1m[Error]\u001b[0m ( \u001b[33;1m%s\u001b[0m )\r\n", Msg.c_str());
    } else {
        printf("\u001b[32;1m[ Ok  ]\u001b[0m ( \u001b[33;1m%s\u001b[0m )\r\n", Msg.c_str());
    }
}
//-Public----------------------------------------------------------------------
// Name: CpuMemoryMapRead(uint16_t address)
//-----------------------------------------------------------------------------
uint8_t MC_Hardware6502::CpuMemoryMapRead(uint16_t address)
{
    uint8_t Data = 0xFF;

#if CPU6502_TESTMODE
    return m_MemoryMap[address];
#else
    if (address >= MemoryRamAddress && address <= MemoryRamEndAddress) {
        Data = m_MemoryMap[address];
    } else  if (address >= MemoryExtRomAddress && address <= MemoryExtRomEndAddress) {
        Data = m_MemoryMap[address];
    } else  if (address >= MemoryParPortAddress && address <= MemoryParPortEndAddress) {
        Data = m_MemoryMap[address];
    } else  if (address >= MemoryBasic5RomAddress && address <= MemoryBasic5RomEndAddress) {
        Data = m_MemoryMap[address];
    } else  if (address >= MemoryDiskRomAddress && address <= MemoryDiskRomEndAddress) {
        Data = m_MemoryMap[address];
    } else  if (address >= MemoryBasicRomAddress && address <= MemoryBasicRomEndAddress) {
        Data = m_MemoryMap[address];
    } else  if (address >= MemoryDiskAddress && address <= MemoryDiskEndAddress) {
        Data = m_MemoryMap[address];
    } else  if (address >= MemoryVideoAddress && address <= MemoryVideoEndAddress) {
        Data = m_MemoryMap[address];
    } else  if (address >= (MemoryVideoAddress + MemoryVideoSizeAddress) && address <= ((MemoryVideoAddress + MemoryVideoSizeAddress) + 0x3F) ) {     //Bug Fix 48 Line Scroll Up
        Data = m_MemoryMap[address];
    } else  if (address >= MemoryKeyboardAddress && address <= MemoryKeyboardEndAddress) {
        Data = CpuEmuKeyboard(address, true);
    } else  if (address >= Memory6850Address && address <= Memory6850EndAddress) {
        Data = CpuEmu6850UartRead(address);
    } else  if (address >= MemoryMonitorRomAddress && address <= MemoryMonitorRomEndAddress) {
        Data = m_MemoryMap[address];
    }
#endif
#if BreakPointMemory
    TestForBreakPointMemory(address, Data, true);
#endif
    return Data;
}
//-Public----------------------------------------------------------------------
// Name: CpuMemoryMapWrite(uint16_t address, uint8_t value)
//-----------------------------------------------------------------------------
void MC_Hardware6502::CpuMemoryMapWrite(uint16_t address, uint8_t value)
{
#if CPU6502_TESTMODE
    m_MemoryMap[address] = value;
#else
    if (address >= MemoryRamAddress && address <= MemoryRamEndAddress && (MemoryRamRWAddress || m_MemoryWriteOverride)) {
        m_MemoryMap[address] = value;
    } else  if (address >= MemoryExtRomAddress && address <= MemoryExtRomEndAddress && (MemoryExtRomRWAddress || m_MemoryWriteOverride)) {
        m_MemoryMap[address] = value;
    } else  if (address >= MemoryParPortAddress && address <= MemoryParPortEndAddress && (MemoryParPortRWAddress || m_MemoryWriteOverride)) {
        m_MemoryMap[address] = value;
    } else  if (address >= MemoryBasic5RomAddress && address <= MemoryBasic5RomEndAddress && (MemoryBasic5RomRWAddress || m_MemoryWriteOverride)) {
        m_MemoryMap[address] = value;
    } else  if (address >= MemoryDiskRomAddress && address <= MemoryDiskRomEndAddress && (MemoryDiskRomRWAddress || m_MemoryWriteOverride)) {
        m_MemoryMap[address] = value;
    } else  if (address >= MemoryBasicRomAddress && address <= MemoryBasicRomEndAddress && (MemoryBasicRomRWAddress || m_MemoryWriteOverride)) {
        m_MemoryMap[address] = value;
    } else  if (address >= MemoryDiskAddress && address <= MemoryDiskEndAddress && (MemoryDiskRWAddress || m_MemoryWriteOverride)) {
        m_MemoryMap[address] = value;
    } else  if (address >= MemoryVideoAddress && address <= MemoryVideoEndAddress && (MemoryVideoRWAddress || m_MemoryWriteOverride)) {
        m_MemoryMap[address] = value;
        mc_VideoDisplay.CpuEmuVideoDisplay(address, value);
    } else  if (address >= MemoryKeyboardAddress && address <= MemoryKeyboardEndAddress && (MemoryKeyboardRWAddress || m_MemoryWriteOverride)) {
        m_MemoryMap[address] = value;
        CpuEmuKeyboard(address, false);
    } else  if (address >= Memory6850Address && address <= Memory6850EndAddress && (Memory6850RWAddress || m_MemoryWriteOverride)) {
        CpuEmu6850UartWrite(address, value);
    } else  if (address >= MemoryMonitorRomAddress && address <= MemoryMonitorRomEndAddress && (MemoryMonitorRomRWAddress || m_MemoryWriteOverride)) {
        m_MemoryMap[address] = value;
    }
#endif
#if BreakPointMemory
    TestForBreakPointMemory(address, value, false);
#endif
}
//-Public----------------------------------------------------------------------
// Name: CpuIRQ()
//-----------------------------------------------------------------------------
void MC_Hardware6502::CpuIRQ()
{
    if (m_Cpu6502Run) {
        //PrintStatus(false, "Cpu IRQ");
        //mc_Processor6502.IRQ();
    }
}
//-Public----------------------------------------------------------------------
// Name: CpuNMI()
//-----------------------------------------------------------------------------
void MC_Hardware6502::CpuNMI()
{
    if (m_Cpu6502Run) {
        //PrintStatus(false, "Cpu NMI");
        //mc_Processor6502.NMI();
    }
}
//-Public----------------------------------------------------------------------
// Name: CpuInitializeAndReset()
//-----------------------------------------------------------------------------
void MC_Hardware6502::CpuInitializeAndReset()
{
    CpuMemoryInit();
#if USEINIFILESETTING
    CpuLoadIniFileRoms();
#else
    CpuLoadRoms();
#endif
#if CPU6502_TESTMODE == false
    CpuCegmonukRomMod();
#endif
    ReSizeDisplay();
    mc_VideoDisplay.Forceupdate();
    mc_Processor6502.Reset();
#if CPU6502_TESTMODE
    mc_Processor6502.SetPC(0x0400);
#endif
    PrintStatus(false, "Cpu Initialize And Reset");
    CpuRun();
}
//-Public----------------------------------------------------------------------
// Name: CpuReset()
//-----------------------------------------------------------------------------
void MC_Hardware6502::CpuReset()
{
    bool Cpu6502Run = m_Cpu6502Run;
    m_Cpu6502Run = false;
    ReSizeDisplay();
    mc_VideoDisplay.Forceupdate();
    Sleep(10);
    mc_Processor6502.Reset();
#if CPU6502_TESTMODE
    mc_Processor6502.SetPC(0x0400);
#endif
    PrintStatus(false, "Cpu Reset");
    if (Cpu6502Run) {
        PrintStatus(false, "Cpu Run");
    } else {
        PrintStatus(false, "Cpu Stop");
    }
    Sleep(10);
    m_Cpu6502Run = Cpu6502Run;
}
//-Public----------------------------------------------------------------------
// Name: CpuStop()
//-----------------------------------------------------------------------------
void MC_Hardware6502::CpuStop()
{
    m_Cpu6502Step = false;
    m_Cpu6502Run = false;
    Sleep(10);
    PrintStatus(false, "Cpu Stop");
}
//-Public----------------------------------------------------------------------
// Name: CpuRun()
//-----------------------------------------------------------------------------
void MC_Hardware6502::CpuRun()
{
    PrintStatus(false, "Cpu Run");
    Sleep(10);
    m_Cpu6502Step = false;
    m_Cpu6502Run = true;
}
//-Public----------------------------------------------------------------------
// Name: CpuStep()
//-----------------------------------------------------------------------------
void MC_Hardware6502::CpuStep()
{
    m_Cpu6502Run = false;
    m_Cpu6502Step = true;
}
//-Public----------------------------------------------------------------------
// Name: CpuSetParameters()
//-----------------------------------------------------------------------------
void MC_Hardware6502::CpuSetParameters()
{
    PrintStatus(false, "Cpu SetParameters");
#if CPU6502_TESTMODE
    #if BreakPointMemory
        CpuSetBreakPointMemory(false, 0x0046);
    #endif
    #if BreakPointOpCode
        CpuSetBreakPointOpCode(false, 0x3469);
    #endif
#else
    #if BreakPointMemory
        CpuSetBreakPointMemory(false, 0xF000);
    #endif
    #if BreakPointOpCode
        CpuSetBreakPointOpCode(false, 0xFF00);
    #endif
#endif
}
//-Public----------------------------------------------------------------------
// Name: CpuSetBreakPointOpCode(bool Enable, uint16_t Address)
//-----------------------------------------------------------------------------
void MC_Hardware6502::CpuSetBreakPointOpCode(bool Enable, uint16_t Address)
{
    char buf[256];

    m_BreakPointOpCode.SetFlag = Enable;
    m_BreakPointOpCode.Address = Address;
    m_BreakPointOpCode.Found = false;
    if (m_BreakPointOpCode.SetFlag) {
        snprintf(buf, sizeof(buf), "Cpu Set BreakPoint OpCode $%04X", Address);
        PrintStatus(false, buf);
    }
}
//-Public----------------------------------------------------------------------
// Name: CpuSetBreakPointMemory(bool Enable, uint16_t Address)
//-----------------------------------------------------------------------------
void MC_Hardware6502::CpuSetBreakPointMemory(bool Enable, uint16_t Address)
{
    char buf[256];

    m_BreakPointMemory.SetFlag = Enable;
    m_BreakPointMemory.Address = Address;
    m_BreakPointMemory.Found = false;
    if (m_BreakPointMemory.SetFlag) {
        snprintf(buf, sizeof(buf), "Cpu Set BreakPoint Memory $%04X", Address);
        PrintStatus(false, buf);
    }
}
//-Protected-------------------------------------------------------------------
// Name: CpuSetPC(uint16_t PC)
//-----------------------------------------------------------------------------
void MC_Hardware6502::CpuSetPC(uint16_t PC)
{
    char buf[256];

    snprintf(buf, sizeof(buf), "Cpu Set PC $%04X", PC);
    PrintStatus(false, buf);
    mc_Processor6502.SetPC(0x0400);
}
//-Public----------------------------------------------------------------------
// Name: CpuCalCyclesPer10thSec()
//-----------------------------------------------------------------------------
void MC_Hardware6502::CpuCalCyclesPer10thSec()
{
    long TotalSpeed = (m_CpuSettings.Speed * CPU6502_CLKREFSPEED);
    double CpuSpeed10th = double(TotalSpeed);

    if (mc_Processor6502.m_TotalCyclesPerSec == 0) {
        m_CpuSettings.CyclesPerSec = CpuSpeed10th;
    }  else {
        m_CpuSettings.CyclesPerSec = (double)(mc_Processor6502.m_TotalCyclesPerSec);
        mc_Processor6502.m_TotalCyclesPerSec = 0;
    }
    if (m_Cpu6502Run && !m_Disassembler6502) {
        m_CpuSettings.AvrSpeed = (m_CpuSettings.AvrSpeed * 0.9) + (m_CpuSettings.CyclesPerSec * 0.1);
        m_CpuSettings.AvrBigSpeed = (m_CpuSettings.AvrBigSpeed * 0.9) + (m_CpuSettings.AvrSpeed * 0.1);
        if (m_CpuSettings.Speed) {
            if (m_CpuSettings.AvrSpeed > CpuSpeed10th) {
                m_CpuSettings.SpeedUpDn += (20.0f / (float)(m_CpuSettings.CyclesPerSec / CPU6502_CLKREFSPEED));
            }
            if (m_CpuSettings.AvrSpeed < CpuSpeed10th) {
                m_CpuSettings.SpeedUpDn -= (20.0f / (float)(m_CpuSettings.CyclesPerSec / CPU6502_CLKREFSPEED));
            }
        } else {
            m_CpuSettings.SpeedUpDn = 0.0f;
        }
    }
    if (m_CpuSettings.SpeedUpDn < 0.0f) {
        m_CpuSettings.SpeedUpDn = 0.0f;
    } else if (m_CpuSettings.SpeedUpDn > 10000.0f) {
        m_CpuSettings.SpeedUpDn = 10000.0f;
    }
}
//-Public----------------------------------------------------------------------
// Name: CpuCegmonukRomMod()
//-----------------------------------------------------------------------------
void MC_Hardware6502::CpuCegmonukRomMod()
{
#if(cegmonukRomMod)                                                                 // cegmonuk Res Mod and save Rom
    MemoryLoad(MemoryMonitorRomAddress, MemoryMonitorRomSizeAddress, "MonitorRom/cegmonuk.rom");
    #if HardWareHiResScreen
        m_MemoryWriteOverride = true;
        CpuMemoryMapWrite(0xFBC5, 0x0C);                                            // Mod Rom top Screen
        CpuMemoryMapWrite(0xFBC2, 0x0C);
        CpuMemoryMapWrite(0xFBCB, 0x0C);
        CpuMemoryMapWrite(0xFBBD, 0x0C);
        CpuMemoryMapWrite(0xFBC0, 0xDB);                                            // Mod Rom 48 Lines
        CpuMemoryMapWrite(0xFB8B, 0xDB);                                            // Cls 3k
        CpuMemoryMapWrite(0xFE3B, 0xDB);                                            // Cls 3k
        CpuMemoryMapWrite(0xFE62, 0x0C);                                            // Cls 3k
        m_MemoryWriteOverride = false;
        MemorySave(MemoryMonitorRomAddress, MemoryMonitorRomSizeAddress, "MonitorRom/cegmonuk48x48.rom");
    #endif
    #if HardWareMedResScreen
        m_MemoryWriteOverride = true;
        CpuMemoryMapWrite(0xFBC5, 0x0C);                                            // Mod Rom top Screen
        CpuMemoryMapWrite(0xFBC2, 0x0C);
        CpuMemoryMapWrite(0xFBCB, 0x0C);
        CpuMemoryMapWrite(0xFBBD, 0x0C);
        CpuMemoryMapWrite(0xFBC0, 0xD7);                                            // Mod Rom 32 Lines
        CpuMemoryMapWrite(0xFB8B, 0xD7);                                            // Cls 2k
        CpuMemoryMapWrite(0xFE3B, 0xD7);                                            // Cls 2k
        CpuMemoryMapWrite(0xFE62, 0x08);                                            // Cls 2k
        m_MemoryWriteOverride = false;
        MemorySave(MemoryMonitorRomAddress, MemoryMonitorRomSizeAddress, "MonitorRom/cegmonuk48x32.rom");
    #endif
    #if HardWareLoResScreen
        m_MemoryWriteOverride = true;
        CpuMemoryMapWrite(0xFBC5, 0x0C);                                            // Mod Rom top Screen
        CpuMemoryMapWrite(0xFBC2, 0x0C);
        CpuMemoryMapWrite(0xFBCB, 0x0C);
        CpuMemoryMapWrite(0xFBBD, 0x0C);
        CpuMemoryMapWrite(0xFBC0, 0xD3);                                            // Mod Rom 16 Lines
        CpuMemoryMapWrite(0xFB8B, 0xD3);                                            // Cls 1k
        CpuMemoryMapWrite(0xFE3B, 0xD3);                                            // Cls 1k
        CpuMemoryMapWrite(0xFE62, 0x04);                                            // Cls 1k
        m_MemoryWriteOverride = false;
        MemorySave(MemoryMonitorRomAddress, MemoryMonitorRomSizeAddress, "MonitorRom/cegmonuk48x16.rom");
    #endif
#endif
        CpuMemoryMapWrite(0x01C0, 0x40);  // RTI (Irq)
        CpuMemoryMapWrite(0x0130, 0x40);  // RTI (Nmi)
}
//-Public----------------------------------------------------------------------
// Name: CpuMemoryMapDump()
//-----------------------------------------------------------------------------
void MC_Hardware6502::CpuMemoryMapDump()
{
    PrintHexDump16Bit("Memory Dump", &m_MemoryMap, sizeof(m_MemoryMap), 0);
    if (m_Uart6850.Input.Index > 0) {
        PrintHexDump("Load Buffer Dump", &m_Uart6850.Input.Buffer, m_Uart6850.Input.Index);
    }
    if (m_Uart6850.Output.Index > 0) {
        PrintHexDump("Save Buffer Dump", &m_Uart6850.Output.Buffer, m_Uart6850.Output.Index);
    }
}
//-Public----------------------------------------------------------------------
// Name: CpuMemoryMapDump(uint16_t StartAddress, uint16_t EndAddress)
//-----------------------------------------------------------------------------
void MC_Hardware6502::CpuMemoryMapDump(uint16_t StartAddress, uint16_t EndAddress)
{
    long MemSize = (EndAddress - StartAddress) + 1;

    if (MemSize >= 0 && MemSize <= 0x10000 && StartAddress <= EndAddress) {
        PrintHexDump16Bit("Memory Dump", &m_MemoryMap[StartAddress], (EndAddress - StartAddress) + 1, StartAddress);
    } else {
        printf("CpuMemoryMapDump Error %04lX %04lX\r\n", StartAddress, EndAddress);
    }
}
//-Public----------------------------------------------------------------------
// Name: CpuLoadFile()
//-----------------------------------------------------------------------------
void  MC_Hardware6502::CpuLoadFile()
{
    LoadUartData(FilenameOpenDlg(LOADSAVE_FILE_FILTER, m_App_Hwnd));
}
//-Public----------------------------------------------------------------------
// Name: CpuSaveFile()
//-----------------------------------------------------------------------------
void   MC_Hardware6502::CpuSaveFile()
{
    SaveUartData(FilenameSaveDlg(LOADSAVE_FILE_FILTER, m_App_Hwnd));
}
//-Public----------------------------------------------------------------------
// Name: CpuMemoryLoadFile()
//-----------------------------------------------------------------------------
void   MC_Hardware6502::CpuMemoryLoadFile()
{
    MemoryLoadIntelFormat(MemoryMapAddress, MemoryMapEndAddress, FilenameOpenDlg(LOADHEX_FILE_FILTER, m_App_Hwnd));
}
//-Public----------------------------------------------------------------------
// Name: CpuPrintMemoryInfo()
//-----------------------------------------------------------------------------
void   MC_Hardware6502::CpuPrintMemoryInfo()
{
    printf("Address $%04X-$%04X Size $%04X Read/Write    Ram                                    \r\n", MemoryRamAddress, MemoryRamEndAddress, MemoryRamSizeAddress);
    printf("Address $%04X-$%04X Size $%04X Read          Ext Rom                    (If Loaded) \r\n", MemoryExtRomAddress, MemoryExtRomEndAddress, MemoryExtRomSizeAddress);
    printf("Address $%04X-$%04X Size $%04X Read/Write    Parallel Port Controller               \r\n", MemoryParPortAddress, MemoryParPortEndAddress, MemoryParPortSizeAddress);
    printf("Address $%04X-$%04X Size $%04X Read          Basic5 Rom                 (If Loaded) \r\n", MemoryBasic5RomAddress, MemoryBasic5RomEndAddress, MemoryBasic5RomSizeAddress);
    printf("Address $%04X-$%04X Size $%04X Read          Disk Rom                   (If Loaded) \r\n", MemoryDiskRomAddress, MemoryDiskRomEndAddress, MemoryDiskRomSizeAddress);
    printf("Address $%04X-$%04X Size $%04X Read          Basic Roms                             \r\n", MemoryBasicRomAddress, MemoryBasicRomEndAddress, MemoryBasicRomSizeAddress);
    printf("Address $%04X-$%04X Size $%04X Read/Write    Disk Controller                        \r\n", MemoryDiskAddress, MemoryDiskEndAddress, MemoryDiskSizeAddress);
    printf("Address $%04X-$%04X Size $%04X Read/Write    Video Ram                              \r\n", MemoryVideoAddress, MemoryVideoEndAddress, MemoryVideoSizeAddress);
    printf("Address $%04X-$%04X Size $%04X Read/Write    Keyboard                               \r\n", MemoryKeyboardAddress, MemoryKeyboardEndAddress, MemoryKeyboardSizeAddress);
    printf("Address $%04X-$%04X Size $%04X Read/Write    6850 ACIA Chip                         \r\n", Memory6850Address, Memory6850EndAddress, Memory6850SizeAddress);
    printf("Address $%04X-$%04X Size $%04X Read          Monitor Rom                            \r\n", MemoryMonitorRomAddress, MemoryMonitorRomEndAddress, MemoryMonitorRomSizeAddress);
}












//*****************************************************************************  
// Protected Code
//*****************************************************************************

//-Protected-------------------------------------------------------------------
// Name: CpuMemoryInit()
//-----------------------------------------------------------------------------
void MC_Hardware6502::CpuMemoryInit()
{
    m_Cpu6502Run = false;
    m_Disassembler6502 = false;
    m_MemoryWriteOverride = false;
#if CPU6502_TESTMODE
    memset(&m_MemoryMap, 0xFF, sizeof(m_MemoryMap));
#else
    memset(&m_MemoryMap, 0xFF, sizeof(m_MemoryMap));
    memset(&m_MemoryMap[MemoryRamAddress], 0x00, MemoryRamSizeAddress);
    memset(&m_MemoryMap[MemoryExtRomAddress], 0xFF, MemoryExtRomSizeAddress);
    memset(&m_MemoryMap[MemoryParPortAddress], 0x00, MemoryParPortSizeAddress);
    memset(&m_MemoryMap[MemoryDiskRomAddress], 0xFF, MemoryDiskRomSizeAddress);
    memset(&m_MemoryMap[MemoryBasicRomAddress], 0xFF, MemoryBasicRomSizeAddress);
    memset(&m_MemoryMap[MemoryDiskAddress], 0x00, MemoryDiskSizeAddress);
    memset(&m_MemoryMap[MemoryVideoAddress], 0x00, MemoryVideoSizeAddress);
    memset(&m_MemoryMap[MemoryVideoAddress + MemoryVideoSizeAddress], 0x20, 0x40);    //Bug Fix 48 Line Scroll Up
    memset(&m_MemoryMap[MemoryKeyboardAddress], 0xFF, MemoryKeyboardSizeAddress);
    memset(&m_MemoryMap[Memory6850Address], 0x00, Memory6850SizeAddress);
    memset(&m_MemoryMap[MemoryMonitorRomAddress], 0xFF, MemoryMonitorRomSizeAddress);
#endif
    memset(&m_MemoryKeyScan, 0x00, sizeof(m_MemoryKeyScan));
    m_MemoryKeyScan.KeysDone = true;
    memset(&m_Uart6850, 0x00, sizeof(m_Uart6850));
    Cpu6850Uartinit();
    PrintStatus(false, "Hardware Cpu Memory Initialize");
 }
//-Protected-------------------------------------------------------------------
// Name: Cpu6850Uartinit()
//-----------------------------------------------------------------------------
void MC_Hardware6502::Cpu6850Uartinit()
{
    m_Uart6850.Registers_SR.byte = 0;
    m_Uart6850.Registers_SR.bits.TDRE = 1;                                     // we are always ready to output data
    m_Uart6850.Registers_SR.bits.RDRF = 0;
    m_Uart6850.Registers_SR.bits.DCD = 1;
    m_Uart6850.Registers_SR.bits.CTS = 1;
    m_Uart6850.Registers_SR.bits.FE = 1;
    m_Uart6850.Registers_SR.bits.OVRN = 0;
    m_Uart6850.Registers_SR.bits.PE = 1;
    m_Uart6850.Registers_SR.bits.IRQ = 1;
    memset(&m_Uart6850.Input.Buffer, 0x00, sizeof(m_Uart6850.Input.Buffer));
    m_Uart6850.Input.CharData = 0;
    m_Uart6850.Input.ProcessedIndex = 0;
    m_Uart6850.Input.Index = 0;
    memset(&m_Uart6850.Output.Buffer, 0x00, sizeof(m_Uart6850.Output.Buffer));
    m_Uart6850.Output.CharData = 0;
    m_Uart6850.Output.ProcessedIndex = 0;
    m_Uart6850.Output.Index = 0;
#if CPU6502_TESTMODE == false
    m_MemoryMap[DATA_6850ADDR] = m_Uart6850.Input.CharData;
    m_MemoryMap[CTRL_6850ADDR] = m_Uart6850.Registers_SR.byte;
#endif
    PrintStatus(false, "Hardware 6850 ACIA Initialize");
}
//-Protected-------------------------------------------------------------------
// Name: CpuEmu6850UartRead(uint16_t address)
//-----------------------------------------------------------------------------
uint8_t MC_Hardware6502::CpuEmu6850UartRead(uint16_t address)
{
    static uint16_t s_RxDelayTick = 0;
    static uint16_t s_TxDelayTick = 0;
    switch (address) {
        case CTRL_6850ADDR: {
            s_RxDelayTick++;
            s_TxDelayTick++;
            uint8_t flags = 0;
            if (m_Uart6850.Input.ProcessedIndex < m_Uart6850.Input.Index) {
                if (m_Uart6850.Registers_SR.bits.RDRF) {
                    if (s_RxDelayTick > 10) {
                        s_RxDelayTick = 0;
                        m_Uart6850.Registers_SR.bits.RDRF = 0;
                    }
                    flags |= RDRF6850;
                }
            } 
            if (m_Uart6850.Registers_SR.bits.TDRE) {
                if (s_TxDelayTick > 10) {
                    s_TxDelayTick = 0;
                    m_Uart6850.Registers_SR.bits.TDRE = 0;
                 }
            } else {
                flags |= TDRE6850;
            }
            return flags;
            break;
        }
        case DATA_6850ADDR: {
            m_Uart6850.Input.CharData = m_Uart6850.Input.Buffer[m_Uart6850.Input.ProcessedIndex++];
            m_Uart6850.Input.ProcessedIndex %= sizeof(m_Uart6850.Input.Buffer);
            m_Uart6850.Registers_SR.bits.RDRF = 1;
            return m_Uart6850.Input.CharData;
            break;
        }
        default:
            break;
    }
    return 0xff;
}
//-Protected-------------------------------------------------------------------
// Name: CpuEmu6850UartWrite(uint16_t address, uint8_t value)
//-----------------------------------------------------------------------------
void MC_Hardware6502::CpuEmu6850UartWrite(uint16_t address, uint8_t value)
{
    switch (address) {
        case CTRL_6850ADDR: {
            m_MemoryMap[address] = value;
            m_Uart6850.Registers_SR.byte = value;
            // TODO: decode baudrate, mode, break control, interrupt
            break;
        }
        case DATA_6850ADDR: {
            m_MemoryMap[address] = value;
            m_Uart6850.Output.CharData = value;
            m_Uart6850.Output.Buffer[m_Uart6850.Output.ProcessedIndex++] = m_Uart6850.Output.CharData;
            m_Uart6850.Output.ProcessedIndex %= sizeof(m_Uart6850.Output.Buffer);
            m_Uart6850.Output.Index = m_Uart6850.Output.ProcessedIndex;
            m_Uart6850.Registers_SR.bits.TDRE = 1;
            break;
        }
        default: {
            break;
        }
    }
}
//-Protected-------------------------------------------------------------------
// Name: CpuEmuKeyboard(uint8_t* MemData, bool RW)
//-----------------------------------------------------------------------------
uint8_t MC_Hardware6502::CpuEmuKeyboard(uint16_t address, bool RW)
{
    uint8_t Data;
    uint8_t CpuRow;
    uint8_t RowScanCode;

    if (RW) {
        Data = 0xFF;
        CpuRow = ((m_MemoryKeyScan.CpuRow ^ 0xFF) & 0x01);
        if (CpuRow && m_MemoryKeyScan.KeysDone == false) {
            if (m_MemoryKeyScan.caplock) {
                Data = (Data & 0xFE);
            }
            if (m_MemoryKeyScan.RShift) {
                Data = (Data & 0xFD);
            }
            if (m_MemoryKeyScan.LShift) {
                Data = (Data & 0xFB);
            }
            if (m_MemoryKeyScan.ctrl) {
                Data = (Data & 0xBF);
            }
        }
        CpuRow = ((m_MemoryKeyScan.CpuRow | 0x01) ^ 0xFF);
        RowScanCode = (m_MemoryKeyScan.RowScanCode | 0x01) ^ 0xFF;
        if ((CpuRow & RowScanCode) && m_MemoryKeyScan.KeysDone == false) {
            Data = (Data & m_MemoryKeyScan.ColScanCode);
            m_MemoryKeyScan.CpuCount++;
            if (m_MemoryKeyScan.CpuCount > 4) {
                m_MemoryKeyScan.CpuCount = 0;
                m_MemoryKeyScan.KeysDone = true;
            }
        }
    } else {
        Data = m_MemoryMap[address];
        m_MemoryKeyScan.CpuRow = Data;
    }
    return Data;
}
//-Protected-------------------------------------------------------------------
// Name: CpuLoadIniFileRoms()
//-----------------------------------------------------------------------------
void MC_Hardware6502::CpuLoadIniFileRoms()
{
    std::string StringLine;
    std::string StringFind;
    std::size_t Found;
    std::string Name;
    std::string Value;

    for (auto LineItem = m_IniFileString.LineData.begin(); LineItem != m_IniFileString.LineData.end(); ++LineItem) {
        StringLine = LineItem->c_str();
        if (StringLine.length() > 0) {
            StringFind = "ExtRomFile = ";
            Found = StringLine.find(StringFind);
            if (Found != std::string::npos) {
                StringLine.replace(StringLine.find(StringFind), StringFind.length(), "");
                if (StringLine.length() > 0) {
                    MemoryLoad(MemoryExtRomAddress, MemoryExtRomSizeAddress, StringLine);
                }
            }
            StringFind = "DiskRomFile = ";
            Found = StringLine.find(StringFind);
            if (Found != std::string::npos) {
                StringLine.replace(StringLine.find(StringFind), StringFind.length(), "");
                if (StringLine.length() > 0) {
                    MemoryLoad(MemoryDiskRomAddress, MemoryDiskRomAddress, StringLine);
                }
            }
            StringFind = "BasicRomFile = ";
            Found = StringLine.find(StringFind);
            if (Found != std::string::npos) {
                StringLine.replace(StringLine.find(StringFind), StringFind.length(), "");
                if (StringLine.length()> 0) {
                    MemoryLoad(MemoryBasicRomAddress, MemoryBasicRomSizeAddress, StringLine);
                }
            }
            StringFind = "MonitorRomFile = ";
            Found = StringLine.find(StringFind);
            if (Found != std::string::npos) {
                StringLine.replace(StringLine.find(StringFind), StringFind.length(), "");
                if (StringLine.length() > 0) {
                    MemoryLoad(MemoryMonitorRomAddress, MemoryMonitorRomSizeAddress, StringLine);
                }
            }
            StringFind = "CharacterSetRomFile = ";
            Found = StringLine.find(StringFind);
            if (Found != std::string::npos) {
                StringLine.replace(StringLine.find(StringFind), StringFind.length(), "");
                if (StringLine.length() > 0) {
                    mc_VideoDisplay.CpuEmuLoadCharacterSetRom(StringLine);
                }
            }
        }
    }
}
//-Protected-------------------------------------------------------------------
// Name: CpuLoadRoms()
//-----------------------------------------------------------------------------
void MC_Hardware6502::CpuLoadRoms()
{
#if CPU6502_TESTMODE
    MemoryLoad(0x000A, 65526, "GoodRoms/6502_functional_test.bin");
#else
    #if Basic5Test
        MemoryLoad(0x9000, 0x0800, "GoodRoms/Roms/Basic5ROM");
        MemoryLoad(0x9800, 0x0800, "GoodRoms/Roms/BasicXROM");
        MemoryLoad(0xA000, 0x0800, "GoodRoms/Roms/Basic1ROM");
        MemoryLoad(0xA800, 0x0800, "GoodRoms/Roms/Basic2ROM");
        MemoryLoad(0xB000, 0x0800, "GoodRoms/Roms/Basic3ROM");
        MemoryLoad(0xB800, 0x0800, "GoodRoms/Roms/PremierBasic4ROM");
    #else
        MemoryLoad(MemoryExtRomAddress, MemoryExtRomSizeAddress, "GoodRoms/ExtMonitor.rom");                                                // Ext Rom
        //MemoryLoad(MemoryDiskRomAddress, MemoryDiskRomAddress, "GoodRoms/Disk.rom");                                                      // Disk Rom
        if (m_BasicSelectUk101OrOsi) {
            MemoryLoad(MemoryBasicRomAddress, MemoryBasicRomSizeAddress, "GoodRoms/BASIC-OSI-8k.rom");                                      // Basic Osi Rom
        } else {
            MemoryLoad(MemoryBasicRomAddress, MemoryBasicRomSizeAddress, "GoodRoms/BASIC-UK101-8k.rom");                                    // Basic CompuKit Rom
        }
    #endif

    #if F000OrF800_Rom
        #if HardWareHiResScreen
            MemoryLoad(MemoryMonitorRomAddress, MemoryMonitorRomSizeAddress, "GoodRoms/WEMonUK101_48x48.rom");                             // Monitor Rom WEMonUK101 48x48
        #else
            MemoryLoad(MemoryMonitorRomAddress, MemoryMonitorRomSizeAddress, "GoodRoms/WEMonUK101.rom");                                   // Monitor Rom WEMonUK101
        #endif
    #else
        #if HardWareHiResScreen
            MemoryLoad(MemoryMonitorRomAddress, MemoryMonitorRomSizeAddress, "GoodRoms/cegmonuk48x48.rom");                                // Monitor  cegmonuk 48x48
        #endif
        #if HardWareMedResScreen     
            MemoryLoad(MemoryMonitorRomAddress, MemoryMonitorRomSizeAddress, "GoodRoms/cegmonuk48x32.rom");                                // Monitor  cegmonuk 48x32
        #endif
        #if HardWareLoResScreen
            MemoryLoad(MemoryMonitorRomAddress, MemoryMonitorRomSizeAddress, "GoodRoms/cegmonuk48x16.rom");                                // Monitor  cegmonuk 48x16
            //MemoryLoad(MemoryMonitorRomAddress, MemoryMonitorRomSizeAddress, "GoodRoms/cegmonuk.rom");                                   // Monitor  cegmonuk
            //MemoryLoadIntelFormat(MemoryMonitorRomAddress, MemoryMonitorRomSizeAddress, "GoodRoms/cegmonuk.int");                        // Monitor  cegmonuk Intel File Format  
        #endif

    #endif
    mc_VideoDisplay.CpuEmuLoadCharacterSetRom("GoodRoms/CharacterSetuk101.rom");                                                            // Character Set Rom
#endif
}
//-Protected-------------------------------------------------------------------
// Name: MemoryLoad(uint16_t MemoryAddress , uint16_t MemorySize, std::string FileName)
//-----------------------------------------------------------------------------
void MC_Hardware6502::MemoryLoad(uint16_t MemoryAddress, uint16_t MemorySize, std::string FileName)
{
    streampos size;
    uint8_t* memblock;
    uint32_t FileSize = 0;
    uint32_t MemMapMaxSize = MemoryAddress + MemorySize;
    char StatusMsg[256];
    bool Error = false;

    StatusMsg[0] = 0;
    ifstream file(FileName, ios::in | ios::binary | ios::ate);
    if (file.is_open()) {
        size = file.tellg();
        FileSize = (uint32_t)size;
        memblock = new uint8_t[FileSize];
        file.seekg(0, ios::beg);
        file.read((char*)memblock, size);
        file.close();
        if (FileSize <= MemorySize && MemMapMaxSize <= 0x10000) {
            memcpy(&m_MemoryMap[MemoryAddress], memblock, FileSize);
            snprintf(StatusMsg, sizeof(StatusMsg), "Load Ram/Rom ($%04X - $%04X) %s", MemoryAddress , MemoryAddress + (FileSize - 1), FileName.c_str());
        } else {
            snprintf(StatusMsg, sizeof(StatusMsg), "Load Ram/Rom file to Big for Memory(%06X,%06X) Slot File %s", FileSize, MemMapMaxSize -1, FileName.c_str());
            Error = true;
        }
        delete[] memblock;
    } else {
        snprintf(StatusMsg, sizeof(StatusMsg), "Load Ram/Rom Unable to open file %s", FileName.c_str());
        Error = true;
    }
    PrintStatus(Error, StatusMsg);
}
//-Protected-------------------------------------------------------------------
// Name: MemorySave(uint16_t MemoryAddress , uint16_t MemorySize, std::string FileName)
//-----------------------------------------------------------------------------
void MC_Hardware6502::MemorySave(uint16_t MemoryAddress, uint16_t MemorySize, std::string FileName)
{
    streampos size;
    uint8_t* memblock;
    char StatusMsg[256];
    bool Error = false;

    StatusMsg[0] = 0;
    size = MemorySize;
    fstream file(FileName, ios::out | ios::binary | ios::ate);
    if (file.is_open()) {
        memblock = new uint8_t[(uint16_t)size];
        memcpy(memblock, &m_MemoryMap[MemoryAddress], (size_t)size);
        file.seekg(0, ios::beg);
        file.write((char*)memblock, size);
        file.close();
        delete[] memblock;
        snprintf(StatusMsg, sizeof(StatusMsg), "Save Ram/Rom ($%04X - $%04X) %s", MemoryAddress, MemoryAddress + (MemorySize - 1), FileName.c_str());
    } else {
        snprintf(StatusMsg, sizeof(StatusMsg), "Save Ram/Rom Unable to open file %s", FileName.c_str());
        Error = true;
    }
    PrintStatus(Error, StatusMsg);
}
//-Protected-------------------------------------------------------------------
// Name: MemoryLoadIntelFormat(uint16_t MemoryAddress, uint16_t MemorySize, std::string FileName)
//-----------------------------------------------------------------------------
void MC_Hardware6502::MemoryLoadIntelFormat(uint16_t MemoryAddress, uint16_t MemorySize, std::string FileName)
{
    uint16_t l;
    size_t LineLen;
    size_t EndOfDataLine;
    std::string StringLine;
    std::string StringLineStart;
    std::string StringNumberOfBytes;
    std::string StringMemAddress;
    std::string StringRecordType;
    std::string StringData;
    std::string StringByte;
    std::string StringCrc;
    std::string StringCrcCal;
    uint8_t NumberOfBytes;
    uint16_t MemAddress;
    uint8_t RecordType;
    uint8_t Data;
    uint8_t Crc;
    uint8_t LineCrc;
    char StatusMsg[256];
    bool Error = false;

    StatusMsg[0] = 0;
    if (FileName.length() == 0) {
        return;
    }
    ifstream file(FileName, ios::in | ios::ate);
    if (file.is_open()) {
        snprintf(StatusMsg, sizeof(StatusMsg), "Load Intel File Format %s", FileName.c_str());
        PrintStatus(false, StatusMsg);
        file.seekg(0, ios::beg);
        while (std::getline(file, StringLine)) {
            if (StringLine.length() >= 11) {
                LineLen = strlen(StringLine.c_str());
                StringCrc = StringLine.substr(LineLen - 2, 2);
                StringCrcCal = StringLine.substr(1, LineLen - 3);
                LineCrc = 0x00;
                for (l = 0; l < StringCrcCal.length(); l += 2) {
                    StringByte = StringCrcCal.substr(l, 2);
                    Data = (uint8_t)Hex2Dec(StringByte);
                    LineCrc += Data;
                }
                LineCrc ^= 0xFF;
                LineCrc++;
                Crc = (uint8_t)Hex2Dec(StringCrc);
                if (LineCrc == Crc) {
                    StringLineStart = StringLine.substr(0, 1);
                    StringNumberOfBytes = StringLine.substr(1, 2);
                    StringMemAddress = StringLine.substr(3, 4);
                    StringRecordType = StringLine.substr(7, 2);
                    EndOfDataLine = LineLen - (8 + 2);
                    StringData = StringLine.substr(9, EndOfDataLine);
                    if (StringData.length()) {
                        NumberOfBytes = (uint8_t)Hex2Dec(StringNumberOfBytes);
                        MemAddress = Hex2Dec(StringMemAddress);
                        RecordType = (uint8_t)Hex2Dec(StringRecordType);
                        if (RecordType == 0 && NumberOfBytes > 0) {
                            for (l = 0; l < StringData.length(); l += 2) {
                                StringByte = StringData.substr(l, 2);
                                Data = (uint8_t)Hex2Dec(StringByte);
                                if (MemAddress < MemorySize) {
                                    m_MemoryWriteOverride = true;
                                    CpuMemoryWrite(MemoryAddress + MemAddress, Data);
                                    m_MemoryWriteOverride = false;
                                }
                                MemAddress++;
                            }
                        }
                    }
                } else {
                    StringNumberOfBytes = StringLine.substr(1, 2);
                    NumberOfBytes = (uint8_t)Hex2Dec(StringNumberOfBytes);
                    if (NumberOfBytes > 0) {
                        snprintf(StatusMsg, sizeof(StatusMsg), "Crc Error %02X %02X %s", Crc, LineCrc, StringLine.c_str());
                        PrintStatus(true, StatusMsg);
                    }
                }
            }
        }
        file.close();
    } else {
        snprintf(StatusMsg, sizeof(StatusMsg), "Unable to open file %s", FileName.c_str());
        PrintStatus(true, StatusMsg);
    }
}
//-Protected-------------------------------------------------------------------
// Name: FilenameOpenDlg(const char* filter , HWND hwnd)
//-----------------------------------------------------------------------------
std::string MC_Hardware6502::FilenameOpenDlg(const char* filter, HWND hwnd)
{
    OPENFILENAMEA OpenFileDialog;
    char fileName[MAX_PATH] = "";
    std::string fileNameStr;

    ZeroMemory(&OpenFileDialog, sizeof(OpenFileDialog));
    ZeroMemory(&fileName, sizeof(fileName));
    fileName[0] = 0;
    OpenFileDialog.lStructSize = sizeof(OPENFILENAMEA);
    OpenFileDialog.hwndOwner = hwnd;
    OpenFileDialog.lpstrFilter = filter;
    OpenFileDialog.lpstrFile = fileName;
    OpenFileDialog.nMaxFile = MAX_PATH;
    OpenFileDialog.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_NOCHANGEDIR;
    OpenFileDialog.lpstrDefExt = "txt";
    OpenFileDialog.lpstrTitle = { "Open File" };
    if (GetOpenFileNameA(&OpenFileDialog)) {
        fileNameStr = fileName;
    }
    return fileNameStr;
}
//-Protected-------------------------------------------------------------------
// Name: FilenameSaveDlg(const char* filter, HWND hwnd)
//-----------------------------------------------------------------------------
std::string MC_Hardware6502::FilenameSaveDlg(const char* filter, HWND hwnd)
{
    OPENFILENAMEA SaveFileDialog;
    char fileName[MAX_PATH];
    std::string fileNameStr;

    ZeroMemory(&SaveFileDialog, sizeof(SaveFileDialog));
    ZeroMemory(&fileName, sizeof(fileName));
    fileName[0] = 0;
    SaveFileDialog.lStructSize = sizeof(OPENFILENAMEA);
    SaveFileDialog.hwndOwner = hwnd;
    SaveFileDialog.lpstrFilter = filter;
    SaveFileDialog.lpstrFile = fileName;
    SaveFileDialog.nMaxFile = MAX_PATH;
    SaveFileDialog.Flags = OFN_EXPLORER | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR;
    SaveFileDialog.lpstrDefExt = "txt";
    SaveFileDialog.lpstrTitle = { "Save File" };
    if (GetSaveFileNameA(&SaveFileDialog)) {
        fileNameStr = fileName;
    }
    return fileNameStr;
}
//-Protected-------------------------------------------------------------------
// Name: LoadUartData(std::string FileName)
//-----------------------------------------------------------------------------
void MC_Hardware6502::LoadUartData(std::string FileName)
{
    streampos size;
    uint8_t* memblock;
    uint16_t FileSize = 0;

    if (FileName.length() == 0) {
        return;
    }
    memset(&m_Uart6850.Input, 0x00, sizeof(m_Uart6850.Input));
    ifstream file(FileName, ios::in | ios::binary | ios::ate);
    if (file.is_open()) {
        size = file.tellg();
        memblock = new uint8_t[(uint16_t)size];
        file.seekg(0, ios::beg);
        file.read((char*)memblock, size);
        file.close();
        FileSize = (uint16_t)size;
        if (FileSize < UARTBUFFER6850) {
            memcpy(&m_Uart6850.Input.Buffer, memblock, FileSize);
            m_Uart6850.Input.Index = FileSize;
        } else {
            printf("file to Big for Memory Slot File %s\r\n", FileName.c_str());
        }
        delete[] memblock;
    } else {
        printf("Unable to Open file %s\r\n", FileName.c_str());
    }
}
//-Protected-------------------------------------------------------------------
// Name: SaveUartData(std::string FileName)
//-----------------------------------------------------------------------------
void MC_Hardware6502::SaveUartData(std::string FileName)
{
    streampos size;
    uint8_t* memblock;
    uint16_t FileSize = 0;

    if (FileName.length() == 0) {
        return;
    }
    size = m_Uart6850.Output.Index;
    fstream file(FileName, ios::out | ios::binary | ios::ate);
    if (file.is_open()) {
        memblock = new uint8_t[(uint16_t)size];
        memcpy(memblock ,&m_Uart6850.Output.Buffer, (size_t)size);
        file.seekg(0, ios::beg);
        file.write((char*)memblock, size);
        file.close();
        delete[] memblock;
    } else {
        printf("Unable to Save file %s\r\n", FileName.c_str());
    }
}
//-Protected-------------------------------------------------------------------
// Name: Hex2Dec(std::string s)
//-----------------------------------------------------------------------------
uint16_t MC_Hardware6502::Hex2Dec(std::string s)
{
    uint16_t i = 0;

    std::stringstream ss(s);
    ss >> std::hex >> i;
    return i;
}
//-Protected-------------------------------------------------------------------
// Name: PrintHexDump(char *desc,void *addr,long len)
//-----------------------------------------------------------------------------
void MC_Hardware6502::PrintHexDump(const char* desc, void* addr, long len)
{
    long i;
    uint8_t* pc = (uint8_t*)addr;
    char buff[20];
    uint8_t Data;

    if (desc != NULL) {
        printf("\n%s:\r\n", desc);
    }
    if (len == 0) {
        printf("\nZERO LENGTH\r\n");
        return;
    }
    else if (len < 0) {
        printf("\nNEGATIVE LENGTH: %ld\r\n", len);
        return;
    }
    printf("Address:  00 01 02 03 04 05 06 07  08 09 0A 0B 0C 0D 0E 0F\r\n");
    printf("--------  ------------------------------------------------\r\n");
    for (i = 0; i < len; i++) {
        if ((i % 16) == 0) {
            if (i != 0) {
                printf("  | %s |\r\n", buff);
            }
            printf("%08lX ", i);
        }
        Data = pc[i];
        if ((i % 16) == 8)
            printf("  %02X", Data);
        else
            printf(" %02X", Data);

        if ((Data < 0x20) || (Data > 0x7e))
            buff[i % 16] = '.';
        else
            buff[i % 16] = Data;

        buff[(i % 16) + 1] = '\0';
    }
    while ((i % 16) != 0) {
        if ((i % 16) == 8)
            printf("    ");
        else
            printf("   ");

        i++;
    }
    printf("  | %s |\r\n", buff);
    printf("--------  ------------------------------------------------\r\n");
}
//-Protected-------------------------------------------------------------------
// Name: PrintHexDump16Bit(const char* desc, void* addr, long len, long offset)
//-----------------------------------------------------------------------------
void MC_Hardware6502::PrintHexDump16Bit(const char* desc, void* addr, long len, long offset)
{
    long i;
    uint8_t* pc = (uint8_t*)addr;
    char buff[20];
    uint8_t Data;

    if (desc != NULL) {
        printf("\n%s:\r\n", desc);
    }
    if (len == 0) {
        printf("\nZERO LENGTH\r\n");
        return;
    } else if (len < 0) {
        printf("\nNEGATIVE LENGTH: %ld\r\n", len);
        return;
    }
    printf("Addr:  00 01 02 03 04 05 06 07  08 09 0A 0B 0C 0D 0E 0F\r\n");
    printf("-----  ------------------------------------------------\r\n");
    for (i = 0; i < len; i++) {
        if ((i % 16) == 0) {
            if (i != 0) {
                printf("  | %s |\r\n", buff);
            }
            printf("$%04lX ", i + offset);
        }
        Data = pc[i];
        if ((i % 16) == 8)
            printf("  %02X", Data);
        else
            printf(" %02X", Data);

        if ((Data < 0x20) || (Data > 0x7e))
            buff[i % 16] = '.';
        else
            buff[i % 16] = Data;

        buff[(i % 16) + 1] = '\0';
    }
    while ((i % 16) != 0) {
        if ((i % 16) == 8)
            printf("    ");
        else
            printf("   ");

        i++;
    }
    printf("  | %s |\r\n", buff);
    printf("-----  ------------------------------------------------\r\n");
}
//-Protected-------------------------------------------------------------------
// Name: IniFileDefault()
//-----------------------------------------------------------------------------
void MC_Hardware6502::IniFileDefault()
{
    std::string LineData;

    PrintStatus(false, "Set IniFile Default");

    LineData = IniFileHeaderAddress("Ram", MemoryRamAddress, MemoryRamSizeAddress);
    m_IniFileString.LineData.push_back(LineData);
    LineData = "RamFile = ";
    m_IniFileString.LineData.push_back(LineData);

    LineData = IniFileHeaderAddress("ExtRom", MemoryExtRomAddress, MemoryExtRomSizeAddress);
    m_IniFileString.LineData.push_back(LineData);
    LineData = "ExtRomFile = GoodRoms/ExtMonitor.rom";
    m_IniFileString.LineData.push_back(LineData);

    LineData = IniFileHeaderAddress("DiskRom", MemoryDiskRomAddress, MemoryDiskRomSizeAddress);
    m_IniFileString.LineData.push_back(LineData);
//    LineData = "DiskRomFile = GoodRoms/Disk.rom";
    LineData =  "DiskRomFile = ";
    m_IniFileString.LineData.push_back(LineData);

    LineData = IniFileHeaderAddress("BasicRom", MemoryBasicRomAddress, MemoryBasicRomSizeAddress);
    m_IniFileString.LineData.push_back(LineData);
    LineData = "BasicRomFile = GoodRoms/BASIC-UK101-8k.rom";
    m_IniFileString.LineData.push_back(LineData);

    LineData = IniFileHeaderAddress("MonitorRom", MemoryMonitorRomAddress, MemoryMonitorRomSizeAddress);
    m_IniFileString.LineData.push_back(LineData);
    if (HardWareHiResScreen) {
        LineData = "MonitorRomFile = GoodRoms/cegmonuk48x48.rom";
    } else if (HardWareMedResScreen) {
        LineData = "MonitorRomFile = GoodRoms/cegmonuk48x32.rom";
    } else {
        LineData = "MonitorRomFile = GoodRoms/cegmonuk48x16.rom";
    }
    m_IniFileString.LineData.push_back(LineData);

    LineData = IniFileHeaderAddress("CharacterSetRom", 0x0000, 0x0800);
    m_IniFileString.LineData.push_back(LineData);
    LineData = "CharacterSetRomFile = GoodRoms/CharacterSetuk101.rom";
    m_IniFileString.LineData.push_back(LineData);
}
//-Protected-------------------------------------------------------------------
// Name: IniFileAddress(std::string Header, uint16_t MemAddress, uint16_t MemSize)
//-----------------------------------------------------------------------------
std::string MC_Hardware6502::IniFileHeaderAddress(std::string Header, uint16_t MemAddress, uint16_t MemSize)
{
    char Data[256];
    std::string StrData;

    memset(&Data, 0x00, sizeof(Data));
    snprintf(Data, sizeof(Data), "[%s $%04X-$%04X]", Header.c_str(), MemAddress, (MemAddress + MemSize) - 1);
    StrData = Data;
    return StrData;
}
//-Protected-------------------------------------------------------------------
// Name: IniFileRead(std::string FileName)
//-----------------------------------------------------------------------------
bool MC_Hardware6502::IniFileRead(std::string FileName)
{
    std::string LineData;
    char StatusMsg[256];
    bool Error = false;

    StatusMsg[0] = 0;
    if (FileName.length() == 0) {
        return Error;
    }
    ifstream file(FileName, ios::in | ios::ate);
    if (file.is_open()) {
        m_IniFileString.LineData.clear();
        m_IniFileString.LineData.shrink_to_fit();
        snprintf(StatusMsg, sizeof(StatusMsg), "IniFileRead %s", FileName.c_str());
        PrintStatus(Error, StatusMsg);
        file.seekg(0, ios::beg);
        while (std::getline(file, LineData)) {
            if (LineData.length() > 0) {
                m_IniFileString.LineData.push_back(LineData);
            }
        }
        file.close();
    } else {
        Error = true;
        snprintf(StatusMsg, sizeof(StatusMsg), "IniFileRead Unable to open file %s", FileName.c_str());
        PrintStatus(Error, StatusMsg);
    }
    return Error;
}
//-Protected-------------------------------------------------------------------
// Name: IniFileWrite(std::string FileName)
//-----------------------------------------------------------------------------
bool MC_Hardware6502::IniFileWrite(std::string FileName)
{
    std::string StringLine;
    char StatusMsg[256];
    bool Error = false;

    StatusMsg[0] = 0;
    if (FileName.length() == 0) {
        return Error;
    }
    fstream file(FileName, ios::out | ios::ate);
    if (file.is_open()) {
        snprintf(StatusMsg, sizeof(StatusMsg), "IniFileWrite %s", FileName.c_str());
        PrintStatus(Error, StatusMsg);
        file.seekg(0, ios::beg);
        for (auto LineItem = m_IniFileString.LineData.begin(); LineItem != m_IniFileString.LineData.end(); ++LineItem) {
            StringLine = LineItem->c_str();
            if (StringLine.length() > 0) {
                StringLine.append("\n");
                file.write(StringLine.c_str(), StringLine.length());
            }
        }
        file.close();
    } else {
        Error = true;
        snprintf(StatusMsg, sizeof(StatusMsg), "IniFileWrite Unable to save file %s", FileName.c_str());
        PrintStatus(Error, StatusMsg);
    }
    return Error;
}
//-Protected-------------------------------------------------------------------
// Name: TestForBreakPointOpCode()
//-----------------------------------------------------------------------------
bool MC_Hardware6502::TestForBreakPointOpCode()
{
    if (m_BreakPointOpCode.SetFlag && m_BreakPointOpCode.Address == mc_Processor6502.GetPC()) {
        m_BreakPointOpCode.Found = true;
        m_CpuDebugPanel.Update = true;
        PrintStatus(false, "Cpu BreakPoint OpCode");
    } else {
        m_BreakPointOpCode.Found = false;
    }
    return m_BreakPointOpCode.Found;
}
//-Protected-------------------------------------------------------------------
// Name: TestForBreakPointMemory(uint16_t& address, uint8_t& data, bool ReadWrite)
//-----------------------------------------------------------------------------
void MC_Hardware6502::TestForBreakPointMemory(uint16_t& address, uint8_t& data, bool ReadWrite)
{
    char buf[256];

    if (m_BreakPointMemory.SetFlag && m_BreakPointMemory.Address == address) {
        m_BreakPointMemory.Found = true;
        m_CpuDebugPanel.Update = true;
        if (ReadWrite) {
            snprintf(buf, sizeof(buf), "Cpu BreakPoint Memory $%04X Read %02X", address, data);
            PrintStatus(false, buf);
        } else {
            snprintf(buf, sizeof(buf), "Cpu BreakPoint Memory $%04X Write %02X", address, data);
            PrintStatus(false, buf);
        }
    }
}
//-Protected-------------------------------------------------------------------
// Name: Thread_Create()
//-----------------------------------------------------------------------------
void MC_Hardware6502::Thread_Create()
{
    mc_ThreadMain.Thread = std::thread(&MC_Hardware6502::Thread_CallBack_Main, this, 0);
    Sleep(10);
    mc_ThreadVideo.Thread = std::thread(&MC_Hardware6502::Thread_CallBack_Video, this, 0);
    Sleep(10);
    if (mc_ThreadMain.Thread.joinable()) {
        //  mc_ThreadMain.join();
    }
    if (mc_ThreadVideo.Thread.joinable()) {
        // mc_ThreadVideo.join();
    }
}
//-Protected-------------------------------------------------------------------
// Name: Thread_Stop()
//-----------------------------------------------------------------------------
void MC_Hardware6502::Thread_Stop()
{
    int Timeout = 0;

    mc_ThreadMain.Quit = true;
    mc_ThreadVideo.Quit = true;
    Sleep(20);
    while (Timeout < 40) {                                                      // 4 sec Timeout
        Sleep(100);
        if (mc_ThreadMain.Running == false && mc_ThreadVideo.Running == false) {
            break;
        }
        Timeout++;
    }
}
//-Protected-------------------------------------------------------------------
// Name: Thread_CallBack_Main(int MultiThread_ID)
//-----------------------------------------------------------------------------
void MC_Hardware6502::Thread_CallBack_Main(int MultiThread_ID)
{
    uint32_t CpuDelayTime = 0;

    mc_ThreadMain.Running = true;
    while (!mc_ThreadMain.Quit) {
        CpuDelayTime++;
        if (CpuDelayTime > m_CpuSettings.SpeedUpDn) {
            CpuDelayTime = 0;
#if BreakPointMemory
            m_BreakPointMemory.Found = false;
#endif
            if (m_Cpu6502Run) {
#if BreakPointOpCode
                if (TestForBreakPointOpCode()) {
                    m_Cpu6502Run = false;
                    m_Disassembler6502 = true;
                }
#endif
                if (mc_Processor6502.RunOneOp()) {
                    m_Cpu6502Run = false;                                       // Cpu Crash (Stop Cpu + Memory Dump + Debug Info)
                    mc_Processor6502.DebugCrashInfo(m_MemoryMap);
                } else {
                    if (m_Disassembler6502) {
                        mc_Processor6502.DebugInfo(m_MemoryMap);
                    } 
                    if(m_BreakPointMemory.Found) {
#if BreakPointMemory
                        m_Cpu6502Run = false;
                        if (!m_Disassembler6502) {
                            mc_Processor6502.DebugInfo(m_MemoryMap);
                        }
#endif
                    }

                }
            } else if(m_Cpu6502Step) {
                m_Cpu6502Step = false;
#if BreakPointOpCode
                if (TestForBreakPointOpCode()) {
                    m_Disassembler6502 = true;
                }
#endif
                if (mc_Processor6502.RunOneOp()) {
                    mc_Processor6502.DebugCrashInfo(m_MemoryMap);
                } else {
                    mc_Processor6502.DebugInfo(m_MemoryMap);
                }
            }
        }
    }
    mc_ThreadMain.Running = false;
    mc_ThreadMain.Thread.detach();
}
//-Protected-------------------------------------------------------------------
// Name: Thread_CallBack_Video(int MultiThread_ID)
//-----------------------------------------------------------------------------
void MC_Hardware6502::Thread_CallBack_Video(int MultiThread_ID)
{
#if CPU6502_TESTMODE == false
    int VideoCompare;
#endif
    mc_ThreadVideo.Running = true;
    while (!mc_ThreadVideo.Quit) {
        Sleep(40);
#if CPU6502_TESTMODE == false
        if (mc_VideoDisplay.m_Update) {
            mc_VideoDisplay.m_Update = false;
            VideoCompare = memcmp(&m_MemoryMap[MemoryVideoAddress], mc_VideoDisplay.m_MemoryVideoCompare, sizeof(mc_VideoDisplay.m_MemoryVideoCompare));
            if (VideoCompare) {
                memcpy(mc_VideoDisplay.m_MemoryVideoCompare, &m_MemoryMap[MemoryVideoAddress], sizeof(mc_VideoDisplay.m_MemoryVideoCompare));
                mc_VideoDisplay.CpuEmuRenderDisplay();
            }
        }
#endif
    }
    mc_ThreadVideo.Running = false;
    mc_ThreadVideo.Thread.detach();
}
