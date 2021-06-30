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

using namespace std;

//*****************************************************************************  
// Public Code
//*****************************************************************************

//-----------------------------------------------------------------------------
// IMPLEMENT_DYNCREATE
//-----------------------------------------------------------------------------
HINSTANCE m_hInst;                                                              // current instance
WCHAR m_szTitle[MAX_LOADSTRING];                                                // The title bar text
WCHAR m_szWindowClass[MAX_LOADSTRING];                                          // the main window class name

extern MC_Hardware6502 mc_Hardware6502;

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
                    case IDM_EXIT:
                        KillTimer(hWnd, IDT_TIMERCyclesPerSec);
                        KillTimer(hWnd, IDT_TIMERIrq);
                        KillTimer(hWnd, IDT_TIMERNmi);
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
                        break;
                    case IDM_ROMS_BASICCOMPUKITUK101:
                        mc_Hardware6502.m_BasicSelectUk101OrOsi = false;
                        mc_Hardware6502.CpuInitializeAndReset();
                        break;
                    case IDM_ROMS_BASICOSI600:
                        mc_Hardware6502.m_BasicSelectUk101OrOsi = true;
                        mc_Hardware6502.CpuInitializeAndReset();
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
                EndPaint(hWnd, &ps);
            }
            break;
        case WM_SIZE: {
                mc_Hardware6502.ReSizeDisplay();
            }
            break;
        case WM_DESTROY:
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
                    mc_Hardware6502.CpuCalCyclesPerSec();
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
// Name:  UpdateMenus(HWND hWnd)
//-----------------------------------------------------------------------------
void UpdateMenus(HWND hWnd)
{
    HMENU hmenu = nullptr;
    static bool Disassembler6502 = false;
    static bool Cpu6502Run = true;
    static bool BasicSelect = false;
    static uint16_t CpuSpeed = 0;

    if (Disassembler6502 != mc_Hardware6502.m_Disassembler6502) {
        Disassembler6502 = mc_Hardware6502.m_Disassembler6502;
        hmenu = GetMenu(hWnd);
        if (Disassembler6502) {
            CheckMenuItem(hmenu, IDM_DEBUG_CPUDEBUGON, MF_CHECKED);
            CheckMenuItem(hmenu, IDM_DEBUG_CPUDEBUGOFF, MF_UNCHECKED);
        } else {
            CheckMenuItem(hmenu, IDM_DEBUG_CPUDEBUGON, MF_UNCHECKED);
            CheckMenuItem(hmenu, IDM_DEBUG_CPUDEBUGOFF, MF_CHECKED);
        }
    }
    if (Cpu6502Run != mc_Hardware6502.m_Cpu6502Run) {
        Cpu6502Run = mc_Hardware6502.m_Cpu6502Run;
        hmenu = GetMenu(hWnd);
        if (Cpu6502Run) {
            CheckMenuItem(hmenu, IDM_DEBUG_CPURUN, MF_CHECKED);
            CheckMenuItem(hmenu, IDM_DEBUG_CPUSTOP, MF_UNCHECKED);
        } else {
            CheckMenuItem(hmenu, IDM_DEBUG_CPURUN, MF_UNCHECKED);
            CheckMenuItem(hmenu, IDM_DEBUG_CPUSTOP, MF_CHECKED);
        }
    }
    if (BasicSelect != mc_Hardware6502.m_BasicSelectUk101OrOsi) {
        BasicSelect = mc_Hardware6502.m_BasicSelectUk101OrOsi;
        hmenu = GetMenu(hWnd);
        if (BasicSelect) {
            CheckMenuItem(hmenu, IDM_ROMS_BASICOSI600, MF_CHECKED);
            CheckMenuItem(hmenu, IDM_ROMS_BASICCOMPUKITUK101, MF_UNCHECKED);
        } else {
            CheckMenuItem(hmenu, IDM_ROMS_BASICOSI600, MF_UNCHECKED);
            CheckMenuItem(hmenu, IDM_ROMS_BASICCOMPUKITUK101, MF_CHECKED);
        }
    }
    if (CpuSpeed != mc_Hardware6502.m_CpuSettings.Speed) {
        CpuSpeed = mc_Hardware6502.m_CpuSettings.Speed;
        hmenu = GetMenu(hWnd);
        CheckMenuItem(hmenu, IDM_CPUSPEED1, MF_UNCHECKED);
        CheckMenuItem(hmenu, IDM_CPUSPEED2, MF_UNCHECKED);
        CheckMenuItem(hmenu, IDM_CPUSPEED4, MF_UNCHECKED);
        CheckMenuItem(hmenu, IDM_CPUSPEED8, MF_UNCHECKED);
        CheckMenuItem(hmenu, IDM_CPUSPEED16, MF_UNCHECKED);
        CheckMenuItem(hmenu, IDM_CPUSPEED32, MF_UNCHECKED);
        CheckMenuItem(hmenu, IDM_CPUSPEEDMAX, MF_UNCHECKED);
        switch (CpuSpeed) {
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
// Name:  AddConsole()
//-----------------------------------------------------------------------------
void AddConsole()
{
    if (AllocConsole() == 0) {
        return;                                                                 // Handle error here. Use ::GetLastError() to get the error.
    }
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









//*****************************************************************************  
// Protected Code
//*****************************************************************************

//-Protected-------------------------------------------------------------------
// Name: 
//-----------------------------------------------------------------------------

