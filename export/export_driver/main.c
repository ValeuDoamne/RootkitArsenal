#include <ntddk.h>
#include "dbgmsg.h"

NTSTATUS DllInitialize(__in PUNICODE_STRING RegistryPath)
{
	DBG_TRACE("ExportDriver", "Dllinitialize() invoked");
	return(STATUS_SUCCESS);
}

NTSTATUS DllUnload()
{
	DBG_TRACE("ExportDriver", "DllUnload() invoked");
	return(STATUS_SUCCESS);
}

void ExportedRoutine()
{
	DBG_TRACE("ExportedRoutine", "Someone just called me!");
}

DRIVER_INITIALIZE DriverEntry;

NTSTATUS DriverEntry(
	PDRIVER_OBJECT  DriverObject,
	PUNICODE_STRING  RegistryPath
)
{

	return(STATUS_SUCCESS);
}
