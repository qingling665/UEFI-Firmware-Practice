#include <Library/PeiServicesLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Pi/PiHob.h>
#include "../TestppiA/TestppiA.h" 


EFI_GUID gJeanGuid = Jean_VAR_GUID;

EFI_STATUS EFIAPI TestppiBEntryPoint(IN EFI_PEI_FILE_HANDLE FileHandle, IN CONST EFI_PEI_SERVICES **PeiServices) {
  EFI_STATUS Status;
  MY_TEST_PPI_A *TestPpi;
  UINTN DataSize = 0;
  VOID *HobData;

  DEBUG((EFI_D_INFO, "##########~TestppiB start~##########\n"));

  Status = PeiServicesLocatePpi(&gMyTestPpiAGuid, 0, NULL, (VOID **)&TestPpi);
  if (EFI_ERROR(Status)) {
    DEBUG((EFI_D_ERROR, "LocatePpi B = Not Found\n"));
    return Status;
  }
  DEBUG((EFI_D_INFO, "LocatePpi B = Success\n"));

  Status = TestPpi->GetVariable(L"Jean", &gJeanGuid, NULL, &DataSize, NULL);
  if (Status == EFI_BUFFER_TOO_SMALL) {
    UINT16 HobLength = (UINT16)(sizeof(EFI_HOB_GUID_TYPE) + DataSize);
    Status = TestPpi->CreateHob(EFI_HOB_TYPE_GUID_EXTENSION, HobLength, &HobData);
    
    if (!EFI_ERROR(Status)) {
     
      EFI_HOB_GUID_TYPE *GuidHob; 
      VOID *Payload;

      GuidHob = (EFI_HOB_GUID_TYPE *)HobData;
      CopyMem(&GuidHob->Name, &gJeanGuid, sizeof(EFI_GUID));
      
      Payload = (VOID *)(GuidHob + 1);
      Status = TestPpi->GetVariable(L"Jean", &gJeanGuid, NULL, &DataSize, Payload);
      if (!EFI_ERROR(Status)) {
        DEBUG((EFI_D_INFO, "Create HOB and GetVariable = Success\n"));
      }
    }
  } else {
    DEBUG((EFI_D_INFO, "Jean Variable = Not Found (Please set it first in Shell)\n"));
  }

  DEBUG((EFI_D_INFO, "##########~TestppiB end~##########\n"));
  return EFI_SUCCESS;
}