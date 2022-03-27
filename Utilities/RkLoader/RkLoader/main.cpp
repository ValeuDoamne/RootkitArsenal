#include <iostream>
#include <windows.h>

SC_HANDLE installDriver(LPCTSTR driverName, LPCTSTR binaryPath)
{
	SC_HANDLE scmDBHandle = NULL;
	SC_HANDLE svcHandle   = NULL;

	scmDBHandle = OpenSCManager(
		NULL,
		NULL,
		SC_MANAGER_ALL_ACCESS
	);

	if (scmDBHandle == NULL)
	{
		printf("[installDriver]: could not open handle to SCM database\n");
		return(NULL);
	}

	svcHandle = CreateService(
		scmDBHandle,
		driverName,
		driverName,
		SERVICE_ALL_ACCESS,
		SERVICE_KERNEL_DRIVER,
		SERVICE_DEMAND_START,
		SERVICE_ERROR_NORMAL,
		binaryPath,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL
	);
	if (svcHandle == NULL)
	{
		if (GetLastError() == ERROR_SERVICE_EXISTS)
		{
			printf("[installDriver]: service already exists\n");
			svcHandle = OpenService(
				scmDBHandle,
				driverName,
				SERVICE_ALL_ACCESS
			);
			if (svcHandle == NULL)
			{
				printf("[installDriver]: couldn't open handle to driver\n");
				CloseServiceHandle(scmDBHandle);
				return(NULL);
			}
			CloseServiceHandle(scmDBHandle);
			return(svcHandle);
		}
		printf("[installDriver]: could not open handle to driver\n");
		CloseServiceHandle(scmDBHandle);
		return(NULL);
	}
	printf("[intallDriver]: returning successfully\n");
	CloseServiceHandle(scmDBHandle);
	return(svcHandle);
}

BOOL loadDriver(SC_HANDLE svcHandle)
{
	if (StartService(svcHandle, 0, NULL) == 0)
	{
		if (GetLastError() == ERROR_SERVICE_ALREADY_RUNNING)
		{
			printf("[loadDriver]: driver already running\n");
			return(TRUE);
		}
		else
		{
			printf("[loadDriver]: failed to load driver\n");
			return(FALSE);
		}
	}
	printf("[loadDriver]: driver loaded successfully\n");
	return(TRUE);
}


int main(int argc, char *argv[]) 
{
	if (argc < 3)
	{	
		printf("Usage: %s driverName pathToDriver\n", argv[0]);
		return 0;
	}
	SC_HANDLE svcHandle = NULL;
	svcHandle = installDriver(argv[1], argv[2]);
	if (svcHandle == NULL)
	{
		printf("[main]: cannot install driver\n");
		return -1;
	}
	if (loadDriver(svcHandle) == FALSE)
	{
		printf("[main]: big failure bro\n");
		return -1;
	}
	printf("[main]: We did it Reddit!!\n");
	return 0;
}