#include <stdio.h>
#include <tchar.h>
#include <locale.h>

#include <windows.h>
#include <setupapi.h>
#include <cfgmgr32.h>

#pragma comment(lib, "setupapi.lib")

int _tmain(int argc, TCHAR** argv) {

	TCHAR* devID = NULL;
	HDEVINFO devInfo;
	SP_DEVINFO_DATA devInfoData;
	CONFIGRET ret;
	PNP_VETO_TYPE vetoType;
	TCHAR vetoName[MAX_PATH];

	setlocale(LC_ALL, "");

	if (argc == 2) {
		devID = *(argv + 1);
	} else {
		_tprintf(_T("Usage: deveject DEVICE_ID\n"));
	}

	if (devID != NULL) {
		devInfo = SetupDiCreateDeviceInfoList(NULL, NULL);
		if (devInfo == INVALID_HANDLE_VALUE) {
			_tprintf(_T("SetupDiCreateDeviceInfoList failed: 0x%08lx\n"), GetLastError());
			return EXIT_FAILURE;
		}

		devInfoData.cbSize = sizeof(devInfoData);
		if (! SetupDiOpenDeviceInfo(devInfo, devID, NULL, 0, &devInfoData)) {
			_tprintf(_T("SetupDiOpenDeviceInfo failed: 0x%08lx\n"), GetLastError());
			return EXIT_FAILURE;
		}

		ret = CM_Request_Device_Eject(devInfoData.DevInst, &vetoType, vetoName, MAX_PATH, 0);
		if (ret == CR_SUCCESS) {
			_tprintf(_T("CM_Request_Device_Eject succeed.\n"));
		} else {
			_tprintf(_T("CM_Request_Device_Eject failed: 0x%08lx\n"), ret);
		}
	}

	return EXIT_SUCCESS;
}
