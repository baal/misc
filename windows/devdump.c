#include <stdio.h>
#include <stdlib.h>
#include <locale.h>

#include <windows.h>
#include <setupapi.h>
#include <cfgmgr32.h>
#include <devguid.h>

#pragma comment(lib, "setupapi.lib")

void printIndent(const unsigned int depth)
{
	unsigned int i;
	for (i = 0; i < depth; i++) printf("\t");
}

void printDeviceRegistryProperty(HDEVINFO devInfo, PSP_DEVINFO_DATA devInfoData, DWORD propertyKey, wchar_t* propertyName, const unsigned int depth)
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
			wprintf(L"%s: (N/A)\n", propertyName);
			break;
		} else {
			printf("SetupDiGetDeviceRegistryProperty failed: 0x%08lx\n", error);
			break;
		}
	}

	if (buf != NULL) {
		size_t len;
		PWSTR str;
		switch (type) {
		case REG_SZ:
			str = (PWSTR)buf;
			printIndent(depth);
			wprintf(L"%s: \"%s\"\n", propertyName, str);
			break;
		case REG_MULTI_SZ:
			str = (PWSTR)buf;
			len = wcslen(str);
			while (len > 0) {
				printIndent(depth);
				wprintf(L"%s: \"%s\"\n", propertyName, str);
				str += len + 1;
				len = wcslen(str);
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
	wchar_t devID[MAX_DEVICE_ID_LEN + 1];
	CONFIGRET ret;
	HDEVINFO devInfo;
	SP_DEVINFO_DATA devInfoData;

	ret = CM_Get_Device_ID(devInst, devID, sizeof(devID), 0);
	if (ret != CR_SUCCESS) {
		printf("CM_Get_Device_ID failed: 0x%08lx\n", ret);
		return;
	}

	ret = CM_Get_DevNode_Status(&status, &problemNumber, devInst, 0);
	if (ret != CR_SUCCESS && ret != CR_NO_SUCH_DEVNODE) {
		printf("CM_Get_DevNode_Status failed: 0x%08lx\n", ret);
		return;
	}

	devInfo = SetupDiCreateDeviceInfoList(NULL, NULL);
	if (devInfo == INVALID_HANDLE_VALUE) {
		printf("SetupDiCreateDeviceInfoList failed: 0x%08lx\n", GetLastError());
		return;
	}

	devInfoData.cbSize = sizeof(devInfoData);
	if (! SetupDiOpenDeviceInfo(devInfo, devID, NULL, 0, &devInfoData)) {
		printf("SetupDiOpenDeviceInfo failed: 0x%08lx\n", GetLastError());
		return;
	}

	printIndent(depth);
	wprintf(L"DEVICE ID: \"%s\" (0x%08lx,0x%08lx)\n", devID, status, problemNumber);

	printDeviceRegistryProperty(devInfo, &devInfoData, SPDRP_CLASS, L"SPDRP_CLASS", depth);
	printDeviceRegistryProperty(devInfo, &devInfoData, SPDRP_CLASSGUID, L"SPDRP_CLASSGUID", depth);
	printDeviceRegistryProperty(devInfo, &devInfoData, SPDRP_FRIENDLYNAME, L"SPDRP_FRIENDLYNAME", depth);
	printDeviceRegistryProperty(devInfo, &devInfoData, SPDRP_DEVICEDESC, L"SPDRP_DEVICEDESC", depth);
	printDeviceRegistryProperty(devInfo, &devInfoData, SPDRP_MFG, L"SPDRP_MFG", depth);
	printDeviceRegistryProperty(devInfo, &devInfoData, SPDRP_UPPERFILTERS, L"SPDRP_UPPERFILTERS", depth);
	printDeviceRegistryProperty(devInfo, &devInfoData, SPDRP_LOWERFILTERS, L"SPDRP_LOWERFILTERS", depth);

	if (! SetupDiDestroyDeviceInfoList(devInfo)) {
		printf("SetupDiDestroyDeviceInfoList failed: 0x%08lx\n", GetLastError());
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
			printf("CM_Get_Child failed: 0x%08lx\n", ret);
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
		printf("CM_Locate_DevNode failed: 0x%08lx\n", ret);
		return EXIT_FAILURE;
	}

//	ret = CM_Reenumerate_DevNode(devInst, 0);
//	if (ret != CR_SUCCESS) {
//		printf("CM_Reenumerate_DevNode failed: 0x%08lx\n", ret);
//		return EXIT_FAILURE;
//	}

	printDevice(devInst, 0);
	recChild(devInst, 1);

	return EXIT_SUCCESS;
}
