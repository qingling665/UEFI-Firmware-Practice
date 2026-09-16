#include <Library/SmmServicesTableLib.h>
#include <Library/IoLib.h>
#include <Library/DebugLib.h>
#include <Protocol/SmmSwDispatch2.h>
#include <SmiTable.h>
#define CMOS_ADDR_PORT        0x70
#define CMOS_DATA_PORT        0x71
#define RTC_SECOND_REGISTER   0x00

EFI_STATUS
EFIAPI
SwSmiHandler (
  IN EFI_HANDLE  DispatchHandle,
  IN CONST VOID  *Context,
  IN OUT VOID    *CommBuffer,
  IN OUT UINTN   *CommBufferSize
  )
{
  UINT8 Seconds;
  IoWrite8 (CMOS_ADDR_PORT, RTC_SECOND_REGISTER);
  Seconds = IoRead8 (CMOS_DATA_PORT);
  if ((Seconds & 0x0F) < 0x09) {
    Seconds++;
  } else {
    Seconds = (Seconds & 0xF0) + 0x10;
  }
  if (Seconds >= 0x60) {
    Seconds = 0x00;
  }
  IoWrite8 (CMOS_ADDR_PORT, RTC_SECOND_REGISTER);
  IoWrite8 (CMOS_DATA_PORT, Seconds);
  DEBUG ((EFI_D_INFO, "[SMM] RTC Second Updated via SW SMI 0x66\n"));
  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
SwSmiHomeworkEntryPoint (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS                     Status;
  EFI_SMM_SW_DISPATCH2_PROTOCOL  *SwDispatch;
  EFI_SMM_SW_REGISTER_CONTEXT    SwContext;
  EFI_HANDLE                     SwHandle;
  Status = gSmst->SmmLocateProtocol (&gEfiSmmSwDispatch2ProtocolGuid, NULL, (VOID **)&SwDispatch);
  if (EFI_ERROR (Status)) return Status;
  SwContext.SwSmiInputValue = SMM_HOMEWORK_SW_SMI;
  Status = SwDispatch->Register (SwDispatch, SwSmiHandler, &SwContext, &SwHandle);
  return Status;
}