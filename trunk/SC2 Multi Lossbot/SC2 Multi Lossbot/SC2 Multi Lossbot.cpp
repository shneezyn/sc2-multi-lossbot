// SC2 Multi Lossbot.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "SC2 Multi Lossbot.h"
#include "clsBot.h"
#include "clsGame.h"
#include <string>
#include <commctrl.h>
#include <fstream>

using namespace std;

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name
WNDPROC TabControlProc;
HWND hStart, hNote, hSaveSettings, hTab, hStaticCommon[8] = {NULL}, hStaticTab1[40] = {NULL}, hStaticTab2[40] = {NULL}, hStaticTab3[40] = {NULL}, hStaticTab4[40] = {NULL}, hStartHotkey, hStopHotkey;
int appWidth, appHeight;

// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK	TabControlRelay(HWND, UINT, WPARAM, LPARAM);

int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);
	
	MSG msg;
	HACCEL hAccelTable;

	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_MULTILOSSBOT, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow))
		return FALSE;

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_MULTILOSSBOT));

	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int) msg.wParam;
}

ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;
	
	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MULTILOSSBOT));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)COLOR_BTNSHADOW;
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_MULTILOSSBOT);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_MULTILOSSBOT));

	return RegisterClassEx(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;

   hInst = hInstance; // Store instance handle in our global variable

   int x = CW_USEDEFAULT, y = 0;

   try
   {
	   fstream myFile;
	   myFile.open("multi-lossbot.sc2", ios::in);
	   myFile >> x;
	   myFile >> y;
	   myFile.close();
   } catch (...) {}

   appWidth = 700;
   appHeight = 310;

   hWnd = CreateWindow(szWindowClass, szTitle, WS_SYSMENU | WS_MINIMIZEBOX, x, y, appWidth, appHeight, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;
	TCITEM tie;

	INITCOMMONCONTROLSEX InitCtrlEx;
	InitCtrlEx.dwSize = sizeof(INITCOMMONCONTROLSEX);
	InitCtrlEx.dwICC  = ICC_PROGRESS_CLASS;
	InitCommonControlsEx(&InitCtrlEx);
	
	switch (message)
	{
	case WM_TIMER:
		if (IDT_Leave <= wParam && wParam <= IDT_Leave+8)
			clsGame::TimesUp[wParam - IDT_Leave] = true;
		else if (IDT_1stChat <= wParam && wParam <= IDT_1stChat+8)
		{
			KillTimer(hWnd, wParam);
			wchar_t buffer[255] = {NULL};

			SendMessage(clsBot::h1stChat, WM_GETTEXT, 255, (LPARAM)buffer);
			
			if (SendMessage(clsBot::hAllies[0], BM_GETCHECK, NULL, NULL) == BST_CHECKED)
			{	
				PostMessage(clsGame::Handle[wParam - IDT_1stChat], WM_KEYDOWN, VK_CONTROL, 1900544);			
				PostMessage(clsGame::Handle[wParam - IDT_1stChat], WM_KEYDOWN, VK_RETURN, 1835008);
				PostMessage(clsGame::Handle[wParam - IDT_1stChat], WM_KEYUP, VK_RETURN, -1071906816);
				PostMessage(clsGame::Handle[wParam - IDT_1stChat], WM_KEYUP, VK_CONTROL, -1071841280);
			}
			else
			{
				PostMessage(clsGame::Handle[wParam - IDT_1stChat], WM_KEYDOWN, VK_RETURN, 1835008);
				PostMessage(clsGame::Handle[wParam - IDT_1stChat], WM_KEYUP, VK_RETURN, -1071906816);
			}
			
			for (int i = 0; i < 255 && buffer[i] != NULL; i++)
			{
				PostMessage(clsGame::Handle[wParam - IDT_1stChat], WM_CHAR, (WPARAM)buffer[i], NULL);
			}

			PostMessage(clsGame::Handle[wParam - IDT_1stChat], WM_KEYDOWN, VK_RETURN, 1835008);
			PostMessage(clsGame::Handle[wParam - IDT_1stChat], WM_KEYUP, VK_RETURN, -1071906816);

			if (clsGame::Timer2ndChat[wParam - IDT_1stChat] == false && SendMessage(clsBot::hShow2ndChat, BM_GETCHECK, NULL, NULL) == BST_CHECKED)
			{
				SendMessage(clsBot::h2ndChatDelay[wParam - IDT_1stChat], WM_GETTEXT, 5, (LPARAM)buffer);
				SetTimer(hWnd, IDT_2ndChat + wParam - IDT_1stChat, (UINT)(1000*_wtoi(buffer)), (TIMERPROC)NULL);
				clsGame::Timer2ndChat[wParam - IDT_1stChat] = true;
			}
		}
		else if (IDT_2ndChat <= wParam && wParam <= IDT_2ndChat+8)
		{
			KillTimer(hWnd, wParam);
			wchar_t buffer[255] = {NULL};
			
			SendMessage(clsBot::h2ndChat, WM_GETTEXT, 255, (LPARAM)buffer);

			if (SendMessage(clsBot::hAllies[1], BM_GETCHECK, NULL, NULL) == BST_CHECKED)
			{
				PostMessage(clsGame::Handle[wParam - IDT_2ndChat], WM_KEYDOWN, VK_CONTROL, 1900544);			
				PostMessage(clsGame::Handle[wParam - IDT_2ndChat], WM_KEYDOWN, VK_RETURN, 1835008);
				PostMessage(clsGame::Handle[wParam - IDT_2ndChat], WM_KEYUP, VK_RETURN, -1071906816);
				PostMessage(clsGame::Handle[wParam - IDT_2ndChat], WM_KEYUP, VK_CONTROL, -1071841280);
			}
			else
			{
				PostMessage(clsGame::Handle[wParam - IDT_2ndChat], WM_KEYDOWN, VK_RETURN, 1835008);
				PostMessage(clsGame::Handle[wParam - IDT_2ndChat], WM_KEYUP, VK_RETURN, -1071906816);
			}

			for (int i = 0; i < 255 && buffer[i] != NULL; i++)
			{
				PostMessage(clsGame::Handle[wParam - IDT_2ndChat], WM_CHAR, (WPARAM)buffer[i], NULL);
			}
			
			PostMessage(clsGame::Handle[wParam - IDT_2ndChat], WM_KEYDOWN, VK_RETURN, 1835008);
			PostMessage(clsGame::Handle[wParam - IDT_2ndChat], WM_KEYUP, VK_RETURN, -1071906816);
		}
		break;

	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);

		switch (wmId)
		{
		case BN_CLICKED:
			for (int i = 0; i < 8; i++)
			{
				if ((HWND)lParam == clsBot::hWindowStatus[i])
				{
					if ((HWND)lParam == clsBot::hVisible[i])
							ShowWindow(clsGame::Handle[i], SW_SHOW);

					if ((HWND)lParam == clsBot::hHidden[i])
							ShowWindow(clsGame::Handle[i], SW_HIDE);
				}
			}
			break;

		case IDC_Save:
			try
			{
				wfstream myFile;
				myFile.open("multi-lossbot.sc2", ios::out);

				RECT rect;
				GetWindowRect(hWnd, &rect);
				wchar_t buffer[255] = {NULL};
				
				myFile << rect.left << endl << rect.top << endl;				
				myFile << (SendMessage(clsBot::hShow1stChat, BM_GETCHECK, NULL, NULL) == BST_CHECKED ? BST_CHECKED : BST_UNCHECKED) << endl;
				myFile << (SendMessage(clsBot::hShow2ndChat, BM_GETCHECK, NULL, NULL) == BST_CHECKED ? BST_CHECKED : BST_UNCHECKED) << endl;
				myFile << SendMessage(hStartHotkey, HKM_GETHOTKEY, NULL, NULL) << endl;
				myFile << SendMessage(hStopHotkey, HKM_GETHOTKEY, NULL, NULL) << endl;
				SendMessage(hWnd, WM_SETHOTKEY, SendMessage(hStartHotkey, HKM_GETHOTKEY, NULL, NULL), NULL);
				SendMessage(hWnd, WM_SETHOTKEY, SendMessage(hStopHotkey, HKM_GETHOTKEY, NULL, NULL), NULL);
				SendMessage(clsBot::h1stChat, WM_GETTEXT, 255, (LPARAM)buffer);
				myFile << buffer << endl;
				SendMessage(clsBot::h2ndChat, WM_GETTEXT, 255, (LPARAM)buffer);
				myFile << buffer << endl;
				SendMessage(clsBot::hSurrender, WM_GETTEXT, 255, (LPARAM)buffer);
				myFile << buffer << endl;
				SendMessage(clsBot::hScoreScreen, WM_GETTEXT, 255, (LPARAM)buffer);
				myFile << buffer << endl;

				for (int i = 0; i < 8; i++)
				{
					SendMessage(clsBot::hLeaveDelay[i], WM_GETTEXT, 5, (LPARAM)buffer);
					myFile << buffer << endl;
					SendMessage(clsBot::hGameLimit[i], WM_GETTEXT, 5, (LPARAM)buffer);
					myFile << buffer << endl;
					SendMessage(clsBot::h1stChatDelay[i], WM_GETTEXT, 5, (LPARAM)buffer);
					myFile << buffer << endl;
					SendMessage(clsBot::h2ndChatDelay[i], WM_GETTEXT, 5, (LPARAM)buffer);
					myFile << buffer << endl;
				}

				myFile.close();
				MessageBox(GetWindow(NULL, NULL), L"Settings have been saved.", szTitle, NULL);
			} catch(...) {}
			break;

		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;

	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		EndPaint(hWnd, &ps);
		break;

	case WM_HOTKEY:
		switch (wParam)
		{
		case HOTKEY_START:
			if (clsBot::hStartBot == NULL)
			{
				clsBot::hStartBot = CreateThread(NULL, NULL, clsBot::StartBot, hWnd, NULL, NULL);
			}
			break;

		case HOTKEY_STOP:
			TerminateThread(clsBot::hStartBot, NULL);
			clsBot::hStartBot = NULL;
			//clsBot::setup = false;
			for (int i = 0; i < 8; i++)
			{
				ShowWindow(clsGame::Handle[i], SW_SHOW);
				SendMessage(clsBot::hStatus[i], WM_SETTEXT, NULL, (LPARAM)L"Idle");
				clsGame::TimerLeave[i] = false;
				clsGame::Timer1stChat[i] = false;
				clsGame::Timer2ndChat[i] = false;
				KillTimer(hWnd, IDT_Leave + i);
			}
		}
		break;

	case WM_NOTIFY:
		switch (((LPNMHDR)lParam)->code)
		{
		case TCN_SELCHANGE:
			// Hide all controls
			for (int i = 0; i < 8; i++)
			{
				// Common
				ShowWindow(hStaticCommon[i], SW_HIDE);
				// Overview
				ShowWindow(clsBot::hStatus[i], SW_HIDE);
				ShowWindow(clsBot::hGames[i], SW_HIDE);
				ShowWindow(clsBot::hWon[i], SW_HIDE);
				ShowWindow(clsBot::hLoss[i], SW_HIDE);
				ShowWindow(clsBot::hVisible[i], SW_HIDE);
				ShowWindow(clsBot::hHidden[i], SW_HIDE);
				ShowWindow(clsBot::hWindowStatus[i], SW_HIDE);
				ShowWindow(clsBot::hDisabled[i], SW_HIDE);
				// Delays
				ShowWindow(clsBot::hLeaveDelay[i], SW_HIDE);
				ShowWindow(clsBot::h1stChatDelay[i], SW_HIDE);
				ShowWindow(clsBot::h2ndChatDelay[i], SW_HIDE);
				// Game Settings
				ShowWindow(clsBot::hGameLimit[i], SW_HIDE);
				ShowWindow(clsBot::hGameType[i], SW_HIDE);
				ShowWindow(clsBot::hMultiplayer[i], SW_HIDE);
				ShowWindow(clsBot::hCooperative[i], SW_HIDE);
			}

			for (int i = 0; i < 12; i++)
				ShowWindow(hStaticTab1[i], SW_HIDE);

			for (int i = 0; i < 40; i++)
				ShowWindow(hStaticTab2[i], SW_HIDE);

			for (int i = 0; i < 40; i++)
				ShowWindow(hStaticTab3[i], SW_HIDE);

			for (int i = 0; i < 40; i++)
				ShowWindow(hStaticTab4[i], SW_HIDE);

			for (int i = 0; i < 2; i++)
			{
				ShowWindow(clsBot::hChat[i], SW_HIDE);
				ShowWindow(clsBot::hAll[i], SW_HIDE);
				ShowWindow(clsBot::hAllies[i], SW_HIDE);
			}

			ShowWindow(clsBot::hShow1stChat, SW_HIDE);
			ShowWindow(clsBot::hShow2ndChat, SW_HIDE);
			ShowWindow(hStartHotkey, SW_HIDE);
			ShowWindow(hStopHotkey, SW_HIDE);
			ShowWindow(clsBot::h1stChat, SW_HIDE);
			ShowWindow(clsBot::h2ndChat, SW_HIDE);
			ShowWindow(hSaveSettings, SW_HIDE);
			ShowWindow(hNote, SW_HIDE);
			ShowWindow(clsBot::hScoreScreen, SW_HIDE);
			ShowWindow(clsBot::hSurrender, SW_HIDE);

			// Show only the controls for the selected tab
			if (TabCtrl_GetCurSel(hTab) == 0) // Overview
			{
				for (int i = 0; i < 8; i++)
				{
					ShowWindow(hStaticCommon[i], SW_SHOW);
					ShowWindow(clsBot::hStatus[i], SW_SHOW);
					ShowWindow(clsBot::hGames[i], SW_SHOW);
					ShowWindow(clsBot::hWon[i], SW_SHOW);
					ShowWindow(clsBot::hLoss[i], SW_SHOW);
					ShowWindow(clsBot::hVisible[i], SW_SHOW);
					ShowWindow(clsBot::hHidden[i], SW_SHOW);
					ShowWindow(clsBot::hWindowStatus[i], SW_SHOW);
					ShowWindow(clsBot::hDisabled[i], SW_SHOW);
				}

				for (int i = 0; i < 40; i++)
				{
					ShowWindow(hStaticTab1[i], SW_SHOW);
				}
			}
			else if (TabCtrl_GetCurSel(hTab) == 1) // Delays
			{
				ShowWindow(hNote, SW_SHOW);

				for (int i = 0; i < 8; i++)
				{
					ShowWindow(hStaticCommon[i], SW_SHOW);
					ShowWindow(clsBot::hLeaveDelay[i], SW_SHOW);
					ShowWindow(clsBot::h1stChatDelay[i], SW_SHOW);
					ShowWindow(clsBot::h2ndChatDelay[i], SW_SHOW);
				}

				for (int i = 0; i < 40; i++)
				{
					ShowWindow(hStaticTab2[i], SW_SHOW);
				}
			}			
			else if (TabCtrl_GetCurSel(hTab) == 2) // Game Settings
			{
				for (int i = 0; i < 40; i++)
				{
					ShowWindow(hStaticTab3[i], SW_SHOW);
				}

				for (int i = 0; i < 8; i++)
				{
					ShowWindow(hStaticCommon[i], SW_SHOW);
					ShowWindow(clsBot::hGameLimit[i], SW_SHOW);
					ShowWindow(clsBot::hGameType[i], SW_SHOW);
					ShowWindow(clsBot::hMultiplayer[i], SW_SHOW);
					ShowWindow(clsBot::hCooperative[i], SW_SHOW);
				}
			}
			else if (TabCtrl_GetCurSel(hTab) == 3) // Settings
			{
				for (int i = 0; i < 40; i++)
					ShowWindow(hStaticTab4[i], SW_SHOW);

				for (int i = 0; i < 2; i++)
				{
					ShowWindow(clsBot::hChat[i], SW_SHOW);
					ShowWindow(clsBot::hAll[i], SW_SHOW);
					ShowWindow(clsBot::hAllies[i], SW_SHOW);
				}

				ShowWindow(clsBot::hShow1stChat, SW_SHOW);
				ShowWindow(clsBot::hShow2ndChat, SW_SHOW);
				ShowWindow(hStartHotkey, SW_SHOW);
				ShowWindow(hStopHotkey, SW_SHOW);
				ShowWindow(hSaveSettings, SW_SHOW);
				ShowWindow(clsBot::h1stChat, SW_SHOW);
				ShowWindow(clsBot::h2ndChat, SW_SHOW);
				ShowWindow(clsBot::hScoreScreen, SW_SHOW);
				ShowWindow(clsBot::hSurrender, SW_SHOW);
			}
			break;

		break;
		}
		break;

	case WM_SETHOTKEY:
		UnregisterHotKey(hWnd, HOTKEY_START);
		UnregisterHotKey(hWnd, HOTKEY_STOP);
		RegisterHotKey(hWnd, HOTKEY_START, HIWORD(SendMessage(hStartHotkey, HKM_GETHOTKEY, NULL, NULL)), LOWORD(SendMessage(hStartHotkey, HKM_GETHOTKEY, NULL, NULL)));
		RegisterHotKey(hWnd, HOTKEY_STOP, HIWORD(SendMessage(hStopHotkey, HKM_GETHOTKEY, NULL, NULL)), LOWORD(SendMessage(hStopHotkey, HKM_GETHOTKEY, NULL, NULL)));
		break;

	case WM_CREATE:
		RECT rect;
		GetClientRect(hWnd, &rect);
		hTab = CreateWindowEx(NULL, WC_TABCONTROL, L"", WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE, 5, 5, rect.right - 10, rect.bottom - 10, hWnd, NULL, NULL, NULL); 
		tie.mask = TCIF_TEXT | TCIF_IMAGE; 
		tie.iImage = -1;
		tie.pszText = L"Overview"; 
		TabCtrl_InsertItem(hTab, 0, &tie);
		tie.pszText = L"Delays"; 
		TabCtrl_InsertItem(hTab, 1, &tie);
		tie.pszText = L"Game Settings"; 
		TabCtrl_InsertItem(hTab, 2, &tie);
		tie.pszText = L"Settings"; 
		TabCtrl_InsertItem(hTab, 3, &tie);

		// Common Controls
		{
			hStaticCommon[0] = CreateWindowEx(NULL, L"static", L"SC2 #1:", WS_CHILD | WS_VISIBLE, 10, 50+10, 75, 15, hTab, NULL, NULL, NULL);
			hStaticCommon[1] = CreateWindowEx(NULL, L"static", L"SC2 #2:", WS_CHILD | WS_VISIBLE, 10, 75+10, 75, 15, hTab, NULL, NULL, NULL);
			hStaticCommon[2] = CreateWindowEx(NULL, L"static", L"SC2 #3:", WS_CHILD | WS_VISIBLE, 10, 100+10, 75, 15, hTab, NULL, NULL, NULL);
			hStaticCommon[3] = CreateWindowEx(NULL, L"static", L"SC2 #4:", WS_CHILD | WS_VISIBLE, 10, 125+10, 75, 15, hTab, NULL, NULL, NULL);
			hStaticCommon[4] = CreateWindowEx(NULL, L"static", L"SC2 #5:", WS_CHILD | WS_VISIBLE, 10, 150+10, 75, 15, hTab, NULL, NULL, NULL);
			hStaticCommon[5] = CreateWindowEx(NULL, L"static", L"SC2 #6:", WS_CHILD | WS_VISIBLE, 10, 175+10, 75, 15, hTab, NULL, NULL, NULL);
			hStaticCommon[6] = CreateWindowEx(NULL, L"static", L"SC2 #7:", WS_CHILD | WS_VISIBLE, 10, 200+10, 75, 15, hTab, NULL, NULL, NULL);
			hStaticCommon[7] = CreateWindowEx(NULL, L"static", L"SC2 #8:", WS_CHILD | WS_VISIBLE, 10, 225+10, 75, 15, hTab, NULL, NULL, NULL);
		}
		// Overview Tab
		{
			hStaticTab1[0] = CreateWindowEx(NULL, L"static", L"Game Status", WS_CHILD | WS_VISIBLE, 80, 35, 100, 15, hTab, NULL, NULL, NULL);
			hStaticTab1[1] = CreateWindowEx(NULL, L"static", L"Games", WS_CHILD | WS_VISIBLE, 200, 35, 50, 15, hTab, NULL, NULL, NULL);
			hStaticTab1[2] = CreateWindowEx(NULL, L"static", L"Won", WS_CHILD | WS_VISIBLE, 270, 35, 50, 15, hTab, NULL, NULL, NULL);
			hStaticTab1[3] = CreateWindowEx(NULL, L"static", L"Loss", WS_CHILD | WS_VISIBLE, 340, 35, 50, 15, hTab, NULL, NULL, NULL);
			hStaticTab1[4] = CreateWindowEx(NULL, L"static", L"Window Status", WS_CHILD | WS_VISIBLE, 490, 35, 100, 15, hTab, NULL, NULL, NULL);

			for (int i = 0; i < 8; i++)
			{
				clsBot::hStatus[i] = CreateWindowEx(NULL, L"edit", L"Idle", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_READONLY | ES_CENTER, 80, 58 + 25*i, 100, 20, hTab, NULL, NULL, NULL);
				clsBot::hGames[i] = CreateWindowEx(NULL, L"edit", L"0", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_READONLY | ES_CENTER, 200, 58 + 25*i, 50, 20, hTab, NULL, NULL, NULL);
				clsBot::hWon[i] = CreateWindowEx(NULL, L"edit", L"0", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_READONLY | ES_CENTER, 270, 58 + 25*i, 50, 20, hTab, NULL, NULL, NULL);
				clsBot::hLoss[i] = CreateWindowEx(NULL, L"edit", L"0", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_READONLY | ES_CENTER, 340, 58 + 25*i, 50, 20, hTab, NULL, NULL, NULL);
				clsBot::hWindowStatus[i] = CreateWindowEx(NULL, L"button", L"", WS_CHILD | WS_VISIBLE | BS_GROUPBOX, 400, 45 + 25*i, 270, 35, hTab, NULL, NULL, NULL);
				clsBot::hVisible[i] = CreateWindowEx(NULL, L"button", L"Visible", WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON, 15, 12, 80, 20, clsBot::hWindowStatus[i], NULL, NULL, NULL);
				clsBot::hHidden[i] = CreateWindowEx(NULL, L"button", L"Hidden", WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON, 95, 12, 80, 20, clsBot::hWindowStatus[i], NULL, NULL, NULL);
				clsBot::hDisabled[i] = CreateWindowEx(NULL, L"button", L"Disabled", WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON, 175, 12, 80, 20, clsBot::hWindowStatus[i], NULL, NULL, NULL);
				SendMessage(clsBot::hVisible[i], BM_CLICK, NULL, NULL);
			}
		}
		// Delays Tab
		{
			hStaticTab2[0] = CreateWindowEx(NULL, L"static", L"Leave", WS_CHILD, 80, 35, 50, 15, hTab, NULL, NULL, NULL);
			hStaticTab2[1] = CreateWindowEx(NULL, L"static", L"1st Chat", WS_CHILD, 150, 35, 100, 15, hTab, NULL, NULL, NULL);
			hStaticTab2[2] = CreateWindowEx(NULL, L"static", L"2nd Chat", WS_CHILD, 220, 35, 100, 15, hTab, NULL, NULL, NULL);

			for (int i = 0; i < 8; i++)
			{
				clsBot::hLeaveDelay[i] = CreateWindowEx(NULL, L"edit", L"150", WS_CHILD | WS_BORDER | ES_CENTER, 80, 58 + 25*i, 50, 20, hTab, NULL, NULL, NULL);
				hStaticTab2[4+i] = CreateWindowEx(NULL, L"static", L"s", WS_CHILD, 131, 60 + 25*i, 10, 15, hTab, NULL, NULL, NULL);
				clsBot::h1stChatDelay[i] = CreateWindowEx(NULL, L"edit", L"15", WS_CHILD | WS_BORDER | ES_CENTER, 150, 58 + 25*i, 50, 20, hTab, NULL, NULL, NULL);
				hStaticTab2[12+i] = CreateWindowEx(NULL, L"static", L"s", WS_CHILD, 201, 60 + 25*i, 10, 15, hTab, NULL, NULL, NULL);
				clsBot::h2ndChatDelay[i] = CreateWindowEx(NULL, L"edit", L"10", WS_CHILD | WS_BORDER | ES_CENTER, 220, 58 + 25*i, 50, 20, hTab, NULL, NULL, NULL);
				hStaticTab2[20+i] = CreateWindowEx(NULL, L"static", L"s", WS_CHILD, 271, 60 + 25*i, 10, 15, hTab, NULL, NULL, NULL);
			}

			 hNote = CreateWindowEx(NULL, L"static", L"NOTE:\n\nLeave is the total time it will stay in game, and all delays are in seconds.", WS_CHILD, 300, 35, 200, 200, hTab, NULL, NULL, NULL);
		}
		// Game Settings Tab
		{
			hStaticTab3[1] = CreateWindowEx(NULL, L"static", L"Game Type", WS_CHILD, 80, 35, 100, 15, hTab, NULL, NULL, NULL);
			hStaticTab3[2] = CreateWindowEx(NULL, L"static", L"Limit", WS_CHILD, 350, 35, 50, 15, hTab, NULL, NULL, NULL);

			for (int i = 0; i < 8; i++)
			{
				clsBot::hGameType[i] = CreateWindowEx(NULL, L"button", L"", WS_CHILD | BS_GROUPBOX, 80, 48 + 25*i, 240, 35, hTab, NULL, NULL, NULL);
				clsBot::hMultiplayer[i] = CreateWindowEx(NULL, L"button", L"Multiplayer", WS_CHILD | BS_AUTORADIOBUTTON, 15, 12, 100, 20, clsBot::hGameType[i], NULL, NULL, NULL);
				clsBot::hCooperative[i] = CreateWindowEx(NULL, L"button", L"Cooperative", WS_CHILD | BS_AUTORADIOBUTTON, 125, 12, 100, 20, clsBot::hGameType[i], NULL, NULL, NULL);
				clsBot::hGameLimit[i] = CreateWindowEx(NULL, L"edit", L"0", WS_CHILD | WS_BORDER | ES_CENTER, 350, 58 + 25*i, 50, 20, hTab, NULL, NULL, NULL);
				hStaticTab3[3+i] = CreateWindowEx(NULL, L"static", L"games", WS_CHILD, 410, 60 + 25*i, 50, 20, hTab, NULL, NULL, NULL);

				SendMessage(clsBot::hMultiplayer[i], BM_CLICK, NULL, NULL);				
			}

			hStaticTab3[0] = CreateWindowEx(NULL, L"static", L"NOTE:\n\nSetting 0 will make the bot have no limit.", WS_CHILD, 475, 60, 200, 200, hTab, NULL, NULL, NULL);
		}
		// Settings Tab
		{
			hStaticTab4[0] = CreateWindowEx(NULL, L"static", L"Start Hotkey:", WS_CHILD, 10, 25+10, 100, 15, hTab, NULL, NULL, NULL);
			hStartHotkey = CreateWindowEx(NULL, HOTKEY_CLASS, L"", WS_CHILD, 100, 25+8, 200, 20, hTab, NULL, NULL, NULL);
			hStaticTab4[1] = CreateWindowEx(NULL, L"static", L"Stop Hotkey:", WS_CHILD, 10, 50+10, 100, 15, hTab, NULL, NULL, NULL);
			hStopHotkey = CreateWindowEx(NULL, HOTKEY_CLASS, L"", WS_CHILD, 100, 50+8, 200, 20, hTab, NULL, NULL, NULL);
			hStaticTab4[2] = CreateWindowEx(NULL, L"static", L"Surrender:", WS_CHILD, 350, 25+10, 100, 15, hTab, NULL, NULL, NULL);
			clsBot::hSurrender = CreateWindowEx(NULL, L"edit", L"N", WS_CHILD | WS_BORDER | ES_AUTOHSCROLL, 425, 25+8, 25, 20, hTab, NULL, NULL, NULL);
			hStaticTab4[3] = CreateWindowEx(NULL, L"static", L"Score Screen:", WS_CHILD, 350, 50+10, 100, 15, hTab, NULL, NULL, NULL);
			clsBot::hScoreScreen = CreateWindowEx(NULL, L"edit", L"S", WS_CHILD | WS_BORDER | ES_AUTOHSCROLL, 445, 50+8, 25, 20, hTab, NULL, NULL, NULL);
			clsBot::hShow1stChat = CreateWindowEx(NULL, L"button", L"Show 1st Chat", WS_CHILD | BS_CHECKBOX | BS_AUTOCHECKBOX, 10, 88+10, 140, 15, hTab, NULL, NULL, NULL);
			clsBot::hShow2ndChat = CreateWindowEx(NULL, L"button", L"Show 2nd Chat", WS_CHILD | BS_CHECKBOX | BS_AUTOCHECKBOX, 10, 113+10, 140, 15, hTab, NULL, NULL, NULL);			
			clsBot::hChat[0] = CreateWindowEx(NULL, L"button", L"", WS_CHILD | BS_GROUPBOX, 150, 83 + 25*0, 125, 35, hTab, NULL, NULL, NULL);
			clsBot::hAll[0] = CreateWindowEx(NULL, L"button", L"All", WS_CHILD | BS_AUTORADIOBUTTON, 15, 12, 40, 20, clsBot::hChat[0], NULL, NULL, NULL);
			clsBot::hAllies[0] = CreateWindowEx(NULL, L"button", L"Allies", WS_CHILD | BS_AUTORADIOBUTTON, 60, 12, 55, 20, clsBot::hChat[0], NULL, NULL, NULL);
			clsBot::hChat[1] = CreateWindowEx(NULL, L"button", L"", WS_CHILD | BS_GROUPBOX, 150, 83 + 25*1, 125, 35, hTab, NULL, NULL, NULL);
			clsBot::hAll[1] = CreateWindowEx(NULL, L"button", L"All", WS_CHILD | BS_AUTORADIOBUTTON, 15, 12, 40, 20, clsBot::hChat[1], NULL, NULL, NULL);
			clsBot::hAllies[1] = CreateWindowEx(NULL, L"button", L"Allies", WS_CHILD | BS_AUTORADIOBUTTON, 60, 12, 55, 20, clsBot::hChat[1], NULL, NULL, NULL);			
			hStaticTab4[4] = CreateWindowEx(NULL, L"static", L"1st Chat:", WS_CHILD, 10, 151+10, 100, 15, hTab, NULL, NULL, NULL);
			clsBot::h1stChat = CreateWindowEx(NULL, L"edit", L"hi", WS_CHILD | WS_BORDER | ES_AUTOHSCROLL, 80, 151+8, 590, 20, hTab, NULL, NULL, NULL);
			hStaticTab4[5] = CreateWindowEx(NULL, L"static", L"2nd Chat:", WS_CHILD, 10, 176+10, 100, 15, hTab, NULL, NULL, NULL);
			clsBot::h2ndChat = CreateWindowEx(NULL, L"edit", L"lag", WS_CHILD | WS_BORDER | ES_AUTOHSCROLL, 80, 176+8, 590, 20, hTab, NULL, NULL, NULL);
			hSaveSettings = CreateWindowEx(NULL, L"button", L"Save", WS_CHILD | BS_PUSHBUTTON, 90, 225+10, 200, 25, hTab, (HMENU)IDC_Save, NULL, NULL);
			
			SendMessage(hStartHotkey, HKM_SETHOTKEY, VK_F8, NULL);
			SendMessage(hStopHotkey, HKM_SETHOTKEY, VK_F9, NULL);
			SendMessage(hWnd, WM_SETHOTKEY, NULL, NULL);
			SendMessage(clsBot::hAll[0], BM_CLICK, NULL, NULL);
			SendMessage(clsBot::hAll[1], BM_CLICK, NULL, NULL);
		}

		TabControlProc = reinterpret_cast<WNDPROC>(GetWindowLongPtr(hTab, GWLP_WNDPROC));
		SetWindowLongPtr(hTab, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(TabControlRelay));

		try
		{
			wifstream myFile;
			myFile.open("multi-lossbot.sc2", ios::in);

			if (myFile.is_open() == true)
			{
				RECT rect;
				GetWindowRect(hWnd, &rect);
				wchar_t buffer[255] = {NULL};
				
				myFile >> buffer >> buffer; // ignoring the 1st 2 x, y coords
				myFile >> buffer;
				(_wtoi(buffer) == BST_CHECKED) ? SendMessage(clsBot::hShow1stChat, BM_CLICK, NULL, NULL) : NULL;
				myFile >> buffer;
				(_wtoi(buffer) == BST_CHECKED) ? SendMessage(clsBot::hShow2ndChat, BM_CLICK, NULL, NULL) : NULL;
				myFile >> buffer;
				SendMessage(hStartHotkey, HKM_SETHOTKEY, _wtoi(buffer), NULL);
				myFile >> buffer;
				SendMessage(hStopHotkey, HKM_SETHOTKEY, _wtoi(buffer), NULL);
				myFile.getline(buffer, 255);
				myFile.getline(buffer, 255);
				SendMessage(clsBot::h1stChat, WM_SETTEXT, NULL, (LPARAM)buffer);
				myFile.getline(buffer, 255);
				SendMessage(clsBot::h2ndChat, WM_SETTEXT, NULL, (LPARAM)buffer);
				myFile.getline(buffer, 255);
				SendMessage(clsBot::hSurrender, WM_SETTEXT, NULL, (LPARAM)buffer);
				myFile.getline(buffer, 255);
				SendMessage(clsBot::hScoreScreen, WM_SETTEXT, NULL, (LPARAM)buffer);

				for (int i = 0; i < 8; i++)
				{
					myFile >> buffer;
					SendMessage(clsBot::hLeaveDelay[i], WM_SETTEXT, NULL, (LPARAM)buffer);
					myFile >> buffer;
					SendMessage(clsBot::hGameLimit[i], WM_SETTEXT, NULL, (LPARAM)buffer);
					myFile >> buffer;
					SendMessage(clsBot::h1stChatDelay[i], WM_SETTEXT, NULL, (LPARAM)buffer);
					myFile >> buffer;
					SendMessage(clsBot::h2ndChatDelay[i], WM_SETTEXT, NULL, (LPARAM)buffer);
				}

				myFile.close();
				SendMessage(hWnd, WM_SETHOTKEY, NULL, NULL);
			}
		} catch(...) {}

		break;

	case WM_DESTROY:
		for (int i = 0; i < 8; i++)
			ShowWindow(clsGame::Handle[i], SW_SHOW);
		PostQuitMessage(0);
		break;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

LRESULT CALLBACK TabControlRelay(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
     if (msg == WM_COMMAND || msg == WM_SETHOTKEY)
         return SendMessage(GetParent(hwnd), msg, wParam, lParam);
  
     return CallWindowProc(TabControlProc, hwnd, msg, wParam, lParam);
}