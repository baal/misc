#include <tchar.h>

#define INITGUID

#include <windows.h>
#include <dbt.h> // DBT_DEVTYP_DEVICEINTERFACE
//#include <devguid.h>
#include <initguid.h>

DEFINE_GUID(GUID_DEVCLASS_WCEUSBS,0x25dbce51, 0x6c8f, 0x4a72, 0x8a, 0x6d, 0xb5, 0x4c, 0x2b, 0x4f, 0xc8, 0x35 );

LRESULT CALLBACK WinProc(
	HWND hwnd,
	UINT uMsg,
	WPARAM wParam,
	LPARAM lParam)
{
	static HDEVNOTIFY devnotify = NULL;
	DEV_BROADCAST_DEVICEINTERFACE devif;

	switch (uMsg) {
	case WM_CREATE:
		devif.dbcc_size = sizeof(DEV_BROADCAST_DEVICEINTERFACE);
		devif.dbcc_devicetype = DBT_DEVTYP_DEVICEINTERFACE;
		devif.dbcc_classguid = GUID_DEVCLASS_WCEUSBS;
		devnotify = RegisterDeviceNotification(hwnd, &devif, DEVICE_NOTIFY_WINDOW_HANDLE);
		if (devnotify == NULL) {
			MessageBox(hwnd, _T("ERROR: RegisterDeviceNotification"), _T("ERROR"), MB_OK);
		}
		break;
	case WM_DEVICECHANGE:
		MessageBox(hwnd, _T("WM_DEVICECHANGE"), _T("WM_DEVICECHANGE"), MB_OK);
		break;
	case WM_CLOSE:
		if (MessageBox(hwnd, _T("Quit?"), _T("DevNotify"), MB_OKCANCEL) == IDOK) {
			DestroyWindow(hwnd);
		}
		break;
	case WM_DESTROY:
		if (devnotify != NULL) {
			if (! UnregisterDeviceNotification(devnotify)) {
				MessageBox(hwnd, _T("ERROR: UnregisterDeviceNotification"), _T("ERROR"), MB_OK);
			}
		}
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}

	return 0;
}

int WINAPI WinMain(
	HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR lpCmdLine,
	int nCmdShow)
{
	WNDCLASSEX wc;
	ATOM atom;
	HWND hwnd;
	MSG msg;
	BOOL ret;

	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = 0;
	wc.lpfnWndProc = WinProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = (HICON)LoadImage(NULL, IDI_APPLICATION, IMAGE_ICON, 0, 0, LR_SHARED);
	wc.hCursor = (HCURSOR)LoadImage(NULL, IDC_ARROW, IMAGE_CURSOR, 0, 0, LR_SHARED);
	wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = _T("DevNotify");
	wc.hIconSm = (HICON)LoadImage(NULL, IDI_APPLICATION, IMAGE_ICON, 0, 0, LR_SHARED);
	atom = RegisterClassEx(&wc);
	if (atom == 0) return 1;

	hwnd = CreateWindowEx(
		WS_EX_OVERLAPPEDWINDOW,
		wc.lpszClassName,
		_T("DevNotify"),
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		NULL,
		NULL,
		hInstance,
		NULL);
	if (hwnd == NULL) return 1;

	ShowWindow(hwnd, nCmdShow);
	UpdateWindow(hwnd);

	do {
		ret = GetMessage(&msg, NULL, 0, 0);
		if (ret == -1) break;
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	} while (ret != 0);

	return (int)msg.wParam;
}
