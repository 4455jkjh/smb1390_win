/*++

Module Name:

    driver.h

Abstract:

    This file contains the driver definitions.

Environment:

    Kernel-mode Driver Framework

--*/

#include <ntddk.h>
#include <wdf.h>
#include <initguid.h>

#include "device.h"
#include "queue.h"
#include "trace.h"

EXTERN_C_START

//
// WDFDRIVER Events
//

DRIVER_INITIALIZE DriverEntry;
EVT_WDF_DRIVER_DEVICE_ADD smb1390EvtDeviceAdd;
EVT_WDF_OBJECT_CONTEXT_CLEANUP smb1390EvtDriverContextCleanup;


EVT_WDF_DEVICE_PREPARE_HARDWARE      OnPrepareHardware;
EVT_WDF_DEVICE_RELEASE_HARDWARE      OnReleaseHardware;
PFN_WDF_DEVICE_SELF_MANAGED_IO_INIT     SM1390ManagedIoInit;
EVT_WDF_DEVICE_D0_ENTRY              OnD0Entry;
EVT_WDF_DEVICE_D0_EXIT               OnD0Exit;


VOID
SpbDeviceOpen(
    _In_  PDEVICE_CONTEXT  pDevice
);
VOID
SpbDeviceClose(
    _In_  PDEVICE_CONTEXT  pDevice
);
VOID
SpbDeviceWrite(
    _In_ PDEVICE_CONTEXT pDevice,
    _In_ PVOID pInputBuffer,
    _In_ size_t inputBufferLength
);
VOID
SpbDeviceWriteRead(
    _In_ PDEVICE_CONTEXT pDevice,
    _In_ PVOID pInputBuffer,
    _In_ PVOID pOutputBuffer,
    _In_ size_t inputBufferLength,
    _In_ size_t outputBufferLength
);
EXTERN_C_END
