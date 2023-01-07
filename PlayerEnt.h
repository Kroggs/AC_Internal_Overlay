#pragma once
#include <Windows.h>
#include <stdint.h>
#include <map>

//ReClass.NET generated class

struct Vec3f { float x, y, z; };
typedef struct _PLAYER_ENT
{
public:
	void* VTable; //0x0000
	Vec3f HeadPosition; //0x0004
	Vec3f Velocity; //0x0010
	char pad_001C[24]; //0x001C
	Vec3f Position; //0x0034
	Vec3f ViewAngle; //0x0040
	char pad_004C[36]; //0x004C
	bool IsStatic; //0x0070
	char pad_0071[135]; //0x0071
	uint32_t Health; //0x00F8
	uint32_t Armor; //0x00FC
	char pad_0100[20]; //0x0100
	uint32_t PistolReserveAmmo; //0x0114
	uint32_t CarbineReserveAmmo; //0x0118
	uint32_t ShotgunReserveAmmo; //0x011C
	uint32_t SubgunReserveAmmo; //0x0120
	uint32_t SniperReserveAmmo; //0x0124
	uint32_t AssaultRifleReserveAmmo; //0x0128
	char pad_012C[8]; //0x012C
	uint32_t AkimboReserveAmmo; //0x0134
	char pad_0138[4]; //0x0138
	uint32_t PistolMagAmmo; //0x013C
	uint32_t CarbineMagAmmo; //0x0140
	uint32_t ShotgunMagAmmo; //0x0144
	uint32_t SubgunMagAmmo; //0x0148
	uint32_t SniperMagAmmo; //0x014C
	uint32_t AssaultRifeMagAmmo; //0x0150
	char pad_0154[8]; //0x0154
	uint32_t AkimboMagAmmo; //0x015C
	uint32_t KnifeAnimPauseTime; //0x0160
	uint32_t PistolAnimPauseTime; //0x0164
	uint32_t CarbineAnimPauseTime; //0x0168
	uint32_t ShotgunAnimPauseTime; //0x016C
	uint32_t SubgunAnimPauseTime; //0x0170
	uint32_t SniperAnimPauseTime; //0x0174
	uint32_t AssaultRifleAnimPauseTime; //0x0178
	char pad_017C[8]; //0x017C
	uint32_t AkimboAnimPauseTime; //0x0184
	char pad_0188[4]; //0x0188
	uint32_t PistolShotFired; //0x018C
	uint32_t CarbineShotFired; //0x0190
	uint32_t ShotgunShotFired; //0x0194
	uint32_t SubgunShotFired; //0x0198
	uint32_t SniperShotFired; //0x019C
	uint32_t AssaultRifleShotFired; //0x01A0
	char pad_01A4[8]; //0x01A4
	uint32_t AkimboShotFired; //0x01AC
	char pad_01B0[116]; // 0x1B0
	BYTE bAttacking; //0x0224
	char name[16]; //0x0225
	char pad_0235[247]; // 0x235
	BYTE Team; //0x032C
	char pad_032D[11]; // 0x032D
	BYTE State;//0x0338 
	char pad_0339[15]; // 0x0339
	uint32_t KnifePtr; //0x0348
	uint32_t PistolPtr; //0x034C
	uint32_t CarbinePtr; //0x0350
	uint32_t ShotgunPtr; //0x0354
	uint32_t SubgunPtr; //0x0358
	uint32_t SniperPtr; //0x035C
	uint32_t AssaultPtr; //0x0360
	char pad_0364[4]; //0x0364
	uint32_t GrenadePtr; //0x0368
	char pad_036C[8]; // 0x036C
	uint32_t EquippedWeapon; //0x0374
}PLAYER_ENT, *P_PLAYER_ENT;