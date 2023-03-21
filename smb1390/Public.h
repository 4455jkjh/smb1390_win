/*++

Module Name:

    public.h

Abstract:

    This module contains the common declarations shared by driver
    and user applications.

Environment:

    user and kernel

--*/

//
// Define an Interface Guid so that apps can find the device and talk to it.
//

DEFINE_GUID (GUID_DEVINTERFACE_smb1390,
    0xf9be5dc2,0xcb1e,0x4a82,0x8d,0xd4,0x5e,0x87,0xe7,0xe1,0xdd,0x4f);
// {f9be5dc2-cb1e-4a82-8dd4-5e87e7e1dd4f}
