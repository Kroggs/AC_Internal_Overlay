#pragma once

#include <Windows.h>

const int holdSize = 15;
const int weapSize = 7;

static const char* holdKeys[holdSize]{
   "Left mouse button",
   "Right mouse button",
   "Middle mouse button",
   "TAB key",
   "SHIFT key",
   "CTRL key",
   "ALT key",
   "DEL key",
   "INS key",
   "Numeric keypad 0 key",
   "NUM LOCK key",
   "Left SHIFT key",
   "Right SHIFT key",
   "Left CONTROL key",
   "Right CONTROL key",
};

static const char* weaponList[weapSize]{
   "Knife",
   "Pistol",
   "Carbine",
   "Shotgun",
   "Sub Machine gun",
   "Sniper rifle",
   "Assault rifle",
};

static int weaponValues[weapSize]{
	0,
	0,
	0,
	0,
	0,
	0,
	0
};

static const int holdKeysCodes[holdSize]{
   VK_LBUTTON,
   VK_RBUTTON,
   VK_MBUTTON,
   VK_TAB,
   VK_SHIFT,
   VK_CONTROL,
   VK_MENU,
   VK_DELETE,
   VK_INSERT,
   VK_NUMPAD0,
   VK_NUMLOCK,
   VK_LSHIFT,
   VK_RSHIFT,
   VK_LCONTROL,
   VK_RCONTROL,
};

static int holdKeyIndex = 0;
static int weapKeyIndex = 0;

static bool bAimbotEnabled = false;

static bool bNoRecoilEnabled = false;
static bool bRapidFireEnabled = false;
static bool bInfiniteHealthEnabled = false;
static bool bInfiniteArmorEnabled = false;
static int holdKey = 0;
