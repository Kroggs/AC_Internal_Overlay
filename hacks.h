#pragma once

#include <Windows.h>
#include <vector>
#include <iostream>

#include <cmath>
#include <tgmath.h>

#include "settings.h"
#include "PlayerEnt.h"

#define PLAYER_AMOUNT_ADDR 0x50F500
#define PLAYER_TABLE_PTR 0x50F4F8 

namespace Hacks
{
	static std::vector<P_PLAYER_ENT> vPlayerList;

	static P_PLAYER_ENT pClosestPlayer = nullptr;
	static P_PLAYER_ENT localPlayer = (P_PLAYER_ENT)(*(DWORD*)0x50F4F4);

	inline float getAngleX(const P_PLAYER_ENT p1, const P_PLAYER_ENT p2)
	{
		float deltaX = p2->Position.x - p1->Position.x;
		float deltaY = p1->Position.y - p2->Position.y;

		if (p2->Position.x > p1->Position.x && p2->Position.y < p1->Position.y) {
			return atanf(deltaX / deltaY) * 180.0f / (3.14159);
		}
		else if (p2->Position.x > p1->Position.x && p2->Position.y > p1->Position.y) {
			return atanf(deltaX / deltaY) * 180.0f / (3.14159) + 180.0f;
		}
		else if (p2->Position.x < p1->Position.x && p2->Position.y > p1->Position.y) {
			return atanf(deltaX / deltaY) * 180.0f / (3.14159) - 180.0f;
		}
		else {
			return atanf(deltaX / deltaY) * 180.0f / (3.14159) + 360.0f;
		}

		return 0;
	}

	inline float getAngleY(const P_PLAYER_ENT p1, const P_PLAYER_ENT p2) {
		float deltaZ = p2->Position.z - p1->Position.z;
		float dist = sqrt(
			pow((p2->Position.x - p1->Position.x), 2) +
			pow((p2->Position.y - p1->Position.y), 2) +
			pow((p2->Position.z - p1->Position.z), 2)
		);
		return asinf(deltaZ / dist) * 180.0f / (3.14159);
	}

	inline bool IsValid(P_PLAYER_ENT player)
	{
		return (player->Health > 0 && player->State == 0);
	}

	inline P_PLAYER_ENT GetClosestPlayer()
	{
		if (vPlayerList.size())
		{
			float currentDistance = 0,
				distance = 0;

			int index = 0;

			currentDistance = sqrt(
				pow((vPlayerList[index]->Position.x - localPlayer->Position.x), 2) +
				pow((vPlayerList[index]->Position.y - localPlayer->Position.y), 2) +
				pow((vPlayerList[index]->Position.z - localPlayer->Position.z), 2)
			);

			for (unsigned int i = 1; i < vPlayerList.size(); i++)
			{
				distance = sqrt(
					pow((vPlayerList[i]->Position.x - localPlayer->Position.x), 2) +
					pow((vPlayerList[i]->Position.y - localPlayer->Position.y), 2) +
					pow((vPlayerList[i]->Position.z - localPlayer->Position.z), 2)
				);

				if (distance < currentDistance)
				{
					if (IsValid(vPlayerList[i])) {
						currentDistance = distance;
						index = i;
					}
				}

			}

			if (IsValid(vPlayerList[index]))
				return vPlayerList[index];
			else return 0;
		}
		else
		{
			return 0;
		}
		
	}

	inline void UpdateWeaponValues(P_PLAYER_ENT pEnt) noexcept
	{
		weaponValues[0] = pEnt->KnifePtr;
		weaponValues[1] = pEnt->PistolPtr;
		weaponValues[2] = pEnt->CarbinePtr;
		weaponValues[3] = pEnt->ShotgunPtr;
		weaponValues[4] = pEnt->SubgunPtr;
		weaponValues[5] = pEnt->SniperPtr;
		weaponValues[6] = pEnt->AssaultPtr;
	}

	inline void UpdatePlayerList() noexcept
	{
		vPlayerList.clear();

		DWORD pTable = ((*(DWORD*)PLAYER_TABLE_PTR) + 0x04);
		int pAmount = (*(int*)PLAYER_AMOUNT_ADDR);

		if (pAmount > 1)
		{
			for (auto i = pTable; i < pTable + ((pAmount - 1) * 4); i += 0x04)
				vPlayerList.push_back((P_PLAYER_ENT)(*(DWORD*)i));
		}
	}

	

	inline void HackTick() noexcept
	{
		UpdatePlayerList();

		if (bAimbotEnabled) {
			if (GetAsyncKeyState(holdKey))
			{
				if (vPlayerList.size())
				{
					P_PLAYER_ENT closestPlayer = GetClosestPlayer();
					if (closestPlayer)
					{
						localPlayer->ViewAngle.x = getAngleX(localPlayer, closestPlayer);
						localPlayer->ViewAngle.y = getAngleY(localPlayer, closestPlayer);
					}
				}
			}
		}
	}
}