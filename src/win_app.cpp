#include "win_app.h"
#include "raylib_wrapper.h"
#include "config.h"

#define WM_TRAYICON (WM_USER + 1)
#define ID_TRAY_EXIT 1001

namespace WinApp {

	namespace {
		HWND g_Hwnd = nullptr;
		WNDPROC g_OriginalWndProc = nullptr;
		NOTIFYICONDATAA g_TrayIconData = {};

		bool g_IsRunning = true;

		LRESULT CALLBACK SubclassProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
			if (msg == WM_TRAYICON) {

				if (LOWORD(lParam) == WM_LBUTTONUP) {
					if (IsWindowVisible(hwnd)) {
						ShowWindow(hwnd, SW_HIDE);
					}
					else {

						ShowWindow(hwnd, SW_RESTORE);
						SetForegroundWindow(hwnd);
					}
				}

				else if (LOWORD(lParam) == WM_RBUTTONUP) {

					HMENU hMenu = CreatePopupMenu();

					AppendMenuA(hMenu, MF_STRING, ID_TRAY_EXIT, "Exit");

					POINT pt;
					GetCursorPos(&pt);

					SetForegroundWindow(hwnd);

					int cmd = TrackPopupMenu(hMenu, TPM_RETURNCMD | TPM_RIGHTBUTTON, pt.x, pt.y, 0, hwnd, NULL);

					DestroyMenu(hMenu);

					if (cmd == ID_TRAY_EXIT) {
						g_IsRunning = false;
					}
				}
			}

			return CallWindowProc(g_OriginalWndProc, hwnd, msg, wParam, lParam);
		}
	}

	void Init(HWND hwnd) {

		g_Hwnd = hwnd;

		g_OriginalWndProc = (WNDPROC)SetWindowLongPtr(g_Hwnd, GWLP_WNDPROC, (LONG_PTR)SubclassProc);

		g_TrayIconData.cbSize = sizeof(NOTIFYICONDATAA);
		g_TrayIconData.hWnd = hwnd;
		g_TrayIconData.uID = 100;
		g_TrayIconData.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;
		g_TrayIconData.uCallbackMessage = WM_TRAYICON;
		g_TrayIconData.hIcon = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(101));
		strncpy_s(g_TrayIconData.szTip, APP_NAME, sizeof(g_TrayIconData.szTip));
		Shell_NotifyIconA(NIM_ADD, &g_TrayIconData);
	}

	void Cleanup() {
		Shell_NotifyIconA(NIM_DELETE, &g_TrayIconData);
	}

	void HideToTray() {
		if (g_Hwnd) ShowWindow(g_Hwnd, SW_HIDE);
	}

	void RestoreFromTray() {
		if (g_Hwnd) {
			ShowWindow(g_Hwnd, SW_RESTORE);
			SetForegroundWindow(g_Hwnd);
		}
	}

	bool ShouldExit() {
		return g_IsRunning;
	}

	void ToggleVisibility() {
		if (g_Hwnd) {
			if (IsWindowVisible(g_Hwnd)) {
				HideToTray();
			}
			else {
				RestoreFromTray();
			}
		}
	}

	void SendNotification(const char* title, const char* message) {
		g_TrayIconData.uFlags = NIF_INFO;
		strncpy_s(g_TrayIconData.szInfoTitle, title, sizeof(g_TrayIconData.szInfoTitle));
		strncpy_s(g_TrayIconData.szInfo, message, sizeof(g_TrayIconData.szInfo));
		g_TrayIconData.dwInfoFlags = NIIF_INFO;

		Shell_NotifyIconA(NIM_MODIFY, &g_TrayIconData);
	}

	void Exit() {
		g_IsRunning = false;
	}
}