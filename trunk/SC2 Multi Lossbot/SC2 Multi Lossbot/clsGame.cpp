#include "stdafx.h"
#include "clsGame.h"
#include "clsMemory.h"
#include "clsBot.h"
#include "resource.h"
#include <time.h>
#include <bitset>

time_t clsGame::Timer[8] = {NULL};
HWND clsGame::Handle[8] = {NULL};
HANDLE clsGame::Process[8] = {NULL};
DWORD clsGame::PID[8] = {NULL};
int clsGame::ResolutionHeight = NULL;
int clsGame::ResolutionWidth = NULL;
bool clsGame::TimesUp[8] = {NULL};
bool clsGame::TimerLeave[8] = {NULL};
bool clsGame::Timer1stChat[8] = {NULL};
bool clsGame::Timer2ndChat[8] = {NULL};
bool clsGame::GameFound[8] = {NULL};
string clsGame::SearchHotkey = "";
string clsGame::QuitHotkey = "";
int clsGame::MyPlayerNumber = 0;
int clsGame::EnemyPlayerNumber = 0;

void clsGame::Setup(void)
{
	bool delayError = false;
	wstring msg1st = L"";
	wstring msg2nd = L"";
	int wc3Instance;
	wchar_t buffer[255] = {NULL};
	wstring title;

	// Count the number of wc3's currently setup
	for (wc3Instance = 0; wc3Instance < 8; wc3Instance++)
	{
		_itow_s(wc3Instance+1, buffer, 10);
		title = L"SC2 #";
		title += buffer;

		clsGame::Handle[wc3Instance] = FindWindow(L"Starcraft II", title.c_str());

		if (clsGame::Handle[wc3Instance] != NULL)
		{
			GetWindowThreadProcessId(clsGame::Handle[wc3Instance], &clsGame::PID[wc3Instance]);
			clsBot::SetupWarcraft(wc3Instance);
		}
		else
		{
			// check if running in sandbox

			clsGame::Handle[wc3Instance] = FindWindow(L"Sandbox:DefaultBox:StarCraft II", title.c_str());

			if (clsGame::Handle[wc3Instance] != NULL)
			{
				GetWindowThreadProcessId(clsGame::Handle[wc3Instance], &clsGame::PID[wc3Instance]);
				clsBot::SetupWarcraft(wc3Instance);
			}
		}
	}

	// Rename the remaining wc3's
	for (wc3Instance = 0; wc3Instance < 8; wc3Instance++)
	{
		if (clsGame::Handle[wc3Instance] == NULL)
		{
			clsGame::Handle[wc3Instance] = FindWindow(L"Starcraft II", L"Starcraft II");

			if (clsGame::Handle[wc3Instance] == NULL)
			{
				// check for sandbox

				clsGame::Handle[wc3Instance] = FindWindow(L"Sandbox:DefaultBox:StarCraft II", L"[#] StarCraft II [#]");

				if (clsGame::Handle[wc3Instance] != NULL)
				{
					GetWindowThreadProcessId(clsGame::Handle[wc3Instance], &clsGame::PID[wc3Instance]);
					clsBot::SetupWarcraft(wc3Instance);

					_itow_s(wc3Instance+1, buffer, 10);
					title = L"SC2 #";
					title += buffer;
					SetWindowText(clsGame::Handle[wc3Instance], title.c_str());
				}
				else
				{
					SendMessage(clsBot::hStatus[wc3Instance], WM_SETTEXT, NULL, (LPARAM)L"Not Found");
				}
			}
			else
			{
				GetWindowThreadProcessId(clsGame::Handle[wc3Instance], &clsGame::PID[wc3Instance]);
				clsBot::SetupWarcraft(wc3Instance);

				_itow_s(wc3Instance+1, buffer, 10);
				title = L"SC2 #";
				title += buffer;
				SetWindowText(clsGame::Handle[wc3Instance], title.c_str());
			}
		}
	
		// Calculate if chat delays are great than leave delays
		SendMessage(clsBot::hLeaveDelay[wc3Instance], WM_GETTEXT, 10, (LPARAM)buffer);
		int delayLeave = _wtoi(buffer);
		SendMessage(clsBot::h1stChatDelay[wc3Instance], WM_GETTEXT, 10, (LPARAM)buffer);
		int delay1stChat = _wtoi(buffer);
		SendMessage(clsBot::h2ndChatDelay[wc3Instance], WM_GETTEXT, 10, (LPARAM)buffer);
		int delay2ndChat = _wtoi(buffer);

		// Check to see if show chat is enabled, then show a warning if it is
		if (SendMessage(clsBot::hShow1stChat, BM_GETCHECK, NULL, NULL) == BST_CHECKED && delay1stChat > delayLeave)
		{
			SendMessage(clsBot::hStatus[wc3Instance], WM_GETTEXT, 255, (LPARAM)&buffer);

			if (wstring(buffer) != L"Not Found")
			{
				msg1st += L"SC2 #";
				_itow_s(wc3Instance+1, buffer, 10);
				msg1st += buffer;
				msg1st += L", ";
				delayError = true;
			}

			if (wc3Instance == 7 && delayError == true)
			{
				msg1st.insert(0, L"1st Chat Delay for ");
				msg1st = msg1st.substr(0, msg1st.find_last_of(L','));
				msg1st += L" is greater than the Leave delay so 1st Chat will never be displayed.";
			}
		}

		if (SendMessage(clsBot::hShow2ndChat, BM_GETCHECK, NULL, NULL) == BST_CHECKED && delay2ndChat > delayLeave)
		{
			SendMessage(clsBot::hStatus[wc3Instance], WM_GETTEXT, 255, (LPARAM)buffer);

			if (wstring(buffer) != L"Not Found")
			{
				msg2nd += L"SC2 #";
				_itow_s(wc3Instance+1, buffer, 10);
				msg2nd += buffer;
				msg2nd += L", ";
				delayError = true;
			}

			if (wc3Instance == 7 && delayError == true && msg2nd != L"")
			{
				msg2nd.insert(0, L"2nd Chat Delay for ");
				msg2nd = msg2nd.substr(0, msg2nd.find_last_of(L','));
				msg2nd += L" is greater than the Leave delay so 2nd Chat will never be displayed.";

				if (msg1st != L"")
					msg2nd.insert(0, L"\n\n");
			}
		}	
	}

	// Display an error if show chats are enabled but the delays are greater than the leave delay
	if (delayError == true)
		MessageBox(NULL, (msg1st + msg2nd).c_str(), L"Warning", MB_ICONWARNING);
}

