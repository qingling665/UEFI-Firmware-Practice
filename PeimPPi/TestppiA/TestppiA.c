#include <Library/PeiServicesLib.h>
#include <Library/DebugLib.h>
#include <Ppi/ReadOnlyVariable2.h>
#include "TestppiA.h"


EFI_STATUS EFIAPI MyGetVariable(
  IN CONST CHAR16 *VariableName, IN CONST EFI_GUID *VariableGuid,
  OUT UINT32 *Attributes OPTIONAL, IN OUT UINTN *DataSize, OUT VOID *Data OPTIONAL) 
{
  EFI_STATUS Status;
  EFI_PEI_READ_ONLY_VARIABLE2_PPI *VarPpi;
  Status = PeiServicesLocatePpi(&gEfiPeiReadOnlyVariable2PpiGuid, 0, NULL, (VOID **)&VarPpi);
  if (EFI_ERROR(Status)) return Status;
  return VarPpi->GetVariable(VarPpi, VariableName, VariableGuid, Attributes, DataSize, Data);
}

EFI_STATUS EFIAPI MyCreateHob(IN UINT16 Type, IN UINT16 Length, OUT VOID **Hob) {
  return PeiServicesCreateHob(Type, Length, Hob);
}

MY_TEST_PPI_A mMyTestPpiA = { MyGetVariable, MyCreateHob };

EFI_PEI_PPI_DESCRIPTOR mPpiList = {
  (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gMyTestPpiAGuid,
  &mMyTestPpiA
};

EFI_STATUS EFIAPI TestppiAEntryPoint(IN EFI_PEI_FILE_HANDLE FileHandle, IN CONST EFI_PEI_SERVICES **PeiServices) {
  EFI_STATUS Status;
  DEBUG((EFI_D_INFO, "\n\n##########~TestppiA start~##########\n"));
  
  Status = PeiServicesInstallPpi(&mPpiList);
  if (!EFI_ERROR(Status)) {
    DEBUG((EFI_D_INFO, "Install PPI = Success\n"));
  }
  DEBUG((EFI_D_INFO, "          ~Hello Peim Word~       \n\n"));
  DEBUG((EFI_D_INFO, "##########~TestppiA end~##########\n\n"));
  return EFI_SUCCESS;
}