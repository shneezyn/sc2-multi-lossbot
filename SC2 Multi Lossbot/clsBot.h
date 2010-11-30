#include <string>
#include <atlstr.h>

using namespace std;

class clsBot
{
public:
	static HWND hChat[2];
	static HWND hAll[2];
	static HWND hAllies[2];
	static HWND hGameType[8];
	static HWND hMultiplayer[8];
	static HWND hCooperative[8];
	static HWND hWindowStatus[8];
	static HWND hVisible[8];
	static HWND hPlayerName[8];
	static HWND hHidden[8];
	static HWND hDisabled[8];
	static HWND hStatus[8];
	static HWND hGameLimit[8];
	static HWND hGames[8];
	static HWND hWon[8];
	static HWND hLoss[8];
	static HWND h2ndChatDelay[8];
	static HWND h1stChatDelay[8];
	static HWND hLeaveDelay[8];
	static HWND h1stChat;
	static HWND h2ndChat;
	static HWND hShow1stChat;
	static HWND hShow2ndChat;
	static HWND hScoreScreen;
	static HWND hSurrender;
	static HANDLE hStartBot;
	static void SetupWarcraft(int instance);
	static DWORD WINAPI StartBot(LPVOID lpParam);
	static void MouseClick(int instance, double x, double y);
	static BOOL SetupWindow(int instance, RECT rctClient, RECT rctWindow);
	static BOOL SupportedResolution(int width, int height);
};