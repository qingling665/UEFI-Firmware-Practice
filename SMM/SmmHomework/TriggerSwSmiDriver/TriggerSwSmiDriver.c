#include <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/IoLib.h>
#include <SmiTable.h>

EFI_STATUS
EFIAPI
TriggerSwSmiEntryPoint (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS                 Status;
  EFI_INPUT_KEY              Key;
  EFI_TIME                   Time;
  gST->ConOut->ClearScreen(gST->ConOut);
  Print(L"==========================================\n");
  Print(L"    SMM RTC Hardware Sync Monitor         \n");
  Print(L"==========================================\n");
  Print(L"Press [F1] to Increment RTC, [ESC] to Exit.\n\n");

  while (TRUE) {
    Status = gRT->GetTime (&Time, NULL);
    if (EFI_ERROR(Status)) {
      Time.Hour = 0; Time.Minute = 0; Time.Second = 0;
    }
    Print(L"\r[Real-Time Clock] %02d:%02d:%02d    ", 
          (UINT32)Time.Hour, (UINT32)Time.Minute, (UINT32)Time.Second);
    Status = gST->ConIn->ReadKeyStroke (gST->ConIn, &Key);
    if (!EFI_ERROR (Status)) {
      if (Key.ScanCode == SCAN_F1) {
        IoWrite8(0xB2, SMM_HOMEWORK_SW_SMI); 
      } else if (Key.ScanCode == SCAN_ESC) {
        break; 
      }
    }
    gBS->Stall (100000); 
  }
  return EFI_SUCCESS;
}