#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/IoLib.h> 

#define KBC_CMD_PORT    0x64
#define KBC_RESET_CMD   0xFE

EFI_STATUS WaitKbcIbfEmpty (VOID) {
    UINT8 Status;
    UINTN Counter = 0;

    do {
        Status = IoRead8 (KBC_CMD_PORT);
        if (Status == 0xFF) {
            return EFI_UNSUPPORTED; 
        }
        Counter++;
        if (Counter > 50000) {
            return EFI_TIMEOUT;
        }
    } while ((Status & 0x02) != 0); 
    return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
UefiMain (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
    EFI_STATUS Status;
    Print(L"=== KBC System Reset Utility ===\n");
    Print(L"Checking KBC Status...\n");
    Status = WaitKbcIbfEmpty();
    if (Status == EFI_UNSUPPORTED) {
        Print(L"[Emulator Detected! System Reset might not work here.]\n");
    } else if (EFI_ERROR(Status)) {
        Print(L"Error: KBC is busy (Timeout).\n");
        return Status;
    }
    Print(L"Sending Reset Command (0xFE) to Port 0x64...\n");
    Print(L"System should reboot NOW...\n");

    gBS->Stall(500000); 
    IoWrite8 (KBC_CMD_PORT, KBC_RESET_CMD); 
    gBS->Stall(2000000); 
    Print(L"If you see this message, the hardware reset failed or is bypassed.\n");

    return EFI_SUCCESS;
}