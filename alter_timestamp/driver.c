

#include <ntddk.h>
#include "dbgmsg.h"

#define FILE_DEVICE_RK 0x00008001

#define DbgMsg DBG_TRACE

FILE_BASIC_INFORMATION getSystemFileTimeStamp() 
{
	HANDLE handle;
	NTSTATUS ntstatus;
	UNICODE_STRING fileName;
	OBJECT_ATTRIBUTES objectAttr;
	IO_STATUS_BLOCK	ioStatusBlock;
	FILE_BASIC_INFORMATION fileBasicInfo;

	RtlInitUnicodeString(&fileName, (PCWSTR)"C:\\Windows\\");
	InitializeObjectAttributes(
		&objectAttr,
		&fileName,
		OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE,
		NULL,
		NULL
	);
	if (KeGetCurrentIrql() != PASSIVE_LEVEL)
	{
		DbgMsg("getSystemFileTimeStamp", "Must be at passive IRQL");
	}
	DbgMsg("getSystemFileTimeStamp", "Initialized attributes");

	ntstatus = ZwOpenFile(&handle,
		FILE_WRITE_ACCESS,
		&objectAttr,
		&ioStatusBlock,
		0,
		FILE_SYNCHRONOUS_IO_NONALERT
	);
	if (ntstatus != STATUS_SUCCESS)
	{
		DbgMsg("getSystemFileTimeStamp", "Could not open file");
	}
	
	ntstatus = ZwQueryInformationFile(
		handle,
		&ioStatusBlock,
		&fileBasicInfo,
		sizeof(fileBasicInfo),
		FileBasicInformation
	);
	if (ntstatus != STATUS_SUCCESS)
	{
		DbgMsg("getSystemFileTimeStamp", "Could not get file information");
	}
	DbgMsg("getSystemFileTimeStamp", "Set file timpestamps");
	ZwClose(handle);
	DbgMsg("getSystemFileTimeStamp", "Close handle");

	return fileBasicInfo;
}

void processFile(
	IN PCWSTR fullPath,
	IN BOOLEAN wipe
)
{
	UNICODE_STRING fileName;
	OBJECT_ATTRIBUTES objAttr;
	HANDLE handle;
	NTSTATUS ntstatus;
	IO_STATUS_BLOCK ioStatusBlock;
	FILE_BASIC_INFORMATION fileBasicInfo;

	RtlInitUnicodeString(&fileName, fullPath);

	InitializeObjectAttributes(
		&objAttr,
		&fileName,
		OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE,
		NULL,
		NULL
	);

	if (KeGetCurrentIrql() != PASSIVE_LEVEL)
	{
		DbgMsg("processFile", "Must be at passive IRQL");
	}
	DbgMsg("processFile", "Initialized attributes");

	ntstatus = ZwOpenFile(
		&handle,
		FILE_WRITE_ACCESS,
		&objAttr,
		&ioStatusBlock,
		0,
		FILE_SYNCHRONOUS_IO_NONALERT
	);

	if (ntstatus != STATUS_SUCCESS)
	{
		DbgMsg("processFile", "Could not open file");
	}
	DbgMsg("processFile", "opened file");

	if (wipe)
	{
		fileBasicInfo.CreationTime.LowPart = 1;
		fileBasicInfo.CreationTime.HighPart = 0;
		fileBasicInfo.LastAccessTime.LowPart = 1;
		fileBasicInfo.LastAccessTime.HighPart = 0;
		fileBasicInfo.LastWriteTime.LowPart = 1;
		fileBasicInfo.LastWriteTime.HighPart = 0;
		fileBasicInfo.ChangeTime.LowPart = 1;
		fileBasicInfo.ChangeTime.HighPart = 0;
		fileBasicInfo.FileAttributes = FILE_ATTRIBUTE_NORMAL;
	}
	else 
	{
		fileBasicInfo = getSystemFileTimeStamp();
	}

	ntstatus = ZwSetInformationFile(
		handle,
		&ioStatusBlock,
		&fileBasicInfo,
		sizeof(fileBasicInfo),
		FileBasicInformation
	);

	if (ntstatus != STATUS_SUCCESS)
	{
		DbgMsg("processFile", "Could not set file information");
	}
	DbgMsg("processFile", "Set file timpestamps");
	ZwClose(handle);
	DbgMsg("processFile", "Close handle");
	return;
}


NTSTATUS DriverEntry(IN PDRIVER_OBJECT DriverObject, IN PUNICODE_STRING regPath)
{
	DBG_TRACE("OnDriverEntry", "Driver has been loaded");

	processFile((PCWSTR)"C:\\Users\\Simon\\Desktop\\note.txt", TRUE);

	DBG_TRACE("OnDriverEntry", "Driver is done");
	return(STATUS_SUCCESS);
}/*end DriverEntry()---------------------------------------------------*/

