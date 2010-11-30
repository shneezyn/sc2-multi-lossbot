#include <string>
#include <Windows.h>

using namespace std;

class clsMemory
{
public:
	static string PlayerName(HANDLE procHandle, int playerNumber);
	static string ScreenChat(HANDLE procHandle);
	static string MapName(HANDLE procHandle);
	static string FoodSupply(HANDLE procHandle);
};