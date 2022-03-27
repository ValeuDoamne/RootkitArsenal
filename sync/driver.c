#include <ntddk.h>
#include "dbgmsg.h"


#define DWORD unsigned int

DWORD LockAcquired, nCPUsLocked;

KIRQL RaiseIRQL() {
	KIRQL curr;
	KIRQL prev;

	
	curr = KeGetCurrentIrql();
	prev = curr;

	if (curr < DISPATCH_LEVEL)
	{
		KeRaiseIrql(DISPATCH_LEVEL, &prev);
	}
	return(prev);
}

void LowerIRQL(KIRQL prev)
{
	KeLowerIrql(prev);
	return;
}

void lockRoutine(
	IN PKDPC dpc,
	IN PVOID context,
	IN PVOID arg1,
	IN PVOID arg2
)
{
	DBG_PRINT2("[lockRoutine]: beginCPU[%u]", KeGetCurrentProcessorNumber());
	InterlockedIncrement(&nCPUsLocked);
	while (InterlockedCompareExchange(&LockAcquired, 1, 1) == 0) {
		__asm {
			nop
		}
	}

	InterlockedDecrement(&nCPUsLocked);
	DBG_PRINT2("[lockRoutine]: endCPU[%u]", KeGetCurrentProcessorNumber());
	return;
}

PKDPC AcquireLock()
{
	PKDPC dpcArray;
	DWORD cpuID;
	DWORD i;
	DWORD nOtherCPUs;
	
	DBG_TRACE("AcquireLock", "Entering procedure");

	if (KeGetCurrentIrql() != DISPATCH_LEVEL) { return(NULL); }
	InterlockedAnd(&LockAcquired, 0);
	InterlockedAnd(&nCPUsLocked, 0);

	dpcArray = (PKDPC)ExAllocatePool(
		NonPagedPool,
		KeNumberProcessors * sizeof(KDPC)
	);
	if (dpcArray == NULL) { return(NULL); }

	cpuID = KeGetCurrentProcessorNumber();

	for (i = 0; i <(unsigned int)KeNumberProcessors; i++)
	{
		PKDPC dpcPtr = &(dpcArray[i]);
		if (i != cpuID)
		{
			KeInitializeDpc(dpcPtr, lockRoutine, NULL);
			KeSetTargetProcessorDpc(dpcPtr, (CCHAR)i);
			KeInsertQueueDpc(dpcPtr, NULL, NULL);
		}
	}

	nOtherCPUs = KeNumberProcessors - 1;
	InterlockedCompareExchange(&nCPUsLocked, nOtherCPUs, nOtherCPUs);

	while (nCPUsLocked != nOtherCPUs)
	{
		__asm {
			nop
		}
		InterlockedCompareExchange(&nCPUsLocked, nOtherCPUs, nOtherCPUs);
	}
	
	DBG_TRACE("AcquireLock", "Exiting procedure");
	
	return(dpcArray);
}


NTSTATUS ReleaseLock(
	IN PVOID dpcPtr
)
{
	DBG_TRACE("ReleaseLock", "Beginning of the end");
	InterlockedIncrement(&LockAcquired);
	InterlockedCompareExchange(&nCPUsLocked, 0, 0);
	while (nCPUsLocked != 0)
	{
		__asm {
			nop
		}
		InterlockedCompareExchange(&nCPUsLocked, 0, 0);
	}
	if (dpcPtr != 0)
	{
		ExFreePool(dpcPtr);
	}
	
	DBG_TRACE("ReleaseLock", "Finishing the end");
	return(STATUS_SUCCESS);
}

NTSTATUS DriverEntry
(
	IN PDRIVER_OBJECT driverObject,
	IN PUNICODE_STRING regPath
) 
{
	KIRQL irql;
	PKDPC dpcPtr;

	DBG_TRACE("DriverEntry", "Driver is loaded in Kernel");
	irql = RaiseIRQL();
	dpcPtr = AcquireLock();

	ReleaseLock(dpcPtr);
	LowerIRQL(irql);
	
	DBG_TRACE("DriverEntry", "Driver is unloaded from Kernel");
	
	return(STATUS_SUCCESS);
}
