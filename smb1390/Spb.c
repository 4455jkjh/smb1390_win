#include "driver.h"
#define RESHUB_USE_HELPER_ROUTINES
#include "reshub.h"

VOID
SpbDeviceOpen(
    _In_  PDEVICE_CONTEXT  pDevice
)
{
    WDF_IO_TARGET_OPEN_PARAMS  openParams;
    NTSTATUS status;
    DECLARE_UNICODE_STRING_SIZE(DevicePath, RESOURCE_HUB_PATH_SIZE);
    RESOURCE_HUB_CREATE_PATH_FROM_ID(
        &DevicePath,
        pDevice->PeripheralId.LowPart,
        pDevice->PeripheralId.HighPart);

    //
    // Open a handle to the SPB controller.
    //

    WDF_IO_TARGET_OPEN_PARAMS_INIT_OPEN_BY_NAME(
        &openParams,
        &DevicePath,
        (GENERIC_READ | GENERIC_WRITE));

    openParams.ShareAccess = 0;
    openParams.CreateDisposition = FILE_OPEN;
    openParams.FileAttributes = FILE_ATTRIBUTE_NORMAL;

    status = WdfIoTargetOpen(
        pDevice->SpbController,
        &openParams);
}
VOID
SpbDeviceClose(
    _In_  PDEVICE_CONTEXT  pDevice
)
{

    WdfIoTargetClose(pDevice->SpbController);
}
VOID
SpbDeviceWrite(
    _In_ PDEVICE_CONTEXT pDevice,
    _In_ PVOID pInputBuffer,
    _In_ size_t inputBufferLength
)
{
    WDF_MEMORY_DESCRIPTOR  inMemoryDescriptor;
    ULONG_PTR  bytesWritten = (ULONG_PTR)NULL;
    NTSTATUS status;


    WDF_MEMORY_DESCRIPTOR_INIT_BUFFER(&inMemoryDescriptor,
        pInputBuffer,
        (ULONG)inputBufferLength);

    status = WdfIoTargetSendWriteSynchronously(
        pDevice->SpbController,
        NULL,
        &inMemoryDescriptor,
        NULL,
        NULL,
        &bytesWritten
    );

    if (!NT_SUCCESS(status))
    {
    }
}

VOID
SpbDeviceWriteRead(
    _In_ PDEVICE_CONTEXT pDevice,
    _In_ PVOID pInputBuffer,
    _In_ PVOID pOutputBuffer,
    _In_ size_t inputBufferLength,
    _In_ size_t outputBufferLength
)
{
    WDF_MEMORY_DESCRIPTOR  inMemoryDescriptor;
    WDF_MEMORY_DESCRIPTOR  outMemoryDescriptor;
    ULONG_PTR  bytesWritten = (ULONG_PTR)NULL;
    ULONG_PTR  bytesRead = (ULONG_PTR)NULL;
    NTSTATUS status;


    WDF_MEMORY_DESCRIPTOR_INIT_BUFFER(&inMemoryDescriptor,
        pInputBuffer,
        (ULONG)inputBufferLength);

    status = WdfIoTargetSendWriteSynchronously(
        pDevice->SpbController,
        NULL,
        &inMemoryDescriptor,
        NULL,
        NULL,
        &bytesWritten
    );

    if (!NT_SUCCESS(status))
    {
        return;
    }

    WDF_MEMORY_DESCRIPTOR_INIT_BUFFER(&outMemoryDescriptor,
        pOutputBuffer,
        (ULONG)outputBufferLength);

    status = WdfIoTargetSendReadSynchronously(
        pDevice->SpbController,
        NULL,
        &outMemoryDescriptor,
        NULL,
        NULL,
        &bytesRead
    );

    if (!NT_SUCCESS(status))
    {
    }
}
