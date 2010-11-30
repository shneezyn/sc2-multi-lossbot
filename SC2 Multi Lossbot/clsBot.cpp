#include "stdafx.h"
#include "clsBot.h"
#include "clsGame.h"
#include "clsMemory.h"
#include <tlhelp32.h>
#include "resource.h"
#include <time.h>
#include <sstream>

using namespace std;

HANDLE clsBot::hStartBot = NULL;
HWND clsBot::hChat[2] = {NULL};
HWND clsBot::hAll[2] = {NULL};
HWND clsBot::hAllies[2] = {NULL};
HWND clsBot::hGameType[8] = {NULL};
HWND clsBot::hMultiplayer[8] = {NULL};
HWND clsBot::hCooperative[8] = {NULL};
HWND clsBot::hWindowStatus[8] = {NULL};
HWND clsBot::hVisible[8] = {NULL};
HWND clsBot::hGameLimit[8] = {NULL};
HWND clsBot::hDisabled[8] = {NULL};
HWND clsBot::hHidden[8] = {NULL};
HWND clsBot::hPlayerName[8] = {NULL};
HWND clsBot::hStatus[8] = {NULL};
HWND clsBot::hGames[8] = {NULL};
HWND clsBot::hWon[8] = {NULL};
HWND clsBot::hLoss[8] = {NULL};
HWND clsBot::hLeaveDelay[8] = {NULL};
HWND clsBot::h1stChatDelay[8] = {NULL};
HWND clsBot::h2ndChatDelay[8] = {NULL};
HWND clsBot::h1stChat;
HWND clsBot::h2ndChat;
HWND clsBot::hShow2ndChat;
HWND clsBot::hShow1stChat;
HWND clsBot::hScoreScreen;
HWND clsBot::hSurrender;

DWORD WINAPI clsBot::StartBot(LPVOID lpParam)
{
	clsGame::Setup();
	
	while (true)
	{
		for (int instance = 0; instance < 8; instance++)
		{
			if (clsGame::PID[instance] != NULL)
			{
				try
				{
					// clean up

					wchar_t buffer[255] = {NULL};					
					SendMessage(clsBot::hStatus[instance], WM_GETTEXT, 255, (LPARAM)buffer);
					
					if (clsMemory::MapName(clsGame::Process[instance]) == "" && wstring(buffer) != L"Searching")
					{
						clsGame::CleanUp(instance, (HWND)lpParam);
					}
				} catch (...) {}

				try
				{
					wchar_t buffer[255] = {NULL};
					SendMessage(clsBot::hStatus[instance], WM_GETTEXT, 255, (LPARAM)buffer);
					
					// Search for game
					if (true)
					{
						wchar_t buffer[255] = {NULL};
						SendMessage(clsBot::hStatus[instance], WM_GETTEXT, 255, (LPARAM)buffer);

						if (wstring(buffer) == L"Idle" || wstring(buffer) == L"Searching")
						{
							clsGame::StartQuickSearch(instance);
							clsBot::MouseClick(instance, 0.6067708333333333, 0.5796296296296296); // disconnect msg
						}
					}
				} catch (...) {}

				try
				{
					string map = clsMemory::MapName(clsGame::Process[instance]);

					wchar_t buffer[255] = {NULL};
					SendMessage(clsBot::hStatus[instance], WM_GETTEXT, 255, (LPARAM)buffer);

					// Check if game was found
					if ((clsGame::GameFound[instance] == false && map != "") || (map != "" && wstring(buffer) == L"Idle"))
					{
						SendMessage(clsBot::hStatus[instance], WM_SETTEXT, NULL, (LPARAM)L"Game Found");
						clsGame::GameFound[instance] = true;
					}
				} catch (...) {}

				try
				{
					// Check if in game
					if (clsMemory::FoodSupply(clsGame::Process[instance]) != "")
					{
						if (clsGame::TimerLeave[instance] == false)
						{
							wchar_t buffer[255] = {NULL};
							SendMessage(clsBot::hLeaveDelay[instance], WM_GETTEXT, 10, (LPARAM)buffer);
							SetTimer((HWND)lpParam, IDT_Leave + instance, (UINT)(1000*_wtoi(buffer)), (TIMERPROC)NULL);
							clsGame::Timer[instance] = time(NULL);
							clsGame::TimerLeave[instance] = true;

							if (clsGame::Timer1stChat[instance] == false && SendMessage(clsBot::hShow1stChat, BM_GETCHECK, NULL, NULL) == BST_CHECKED)
							{
								SendMessage(clsBot::h1stChatDelay[instance], WM_GETTEXT, 5, (LPARAM)buffer);
								SetTimer((HWND)lpParam, IDT_1stChat + instance, (UINT)(1000*_wtoi(buffer)), (TIMERPROC)NULL);
								clsGame::Timer1stChat[instance] = true;
							}
						}

						wchar_t buffer[255] = {NULL};
						SendMessage(clsBot::hStatus[instance], WM_GETTEXT, 255, (LPARAM)buffer);

						if (wstring(buffer) != L"Idle")
						{
							wstringstream ss;
							ss << difftime(time(NULL), clsGame::Timer[instance]);
							SendMessage(clsBot::hStatus[instance], WM_SETTEXT, NULL, (LPARAM)(ss.str() + L" seconds").c_str());
						}
					}
				} catch (...) {}

				try
				{
					// Quitting game
					string chat = clsMemory::ScreenChat(clsGame::Handle[instance]);
					if (clsGame::TimesUp[instance] == true || chat.substr(0, 2) == "<c")
					{
						clsGame::QuitGame(instance);
					}
				} catch (...) {}
			}
		}

		Sleep(100);
	}

	return NULL;
}

