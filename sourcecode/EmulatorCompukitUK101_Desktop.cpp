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
// File: EmulatorCompukitUK101_Desktop.cpp: Defines the entry point for the application.
//-----------------------------------------------------------------------------
#include "framework.h"
#include "EmulatorCompukitUK101_Desktop.h"
#include "MC_Hardware6502.h"
#include "MC_Processor6502.h"

using namespace std;

//*****************************************************************************  
// Public Code
//*****************************************************************************

//-----------------------------------------------------------------------------
// IMPLEMENT_DYNCREATE
//-----------------------------------------------------------------------------
HINSTANCE m_hInst;                                                              // current instance
HWND hwndDlg = nullptr;
WCHAR m_szTitle[MAX_LOADSTRING];                                                // The title bar text
WCHAR m_szWindowClass[MAX_LOADSTRING];                                          // the main window class name
bool  m_HasSetting = false;

extern MC_Hardware6502 mc_Hardware6502;
extern MC_Processor6502 mc_Processor6502;

//-----------------------------------------------------------------------------
// message handlers
//-----------------------------------------------------------------------------

//-Public----------------------------------------------------------------------
// Name:  wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
//-----------------------------------------------------------------------------
int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
    MSG msg;

    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);
    LoadStringW(hInstance, IDS_APP_TITLE, m_szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_EMULATORCOMPUKITUK101DESKTOP, m_szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);
    mc_Hardware6502.Initialize();
    AddConsole();
    if (!InitInstance (hInstance, nCmdShow)) {
        return FALSE;
    }
    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_EMULATORCOMPUKITUK101DESKTOP));
    mc_Hardware6502.Create();
    while (GetMessage(&msg, nullptr, 0, 0)) {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }
    mc_Hardware6502.Destroy();
    return (int) msg.wParam;
}
//-Public----------------------------------------------------------------------
// Name: MyRegisterClass(HINSTANCE hInstance)
//-----------------------------------------------------------------------------
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_EMULATORCOMPUKITUK101DESKTOP));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_EMULATORCOMPUKITUK101DESKTOP);
    wcex.lpszClassName  = m_szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));
    return RegisterClassExW(&wcex);
}
//-Public----------------------------------------------------------------------
// Name: InitInstance(HINSTANCE hInstance, int nCmdShow)
//-----------------------------------------------------------------------------
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    m_hInst = hInstance;                                                           // Store instance handle in our global variable

   HWND hWnd = CreateWindowW(m_szWindowClass, m_szTitle, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);
   if (!hWnd) {
      return FALSE;
   }
   mc_Hardware6502.m_App_Hwnd = hWnd;
   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);
   SetTimer(hWnd, IDT_TIMERCyclesPerSec, 100, (TIMERPROC)NULL);
   SetTimer(hWnd, IDT_TIMERIrq, 1100, (TIMERPROC)NULL);
   SetTimer(hWnd, IDT_TIMERNmi, 3100, (TIMERPROC)NULL);
   return TRUE;
}
//-Public----------------------------------------------------------------------
// Name: WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
//-----------------------------------------------------------------------------
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    int wmId;
    uint8_t KbChar;

    UpdateMenus(hWnd);
    switch (message) {
        case WM_COMMAND: {
                wmId = LOWORD(wParam);
                switch (wmId) {
                    case IDM_ABOUT:
                        DialogBox(m_hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                        break;
                    case IDM_DEBUG_CPUDEBUGCONTROLPANEL:
                        //DialogBox(m_hInst, MAKEINTRESOURCE(IDD_DIALOG_DEBUG), hWnd, DebugControlPanel);
                        if (!IsWindow(hwndDlg)) {
                            hwndDlg = CreateDialog(m_hInst, MAKEINTRESOURCE(IDD_DIALOG_DEBUG), hWnd, (DLGPROC)DebugControlPanel);
                            ShowWindow(hwndDlg, SW_SHOW);
                        }
                        break;
                    case IDM_EXIT:
                        KillTimer(hWnd, IDT_TIMERCyclesPerSec);
                        KillTimer(hWnd, IDT_TIMERIrq);
                        KillTimer(hWnd, IDT_TIMERNmi);
                        mc_Hardware6502.Destroy();
                        DestroyWindow(hWnd);
                        break;
                    case IDM_FILE_LOAD:
                        mc_Hardware6502.CpuLoadFile();
                        break;
                    case IDM_FILE_SAVE:
                        mc_Hardware6502.CpuSaveFile();
                        break;
                    case IDM_DEBUG_MEMORYDUMP:
                        mc_Hardware6502.CpuMemoryMapDump();
                        break;
                    case IDM_DEBUG_CPUDEBUGON:
                        mc_Hardware6502.m_Disassembler6502 = true;
                        break;
                    case IDM_DEBUG_CPUDEBUGOFF:
                        mc_Hardware6502.m_Disassembler6502 = false;
                        break;
                    case IDM_DEBUG_CPURUN:
                        mc_Hardware6502.m_Cpu6502Run = true;
                        break;
                    case IDM_DEBUG_CPUSTOP:
                        mc_Hardware6502.m_Cpu6502Run = false;
                        break;
                    case IDM_RESET_JUSTRESETCPU:
                        mc_Hardware6502.CpuReset();
                        break;
                    case IDM_RESET_INITIALIZEANDRESETCPU:
                        mc_Hardware6502.CpuInitializeAndReset();
                        system("cls");
                        break;
                    case IDM_ROMS_BASICCOMPUKITUK101:
                        mc_Hardware6502.m_BasicSelectUk101OrOsi = false;
                        mc_Hardware6502.CpuInitializeAndReset();
                        system("cls");
                        break;
                    case IDM_ROMS_BASICOSI600:
                        mc_Hardware6502.m_BasicSelectUk101OrOsi = true;
                        mc_Hardware6502.CpuInitializeAndReset();
                        system("cls");
                        break;
                    case IDM_CPUSPEED1:
                        mc_Hardware6502.m_CpuSettings.Speed = 1;
                        break;
                    case IDM_CPUSPEED2:
                        mc_Hardware6502.m_CpuSettings.Speed = 2;
                        break;
                    case IDM_CPUSPEED4:
                        mc_Hardware6502.m_CpuSettings.Speed = 4;
                        break;
                    case IDM_CPUSPEED8:
                        mc_Hardware6502.m_CpuSettings.Speed = 8;
                        break;
                    case IDM_CPUSPEED16:
                        mc_Hardware6502.m_CpuSettings.Speed = 16;
                        break;
                    case IDM_CPUSPEED32:
                        mc_Hardware6502.m_CpuSettings.Speed = 32;
                        break;
                    case IDM_CPUSPEEDMAX:
                        mc_Hardware6502.m_CpuSettings.Speed = 0;
                        break;
                    default:
                        return DefWindowProc(hWnd, message, wParam, lParam);
                }
            }
            break;
        case WM_PAINT: {
                PAINTSTRUCT ps;
                HDC hdc = BeginPaint(hWnd, &ps);
                mc_Hardware6502.ReSizeDisplay();
                EndPaint(hWnd, &ps);
            }
            break;
        case WM_SIZE: {
                mc_Hardware6502.ReSizeDisplay();
            }
            break;
        case WM_DESTROY:
            KillTimer(hWnd, IDT_TIMERCyclesPerSec);
            KillTimer(hWnd, IDT_TIMERIrq);
            KillTimer(hWnd, IDT_TIMERNmi);
            mc_Hardware6502.Destroy();
            PostQuitMessage(0);
            break;
        case WM_CHAR:
            KbChar = uint8_t(wParam & 0xFF);
            mc_Hardware6502.KeyboardMapKey(KbChar);
            break;
        case WM_TIMER:
            switch (wParam) {
                case IDT_TIMERCyclesPerSec: {
                    mc_Hardware6502.CpuCalCyclesPer10thSec();
                    UpdateConsoleTitle();
                }
                break;
                case IDT_TIMERIrq: {
                    mc_Hardware6502.CpuIRQ();
                }
                case IDT_TIMERNmi: {
                    mc_Hardware6502.CpuNMI();
                }
                break;
            }
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}
//-Public----------------------------------------------------------------------
// Name:  About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
//-----------------------------------------------------------------------------
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message) {
        case WM_INITDIALOG:
            return (INT_PTR)TRUE;
        case WM_COMMAND:
            if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL) {
                EndDialog(hDlg, LOWORD(wParam));
                return (INT_PTR)TRUE;
            }
            break;
    }
    return (INT_PTR)FALSE;
}
//-Public----------------------------------------------------------------------
// Name:  DebugControlPanel(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
//-----------------------------------------------------------------------------
INT_PTR CALLBACK DebugControlPanel(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    int wmId;
    TCHAR szNewTitle[MAX_PATH];
    wchar_t StringValue[256];
    static CpuDebugPanel s_CpuDebugPanel;

    UNREFERENCED_PARAMETER(lParam);
    switch (message) {
        case WM_INITDIALOG:
            s_CpuDebugPanel = mc_Hardware6502.m_CpuDebugPanel;
            m_HasSetting = false;
            if (mc_Hardware6502.m_Cpu6502Run) {
                StringCchPrintf(szNewTitle, MAX_PATH, TEXT("Debug ControlPanel Cpu Run)"));
            } else {
                StringCchPrintf(szNewTitle, MAX_PATH, TEXT("Debug ControlPanel Cpu Stop)"));
            }
            SetWindowText(hDlg, szNewTitle);
            SetWindowText(GetDlgItem(hDlg, IDC_EDIT_DUMPSTARTADDR), ConvertHexUint16ToWstring(mc_Hardware6502.m_CpuDebugPanel.DumpStartAddress).c_str());
            SetWindowText(GetDlgItem(hDlg, IDC_EDIT_DUMPSTOPADDR), ConvertHexUint16ToWstring(mc_Hardware6502.m_CpuDebugPanel.DumpEndAddress).c_str());
            SetWindowText(GetDlgItem(hDlg, IDC_EDIT_BREAKPOINTADDR), ConvertHexUint16ToWstring(mc_Hardware6502.m_CpuDebugPanel.BreakPointAddress).c_str());
            if (mc_Hardware6502.m_CpuDebugPanel.BreakPointFlag) {
                CheckDlgButton(hDlg, IDC_CHECK_BREAKPOINTADDR, BST_CHECKED);
            } else {
                CheckDlgButton(hDlg, IDC_CHECK_BREAKPOINTADDR, BST_UNCHECKED);
            }
            SetTimer(hDlg, IDT_TIMERDlg, 500, (TIMERPROC)NULL);
            return (INT_PTR)TRUE;
        case WM_TIMER:
            switch (wParam) {
                case IDT_TIMERDlg:
                    if (mc_Hardware6502.m_Disassembler6502) {
                        CheckDlgButton(hDlg, IDC_CHECK_DEBUG, BST_CHECKED);
                    } else {
                        CheckDlgButton(hDlg, IDC_CHECK_DEBUG, BST_UNCHECKED);
                    }
                    GetWindowText(GetDlgItem(hDlg, IDC_EDIT_BREAKPOINTADDR), StringValue, 5);
                    mc_Hardware6502.m_CpuDebugPanel.BreakPointAddress = ConvertHexLPWSTRTouint16(StringValue);
                    break;
            }
            break;
        case WM_COMMAND:
            if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL) {
                if (LOWORD(wParam) == IDCANCEL) {
                    mc_Hardware6502.m_CpuDebugPanel = s_CpuDebugPanel;
                }
                KillTimer(hDlg, IDT_TIMERDlg);
                EndDialog(hDlg, LOWORD(wParam));
                hwndDlg = nullptr;
                return (INT_PTR)TRUE;
            }
            wmId = LOWORD(wParam);
            switch (wmId) {
                case IDC_BUTTON_MEMORYDUMP:
                    GetWindowText(GetDlgItem(hDlg, IDC_EDIT_DUMPSTARTADDR), StringValue, 5);
                    mc_Hardware6502.m_CpuDebugPanel.DumpStartAddress = ConvertHexLPWSTRTouint16(StringValue);
                    GetWindowText(GetDlgItem(hDlg, IDC_EDIT_DUMPSTOPADDR), StringValue, 5);
                    mc_Hardware6502.m_CpuDebugPanel.DumpEndAddress = ConvertHexLPWSTRTouint16(StringValue);
                    mc_Hardware6502.CpuMemoryMapDump(mc_Hardware6502.m_CpuDebugPanel.DumpStartAddress , mc_Hardware6502.m_CpuDebugPanel.DumpEndAddress);
                    break;
                case IDC_CHECK_BREAKPOINTADDR:
                    GetWindowText(GetDlgItem(hDlg, IDC_EDIT_BREAKPOINTADDR), StringValue, 5);
                    mc_Hardware6502.m_CpuDebugPanel.BreakPointAddress = ConvertHexLPWSTRTouint16(StringValue);
                    if (IsDlgButtonChecked(hDlg, IDC_CHECK_BREAKPOINTADDR) == BST_CHECKED) {
                        mc_Hardware6502.m_CpuDebugPanel.BreakPointFlag = true;
                    } else {
                        mc_Hardware6502.m_CpuDebugPanel.BreakPointFlag = false;
                    }
                    break;
                case IDC_CHECK_DEBUG:
                    if (IsDlgButtonChecked(hDlg, IDC_CHECK_DEBUG) == BST_CHECKED) {
                        mc_Hardware6502.m_Disassembler6502 = true;
                    } else {
                        mc_Hardware6502.m_Disassembler6502 = false;
                    }
                    break;
                case IDC_BUTTON_RESET:
                    mc_Hardware6502.m_Cpu6502Run = false;
                    mc_Hardware6502.CpuReset();
                    Sleep(10);
                    StringCchPrintf(szNewTitle, MAX_PATH, TEXT("Debug ControlPanel Cpu Reset & Stop)"));
                    SetWindowText(hDlg, szNewTitle);
                    printf("Cpu Reset & Stop\r\n");
                    DebugControlPanelSetItems(hDlg);
                    break;
                case IDC_BUTTON_STOP:
                    mc_Hardware6502.m_Cpu6502Run = false;
                    Sleep(10);
                    StringCchPrintf(szNewTitle, MAX_PATH, TEXT("Debug ControlPanel Cpu Stop)"));
                    SetWindowText(hDlg, szNewTitle);
                    printf("Cpu Stop\r\n");
                    DebugControlPanelSetItems(hDlg);
                    break;
                case IDC_BUTTON_RUN:
                    DebugControlPanelGetItems(hDlg);
                    printf("Cpu Run\r\n");
                    Sleep(10);
                    mc_Hardware6502.m_Cpu6502Run = true;
                    StringCchPrintf(szNewTitle, MAX_PATH, TEXT("Debug ControlPanel Cpu Run)"));
                    SetWindowText(hDlg, szNewTitle);
                    break;
                case IDC_BUTTON_STEP:
                    mc_Hardware6502.m_Cpu6502Run = false;
                    Sleep(10);
                    DebugControlPanelGetItems(hDlg);
                    mc_Hardware6502.m_Cpu6502Step = true;
                    StringCchPrintf(szNewTitle, MAX_PATH, TEXT("Debug ControlPanel Cpu Step)"));
                    SetWindowText(hDlg, szNewTitle);
                    DebugControlPanelSetItems(hDlg);
                    break;
            }
    }
    return (INT_PTR)FALSE;
}
//-Public----------------------------------------------------------------------
// Name:  UpdateMenus(HWND hWnd)
//-----------------------------------------------------------------------------
void UpdateMenus(HWND hWnd)
{
    HMENU hmenu = nullptr;
    static bool s_Disassembler6502 = false;
    static bool s_Cpu6502Run = true;
    static bool s_BasicSelect = false;
    static uint16_t s_CpuSpeed = 0;

    if (s_Disassembler6502 != mc_Hardware6502.m_Disassembler6502) {
        s_Disassembler6502 = mc_Hardware6502.m_Disassembler6502;
        hmenu = GetMenu(hWnd);
        if (s_Disassembler6502) {
            CheckMenuItem(hmenu, IDM_DEBUG_CPUDEBUGON, MF_CHECKED);
            CheckMenuItem(hmenu, IDM_DEBUG_CPUDEBUGOFF, MF_UNCHECKED);
        } else {
            CheckMenuItem(hmenu, IDM_DEBUG_CPUDEBUGON, MF_UNCHECKED);
            CheckMenuItem(hmenu, IDM_DEBUG_CPUDEBUGOFF, MF_CHECKED);
        }
    }
    if (s_Cpu6502Run != mc_Hardware6502.m_Cpu6502Run) {
        s_Cpu6502Run = mc_Hardware6502.m_Cpu6502Run;
        hmenu = GetMenu(hWnd);
        if (s_Cpu6502Run) {
            CheckMenuItem(hmenu, IDM_DEBUG_CPURUN, MF_CHECKED);
            CheckMenuItem(hmenu, IDM_DEBUG_CPUSTOP, MF_UNCHECKED);
        } else {
            CheckMenuItem(hmenu, IDM_DEBUG_CPURUN, MF_UNCHECKED);
            CheckMenuItem(hmenu, IDM_DEBUG_CPUSTOP, MF_CHECKED);
        }
    }
    if (s_BasicSelect != mc_Hardware6502.m_BasicSelectUk101OrOsi) {
        s_BasicSelect = mc_Hardware6502.m_BasicSelectUk101OrOsi;
        hmenu = GetMenu(hWnd);
        if (s_BasicSelect) {
            CheckMenuItem(hmenu, IDM_ROMS_BASICOSI600, MF_CHECKED);
            CheckMenuItem(hmenu, IDM_ROMS_BASICCOMPUKITUK101, MF_UNCHECKED);
        } else {
            CheckMenuItem(hmenu, IDM_ROMS_BASICOSI600, MF_UNCHECKED);
            CheckMenuItem(hmenu, IDM_ROMS_BASICCOMPUKITUK101, MF_CHECKED);
        }
    }
    if (s_CpuSpeed != mc_Hardware6502.m_CpuSettings.Speed) {
        s_CpuSpeed = mc_Hardware6502.m_CpuSettings.Speed;
        hmenu = GetMenu(hWnd);
        CheckMenuItem(hmenu, IDM_CPUSPEED1, MF_UNCHECKED);
        CheckMenuItem(hmenu, IDM_CPUSPEED2, MF_UNCHECKED);
        CheckMenuItem(hmenu, IDM_CPUSPEED4, MF_UNCHECKED);
        CheckMenuItem(hmenu, IDM_CPUSPEED8, MF_UNCHECKED);
        CheckMenuItem(hmenu, IDM_CPUSPEED16, MF_UNCHECKED);
        CheckMenuItem(hmenu, IDM_CPUSPEED32, MF_UNCHECKED);
        CheckMenuItem(hmenu, IDM_CPUSPEEDMAX, MF_UNCHECKED);
        switch (s_CpuSpeed) {
            case 1: {
                CheckMenuItem(hmenu, IDM_CPUSPEED1, MF_CHECKED);
            }
            break;
            case 2: {
                CheckMenuItem(hmenu, IDM_CPUSPEED2, MF_CHECKED);
            }
            break;
            case 4: {
                CheckMenuItem(hmenu, IDM_CPUSPEED4, MF_CHECKED);
            }
            break;
            case 8: {
                CheckMenuItem(hmenu, IDM_CPUSPEED8, MF_CHECKED);
            }
            break;
            case 16: {
                CheckMenuItem(hmenu, IDM_CPUSPEED16, MF_CHECKED);
            }
            break;
            case 32: {
                CheckMenuItem(hmenu, IDM_CPUSPEED32, MF_CHECKED);
            }
            break;
            case 0: {
                CheckMenuItem(hmenu, IDM_CPUSPEEDMAX, MF_CHECKED);
            }
            break;
        }
    }
}
//-Public----------------------------------------------------------------------
// Name:  DebugControlPanelSetItems(HWND hWnd)
//-----------------------------------------------------------------------------
void DebugControlPanelSetItems(HWND hWnd)
{
    Sleep(10);
    Registers6502 Registers = mc_Processor6502.GetRegisters();
    SetWindowText(GetDlgItem(hWnd, IDC_EDIT_PC), ConvertHexUint16ToWstring(Registers.pc).c_str());
    SetWindowText(GetDlgItem(hWnd, IDC_EDIT_A), ConvertHexUint8ToWstring(Registers.A).c_str());
    SetWindowText(GetDlgItem(hWnd, IDC_EDIT_X), ConvertHexUint8ToWstring(Registers.X).c_str());
    SetWindowText(GetDlgItem(hWnd, IDC_EDIT_Y), ConvertHexUint8ToWstring(Registers.Y).c_str());
    if (Registers.status & 0x80) {
        SetButtonChecked(hWnd, IDC_CHECK_ST_N, BST_CHECKED);
    } else {
        SetButtonChecked(hWnd, IDC_CHECK_ST_N, BST_UNCHECKED);
    }
    if (Registers.status & 0x40) {
        SetButtonChecked(hWnd, IDC_CHECK_ST_O, BST_CHECKED);
    } else {
        SetButtonChecked(hWnd, IDC_CHECK_ST_O, BST_UNCHECKED);
    }
    if (Registers.status & 0x20) {
        SetButtonChecked(hWnd, IDC_CHECK_ST_NU, BST_CHECKED);
    } else {
        SetButtonChecked(hWnd, IDC_CHECK_ST_NU, BST_UNCHECKED);
    }
    if (Registers.status & 0x10) {
        SetButtonChecked(hWnd, IDC_CHECK_ST_B, BST_CHECKED);
    } else {
        SetButtonChecked(hWnd, IDC_CHECK_ST_B, BST_UNCHECKED);
    }
    if (Registers.status & 0x08) {
        SetButtonChecked(hWnd, IDC_CHECK_ST_D, BST_CHECKED);
    } else {
        SetButtonChecked(hWnd, IDC_CHECK_ST_D, BST_UNCHECKED);
    }
    if (Registers.status & 0x04) {
        SetButtonChecked(hWnd, IDC_CHECK_ST_I, BST_CHECKED);
    } else {
        SetButtonChecked(hWnd, IDC_CHECK_ST_I, BST_UNCHECKED);
    }
    if (Registers.status & 0x02) {
        SetButtonChecked(hWnd, IDC_CHECK_ST_Z, BST_CHECKED);
    } else {
        SetButtonChecked(hWnd, IDC_CHECK_ST_Z, BST_UNCHECKED);
    }
    if (Registers.status & 0x01) {
        SetButtonChecked(hWnd, IDC_CHECK_ST_C, BST_CHECKED);
    } else {
        SetButtonChecked(hWnd, IDC_CHECK_ST_C, BST_UNCHECKED);
    }
    Sleep(10);
    m_HasSetting = true;
}
//-Public----------------------------------------------------------------------
// Name:  DebugControlPanelGetItems(HWND hWnd)
//-----------------------------------------------------------------------------
void DebugControlPanelGetItems(HWND hWnd)
{
    wchar_t StringValue[256];
    
    Sleep(10);
    if (m_HasSetting) {
        Registers6502 Registers = mc_Processor6502.GetRegisters();
        GetWindowText(GetDlgItem(hWnd, IDC_EDIT_PC), StringValue, 5);
        Registers.pc = ConvertHexLPWSTRTouint16(StringValue);
        GetWindowText(GetDlgItem(hWnd, IDC_EDIT_A), StringValue, 3);
        Registers.A = ConvertHexLPWSTRTouint8(StringValue);
        GetWindowText(GetDlgItem(hWnd, IDC_EDIT_X), StringValue, 3);
        Registers.X = ConvertHexLPWSTRTouint8(StringValue);
        GetWindowText(GetDlgItem(hWnd, IDC_EDIT_Y), StringValue, 3);
        Registers.Y = ConvertHexLPWSTRTouint8(StringValue);
        if (IsDlgButtonChecked(hWnd, IDC_CHECK_ST_N) == BST_CHECKED) {
            Registers.status = (Registers.status | 0x80);
        } else {
            Registers.status = (Registers.status & 0x7F);
        }
        if (IsDlgButtonChecked(hWnd, IDC_CHECK_ST_O) == BST_CHECKED) {
            Registers.status = (Registers.status | 0x40);
        } else {
            Registers.status = (Registers.status & 0xBF);
        }
        if (IsDlgButtonChecked(hWnd, IDC_CHECK_ST_NU) == BST_CHECKED) {
            Registers.status = (Registers.status | 0x20);
        } else {
            Registers.status = (Registers.status & 0xDF);
        }
        if (IsDlgButtonChecked(hWnd, IDC_CHECK_ST_B) == BST_CHECKED) {
            Registers.status = (Registers.status | 0x10);
        } else {
            Registers.status = (Registers.status & 0xEF);
        }
        if (IsDlgButtonChecked(hWnd, IDC_CHECK_ST_D) == BST_CHECKED) {
            Registers.status = (Registers.status | 0x08);
        } else {
            Registers.status = (Registers.status & 0xF7);
        }
        if (IsDlgButtonChecked(hWnd, IDC_CHECK_ST_I) == BST_CHECKED) {
            Registers.status = (Registers.status | 0x04);
        } else {
            Registers.status = (Registers.status & 0xFB);
        }
        if (IsDlgButtonChecked(hWnd, IDC_CHECK_ST_Z) == BST_CHECKED) {
            Registers.status = (Registers.status | 0x02);
        } else {
            Registers.status = (Registers.status & 0xFD);
        }
        if (IsDlgButtonChecked(hWnd, IDC_CHECK_ST_C) == BST_CHECKED) {
            Registers.status = (Registers.status | 0x01);
        } else {
            Registers.status = (Registers.status & 0xFE);
        }
        mc_Processor6502.SetRegisters(Registers);
    }
    Sleep(10);
}
//-Public----------------------------------------------------------------------
// Name:  SetButtonChecked(HWND hWnd, int Button, int Mode)
//-----------------------------------------------------------------------------
void SetButtonChecked(HWND hWnd, int Button, int Mode)
{
    if (IsDlgButtonChecked(hWnd, Button) != Mode) {
        CheckDlgButton(hWnd, Button, Mode);
    }
}
//-Public----------------------------------------------------------------------
// Name:  AddConsole()
//-----------------------------------------------------------------------------
void AddConsole()
{
    HANDLE hConsole = nullptr;

    if (AllocConsole() == 0) {
        return;                                                                 // Handle error here. Use ::GetLastError() to get the error.
    }
    hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
    // Redirect CRT standard input, output and error handles to the console window.
    FILE* pNewStdout = nullptr;
    FILE* pNewStderr = nullptr;
    FILE* pNewStdin = nullptr;
    ::freopen_s(&pNewStdout, "CONOUT$", "w", stdout);
    ::freopen_s(&pNewStderr, "CONOUT$", "w", stderr);
    ::freopen_s(&pNewStdin, "CONIN$", "r", stdin);
    std::cout.clear();
    std::cerr.clear();
    std::cin.clear();
    std::wcout.clear();
    std::wcerr.clear();
    std::wcin.clear();
}
//-Public----------------------------------------------------------------------
// Name:  UpdateConsoleTitle()
//-----------------------------------------------------------------------------
void UpdateConsoleTitle()
{
    TCHAR szNewTitle[MAX_PATH];
    static uint8_t s_OnceASec = 10;

    s_OnceASec++;
    if (s_OnceASec > 10) {
        s_OnceASec = 0;
        if (mc_Hardware6502.m_Disassembler6502 && mc_Hardware6502.m_Cpu6502Run) {
            StringCchPrintf(szNewTitle, MAX_PATH, TEXT("Compukit UK101 Emulator (6502@%.3f Mhz)"), mc_Hardware6502.m_CpuSettings.CyclesPerSec / 100000);
            if (!SetConsoleTitle(szNewTitle)) {
                _tprintf(TEXT("SetConsoleTitle failed (%d)\n"), GetLastError());
            }
        } else if (mc_Hardware6502.m_Cpu6502Run) {
            StringCchPrintf(szNewTitle, MAX_PATH, TEXT("Compukit UK101 Emulator (6502@%.3f Mhz)"), mc_Hardware6502.m_CpuSettings.AvrBigSpeed / 100000);
            if (!SetConsoleTitle(szNewTitle)) {
                _tprintf(TEXT("SetConsoleTitle failed (%d)\n"), GetLastError());
            }
        }
    }
}
//-Public----------------------------------------------------------------------
// Name:  ConvertHexUint16ToWstring(uint16_t Value)
//-----------------------------------------------------------------------------
std::wstring ConvertHexUint16ToWstring(uint16_t Value)
{
    char TmpBuffer[255];
    std::string StrHexValue;
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> convert;

    sprintf_s(TmpBuffer, sizeof(TmpBuffer), "%04lX", Value);
    StrHexValue = TmpBuffer;
    std::wstring WStrHexValue = convert.from_bytes(StrHexValue);
    return WStrHexValue;
}
//-Public----------------------------------------------------------------------
// Name:  ConvertHexUint8ToWstring(uint8_t Value)
//-----------------------------------------------------------------------------
std::wstring ConvertHexUint8ToWstring(uint8_t Value)
{
    char TmpBuffer[255];
    std::string StrHexValue;
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> convert;

    sprintf_s(TmpBuffer, sizeof(TmpBuffer), "%02X", Value);
    StrHexValue = TmpBuffer;
    std::wstring WStrHexValue = convert.from_bytes(StrHexValue);
    return WStrHexValue;
}
//-Public----------------------------------------------------------------------
// Name:  ConvertHexLPWSTRTouint16(LPWSTR Value)
//-----------------------------------------------------------------------------
uint16_t ConvertHexLPWSTRTouint16(LPWSTR Value)
{
    unsigned int UintHexValue = 0;
    uint16_t HexValue;

    std::wstring Wstr = Value;
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> convert;
    std::string StrHexValue = convert.to_bytes(Wstr);
    sscanf_s(StrHexValue.c_str(), "%x", &UintHexValue);
    if (UintHexValue >= 0 && UintHexValue <= 0xFFFF) {
        HexValue = (uint16_t)(UintHexValue);
    }
    return HexValue;
}
//-Public----------------------------------------------------------------------
// Name:  ConvertHexLPWSTRTouint8(LPWSTR Value)
//-----------------------------------------------------------------------------
uint8_t ConvertHexLPWSTRTouint8(LPWSTR Value)
{
    unsigned int UintHexValue = 0;
    uint8_t HexValue;

    std::wstring Wstr = Value;
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> convert;
    std::string StrHexValue = convert.to_bytes(Wstr);
    sscanf_s(StrHexValue.c_str(), "%x", &UintHexValue);
    if (UintHexValue >= 0 && UintHexValue <= 0xFF) {
        HexValue = (uint8_t)(UintHexValue);
    }
    return HexValue;
}










//*****************************************************************************  
// Protected Code
//*****************************************************************************

//-Protected-------------------------------------------------------------------
// Name: 
//-----------------------------------------------------------------------------

