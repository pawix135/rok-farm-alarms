#pragma once
#define _CRT_SECURE_NO_WARNINGS

#include <string>
#include <cstdint>
#include <cmath>
#include <cstdarg>
#include <cstdbool>
#include <cstddef>
#include <cstdlib>
#include <vector>

// 1. Wrap Raylib cleanly
namespace rl {
#include <raylib.h>
#include <raymath.h> 
#include <raygui.h>
}

// 2. Rename the colliding Windows API functions BEFORE including windows.h.
#define CloseWindow Win32CloseWindow
#define ShowCursor Win32ShowCursor
#define Rectangle Win32Rectangle

// 3. Include Windows API
#define WIN32_LEAN_AND_MEAN 
#include <windows.h>
#include <shellapi.h>

// 4. Clean up our temporary Windows renaming macros
#undef CloseWindow
#undef ShowCursor
#undef Rectangle

// 5. Clean up the standard Windows macros that overwrite Raylib functions
#undef DrawText
#undef DrawTextEx
#undef PlaySound
#undef LoadImage
#undef Yield