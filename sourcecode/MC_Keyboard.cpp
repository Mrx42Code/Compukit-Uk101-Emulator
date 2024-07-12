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
// File: MC_Keyboard.cpp: implementation of the MC_Keyboard class.
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// include Windows standard Libs
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// include Project
//-----------------------------------------------------------------------------
#include "MC_Keyboard.h"
#include "MC_Hardware6502.h"

//-----------------------------------------------------------------------------
// include Vendors
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Implementation Classes
//-----------------------------------------------------------------------------
extern MC_Hardware6502 mc_Hardware6502;

//-----------------------------------------------------------------------------
// Implementation Variables
//-----------------------------------------------------------------------------
static const uint8_t m_KeyboardOutputTable[8] = { 0xFE, 0xFD, 0xFB, 0xF7, 0xEF, 0xDF, 0xBF, 0x7F };

static const uint8_t m_KeyCodeTableKeys[] = {
		'1', 7, 7, 0, 0, 1, 0,  '2', 7, 6, 0, 0, 1, 0,  '3', 7, 5, 0, 0, 1, 0,  '4', 7, 4, 0, 0, 1, 0,  '5', 7, 3, 0, 0, 1, 0,  '6', 7, 2, 0, 0, 1, 0,  '7', 7, 1, 0, 0, 1, 0,
		'!', 7, 7, 0, 1, 0, 0,  '"', 7, 6, 0, 1, 0, 0,  '#', 7, 5, 0, 1, 0, 0,  '$', 7, 4, 0, 1, 0, 0,  '%', 7, 3, 0, 1, 0, 0,  '&', 7, 2, 0, 1, 0, 0, 0x27, 7, 1, 0, 1, 0, 0, // '

		'8', 6, 7, 0, 0, 1, 0,  '9', 6, 6, 0, 0, 1, 0,  '0', 6, 5, 0, 0, 1, 0,  ':', 6, 4, 0, 0, 1, 0,  '-', 6, 3, 0, 0, 1, 0, 0x08, 6, 2, 0, 0, 1, 0, // "Del"
		'(', 6, 7, 0, 1, 0, 0,  ')', 6, 6, 0, 1, 0, 0,  '*', 6, 4, 0, 1, 0, 0,  '=', 6, 3, 0, 1, 0, 0,  '.', 5, 7, 0, 0, 1, 0,

		'L', 5, 6, 0, 0, 1, 0,  'O', 5, 5, 0, 0, 1, 0,  0x0D, 5, 3, 0, 0, 1, 0, // CR
		'l', 5, 6, 0, 0, 0, 0,  'o', 5, 5, 0, 0, 0, 0,
		'>', 5, 7, 0, 1, 0, 0, 0x5C, 5, 6, 0, 1, 1, 0, // Backward Slash

		'W', 4, 7, 0, 0, 1, 0,  'E', 4, 6, 0, 0, 1, 0,  'R', 4, 5, 0, 0, 1, 0,  'T', 4, 4, 0, 0, 1, 0,  'Y', 4, 3, 0, 0, 1, 0,  'U', 4, 2, 0, 0, 1, 0,  'I', 4, 1, 0, 0, 1, 0,
		'w', 4, 7, 0, 0, 0, 0,  'e', 4, 6, 0, 0, 0, 0,  'r', 4, 5, 0, 0, 0, 0,  't', 4, 4, 0, 0, 0, 0,  'y', 4, 3, 0, 0, 0, 0,  'u', 4, 2, 0, 0, 0, 0,  'i', 4, 1, 0, 0, 0, 0,

		'S', 3, 7, 0, 0, 1, 0,  'D', 3, 6, 0, 0, 1, 0,  'F', 3, 5, 0, 0, 1, 0,  'G', 3, 4, 0, 0, 1, 0,  'H', 3, 3, 0, 0, 1, 0,  'J', 3, 2, 0, 0, 1, 0,  'K', 3, 1, 0, 0, 1, 0,
		's', 3, 7, 0, 0, 0, 0,  'd', 3, 6, 0, 0, 0, 0,  'f', 3, 5, 0, 0, 0, 0,  'g', 3, 4, 0, 0, 0, 0,  'h', 3, 3, 0, 0, 0, 0,  'j', 3, 2, 0, 0, 0, 0,  'k', 3, 1, 0, 0, 0, 0,
		'[', 3, 1, 1, 0, 1, 0,

		'X', 2, 7, 0, 0, 1, 0,  'C', 2, 6, 0, 0, 1, 0,  'V', 2, 5, 0, 0, 1, 0,  'B', 2, 4, 0, 0, 1, 0,  'N', 2, 3, 0, 0, 1, 0,  'M', 2, 2, 0, 0, 1, 0,  ',', 2, 1, 0, 0, 1, 0,
		'x', 2, 7, 0, 0, 0, 0,  'c', 2, 6, 0, 0, 0, 0,  'v', 2, 5, 0, 0, 0, 0,  'b', 2, 4, 0, 0, 0, 0,  'n', 2, 3, 0, 0, 0, 0,  'm', 2, 2, 0, 0, 0, 0,
		0x03, 2, 6, 0, 0, 1, 1, ']', 2, 2, 1, 0, 1, 0,  '<', 2, 1, 0, 1, 0, 0,

		'Q', 1, 7, 0, 0, 1, 0,  'A', 1, 6, 0, 0, 1, 0,  'Z', 1, 5, 0, 0, 1, 0,  ' ', 1, 4, 0, 0, 0, 0,  '/', 1, 3, 0, 0, 1, 0,  ';', 1, 2, 0, 0, 1, 0,  'P', 1, 1, 0, 0, 1, 0,
		'q', 1, 7, 0, 0, 0, 0,  'a', 1, 6, 0, 0, 0, 0,  'z', 1, 5, 0, 0, 0, 0,  'p', 1, 1, 0, 0, 0, 0,  '?', 1, 3, 0, 1, 0, 0,  '+', 1, 2, 0, 1, 0, 0,  '@', 1, 1, 1, 0, 1, 0,

		0x7B, 0, 1, 1, 0, 0, 0, 0x7D, 0, 2, 1, 0, 0, 0, 0x05, 4, 6, 0, 0, 1, 1, 0x0B, 3, 1, 0, 0, 1, 1, 0x0A, 3, 2, 0, 0, 1, 1, 0x09, 5, 4, 0, 0, 1, 0, 0x0C, 5, 6, 0, 0, 1, 1,
};

