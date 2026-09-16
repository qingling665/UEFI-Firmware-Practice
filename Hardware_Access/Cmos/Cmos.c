#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/BaseLib.h> 
#include <Protocol/CpuIo2.h>
#include "Cmos.h"

EFI_STATUS
EFIAPI
UefiMain (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
    EFI_STATUS Status;
    EFI_CPU_IO2_PROTOCOL *CpuIo;
    UINT8 CmosReg;
    UINT8 CmosData;
    UINT8 OutIndex;
    UINT8 RestoreNmi = 0x00;

    Status = gBS->LocateProtocol(&gEfiCpuIo2ProtocolGuid, NULL, (VOID **)&CpuIo);
    if (EFI_ERROR(Status)) {
        Print(L"Locate CPU IO2 Protocol Failed!\n");
        return Status;
    }
    Print(L"     00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F\n");
    Print(L"-----------------------------------------------------\n");

    for (CmosReg = 0x00; CmosReg <= 0x7F; CmosReg++) { 
        if (CmosReg % 16 == 0) {
            Print(L" %02X | ", CmosReg);
        }
        CmosData = 0xFF; 
        OutIndex = CmosReg | NMI_DISABLE_BIT; // Or al, 80h;
        CpuIo->Io.Write(CpuIo, EfiCpuIoWidthUint8, CMOS_INDEX_PORT, 1, &OutIndex); // out 70h, al;
        gBS->Stall(10); 
        CpuIo->Io.Read(CpuIo, EfiCpuIoWidthUint8, CMOS_DATA_PORT, 1, &CmosData); // in al, 71h;
        CpuIo->Io.Write(CpuIo, EfiCpuIoWidthUint8, CMOS_INDEX_PORT, 1, &RestoreNmi); // mov al, 0; Out 70h, al;
        gBS->Stall(10); // lodelay

        Print(L"%02X ", CmosData);
        if ((CmosReg + 1) % 16 == 0) {
            Print(L"\n");
        }
    }
    return EFI_SUCCESS;
}