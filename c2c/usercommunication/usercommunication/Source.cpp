#include <windows.h>
#include <stdio.h>

const char UserlandPath[] = "\\\.\\msnetdiag";

int setDeviceHandle(PHANDLE hDeviceFile)
{
	printf("[setDeviceHandle]: Opening handle %s\n", UserlandPath);
	*hDeviceFile = CreateFile(
		UserlandPath,
		GENERIC_READ | GENERIC_WRITE,
		0,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL
	);
	if (*hDeviceFile == INVALID_HANDLE_VALUE)
	{
		printf("[setDeviceHandle]: path to %s is not valid\n", UserlandPath);
		return(-1);
	}
	printf("[setDeviceHandle]: device handle aquired\n");
	return 0;
}

int TestOperation(HANDLE hDeviceFile)
{
	BOOL opStatus = TRUE;
	char *inBuffer;
	char *outBuffer;

	DWORD nBufferSize = 32;
	DWORD bytesRead = 0;

	inBuffer = (char *)malloc(nBufferSize);
	outBuffer = (char *)malloc(nBufferSize);

	if ((inBuffer == NULL) || (outBuffer == NULL))
	{
		printf("[TestOperation]: Couldn't allocate memory\n");
		return -1;
	}
	sprintf(inBuffer, "This is the INPUT Buffer");
	sprintf(outBuffer, "This is the OUTPUT Buffer");

	opStatus = DeviceIoControl(
		hDeviceFile,
		(DWORD)IO_TEST_CMD,
		(LPVOID)inBuffer,
		nBufferSize,
		(LPVOID)outBuffer,
		nBufferSize,
		&bytesRead,
		NULL
	);

	if (opStatus == FALSE)
	{
		printf("[TestOperation]: DeviceIoControl() Failed\n");
	}
	printf("[TestOperation]: bytes read=%d\n", bytesRead);
	printf("[TestOperation]: outBuffer=%s\n", outBuffer);
	free(inBuffer);
	free(outBuffer);

	return 0;
}


int WinMain()
{
	int retCode = 0;
	HANDLE hDeviceFile = INVALID_HANDLE_VALUE;
	retCode = setDeviceHandle(&hDeviceFile);
	if (retCode != 0) 
	{
		return(retCode);
	}
	
	retCode = TestOperation(hDeviceFile);
	if (retCode != 0) 
	{ 
		return(retCode);
	}
	CloseHandle(hDeviceFile);

	return 0;
}
