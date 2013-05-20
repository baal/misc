#include <tchar.h>
#include <stdio.h>
#include <stdlib.h>
#include <locale.h>

#include <windows.h>
#include <setupapi.h>
#include <cfgmgr32.h>

#pragma comment(lib, "setupapi.lib")

void printIndent(const unsigned int depth)
{
	unsigned int i;
	for (i = 0; i < depth; i++) _tprintf(_T("\t"));
}

void printDeviceRegistryProperty(HDEVINFO devInfo, PSP_DEVINFO_DATA devInfoData, DWORD propertyKey, TCHAR* propertyName, const unsigned int depth)
{
	DWORD size = 0;
	DWORD type = 0;
	DWORD error = 0;
	PBYTE buf = NULL;

	while (! SetupDiGetDeviceRegistryProperty(devInfo, devInfoData, propertyKey, &type, buf, size, &size)) {
		if (buf != NULL) {
			free(buf);
			buf = NULL;
		}
		error = GetLastError();
		if (ERROR_INSUFFICIENT_BUFFER == error) {
			buf = (PBYTE)malloc(sizeof(BYTE) * size);
		} else if (ERROR_INVALID_DATA == error) {
			printIndent(depth);
			_tprintf(_T("%s: (N/A)\n"), propertyName);
			break;
		} else {
			printIndent(depth);
			_tprintf(_T("SetupDiGetDeviceRegistryProperty failed: 0x%08lx\n"), error);
			return;
		}
	}

	if (buf != NULL) {
		size_t len;
		TCHAR* str;
		switch (type) {
		case REG_SZ:
			str = (TCHAR*)buf;
			printIndent(depth);
			_tprintf(_T("%s: \"%s\"\n"), propertyName, str);
			break;
		case REG_MULTI_SZ:
			str = (TCHAR*)buf;
			len = _tcslen(str);
			while (len > 0) {
				printIndent(depth);
				_tprintf(_T("%s: \"%s\"\n"), propertyName, str);
				str += len + 1;
				len = _tcslen(str);
			}
			break;
		}
		free(buf);
		buf = NULL;
	}
}

void printDevice(DEVINST devInst, const unsigned int depth)
{
	unsigned long status;
	unsigned long problemNumber;
	TCHAR devID[MAX_DEVICE_ID_LEN];
	CONFIGRET ret;
	HDEVINFO devInfo;
	SP_DEVINFO_DATA devInfoData;

	ret = CM_Get_Device_ID(devInst, devID, MAX_DEVICE_ID_LEN, 0);
	if (ret != CR_SUCCESS) {
		printIndent(depth);
		_tprintf(_T("CM_Get_Device_ID failed: 0x%08lx\n"), ret);
		return;
	}

	ret = CM_Get_DevNode_Status(&status, &problemNumber, devInst, 0);
	if (ret != CR_SUCCESS && ret != CR_NO_SUCH_DEVNODE) {
		printIndent(depth);
		_tprintf(_T("CM_Get_DevNode_Status failed: 0x%08lx\n"), ret);
		return;
	}

	devInfo = SetupDiCreateDeviceInfoList(NULL, NULL);
	if (devInfo == INVALID_HANDLE_VALUE) {
		printIndent(depth);
		_tprintf(_T("SetupDiCreateDeviceInfoList failed: 0x%08lx\n"), GetLastError());
		return;
	}

	devInfoData.cbSize = sizeof(devInfoData);
	if (! SetupDiOpenDeviceInfo(devInfo, devID, NULL, 0, &devInfoData)) {
		printIndent(depth);
		_tprintf(_T("SetupDiOpenDeviceInfo failed: 0x%08lx\n"), GetLastError());
		return;
	}

	printIndent(depth);
	_tprintf(_T("DEVICE ID: \"%s\" (0x%08lx,0x%08lx)\n"), devID, status, problemNumber);

	printDeviceRegistryProperty(devInfo, &devInfoData, SPDRP_CLASS, _T("SPDRP_CLASS"), depth);
	printDeviceRegistryProperty(devInfo, &devInfoData, SPDRP_CLASSGUID, _T("SPDRP_CLASSGUID"), depth);
	printDeviceRegistryProperty(devInfo, &devInfoData, SPDRP_FRIENDLYNAME, _T("SPDRP_FRIENDLYNAME"), depth);
	printDeviceRegistryProperty(devInfo, &devInfoData, SPDRP_DEVICEDESC, _T("SPDRP_DEVICEDESC"), depth);
	printDeviceRegistryProperty(devInfo, &devInfoData, SPDRP_MFG, _T("SPDRP_MFG"), depth);
	printDeviceRegistryProperty(devInfo, &devInfoData, SPDRP_UPPERFILTERS, _T("SPDRP_UPPERFILTERS"), depth);
	printDeviceRegistryProperty(devInfo, &devInfoData, SPDRP_LOWERFILTERS, _T("SPDRP_LOWERFILTERS"), depth);

	if (! SetupDiDestroyDeviceInfoList(devInfo)) {
		printIndent(depth);
		_tprintf(_T("SetupDiDestroyDeviceInfoList failed: 0x%08lx\n"), GetLastError());
		return;
	}
}

void recChild(DEVINST devInst, const unsigned int depth)
{
	DEVINST child;
	CONFIGRET ret;

	ret = CM_Get_Child(&child, devInst, 0);
	if (ret != CR_SUCCESS) {
		if (ret != CR_NO_SUCH_DEVNODE) {
			printIndent(depth);
			_tprintf(_T("CM_Get_Child failed: 0x%08lx\n"), ret);
		}
		return;
	}

	printDevice(child, depth);
	recChild(child, depth + 1);

	while (CM_Get_Sibling(&child, child, 0) == CR_SUCCESS) {
		printDevice(child, depth);
		recChild(child, depth + 1);
	}
}

int main(int argc, char** argv)
{
	DEVINST devInst;
	CONFIGRET ret;

	setlocale(LC_ALL, "");

	ret = CM_Locate_DevNode(&devInst, NULL, CM_LOCATE_DEVNODE_NORMAL);
	if (ret != CR_SUCCESS) {
		_tprintf(_T("CM_Locate_DevNode failed: 0x%08lx\n"), ret);
		return EXIT_FAILURE;
	}

//	ret = CM_Reenumerate_DevNode(devInst, 0);
//	if (ret != CR_SUCCESS) {
//		_tprintf(_T("CM_Reenumerate_DevNode failed: 0x%08lx\n"), ret);
//		return EXIT_FAILURE;
//	}

	printDevice(devInst, 0);
	recChild(devInst, 1);

	return EXIT_SUCCESS;
}
