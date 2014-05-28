#include <windows.h>
#include "resources.h"

#define APP_NAME "Launcher"

#define WINDOW_WIDTH 100
#define PADDING 8
#define BUTTON_COUNT 10
#define BUTTON_HEIGHT 32
#define BUTTON_NAME_SIZE 256

#define ID_TASKTRAY 12345
#define WM_TASKTRAY (WM_USER + 100)

typedef struct _LauncherButtonInfo {
	char name[BUTTON_NAME_SIZE];
} LauncherButtonInfo;

static LauncherButtonInfo buttoninfo[BUTTON_COUNT] = {
	{"button-1"},
	{"button-2"},
	{"button-3"},
	{"button-4"},
	{"button-5"},
	{"button-6"},
	{"button-7"},
	{"button-8"},
	{"button-9"},
	{"button-10"}};

LRESULT CALLBACK WinProc(
	HWND hWnd,
	UINT uMsg,
	WPARAM wParam,
	LPARAM lParam)
{
	static HWND button[BUTTON_COUNT];
	switch (uMsg) {
	case WM_CREATE:
		{
			int i;
			RECT rect;
			GetClientRect(hWnd, &rect);
			for (i = 0; i < BUTTON_COUNT; i++) {
				button[i] = CreateWindow("BUTTON", buttoninfo[i].name,
					WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
					PADDING,
					PADDING + (PADDING + BUTTON_HEIGHT) * i,
					rect.right - PADDING - PADDING,
					BUTTON_HEIGHT,
					hWnd, NULL, ((LPCREATESTRUCT) lParam)->hInstance, NULL);
			}
		}
		{
			RECT rect;
			GetWindowRect(hWnd, &rect);
			HMONITOR hMonitor = MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST);
			MONITORINFO monitorInfo;
			monitorInfo.cbSize = sizeof(MONITORINFO);
			GetMonitorInfo(hMonitor, &monitorInfo);
			SetWindowPos(hWnd, HWND_TOPMOST,
					monitorInfo.rcWork.right - (rect.right - rect.left),
					monitorInfo.rcWork.top,
					0, 0, SWP_NOSIZE | SWP_NOZORDER);
		}
		{
			NOTIFYICONDATA icon;
			icon.cbSize = sizeof(NOTIFYICONDATA);
			icon.hWnd = hWnd;
			icon.uID = ID_TASKTRAY;
			icon.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;
			icon.uCallbackMessage = WM_TASKTRAY;
			icon.hIcon = LoadIcon(((LPCREATESTRUCT) lParam)->hInstance, MAKEINTRESOURCE(APP_ICON));
			lstrcpyn(icon.szTip, APP_NAME, sizeof(icon.szTip));
			Shell_NotifyIcon(NIM_ADD, &icon);
		}
		break;
	case WM_COMMAND:
		if ((HWND) lParam == button[0]) {
			char *command = "explorer.exe /root,";
			int len = strlen(command);
			len += strlen(getenv("HOMEDRIVE"));
			len += strlen(getenv("HOMEPATH"));
			char *buf = malloc(len + 1);
			strcpy(buf, command);
			strcat(buf, getenv("HOMEDRIVE"));
			strcat(buf, getenv("HOMEPATH"));
			STARTUPINFO si;
			PROCESS_INFORMATION ps;
			GetStartupInfo(&si);
			CreateProcess(NULL, buf, NULL, NULL, TRUE, NORMAL_PRIORITY_CLASS, NULL, NULL, &si, &ps);
			free(buf);
		}
		break;
	case WM_TASKTRAY:
		if (wParam == ID_TASKTRAY) {
			switch (lParam) {
			case WM_LBUTTONDOWN:
			case WM_RBUTTONDOWN:
				if (IsZoomed(hWnd) || IsIconic(hWnd)) {
					ShowWindow(hWnd, SW_RESTORE);
					//SetActiveWindow(hWnd);
					//SetWindowPos(hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
					//SetFocus(hWnd);
					SetForegroundWindow(hWnd);
				}
				break;
			}
		}
		break;
	case WM_SIZE:
		switch (wParam) {
		case SIZE_RESTORED:
			{
				int i;
				int width = LOWORD(lParam);
				//int height = HIWORD(lParam);
				for (i = 0; i < BUTTON_COUNT; i++) {
					MoveWindow(button[i],
						PADDING,
						PADDING + (PADDING + BUTTON_HEIGHT) * i,
						width - PADDING - PADDING,
						BUTTON_HEIGHT,
						TRUE);
				}
			}
			break;
		case SIZE_MINIMIZED:
			ShowWindow(hWnd, SW_HIDE);
			break;
		}
		break;
	case WM_CLOSE:
		if (MessageBox(hWnd, "Quit?", APP_NAME, MB_OKCANCEL) == IDOK) {
			DestroyWindow(hWnd);
		}
		break;
	case WM_DESTROY:
		{
			NOTIFYICONDATA icon;
			icon.cbSize = sizeof(NOTIFYICONDATA);
			icon.hWnd = hWnd;
			icon.uID = ID_TASKTRAY;
			icon.uFlags = 0;
			icon.uCallbackMessage = 0;
			icon.hIcon = NULL;
			icon.szTip[0] = '\0';
			Shell_NotifyIcon(NIM_DELETE, &icon);
		}
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

int WINAPI WinMain(
	HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR lpCmdLine,
	int nCmdShow)
{
	WNDCLASS wc;
	HWND hWnd;
	MSG msg;
	BOOL ret;

	wc.style = 0;
	wc.lpfnWndProc = WinProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = (HICON) LoadImage(NULL, IDI_APPLICATION, IMAGE_ICON, 0, 0, LR_SHARED);
	wc.hCursor = (HCURSOR) LoadImage(NULL, IDC_ARROW, IMAGE_CURSOR, 0, 0, LR_SHARED);
	wc.hbrBackground = (HBRUSH) GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = APP_NAME;

	if (RegisterClass(&wc) == 0) return 1;

	hWnd = CreateWindow(
		wc.lpszClassName,
		APP_NAME,
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		256, //CW_USEDEFAULT,
		256, //CW_USEDEFAULT,
		NULL,
		NULL,
		hInstance,
		NULL);
	if (hWnd == NULL) return 1;

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	do {
		ret = GetMessage(&msg, NULL, 0, 0);
		if (ret == -1) break;
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	} while (ret != 0);

	return (int) msg.wParam;
}
