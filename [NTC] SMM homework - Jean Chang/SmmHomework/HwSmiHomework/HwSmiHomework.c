#include <Library/SmmServicesTableLib.h>
#include <Protocol/SmmPowerButtonDispatch2.h>
#include <Library/IoLib.h>
#define CMOS_ADDR_PORT        0x70
#define CMOS_DATA_PORT        0x71
#define RTC_SECOND_REGISTER   0x00

EFI_STATUS
EFIAPI
HwSmiHomeworkCallback (
  IN  EFI_HANDLE                    DispatchHandle,
  IN  CONST VOID                    *Context,
  IN  OUT VOID                      *CommBuffer,
  IN  OUT UINTN                     *CommBufferSize
  )
{
  UINT8  Seconds;
  IoWrite8 (CMOS_ADDR_PORT, RTC_SECOND_REGISTER);
  Seconds = IoRead8 (CMOS_DATA_PORT);
  
  IoWrite8 (0x80, Seconds);
  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
HwSmiHomeworkEntryPoint (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS                                Status;
  EFI_SMM_POWER_BUTTON_DISPATCH2_PROTOCOL   *PwrBtnDispatch;
  EFI_SMM_POWER_BUTTON_REGISTER_CONTEXT     PwrBtnContext;
  EFI_HANDLE                                PwrBtnHandle;

  Status = gSmst->SmmLocateProtocol (
                    &gEfiSmmPowerButtonDispatch2ProtocolGuid,
                    NULL,
                    (VOID **)&PwrBtnDispatch
                    );
  if (EFI_ERROR (Status)) return Status;

  PwrBtnContext.Phase = EfiPowerButtonEntry;
  Status = PwrBtnDispatch->Register (
                             PwrBtnDispatch,
                             HwSmiHomeworkCallback,
                             &PwrBtnContext,
                             &PwrBtnHandle
                             );

  return Status;
}