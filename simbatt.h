/*++

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:

    simbatt.h

Abstract:

    This is the header file for the simulated battery driver.

    N.B. This code is provided "AS IS" without any expressed or implied warranty.

--*/

//---------------------------------------------------------------------- Pragmas

#pragma once

//--------------------------------------------------------------------- Includes

#include <wdm.h>
#include <wdf.h>
#include <batclass.h>
#include <wmistr.h>
#include <wmilib.h>
#include <ntstrsafe.h>
#include "ffs.h"
#include "qpnp-revid.h"
#include "spb.h"

//------------------------------------------------------------- Debug Facilities

#define SIMBATT_ERROR               DPFLTR_ERROR_LEVEL      // ed Kd_IHVDRIVER_Mask 0x1
#define SIMBATT_WARN                DPFLTR_WARNING_LEVEL    // ed Kd_IHVDRIVER_Mask 0x2
#define SIMBATT_TRACE               DPFLTR_TRACE_LEVEL      // ed Kd_IHVDRIVER_Mask 0x4
#define SIMBATT_INFO                DPFLTR_INFO_LEVEL       // ed Kd_IHVDRIVER_Mask 0x8

#if defined(DEBUGPRINT)
    #define DebugPrint(_Level, _Msg, ...) \
        SimBattPrint(_Level, _Msg, __VA_ARGS__)

    #define DebugEnter() \
        DebugPrint(SIMBATT_TRACE, "Entering " __FUNCTION__ "\n")

    #define DebugExit() \
        DebugPrint(SIMBATT_TRACE, "Leaving " __FUNCTION__ "\n")

    #define DebugExitStatus(_status_) \
        DebugPrint(SIMBATT_TRACE, \
                   "Leaving " __FUNCTION__ ": Status=0x%x\n", \
                   _status_)

#else
    #define DebugPrint(l, m, ...)
    #define DebugEnter()
    #define DebugExit()
    #define DebugExitStatus(_status_)
#endif

//--------------------------------------------------------------------- Literals

#define SIMBATT_TAG                 'StaB'
#define SIMBATT_STATE_REG_NAME      L"SMB1390"

//------------------------------------------------------------------ Definitions

typedef struct {
    UNICODE_STRING                  RegistryPath;
} SIMBATT_GLOBAL_DATA, *PSIMBATT_GLOBAL_DATA;

#define SIMBATT_STATE_VERSION_1     1
#define SIMBATT_STATE_VERSION       SIMBATT_STATE_VERSION_1

typedef struct {
    USHORT                          Version;
    BATTERY_MANUFACTURE_DATE        ManufactureDate;
    BATTERY_INFORMATION             BatteryInfo;
    BATTERY_STATUS                  BatteryStatus;
    BATTERY_REPORTING_SCALE         GranularityScale[4];
    ULONG                           GranularityCount;
    ULONG                           EstimatedTime;
    ULONG                           Temperature;
    ULONG                           MaxCurrentDraw;
    WCHAR                           DeviceName[MAX_BATTERY_STRING_SIZE];
    WCHAR                           ManufacturerName[MAX_BATTERY_STRING_SIZE];
    WCHAR                           SerialNumber[MAX_BATTERY_STRING_SIZE];
    WCHAR                           UniqueId[MAX_BATTERY_STRING_SIZE];
} SIMBATT_STATE, *PSIMBATT_STATE;
#define u16 UINT16

#define BIT(x) (1 << x)
#define GENMASK(h, l) \
	(((~0UL) - (1UL << (l)) + 1) & (~0UL >> (32 - 1 - (h))))

#define I2C_INTR_STATUS_BASE	0x0550
#define INT_RT_STS_OFFSET	0x10
#define INT_SET_TYPE_OFFSET	0x11
#define INT_POL_HIGH_OFFSET	0x12
#define INT_POL_LOW_OFFSET	0x13
#define INT_LATCHED_CLR_OFFSET	0x14
#define INT_EN_SET_OFFSET	0x15
#define INT_EN_CLR_OFFSET	0x16
#define INT_LATCHED_STS_OFFSET	0x18
#define INT_PENDING_STS_OFFSET	0x19
#define INT_MID_SEL_OFFSET	0x1A
#define INT_MID_SEL_MASK	GENMASK(1, 0)
#define INT_PRIORITY_OFFSET	0x1B
#define INT_PRIORITY_BIT	BIT(0)

enum {
    IRQ_SET_TYPE = 0,
    IRQ_POL_HIGH,
    IRQ_POL_LOW,
    IRQ_LATCHED_CLR, /* not needed but makes life easy */
    IRQ_EN_SET,
    IRQ_MAX_REGS,
};

enum {
    SWITCHER_OFF_WINDOW_IRQ = 0,
    SWITCHER_OFF_FAULT_IRQ,
    TSD_IRQ,
    IREV_IRQ,
    VPH_OV_HARD_IRQ,
    VPH_OV_SOFT_IRQ,
    ILIM_IRQ,
    TEMP_ALARM_IRQ,
    NUM_IRQS,
};
struct smb1390 {
    int			irqs[NUM_IRQS];
};
typedef struct  device_node {

    //
    // Battery class registration
    //

    PVOID                           ClassHandle;
    WDFWAITLOCK                     ClassInitLock;
    WMILIB_CONTEXT                  WmiLibContext;

    //
    // Battery state
    //

    WDFWAITLOCK                     StateLock;
    ULONG                           BatteryTag;
    SIMBATT_STATE                   State;
    SPB_CONTEXT                     I2CContext;
    u8		addr[2];
    u8		cached[IRQ_MAX_REGS];
    u8		synced[IRQ_MAX_REGS];
    struct pmic_revid_data data;
    struct smb1390 smb1390;
} SIMBATT_FDO_DATA, *PSIMBATT_FDO_DATA;

//------------------------------------------------------ WDF Context Declaration

WDF_DECLARE_CONTEXT_TYPE_WITH_NAME(SIMBATT_GLOBAL_DATA, GetGlobalData);
WDF_DECLARE_CONTEXT_TYPE_WITH_NAME(SIMBATT_FDO_DATA, GetDeviceExtension);

//----------------------------------------------------- Prototypes (miniclass.c)

_IRQL_requires_same_
VOID
SimBattPrepareHardware (
    _In_ WDFDEVICE Device
    );

EVT_WDF_IO_QUEUE_IO_DEVICE_CONTROL SimBattIoDeviceControl;
BCLASS_QUERY_TAG_CALLBACK SimBattQueryTag;
BCLASS_QUERY_INFORMATION_CALLBACK SimBattQueryInformation;
BCLASS_SET_INFORMATION_CALLBACK SimBattSetInformation;
BCLASS_QUERY_STATUS_CALLBACK SimBattQueryStatus;
BCLASS_SET_STATUS_NOTIFY_CALLBACK SimBattSetStatusNotify;
BCLASS_DISABLE_STATUS_NOTIFY_CALLBACK SimBattDisableStatusNotify;

_IRQL_requires_same_
VOID
SimBattPrint (
    _In_ ULONG Level,
    _In_ PCSTR Format,
    ...
    );

