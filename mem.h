#pragma once
#include <Windows.h>

namespace mem {
    void Patch(uintptr_t* dst, uintptr_t* src, int size);
    bool Detour32(BYTE* src, BYTE* dst, const uintptr_t len);
    BYTE* THook32(BYTE* src, BYTE* dst, const uintptr_t len);
}