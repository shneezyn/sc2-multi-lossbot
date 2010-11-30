#include <iostream>
#include <tlhelp32.h>
#include <time.h>

using namespace std;

class clsGame
{
public:
	static int EnemyPlayerNumber;
	static int MyPlayerNumber;
	static string SearchHotkey;
	static string QuitHotkey;
	static bool GameFound[8];
	static time_t Timer[8];
	static bool TimerLeave[8];
	static bool Timer1stChat[8];
	static bool Timer2ndChat[8];
	static bool TimesUp[8];
	static HWND Handle[8];
	static HANDLE Process[8];
	static DWORD PID[8];
	static void Setup(void);
	static void EnableDebugPriv(void);
	static DWORD GetModuleBase(DWORD hProcId, TCHAR* lpModName);
	static void StartQuickSearch(int instance);
	static void QuitGame(int instance);
	static void CleanUp(int instance, HWND hWnd);
	static void DetectPlayerNumber(int instance);
private:
	static int ResolutionWidth;
	static int ResolutionHeight;
};