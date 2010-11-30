#include "stdafx.h"
#include "clsMemory.h"

using namespace std;

// Used to check if game has been found or still in game or what the map is
string clsMemory::MapName(HANDLE procHandle)
{
	string value = "";
	char buffer[255] = {NULL};

	DWORD addrValue = 0x1C4C0C8; // base address
	ReadProcessMemory(procHandle, (LPCVOID)addrValue, &addrValue, sizeof(DWORD), NULL); // base address points to
	ReadProcessMemory(procHandle, (LPCVOID)(addrValue + 0x2A0), &buffer, sizeof(buffer), NULL); // offset

	return value = buffer;
}

// Used to check if game is over
string clsMemory::ScreenChat(HANDLE procHandle)
{
	string value = "";
	char buffer[255] = {NULL};

	DWORD addrValue = 0x17D1988; // base address
	ReadProcessMemory(procHandle, (LPCVOID)addrValue, &addrValue, sizeof(DWORD), NULL); // base address points to
	ReadProcessMemory(procHandle, (LPCVOID)(addrValue + 0x2D8), &addrValue, sizeof(DWORD), NULL); // offset
	ReadProcessMemory(procHandle, (LPCVOID)(addrValue + 0x7C), &addrValue, sizeof(DWORD), NULL); // offset
	ReadProcessMemory(procHandle, (LPCVOID)(addrValue + 0x1DC), &addrValue, sizeof(DWORD), NULL); // offset
	ReadProcessMemory(procHandle, (LPCVOID)(addrValue + 0x14), &addrValue, sizeof(DWORD), NULL); // offset
	ReadProcessMemory(procHandle, (LPCVOID)(addrValue + 0x0), &buffer, sizeof(buffer), NULL); // offset

	return value = buffer;
}

// used to determine the player name
string clsMemory::PlayerName(HANDLE procHandle, int playerNumber)
{
	string value = "";
	char buffer[255] = {NULL};

	DWORD addrValue = 0x0;

	if (playerNumber == 1)
		addrValue += 0x0250D6FC;
	else if (playerNumber == 2)
		addrValue += 0x0250DF9C;

	ReadProcessMemory(procHandle, (LPVOID)(addrValue + 0x0), &buffer, sizeof(buffer), NULL); // offset

	return value = buffer;
}

// used to check food supply & if in game
string clsMemory::FoodSupply(HANDLE procHandle)
{
	string value = "";
	char buffer[255] = {NULL};

	DWORD addrValue = 0x800000 + 0xFD1988; // base address
	ReadProcessMemory(procHandle, (LPCVOID)addrValue, &addrValue, sizeof(DWORD), NULL); // base address points to
	ReadProcessMemory(procHandle, (LPCVOID)(addrValue + 0x364), &addrValue, sizeof(DWORD), NULL); // offset
	ReadProcessMemory(procHandle, (LPCVOID)(addrValue + 0x7C), &addrValue, sizeof(DWORD), NULL); // offset
	ReadProcessMemory(procHandle, (LPCVOID)(addrValue + 0x1DC), &addrValue, sizeof(DWORD), NULL); // offset
	ReadProcessMemory(procHandle, (LPCVOID)(addrValue + 0x14), &buffer, sizeof(buffer), NULL); // offset

	return value = buffer;
}