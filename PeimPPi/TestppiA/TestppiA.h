#ifndef _TEST_PPI_A_
#define _TEST_PPI_A_

#include <PiPei.h>

#define MY_TEST_PPI_A_GUID \
  { 0xcb5ef79e, 0x7496, 0x4b8e, { 0xb3, 0x1d, 0xc6, 0xcd, 0xd1, 0xca, 0x7c, 0x8a } }

extern EFI_GUID gMyTestPpiAGuid;

//Jean 專屬 GUID
#define Jean_VAR_GUID \
  { 0x11223344, 0x5566, 0x7788, { 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff, 0x00 } }

typedef EFI_STATUS (EFIAPI *MY_PEI_GET_VARIABLE) (
  IN CONST CHAR16 *VariableName, IN CONST EFI_GUID *VariableGuid, 
  OUT UINT32 *Attributes OPTIONAL, IN OUT UINTN *DataSize, OUT VOID *Data OPTIONAL
);

typedef EFI_STATUS (EFIAPI *MY_PEI_CREATE_HOB) (
  IN UINT16 Type, IN UINT16 Length, OUT VOID **Hob
);

typedef struct {
  MY_PEI_GET_VARIABLE GetVariable;
  MY_PEI_CREATE_HOB   CreateHob;
} MY_TEST_PPI_A;

#endif