#include "utilities/memory.h"
#include <iostream>
#include <string>
#include <vector>
#include <thread>

const std::string red = "\033[31m";
const std::string green = "\033[32m";
const std::string yellow = "\033[33m";
const std::string blue = "\033[34m";
const std::string magenta = "\033[35m";
const std::string cyan = "\033[36m";
const std::string white = "\033[37m";
const std::string reset = "\033[0m";

// 1.2.0.2 ac offsets
constexpr auto localPlayer = 0x10F4F4;
constexpr auto health = 0xF8;
constexpr auto armor = 0xFC;
constexpr auto mtpAmmo = 0x150;
constexpr auto nameOffsetWithinPadding = 0x0225;

std::vector<std::string> logo{
	"            _ _ _            _     ",
	" __   _____(_) | |_ ___  ___| |__  ",
	" \\ \\ / / _ \\ | | __/ _ \\/ __| '_ \\ ",
	"  \\ V /  __/ | | ||  __/ (__| | | |",
	"   \\_/ \\___|_|_|\\__\\___|\\___|_| |_|",
	"                                   "
};

class NamePadding
{
public:
	char preNamePadding_0000[548]; //0x0000
	char maxAcceptableCharacters_0224[16]; //0x0224
	char postNamePadding_0234[3608]; //0x0234
}; //Size: 0x104C

int main()
{
	Memory memory{"ac_client.exe"};
	const auto ModuleBaseAddress = memory.GetModuleAddress("ac_client.exe");
	const auto localPlayerAddress = memory.Read<std::uintptr_t>(ModuleBaseAddress + localPlayer);

	if (!ModuleBaseAddress)
	{
		std::cerr << red <<"[!] No ModuleBaseAddress found: " + std::to_string(GetLastError());
		std::cin.get();
		return -1;
	}

	const auto healthAddress = localPlayerAddress + health;
	const auto armorAddress = localPlayerAddress + armor;
	const auto mtpAmmoAddress = localPlayerAddress + mtpAmmo;
	const auto nameAddress = localPlayerAddress + nameOffsetWithinPadding;

	if (!healthAddress || !armorAddress || !mtpAmmoAddress || !nameAddress)
	{
		std::cerr << "[!] Could not get address: " + std::to_string(GetLastError()) << '\n';
		std::cin.get();
		return -1;
	}


	std::string name = memory.Read<NamePadding>(nameAddress).preNamePadding_0000;

	std::cout << '\n';
	for (const auto& line : logo)
		std::cout << cyan << line << '\n';

	std::cout << '\n' << yellow << "=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=" << reset << '\n';

	std::cout << '\n';
	std::cout << "entry confirmed, " << magenta << name << "\n" << reset << green << '\n';
	std::cout << green << "f1 : max health " << '\n';
	std::cout << green << "f2 : max armor " << '\n';
	std::cout << green << "f1 : max ammo  " << '\n';

	while (true)
	{
		if (GetAsyncKeyState(VK_F1) & 0x8000)
		{
			memory.Write<int>(healthAddress, 1000);
			std::cout << red << "health changed" << '\n';
		}
		else if (GetAsyncKeyState(VK_F2) & 0x8000)
		{
			memory.Write<int>(armorAddress, 1000);
			std::cout << red << "armor changed" << '\n';
		}
		else if (GetAsyncKeyState(VK_F3) & 0x8000)
		{
			memory.Write<int>(mtpAmmoAddress, 1000);
			std::cout << red << "ammo changed" << '\n';
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}
}
