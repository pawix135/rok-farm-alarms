#define RAYGUI_IMPLEMENTATION
#include "raylib_wrapper.h"
#include "app_manager.h"
#if defined(_DEBUG)

int main()
{
    APP::RunApp();
}

#else

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow)
{
    APP::RunApp();
}

#endif