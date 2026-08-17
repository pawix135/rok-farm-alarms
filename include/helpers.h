#pragma once
#include "raylib_wrapper.h"

struct Vec2I {
	int x, y;
};
bool GetEmbeddedBuffer(int resourceId, const unsigned char** outData, unsigned int* outSize);
rl::Texture2D LoadTextureFromMemory(const char* fileType, const unsigned char* fileData, int dataSize);