void clsGame::EnableDebugPriv(void)
{
    HANDLE hToken;
    LUID sedebugnameValue;
    TOKEN_PRIVILEGES tkp;

    if ( ! OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken ) )
        return;
    if ( ! LookupPrivilegeValue( NULL, SE_DEBUG_NAME, &sedebugnameValue ) )
	{
        CloseHandle( hToken );
        return;
    }
    tkp.PrivilegeCount = 1;
    tkp.Privileges[0].Luid = sedebugnameValue;
    tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
    if ( ! AdjustTokenPrivileges( hToken, FALSE, &tkp, sizeof tkp, NULL, NULL ) )
        CloseHandle( hToken );
}

DWORD clsGame::GetModuleBase(DWORD hProcId, TCHAR* lpModName)
{
    DWORD result = NULL;
    HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, hProcId);

    MODULEENTRY32 tm;
    tm.dwSize = sizeof(MODULEENTRY32);
	
    if (Module32First(hSnap, &tm))
    {
        while (Module32Next(hSnap, &tm))
        {
            if (lstrcmpi(tm.szModule, lpModName) == 0)
            {
                result = (DWORD)tm.modBaseAddr;
                break;
            }
        }
    }

    CloseHandle(hSnap);
	
    return result;
}

void clsGame::StartQuickSearch(int instance)
{	
	if (SendMessage(clsBot::hMultiplayer[instance], BM_GETCHECK, NULL, NULL) == BST_CHECKED)
	{
		clsBot::MouseClick(instance, 0.25390625, 0.0768229166666667); // multiplayer
	}
	else
	{
		clsBot::MouseClick(instance, 0.8251953125, 0.474308300395257); // cooperative
	}

	clsBot::MouseClick(instance, 0.1640625, 0.872200263504611); // back (score screen)
	clsBot::MouseClick(instance, 0.146484375, 0.854166666666667); // find match

	SendMessage(clsBot::hStatus[instance], WM_SETTEXT, NULL, (LPARAM)L"Searching");
}

