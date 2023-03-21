/*++

Module Name:

    device.h

Abstract:

    This file contains the device definitions.

Environment:

    Kernel-mode Driver Framework

--*/

#include "public.h"
#define u16 UINT16
#define u8 UINT8
#include "qpnp-revid.h"

EXTERN_C_START
//
// The device context performs the same job as
// a WDM device extension in the driver frameworks
//

typedef struct votable votable;


enum votable_type {
    VOTE_MIN,
    VOTE_MAX,
    VOTE_SET_ANY,
    NUM_VOTABLE_TYPES,
};

enum {
    IRQ_SET_TYPE = 0,
    IRQ_POL_HIGH,
    IRQ_POL_LOW,
    IRQ_LATCHED_CLR, /* not needed but makes life easy */
    IRQ_EN_SET,
    IRQ_MAX_REGS,
};

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
typedef struct i2c_pmic_periph {
    void* data;
    u8		cmd[2];
    u8		cached[IRQ_MAX_REGS];
    u8		synced[IRQ_MAX_REGS];
    u8		wake;
}i2c_pmic_periph;

typedef struct device_node
{
    WDFDEVICE               device;
    WDFIOTARGET             SpbController;
    LARGE_INTEGER           PeripheralId;
    WDFINTERRUPT            Interrupt;
    i2c_pmic_periph         pmic_periph;
    struct pmic_revid_data  revid_data;
} DEVICE_CONTEXT, *PDEVICE_CONTEXT;

//
// This macro will generate an inline function called DeviceGetContext
// which will be used to get a pointer to the device context memory
// in a type safe manner.
//
WDF_DECLARE_CONTEXT_TYPE_WITH_NAME(DEVICE_CONTEXT, GetDeviceContext)

//
// Function to initialize the device and its callbacks
//
NTSTATUS
smb1390CreateDevice(
    _Inout_ PWDFDEVICE_INIT DeviceInit
    );

BOOLEAN
OnInterruptIsr(
    _In_  WDFINTERRUPT FxInterrupt,
    _In_  ULONG        MessageID
);
votable* create_votable(const char* name,
    int votable_type,
    int (*callback)(struct votable* votable,
        void* data,
        int effective_result,
        const char* effective_client),
    void* data);
EXTERN_C_END
