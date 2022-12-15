#include "pch.h"
#include "hook.h"

Hook::Hook(BYTE* src, BYTE* dst, BYTE* PtrToGatewayPtr, uintptr_t len)
{
	this->src = src;
	this->dst = dst;
	this->len = len;
	this->PtrToGatewayPtr = PtrToGatewayPtr;
}

Hook::Hook(const char* exportName, const char* modName, BYTE* dst, BYTE* PtrToGatewayPtr, uintptr_t len)
{
	HMODULE hMod = GetModuleHandleA(modName);

	this->src = (BYTE*)GetProcAddress(hMod, exportName);
	this->dst = dst;
	this->len = len;
	this->PtrToGatewayPtr = PtrToGatewayPtr;
}

void Hook::Enable()
{
	memcpy(originalBytes, src, len);
	*(uintptr_t*)PtrToGatewayPtr = (uintptr_t)mem::THook32(src, dst, len);
	bStatus = true;
}

void Hook::Disable()
{
	mem::Patch((uintptr_t*)src, (uintptr_t*)originalBytes, len);
	bStatus = false;
}

void Hook::Toggle()
{
	bStatus = !bStatus;
	if (!bStatus) Enable();
	else Disable();
}