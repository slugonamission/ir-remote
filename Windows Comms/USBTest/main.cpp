#include <stdio.h>
#include <Windows.h>
#include <hidusage.h>
#include <hidpi.h>

// Seriously Microsoft?...
extern "C" {
	#include <hidsdi.h>
}

#include <SetupAPI.h>

int main(int argc, char** argv)
{
	printf("Hello World!\n");

	printf("Enumerating HID devices\n");
	printf("===================================\n\n");

	GUID hidGuid;

	HidD_GetHidGuid(&hidGuid);

	printf("Got HID GUID: %x%x%x%x\n", hidGuid.Data1, hidGuid.Data2, hidGuid.Data3, hidGuid.Data4);

	HDEVINFO devInfo;
	SP_DEVINFO_DATA devData;
	devInfo = SetupDiGetClassDevs(&hidGuid, NULL, NULL, DIGCF_DEVICEINTERFACE | DIGCF_PRESENT);

	if(devInfo == INVALID_HANDLE_VALUE)
	{
		printf("SetupDiGetClassDevs FAILED\n");
		return 1;
	}

	devData.cbSize = sizeof(SP_DEVINFO_DATA);

	TCHAR devPath[200];
	for(int i = 0; SetupDiEnumDeviceInfo(devInfo, i, &devData); i++)
	{
		DWORD dataT;
		LPTSTR buffer = NULL;
		DWORD bSize = 0;

		while(!SetupDiGetDeviceRegistryProperty(devInfo, &devData, SPDRP_HARDWAREID, &dataT, (PBYTE)buffer, bSize, &bSize))
		{
			if(GetLastError() == ERROR_INSUFFICIENT_BUFFER)
			{
				if(buffer)
					LocalFree(buffer);

				buffer = (LPTSTR)LocalAlloc(LPTR, bSize*2);
			}
			else
			{
				printf("SetupDiGetDeviceRegistryProperty FAILED\n");
				return 1;
			}
		}

		printf("Device %d: %ls\n", i, buffer);

		// Ok, so let's just assume device 0 for the moment, which is correct
		// Get the device interfaces
		SP_DEVICE_INTERFACE_DATA ifData;
		ifData.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);

		for(int j = 0;SetupDiEnumDeviceInterfaces(devInfo, &devData, &hidGuid, j, &ifData);j++)
		{
			SP_DEVICE_INTERFACE_DETAIL_DATA* ifDetails;
			DWORD ifDetailsSize;

			SetupDiGetDeviceInterfaceDetail(devInfo, &ifData, NULL, 0, &ifDetailsSize, NULL);
			ifDetails = (SP_DEVICE_INTERFACE_DETAIL_DATA*)LocalAlloc(LPTR, ifDetailsSize);
			ifDetails->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);
			SetupDiGetDeviceInterfaceDetail(devInfo, &ifData, ifDetails, ifDetailsSize, 0, 0);

			printf("Interface: %ls\n", ifDetails->DevicePath);

			if(i == 3)
				ua_tcscpy(devPath, ifDetails->DevicePath);

			LocalFree(ifDetails);
		}

		if(GetLastError() != ERROR_NO_MORE_ITEMS)
		{
			printf("SetupDiEnumDeviceInterfaces FAILED\n");
			return 1;
		}

		if(buffer)
			LocalFree(buffer);
		printf("\n");
	}

	printf("Opening %ls\n", devPath);

	SetupDiDestroyDeviceInfoList(devInfo);

	HANDLE f;
	HANDLE wf;
	f  = CreateFile(devPath, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, NULL, NULL);
	wf = CreateFile(devPath, GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, NULL, NULL);

	if(f == NULL)
	{
		printf("CreateFile FAILED\n");
		return 1;
	}

	char buf[200];
	DWORD bytesRead;
	DWORD bytesWritten;

	buf[0] = 4;


	while(1)
	{
		BOOL ok = WriteFile(wf, buf, 9, &bytesWritten, NULL);

		if(ok)
			printf("Wrote %d bytes\n", bytesWritten);
		else
		{
			printf("WriteFile FAILED\nErr: %d\n", GetLastError());
			return 1;
		}

		ReadFile(f, buf, 200, &bytesRead, NULL);
		printf("Read %d bytes: %x %x\n", bytesRead, buf[0], buf[1]);
	}
	

	return 0;
}