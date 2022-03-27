#include <ntddk.h>

NTSTATUS DriverEntry
(
 IN PDRIVER_OBJECT DriverObject,
 IN PUNICODE_STRING regPath
)
{
	DbgPrint("Hello World!");
	return(STATUS_SUCCESS);
}