void clsBot::SetupWarcraft(int instance)
{
	// Setup memory reading & get PID
	clsGame::EnableDebugPriv();
	clsGame::Process[instance] = OpenProcess(PROCESS_VM_READ, false, clsGame::PID[instance]);

	if (SendMessage(clsBot::hHidden[instance], BM_GETCHECK, NULL, NULL) == BST_CHECKED)
		ShowWindow(clsGame::Handle[instance], SW_HIDE);
	else if (SendMessage(clsBot::hDisabled[instance], BM_GETCHECK, NULL, NULL) == BST_CHECKED)
	{
		clsGame::PID[instance] = NULL;
		SendMessage(clsBot::hStatus[instance], WM_SETTEXT, NULL, (LPARAM)L"Disabled");
	}
}

void clsBot::MouseClick(int instance, double x, double y)
{
	RECT rctWindow;
	GetWindowRect(clsGame::Handle[instance], &rctWindow);
	RECT rctClient;
	GetClientRect(clsGame::Handle[instance], &rctClient);

	if (clsBot::SetupWindow(instance, rctClient, rctWindow) == FALSE)
	{
		return;
	}
	
	x *= rctClient.right;
	y *= rctClient.bottom;
	
	x = (int)(x + 0.5);
	y = (int)(y + 0.5);

	SendMessage(clsGame::Handle[instance], WM_SETCURSOR, (WPARAM)clsGame::Handle[instance],  MAKELPARAM(HTCLIENT, WM_MOUSEMOVE));
	PostMessage(clsGame::Handle[instance], WM_MOUSEMOVE, MK_LBUTTON, MAKELPARAM(x, y));

	SendMessage(clsGame::Handle[instance], WM_SETCURSOR, (WPARAM)clsGame::Handle[instance],  MAKELPARAM(HTCLIENT, WM_LBUTTONDOWN));				
	PostMessage(clsGame::Handle[instance], WM_LBUTTONDOWN, MK_LBUTTON, MAKELPARAM(x, y));

	PostMessage(clsGame::Handle[instance], WM_LBUTTONUP, NULL, MAKELPARAM(x, y));

	SendMessage(clsGame::Handle[instance], WM_SETCURSOR, (WPARAM)clsGame::Handle[instance],  MAKELPARAM(HTCLIENT, WM_MOUSEMOVE));
	PostMessage(clsGame::Handle[instance], WM_MOUSEMOVE, MK_LBUTTON, MAKELPARAM(x, y));
}

BOOL clsBot::SetupWindow(int instance, RECT rctClient, RECT rctWindow)
{
	int resWidth = GetSystemMetrics(SM_CXSCREEN);
	int resHeight = GetSystemMetrics(SM_CYSCREEN);

	int monitors = GetSystemMetrics(SM_CMONITORS);

	if (monitors > 1)
	{
		resWidth = GetSystemMetrics(SM_CXVIRTUALSCREEN);
		resHeight = GetSystemMetrics(SM_CYVIRTUALSCREEN);
	}

	int xMetrics = GetSystemMetrics(SM_CXFRAME);
	int yMetrics = GetSystemMetrics(SM_CYFRAME) + GetSystemMetrics(SM_CYCAPTION);

	BOOL moveWindow = FALSE;

	int width = rctClient.right;
	int height = rctClient.bottom;

	if (rctClient.bottom != rctWindow.bottom && rctClient.right != rctWindow.right)
	{
		width = 1024;
		height = 768;
	}

	if (clsBot::SupportedResolution(width, height) == FALSE)
	{
		MessageBox(NULL, L"Resolution isn't supported.", L"ERROR!", MB_ICONEXCLAMATION);
		clsGame::PID[instance] = NULL;
		wchar_t szTitle[100];
		LoadString(NULL, IDS_APP_TITLE, szTitle, 100);
		SendMessage(FindWindow(L" ", szTitle), WM_HOTKEY, HOTKEY_STOP, NULL);
		return FALSE;
	}
	
	if (rctClient.right != width && rctClient.bottom != height)
	{
		rctClient.right = width;
		rctClient.bottom = height;
		moveWindow = TRUE;
	}
	if (rctWindow.left < 0)
	{
		rctWindow.left = 0;
		moveWindow = TRUE;
	}
	if (rctWindow.top < 0)
	{
		rctWindow.top = 0;
		moveWindow = TRUE;
	}
	if (rctWindow.right > resWidth)
	{
		rctWindow.left = resWidth - width - xMetrics * 2;
		rctWindow.right = resWidth;
		moveWindow = TRUE;
	}
	if (rctWindow.bottom > resHeight)
	{
		rctWindow.top = resHeight - height - yMetrics;
		rctWindow.bottom = resHeight;
		moveWindow = TRUE;
	}

	if (moveWindow == TRUE)
	{
		MoveWindow(clsGame::Handle[instance], rctWindow.left, rctWindow.top, width + xMetrics, height + yMetrics, false);
	}

	return TRUE;
}

BOOL clsBot::SupportedResolution(int width, int height)
{
	float client = (float)width / height;
	float supported = (float)1024 / 768;

	if (client == supported)
	{
		return TRUE;
	}

	return FALSE;
}