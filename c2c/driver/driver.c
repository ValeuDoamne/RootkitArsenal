

#include <ntddk.h>
#include "dbgmsg.h"
#include <string.h>

#define FILE_DEVICE_RK 0x00008001



#define IOCTL_TEST_CMD CTL_CODE(FILE_DEVICE_RK, 0x801, METHOD_BUFFERED, FILE_READ_DATA|FILE_WRITE_DATA)


VOID TestCommand (VOID* inputBuffer, VOID* outputBuffer, ULONG inLen, ULONG* outLen)
{
	*outLen = 4;
	strncpy(outputBuffer, "Test", outLen);
}


NTSTATUS dispatchIOControl
(
	IN PDEVICE_OBJECT DeviceObject,
	IN PIRP           IRP
)
{
	PIO_STACK_LOCATION irpStack;
	PVOID              inputBuffer;
	PVOID              outputBuffer;
	ULONG              inputBufferLen;
	ULONG              outputBufferLen;
	ULONG              ioctrlcode;
	NTSTATUS           ntStatus;

	ntStatus = STATUS_SUCCESS;

	inputBuffer  = (*IRP).AssociatedIrp.SystemBuffer;
	outputBuffer = (*IRP).AssociatedIrp.SystemBuffer;

	irpStack = IoGetCurrentIrpStackLocation(IRP); 
	inputBufferLen = (*irpStack).Parameters.DeviceIoControl.InputBufferLength;
	outputBufferLen = (*irpStack).Parameters.DeviceIoControl.OutputBufferLength;
	ioctrlcode = (*irpStack).Parameters.DeviceIoControl.IoControlCode;

	DBG_TRACE("dispatchIOControl", "Received a command");

	switch (ioctrlcode)
	{
		case IOCTL_TEST_CMD:
		{
			TestCommand(inputBuffer, outputBuffer, inputBufferLen, &outputBufferLen);
			((*IRP).IoStatus).Information = outputBufferLen;
			break;
		}
		default:
			DBG_TRACE("dispatchIOControl", "control code not recognized");
			break;
	}
	IoCompleteRequest(IRP, IO_NO_INCREMENT);

	return(ntStatus);
}/*end dispatchIOControl()---------------------------------------------------*/



NTSTATUS defaultDispatch(IN PDEVICE_OBJECT DriverObject, IN PIRP IRP)
{
	((*IRP).IoStatus).Status = STATUS_SUCCESS;
	((*IRP).IoStatus).Information = 0;
	IoCompleteRequest(IRP, IO_NO_INCREMENT);

	return(STATUS_SUCCESS);
}/*end defaultDsipatch()---------------------------------------------------*/

const WCHAR DeviceNameBuffer[] = L"\\Device\\msnetdiag";
PDEVICE_OBJECT MSNetDiagDeviceObject;

NTSTATUS RegisterDeviceDriverName
(
	IN PDRIVER_OBJECT DriverObject
)
{
	NTSTATUS ntStatus;
	UNICODE_STRING unicodeString;
	
	RtlInitUnicodeString(&unicodeString, DeviceNameBuffer);
	ntStatus = IoCreateDevice(
		DriverObject,              // Pointer to driver object
		0,                         //# bytes allocated for device extension
		&unicodeString,            // unicode string containing device name
		FILE_DEVICE_RK,            // driver type (vendor defined)
		0,                         // system-defined constants, OR-ed together
		TRUE,                      // the device object is an exclusive device
		&MSNetDiagDeviceObject     // Pointer to global device object
	);
	return(ntStatus);
}/*end RegisterDeviceDriverName()---------------------------------------------------*/

const WCHAR DeviceLinkBuffer[] = L"\\DosDevices\\msnetdiag";

NTSTATUS RegisterDriverDeviceLink()
{
	NTSTATUS ntStatus;
	UNICODE_STRING unicodeString;
	UNICODE_STRING unicodeLinkString;

	RtlInitUnicodeString(&unicodeString, DeviceNameBuffer);
	RtlInitUnicodeString(&unicodeLinkString, DeviceLinkBuffer);

	ntStatus = IoCreateSymbolicLink(
		&unicodeLinkString,
		&unicodeString
	);
	return(ntStatus);
}/*end RegisterDriverDeviceLink()---------------------------------------------------*/


PDEVICE_OBJECT deviceObj;
VOID Unload(IN PDRIVER_OBJECT DriverObject)
{
	UNICODE_STRING unicode_string;
	DBG_TRACE("OnUnload", "Received signal to unload driver");
	
	deviceObj = (*DriverObject).DeviceObject;


	if (deviceObj != NULL)
	{
		DBG_TRACE("OnUnload", "Unregistering driver's symbolic link");
		RtlInitUnicodeString(&unicode_string, DeviceLinkBuffer);

		IoDeleteSymbolicLink(&unicode_string);

		DBG_TRACE("OnUnload", "Unregistering driver's device name");
		IoDeleteDevice((*DriverObject).DeviceObject);
	}
	return;
}



NTSTATUS DriverEntry(IN PDRIVER_OBJECT DriverObject, IN PUNICODE_STRING regPath)
{
	NTSTATUS status;
	int i;
	DBG_TRACE("OnDriverEntry", "Driver has been loaded");

	DbgPrint("IOCTL_TEST_CMD: %x", IOCTL_TEST_CMD);

	DBG_TRACE("OnDriverEntry", "Registering driver's device name");
	status = RegisterDeviceDriverName(DriverObject);

	if (!NT_SUCCESS(status))
	{
		DBG_TRACE("OnDriverEntry", "Failed to register device name");
		return status;
	}

	DBG_TRACE("OneDriverEntry", "Registering driver's symbolic link");

	status = RegisterDriverDeviceLink();
	if (!NT_SUCCESS(status))
	{
		DBG_TRACE("OnDriverEntry", "Failed to create symbolic link");
		return status;
	}

	for (i = 0; i < IRP_MJ_MAXIMUM_FUNCTION; i++)
	{
		(*DriverObject).MajorFunction[i] = (PDRIVER_DISPATCH)defaultDispatch;
	}
	(*DriverObject).MajorFunction[IRP_MJ_DEVICE_CONTROL] = dispatchIOControl;

	(*DriverObject).DriverUnload = Unload;

	//DriverObjectRef = DriverObject;

	return(STATUS_SUCCESS);
}/*end DriverEntry()---------------------------------------------------*/