void clsGame::QuitGame(int instance)
{
	clsGame::GameFound[instance] = false;
	SendMessage(clsBot::hStatus[instance], WM_SETTEXT, NULL, (LPARAM)L"Quitting");

	// Add 1 game to the game control
	wchar_t buffer[255] = {NULL};
	SendMessage(clsBot::hGames[instance], WM_GETTEXT, 10, (LPARAM)buffer);
	_itow_s(_wtoi(buffer)+1, buffer, 10);
	SendMessage(clsBot::hGames[instance], WM_SETTEXT, NULL, (LPARAM)buffer);

	// Determine if game was a win or loss
	string test = clsMemory::ScreenChat(clsGame::Process[instance]);
	if (test != "")
	{
		SendMessage(clsBot::hWon[instance], WM_GETTEXT, 10, (LPARAM)buffer);
		_itow_s(_wtoi(buffer)+1, buffer, 10);
		SendMessage(clsBot::hWon[instance], WM_SETTEXT, NULL, (LPARAM)buffer);
	}
	else
	{
		SendMessage(clsBot::hLoss[instance], WM_GETTEXT, 10, (LPARAM)buffer);
		_itow_s(_wtoi(buffer)+1, buffer, 10);
		SendMessage(clsBot::hLoss[instance], WM_SETTEXT, NULL, (LPARAM)buffer);
	}

	wchar_t surrender[255] = {NULL};
	SendMessage(clsBot::hSurrender, WM_GETTEXT, 255, (LPARAM)surrender);
	wchar_t scorescreen[255] = {NULL};
	SendMessage(clsBot::hScoreScreen, WM_GETTEXT, 255, (LPARAM)scorescreen);

	if (surrender[0] >= 'a' && surrender[0] <= 'z')
	{
		surrender[0] += 'A' - 'a';
	}

	if (scorescreen[0] >= 'a' && scorescreen[0] <= 'z')
	{
		scorescreen[0] += 'A' - 'a';
	}

	do
	{
		PostMessage(clsGame::Handle[instance], WM_SYSKEYDOWN, VK_F10, 4456449);
		PostMessage(clsGame::Handle[instance], WM_SYSKEYUP, VK_F10, -1069285375);

		PostMessage(clsGame::Handle[instance], WM_KEYDOWN, surrender[0], 3211265);
		PostMessage(clsGame::Handle[instance], WM_CHAR, (WPARAM)surrender[0], 3211265);
		PostMessage(clsGame::Handle[instance], WM_KEYUP, surrender[0], -1070530559);

		PostMessage(clsGame::Handle[instance], WM_KEYDOWN, scorescreen[0], 2031617);
		PostMessage(clsGame::Handle[instance], WM_CHAR, (WPARAM)scorescreen[0], 2031617);
		PostMessage(clsGame::Handle[instance], WM_KEYUP, scorescreen[0], -1071710207);

		Sleep(100);
	} while (clsMemory::MapName(clsGame::Process[instance]) != "");

	// Check if game limit has been reached

	wchar_t gameLimit[255] = {NULL};
	SendMessage(clsBot::hGameLimit[instance], WM_GETTEXT, 255, (LPARAM)gameLimit);

	if (wstring(gameLimit) != L"0")
	{
		wchar_t gameTotal[255] = {NULL};
		SendMessage(clsBot::hGames[instance], WM_GETTEXT, 255, (LPARAM)gameTotal);

		if (_wtoi(gameTotal) >= _wtoi(gameLimit))
		{
			SendMessage(clsBot::hStatus[instance], WM_SETTEXT, NULL, (LPARAM)L"Limit Reached");
			clsGame::PID[instance] = NULL;
		}
	}
}

void clsGame::CleanUp(int instance, HWND hWnd)
{
	clsGame::TimesUp[instance] = false;
	clsGame::TimerLeave[instance] = false;
	clsGame::Timer1stChat[instance] = false;
	clsGame::Timer2ndChat[instance] = false;
	KillTimer(hWnd, IDT_Leave + instance);
	KillTimer(hWnd, IDT_1stChat + instance);
	KillTimer(hWnd, IDT_2ndChat + instance);

	SendMessage(clsBot::hStatus[instance], WM_SETTEXT, NULL, (LPARAM)L"Idle");
}

void clsGame::DetectPlayerNumber(int instance)
{
	wchar_t nameBuffer[255] = {NULL};
	SendMessage(clsBot::hPlayerName[instance], WM_GETTEXT, 255, (LPARAM)nameBuffer);

	for (int i = 0; i < 255; i++)
	{
		if (nameBuffer[i] >= 'a' && nameBuffer[i] <= 'z')
		{
			nameBuffer[i] += 'A' - 'a';
		}
	}

	for (int i = 1; i <= 2; i++)
	{
		string testName = clsMemory::PlayerName(clsGame::Process[instance], i);

		for (int i = 0; i < testName.length(); i++)
		{
			if (testName[i] >= 'a' && testName[i] <= 'z')
			{
				testName[i] += 'A' - 'a';
			}
		}

		if (wstring(testName.begin(), testName.end()) == wstring(nameBuffer))
		{
			clsGame::MyPlayerNumber = i;

			if (clsGame::MyPlayerNumber == 1)
				clsGame::EnemyPlayerNumber = 2;
			else
				clsGame::EnemyPlayerNumber = 1;

			break;
		}
	}

	if (clsGame::MyPlayerNumber == 0)
	{
		// we got an bug!
		MessageBox(NULL, L"Couldn't detect player name?", L"Bug!", MB_ICONWARNING);
	}
}