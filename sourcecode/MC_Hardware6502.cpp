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
#include "framework.h"
#include "MC_Hardware6502.h"
#include "MC_VideoDisplay.h"
#include "MC_Processor6502.h"
#include "MC_Disassembler6502.h"

using namespace std;

//*****************************************************************************  
// Public Code
//*****************************************************************************
#define SHOW(T,V) do { T x = V; PrintBits(#T, #V, (unsigned char*) &x, sizeof(x)); } while(0)

const char StatusBits[8] = { 'C', 'Z' , 'I' , 'D', 'B', '-', 'O', 'N'};

//-----------------------------------------------------------------------------
// IMPLEMENT_DYNCREATE
//-----------------------------------------------------------------------------
uint8_t CpuMemoryRead(uint16_t address);
void CpuMemoryWrite(uint16_t address, uint8_t value);

MC_Hardware6502 mc_Hardware6502;
MC_VideoDisplay mc_VideoDisplay;
MC_Processor6502 mc_Processor6502(CpuMemoryRead, CpuMemoryWrite);
MC_Disassembler6502 mc_Disassembler6502;

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
    memset(&m_CpuSettings, 0x00, sizeof(m_CpuSettings));
    m_CpuSettings.SpeedUpDn = CPU6502_SPEED;
    m_CpuSettings.Speed = 2;
    m_Cpu6502Run = false;
    m_Disassembler6502 = false;
    m_BasicSelectUk101OrOsi = BasicSelectUk101OrOsi;
    m_App_Hwnd = nullptr;
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
    CpuMemoryInit();
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
    Thread_Stop();
    mc_VideoDisplay.Destroy();
}
//-Public----------------------------------------------------------------------
// Name: Create()
//-----------------------------------------------------------------------------
void MC_Hardware6502::Create()
{
    mc_VideoDisplay.Create();
    mc_VideoDisplay.m_Display.Hwnd = m_App_Hwnd;
    CpuInitializeAndReset();
    Thread_Create();
    m_Cpu6502Run = true;
    CpuReset();
    m_Disassembler6502 = false;
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
                    //printf("Text(%s) RowCol(%d,%d) LShift(%d) RShift(%d) Caplock(%d) Ctrl(%d) RowScanCode(%02X) ColScanCode(%02X)\r\n", Text, m_MemoryKeyScan.Row, m_MemoryKeyScan.Col, m_MemoryKeyScan.LShift, m_MemoryKeyScan.RShift, m_MemoryKeyScan.caplock, m_MemoryKeyScan.ctrl, m_MemoryKeyScan.RowScanCode, m_MemoryKeyScan.ColScanCode);
                } else {
                    //printf("KeyPress(%02X)\r\n", KeyPress);
                }
            }
        }
        if (KeyPress != m_MemoryKeyScan.Keyin) {
            //printf("KeyPress(%02X)\r\n", KeyPress);
        }
    }
}
//-Public----------------------------------------------------------------------
// Name: CpuMemoryMapRead(uint16_t address)
//-----------------------------------------------------------------------------
uint8_t MC_Hardware6502::CpuMemoryMapRead(uint16_t address)
{
    if (address >= MemoryRamAddress && address <= MemoryRamEndAddress) {
        return m_MemoryMap[address];
    } else  if (address >= MemoryExtRomAddress && address <= MemoryExtRomEndAddress) {
        return m_MemoryMap[address];
    } else  if (address >= MemoryParPortAddress && address <= MemoryParPortEndAddress) {
        return m_MemoryMap[address];
    } else  if (address >= MemoryBasic5RomAddress && address <= MemoryBasic5RomEndAddress) {
        return m_MemoryMap[address];
    } else  if (address >= MemoryDiskRomAddress && address <= MemoryDiskRomEndAddress) {
        return m_MemoryMap[address];
    } else  if (address >= MemoryBasicRomAddress && address <= MemoryBasicRomEndAddress) {
        return m_MemoryMap[address];
    } else  if (address >= MemoryDiskAddress && address <= MemoryDiskEndAddress) {
        return m_MemoryMap[address];
    } else  if (address >= MemoryVideoAddress && address <= MemoryVideoEndAddress) {
        return m_MemoryMap[address];
    } else  if (address >= (MemoryVideoAddress + MemoryVideoSizeAddress) && address <= ((MemoryVideoAddress + MemoryVideoSizeAddress) + 0x3F) ) {     //Bug Fix 48 Line Scroll Up
        return m_MemoryMap[address];
    } else  if (address >= MemoryKeyboardAddress && address <= MemoryKeyboardEndAddress) {
        return CpuEmuKeyboard(address, true);
    } else  if (address >= Memory6850Address && address <= Memory6850EndAddress) {
        return CpuEmu6850UartRead(address);
    } else  if (address >= MemoryMonitorRomAddress && address <= MemoryMonitorRomEndAddress) {
        return m_MemoryMap[address];
    } else {
        return 0xFF;
    }
}
//-Public----------------------------------------------------------------------
// Name: CpuMemoryMapWrite(uint16_t address, uint8_t value)
//-----------------------------------------------------------------------------
void MC_Hardware6502::CpuMemoryMapWrite(uint16_t address, uint8_t value)
{
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
}
//-Public----------------------------------------------------------------------
// Name: CpuIRQ()
//-----------------------------------------------------------------------------
void MC_Hardware6502::CpuCalCyclesPerSec()
{
    double CpuSpeed = m_CpuSettings.Speed * 100000;
    m_CpuSettings.CyclesPerSec = (double)(mc_Processor6502.m_TotalCyclesPerSec);
    mc_Processor6502.m_TotalCyclesPerSec = 0;
    if (m_Cpu6502Run && !m_Disassembler6502) {
        m_CpuSettings.AvrSpeed = (m_CpuSettings.AvrSpeed * 0.9) + (m_CpuSettings.CyclesPerSec * 0.1);
        m_CpuSettings.AvrBigSpeed = (m_CpuSettings.AvrBigSpeed * 0.9) + (m_CpuSettings.AvrSpeed * 0.1);
        if (m_CpuSettings.Speed) {
            if (m_CpuSettings.AvrSpeed > CpuSpeed) {
                m_CpuSettings.SpeedUpDn += (20.0f / (float)(m_CpuSettings.CyclesPerSec / 100000.0));
            }
            if (m_CpuSettings.AvrSpeed < CpuSpeed) {
                m_CpuSettings.SpeedUpDn -= (20.0f / (float)(m_CpuSettings.CyclesPerSec / 100000.0));
            }
        } else {
            m_CpuSettings.SpeedUpDn = 0;
        }
    }
    //printf("m_CpuSettings.Speed = %d m_CpuSettings.SpeedUpDn = %.3f\r\n", m_CpuSettings.Speed, m_CpuSettings.SpeedUpDn);
    //printf("CyclesPerSec %.3f mhz AvrSpeed %.3f mhz SpeedUpDn %d\r\n", CyclesPerSec / 100000.0, m_CpuSettings.AvrSpeed / 100000.0, m_CpuSettings.SpeedUpDn);
}
//-Public----------------------------------------------------------------------
// Name: CpuIRQ()
//-----------------------------------------------------------------------------
void MC_Hardware6502::CpuIRQ()
{
    //mc_Processor6502.IRQ();
}
//-Public----------------------------------------------------------------------
// Name: CpuNMI()
//-----------------------------------------------------------------------------
void MC_Hardware6502::CpuNMI()
{
    //mc_Processor6502.NMI();
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
    mc_Processor6502.Reset();
    m_Cpu6502Run = Cpu6502Run;
}
//-Public----------------------------------------------------------------------
// Name: CpuInitializeAndReset()
//-----------------------------------------------------------------------------
void MC_Hardware6502::CpuInitializeAndReset()
{
    CpuMemoryInit();
    CpuLoadRoms();
    CpuCegmonukRomMod();
    ReSizeDisplay();
    mc_VideoDisplay.Forceupdate();
    mc_Processor6502.Reset();
    m_Cpu6502Run = true;
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
    PrintHexDump16Bit("Memory Dump", &m_MemoryMap, sizeof(m_MemoryMap));
    if (m_Uart6850.Input.Index > 0) {
        PrintHexDump("Load Buffer Dump", &m_Uart6850.Input.Buffer, m_Uart6850.Input.Index);
    }
    if (m_Uart6850.Output.Index > 0) {
        PrintHexDump("Save Buffer Dump", &m_Uart6850.Output.Buffer, m_Uart6850.Output.Index);
    }
}
//-Public----------------------------------------------------------------------
// Name: CpuLoadFile()
//-----------------------------------------------------------------------------
void  MC_Hardware6502::CpuLoadFile()
{
    LoadUartData(FilenameOpenDlg(LOADSAVE_FILE_FILTER, m_App_Hwnd));
    Sleep(1000);
    mc_VideoDisplay.Forceupdate();
}
//-Public----------------------------------------------------------------------
// Name: CpuSaveFile()
//-----------------------------------------------------------------------------
void   MC_Hardware6502::CpuSaveFile()
{
    SaveUartData(FilenameSaveDlg(LOADSAVE_FILE_FILTER, m_App_Hwnd));
    Sleep(1000);
    mc_VideoDisplay.Forceupdate();
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
    memset(&m_MemoryKeyScan, 0x00, sizeof(m_MemoryKeyScan));
    m_MemoryKeyScan.KeysDone = true;
    memset(&m_Uart6850, 0x00, sizeof(m_Uart6850));
    Cpu6850Uartinit();
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
    m_MemoryMap[DATA_6850ADDR] = m_Uart6850.Input.CharData;
    m_MemoryMap[CTRL_6850ADDR] = m_Uart6850.Registers_SR.byte;
}
//-Protected-------------------------------------------------------------------
// Name: CpuEmu6850UartRead(uint16_t address)
//-----------------------------------------------------------------------------
uint8_t MC_Hardware6502::CpuEmu6850UartRead(uint16_t address)
{
    static uint16_t RxDelayTick = 0;
    static uint16_t TxDelayTick = 0;
    switch (address) {
        case CTRL_6850ADDR: {
            RxDelayTick++;
            TxDelayTick++;
            uint8_t flags = 0;
            if (m_Uart6850.Input.ProcessedIndex < m_Uart6850.Input.Index) {
                if (m_Uart6850.Registers_SR.bits.RDRF) {
                    if (RxDelayTick > 10) {
                        RxDelayTick = 0;
                        m_Uart6850.Registers_SR.bits.RDRF = 0;
                    }
                    flags |= RDRF6850;
                }
            } 
            if (m_Uart6850.Registers_SR.bits.TDRE) {
                if (TxDelayTick > 10) {
                    TxDelayTick = 0;
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
// Name: CpuLoadRoms()
//-----------------------------------------------------------------------------
void MC_Hardware6502::CpuLoadRoms()
{

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
    mc_VideoDisplay.CpuEmuLoadCharacterSetRom("GoodRoms/CharacterSetuk101.rom");                                                        // Character Set Rom
}
//-Protected-------------------------------------------------------------------
// Name: MemoryLoad(uint16_t MemoryAddress , uint16_t MemorySize, std::string FileName)
//-----------------------------------------------------------------------------
void MC_Hardware6502::MemoryLoad(uint16_t MemoryAddress, uint16_t MemorySize, std::string FileName)
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
        if (FileSize <= MemorySize) {
            memcpy(&m_MemoryMap[MemoryAddress], memblock, FileSize);
        } else {
            printf("file to Big for Memory(%04X) Slot File %s", FileSize, FileName.c_str());
        }
        delete[] memblock;
    } else {
        printf("Unable to open file %s", FileName.c_str());
    }
}
//-Protected-------------------------------------------------------------------
// Name: MemorySave(uint16_t MemoryAddress , uint16_t MemorySize, std::string FileName)
//-----------------------------------------------------------------------------
void MC_Hardware6502::MemorySave(uint16_t MemoryAddress, uint16_t MemorySize, std::string FileName)
{
    streampos size;
    uint8_t* memblock;
    uint16_t FileSize = 0;

    size = MemorySize;
    fstream file(FileName, ios::out | ios::binary | ios::ate);
    if (file.is_open()) {
        memblock = new uint8_t[(uint16_t)size];
        memcpy(memblock, &m_MemoryMap[MemoryAddress], (size_t)size);
        file.seekg(0, ios::beg);
        file.write((char*)memblock, size);
        file.close();
        delete[] memblock;
    } else {
        printf("Unable to Save file %s", FileName.c_str());
    }
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

    ifstream file(FileName, ios::in || ios::ate);
    if (file.is_open()) {
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
                        printf("Crc Error %02X %02X %s\r\n", Crc, LineCrc, StringLine.c_str());
                    }
                }
            }
        }
        file.close();
    } else {
        printf("Unable to open file %s", FileName.c_str());
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
            printf("file to Big for Memory Slot File %s", FileName.c_str());
        }
        delete[] memblock;
    } else {
        printf("Unable to Open file %s", FileName.c_str());
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
        printf("Unable to Save file %s", FileName.c_str());
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
// Name: PrintHexDump16Bit(const char* desc, void* addr, long len)
//-----------------------------------------------------------------------------
void MC_Hardware6502::PrintHexDump16Bit(const char* desc, void* addr, long len)
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
            printf("$%04lX ", i);
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
// Name: :PrintByteAsBits(char val)
//-----------------------------------------------------------------------------
void MC_Hardware6502::PrintByteAsBits(char val)
{
    for (int i = 7; 0 <= i; i--) {
        printf("%c", (val & (1 << i)) ? StatusBits[i] : '.');
//      printf("%c", (val & (1 << i)) ? '1' : '0');
    }
}
//-Protected-------------------------------------------------------------------
// Name: PrintBits(const char* ty, const char* val, unsigned char* bytes, size_t num_bytes)
//-----------------------------------------------------------------------------
void MC_Hardware6502::PrintBits(const char* ty, const char* val, unsigned char* bytes, size_t num_bytes)
{
    printf("Status Flags [ ");
    for (size_t i = 0; i < num_bytes; i++) {
        PrintByteAsBits(bytes[i]);
        printf(" ");
    }
    printf("]\r\n");
}
//-Protected-------------------------------------------------------------------
// Name: DebugInfo()
//-----------------------------------------------------------------------------
void MC_Hardware6502::DebugInfo()
{
    char tmpstr[512];

    Registers6502 Registers = mc_Processor6502.GetRegisters();
    mc_Disassembler6502.DisassemblerLine(tmpstr, m_MemoryMap, mc_Processor6502.m_DebugInstr.pc, 1);
    printf("%s A %02X X %02X Y %02X Cycles %d SP $%04X ", tmpstr, Registers.A, Registers.X, Registers.Y, mc_Processor6502.m_DebugInstr.Cpu.cycles, 0x0100 + Registers.sp);
    SHOW(uint8_t, Registers.status);
}
//-Protected-------------------------------------------------------------------
// Name: DebugCrashInfo()
//-----------------------------------------------------------------------------
void MC_Hardware6502::DebugCrashInfo()
{
    uint8_t index;
    char tmpstr[512];
    Registers6502 Registers;

    PrintHexDump16Bit("Crash Memory Dump $0000-$03FF", &m_MemoryMap, 1024);
    index = mc_Processor6502.m_CrashDump.Index;
    index++;
    if (index >= CrashDumpSize) {
        index = 0;
    }
    printf("\r\nCrash Debug Info\r\n");
    printf("----------------------------------------------------------\r\n");
    while (index != mc_Processor6502.m_CrashDump.Index) {
        Registers = mc_Processor6502.m_CrashDump.Registers[index];
        mc_Disassembler6502.DisassemblerLine(tmpstr, m_MemoryMap, mc_Processor6502.m_CrashDump.DebugInstr[index].pc, 1);
        printf("%s A %02X X %02X Y %02X Cycles %d SP $%04X ", tmpstr, Registers.A, Registers.X, Registers.Y, mc_Processor6502.m_CrashDump.DebugInstr[index].Cpu.cycles, 0x0100 + Registers.sp);
        SHOW(uint8_t, Registers.status);
        index++;
        if (index >= CrashDumpSize) {
            index = 0;
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
    while (mc_ThreadMain.Quit == false) {
        CpuDelayTime++;
        if (CpuDelayTime > m_CpuSettings.SpeedUpDn) {
            CpuDelayTime = 0;
            Thread_Main();
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
    mc_ThreadVideo.Running = true;
    while (mc_ThreadVideo.Quit == false) {
        Sleep(20);
        Thread_Video();
    }
    mc_ThreadVideo.Running = false;
    mc_ThreadVideo.Thread.detach();
}
//-Protected-------------------------------------------------------------------
// Name: hread_Main()
//-----------------------------------------------------------------------------
void MC_Hardware6502::Thread_Main()
{
    if (m_Cpu6502Run) {
        if (!mc_Processor6502.RunOneOp()) {
            if (m_Disassembler6502) {
                DebugInfo();
            }
        } else {
            m_Cpu6502Run = false;                                               // Cpu Crash (Stop Cpu + Memory Dump + Debug Info)
            DebugCrashInfo();
        }
    }
}
//-Protected-------------------------------------------------------------------
// Name: Thread_Video()
//-----------------------------------------------------------------------------
void MC_Hardware6502::Thread_Video()
{
    int VideoCompare;

    if (mc_VideoDisplay.m_Update) {
        mc_VideoDisplay.m_Update = false;
        VideoCompare = memcmp(&m_MemoryMap[MemoryVideoAddress], mc_VideoDisplay.m_MemoryVideoCompare, sizeof(mc_VideoDisplay.m_MemoryVideoCompare));
        if (VideoCompare) {
            memcpy(mc_VideoDisplay.m_MemoryVideoCompare, &m_MemoryMap[MemoryVideoAddress], sizeof(mc_VideoDisplay.m_MemoryVideoCompare));
            mc_VideoDisplay.CpuEmuRenderDisplay();
        }
    }
}