//-----------------------------------------------------------------------------
// Private Callback Handlers
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Private Task Handlers
//-----------------------------------------------------------------------------

//*****************************************************************************
// Public Code
//*****************************************************************************

//*****************************************************************************  
// Public Code
//*****************************************************************************

//-Public----------------------------------------------------------------------
// Name: MC_Keyboard()
// Desc: MC_Keyboard class
//-----------------------------------------------------------------------------
/** @brief MC_Keyboard @note Public @param None @retval None */
MC_Keyboard::MC_Keyboard()
{
	memset(&m_MemoryKeyScan, 0, sizeof(m_MemoryKeyScan));
	m_MemoryKeyScan.KeysDone = true;
	m_MemoryReadValue = 0xFF;
	m_MemoryWriteValue = 0xFF;
}
//-Public----------------------------------------------------------------------
// Name: ~MC_Keyboard()
// Desc: ~MC_Keyboard Destruction class
//-----------------------------------------------------------------------------
/** @brief ~MC_Keyboard @note Public @param None @retval None */
MC_Keyboard::~MC_Keyboard()
{
}
//-Public----------------------------------------------------------------------
// Name: Initialize()
//-----------------------------------------------------------------------------
/** @brief Initialize @note Public @param None @retval None */
void MC_Keyboard::Initialize()
{
	memset(&m_MemoryKeyScan, 0x00, sizeof(m_MemoryKeyScan));
	m_MemoryKeyScan.KeysDone = true;
	mc_Hardware6502.PrintStatus(false, "Keyboard Initialize");
}
//-Public----------------------------------------------------------------------
// Name: Destroy()
//-----------------------------------------------------------------------------
/** @brief Destroy @note Public @param None @retval None */
void MC_Keyboard::Destroy()
{
	mc_Hardware6502.PrintStatus(false, "Keyboard Destroy");
}
//-Public----------------------------------------------------------------------
// Name: Create()
//-----------------------------------------------------------------------------
/** @brief Create @note Public @param None @retval None */
void MC_Keyboard::Create()
{
}
//-Public----------------------------------------------------------------------
// Name: MemoryRead()
//-----------------------------------------------------------------------------
/** @brief MemoryRead @note Public @param None @retval uint8_t */
uint8_t	MC_Keyboard::MemoryRead()
{
	uint8_t Data;
	uint8_t CpuRow;
	uint8_t RowScanCode;

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
	return Data;
}
//-Public----------------------------------------------------------------------
// Name: MemoryWrite(uint8_t MemoryWriteValue)
//-----------------------------------------------------------------------------
/** @brief MemoryWrite @note Public @param MemoryWriteValue @retval None */
void MC_Keyboard::MemoryWrite(uint8_t MemoryWriteValue)
{
	m_MemoryKeyScan.CpuRow = MemoryWriteValue;
	m_MemoryWriteValue = MemoryWriteValue;
}
//-Public----------------------------------------------------------------------
// Name: KeyboardMapKey(uint8_t KeyPress)
//-----------------------------------------------------------------------------
/** @brief KeyboardMapKey @note Public @param KeyPress @retval None */
bool MC_Keyboard::KeyboardMapKey(uint8_t KeyPress)
{
	int i;
	int len = sizeof(m_KeyCodeTableKeys);
	int index;
	uint8_t Keycp;
	char Text[2];
	Text[1] = 0;
	bool KeyFound = false;

	if (KeyPress) {
		for (i = 0; i < len; i++) {
			index = (i * 7) + 0;
			if (index < len) {
				Keycp = m_KeyCodeTableKeys[index];
				if (Keycp && Keycp == KeyPress) {
					memset(&m_MemoryKeyScan, 0x00, sizeof(m_MemoryKeyScan));
					m_MemoryKeyScan.KeysDone = true;
					m_MemoryKeyScan.Keyin = KeyPress;
					m_MemoryKeyScan.Row = m_KeyCodeTableKeys[index + 1];
					m_MemoryKeyScan.Col = m_KeyCodeTableKeys[index + 2];
					m_MemoryKeyScan.LShift = m_KeyCodeTableKeys[index + 3];
					m_MemoryKeyScan.RShift = m_KeyCodeTableKeys[index + 4];
					m_MemoryKeyScan.caplock = m_KeyCodeTableKeys[index + 5];
					m_MemoryKeyScan.ctrl = m_KeyCodeTableKeys[index + 6];
					m_MemoryKeyScan.RowScanCode = m_KeyboardOutputTable[(uint8_t)m_MemoryKeyScan.Row];
					m_MemoryKeyScan.ColScanCode = m_KeyboardOutputTable[(uint8_t)m_MemoryKeyScan.Col];
					m_MemoryKeyScan.CpuCount = 0;
					m_MemoryKeyScan.KeysDone = false;
					m_MemoryKeyScan.CpuRow = 0xFF;
					KeyFound = true;
					Text[0] = Keycp;
					break;
				}
			}
		}
	}
	return KeyFound;
}

//*****************************************************************************
// Private Code
//*****************************************************************************

//-Private---------------------------------------------------------------------
// Name:
//-----------------------------------------------------------------------------

//*****************************************************************************
// Protected Code
//*****************************************************************************

//-Protected-------------------------------------------------------------------
// Name:
//-----------------------------------------------------------------------------

//*****************************************************************************
// Private CallBack
//*****************************************************************************

//-Private CallBack------------------------------------------------------------
// Name:
//-----------------------------------------------------------------------------

//*****************************************************************************
// Private Task
//*****************************************************************************

//-Private Task----------------------------------------------------------------
// Name:
//-----------------------------------------------------------------------------
