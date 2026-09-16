#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Protocol/SmbusHc.h>

EFI_STATUS
EFIAPI
DumpSpdData (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS                Status;
  EFI_SMBUS_HC_PROTOCOL     *SmbusHc;
  EFI_SMBUS_DEVICE_ADDRESS  SmbusDeviceAddress;
  UINTN                     Length;
  UINT8                     Data;
  UINTN                     Offset;
  UINTN                     Spd7BitAddress;

  Status = gBS->LocateProtocol (&gEfiSmbusHcProtocolGuid, NULL, (VOID **)&SmbusHc);
  if (EFI_ERROR (Status)) {
    Print (L"Failed to locate EFI_SMBUS_HC_PROTOCOL: %r\n", Status);
    return Status;
  }
  Spd7BitAddress = 0x50; 
  SmbusDeviceAddress.SmbusDeviceAddress = Spd7BitAddress;
  Print (L"Dumping SPD data at 8-bit address 0xA0 (7-bit 0x50)...\n");
  for (Offset = 0; Offset < 256; Offset++) {
    Length = 1;
    Status = SmbusHc->Execute (
                        SmbusHc,
                        SmbusDeviceAddress,
                        Offset,            
                        EfiSmbusReadByte,  
                        FALSE,             
                        &Length,
                        &Data
                        );
    if (!EFI_ERROR (Status)) {
      if (Offset % 16 == 0) {
        Print (L"\n%02X: ", Offset);
      }
      Print (L"%02X ", Data);
    } else {
      Print (L"\nFailed to read at offset %02X (Status: %r)\n", Offset, Status);
      Print (L"The target device might be missing or returned NACK.\n");
      break; 
    }
  }
  Print (L"\n\nDump Complete.\n");

  return EFI_SUCCESS;
}
