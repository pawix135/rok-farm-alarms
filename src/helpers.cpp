#include "helpers.h"

bool GetEmbeddedBuffer(int resourceId, const unsigned char** outData, unsigned int* outSize) {
    HMODULE hModule = GetModuleHandle(NULL);
    HRSRC hRes = FindResource(hModule, MAKEINTRESOURCE(resourceId), RT_RCDATA);
    if (!hRes) return false;

    HGLOBAL hMem = LoadResource(hModule, hRes);
    if (!hMem) return false;

    *outData = static_cast<const unsigned char*>(LockResource(hMem));
    *outSize = SizeofResource(hModule, hRes);

    return (*outData != nullptr && *outSize > 0);
}