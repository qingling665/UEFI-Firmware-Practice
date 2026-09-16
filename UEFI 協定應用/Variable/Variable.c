#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/BaseLib.h>
#include <Library/PrintLib.h>

#pragma optimize("", off)

EFI_GUID gTestGuid = { 0x12345678, 0x1234, 0x1234, { 0x12, 0x34, 0x56, 0x78, 0x9A, 0xBC, 0xDE, 0xF0 } };


VOID Pause() {
    EFI_INPUT_KEY Key;
    Print(L"\n[Press any key to continue...]\n");
    gBS->WaitForEvent(1, &gST->ConIn->WaitForKey, NULL);
    gST->ConIn->ReadKeyStroke(gST->ConIn, &Key);
}


VOID GetInputString(CHAR16 *Buffer, UINTN MaxLength) {
    EFI_INPUT_KEY Key;
    UINTN i = 0;
    while (i < MaxLength - 1) {
        gBS->WaitForEvent(1, &gST->ConIn->WaitForKey, NULL);
        gST->ConIn->ReadKeyStroke(gST->ConIn, &Key);
        if (Key.UnicodeChar == L'\r') break;
        if (Key.UnicodeChar == L'\b' && i > 0) {
            i--;
            Print(L"\b \b");
            continue;
        }
        if (Key.UnicodeChar >= L' ') {
            Buffer[i++] = Key.UnicodeChar;
            Print(L"%c", Key.UnicodeChar);
        }
    }
    Buffer[i] = L'\0';
    Print(L"\n");
}

// [F1] 列出所有變數
VOID DumpAllVariables() {
    EFI_STATUS Status;
    UINTN MaxBuffer = 1024;
    UINTN NameSize;
    CHAR16 *Name = AllocateZeroPool(MaxBuffer);
    EFI_GUID Guid;

    if (Name == NULL) return;
    Name[0] = L'\0';

    Print(L"\n--- Dumping All Variables ---\n");
    while (TRUE) {
        NameSize = MaxBuffer;
        Status = gRT->GetNextVariableName(&NameSize, Name, &Guid);

        if (Status == EFI_NOT_FOUND) break;
        if (EFI_ERROR(Status)) break;

        Print(L"GUID: %g | Name: %s\n", &Guid, Name);
    }
    FreePool(Name);
    Pause();
}

// [F2/F3/F4] 搜尋變數
VOID SearchVariable(UINTN Mode) {
    EFI_STATUS Status;
    UINTN MaxBuffer = 1024;
    UINTN NameSize;
    CHAR16 *Name = AllocateZeroPool(MaxBuffer);
    CHAR16 SearchName[100] = {0};
    EFI_GUID Guid;
    BOOLEAN Found = FALSE;
    BOOLEAN Match = FALSE;

    if (Name == NULL) return;
    if (Mode == 2) {
        Print(L"Enter Name to Search: ");
        GetInputString(SearchName, 100);
    }

    Name[0] = L'\0';
    while (TRUE) {
        NameSize = MaxBuffer;
        Status = gRT->GetNextVariableName(&NameSize, Name, &Guid);

        if (Status == EFI_NOT_FOUND) break;
        if (EFI_ERROR(Status)) break;

        Match = FALSE;
        if (Mode == 2 && StrCmp(Name, SearchName) == 0) Match = TRUE;
        if (Mode == 4 && CompareGuid(&Guid, &gTestGuid)) Match = TRUE;

        if (Match) {
            Print(L"Found -> GUID: %g | Name: %s\n", &Guid, Name);
            Found = TRUE;
        }
    }
    if (!Found) Print(L"Not found.\n");
    FreePool(Name);
    Pause();
}

// [F5] 建立變數
VOID CreateVariable() {
    EFI_STATUS Status;
    CHAR16 VarName[100];
    CHAR16 VarData[100];

    Print(L"Enter Name: ");
    GetInputString(VarName, 100);
    Print(L"Enter Data: ");
    GetInputString(VarData, 100);

    Status = gRT->SetVariable(
        VarName, &gTestGuid, 
        EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS, 
        (StrLen(VarData) + 1) * sizeof(CHAR16), 
        VarData
    );

    if (!EFI_ERROR(Status)) Print(L"Success!\n");
    else Print(L"Failed: %r\n", Status);
    Pause();
}

// [F6] 刪除變數
VOID DeleteVariable() {
    EFI_STATUS Status;
    CHAR16 VarName[100];

    Print(L"Enter Name to Delete: ");
    GetInputString(VarName, 100);

    Status = gRT->SetVariable(VarName, &gTestGuid, 0, 0, NULL);

    if (!EFI_ERROR(Status)) Print(L"Deleted!\n");
    else Print(L"Failed: %r\n", Status);
    Pause();
}

// 主程式與選單
EFI_STATUS EFIAPI UefiMain(IN EFI_HANDLE ImageHandle, IN EFI_SYSTEM_TABLE *SystemTable) {
    EFI_INPUT_KEY Key;
    while (TRUE) {
        gST->ConOut->ClearScreen(gST->ConOut);
        Print(L"[F1]:Dump All Variable\n");
        Print(L"[F2]:Search Variable from name and Guid\n");
        Print(L"[F3]:Search Variable from name\n");
        Print(L"[F4]:Search Variable from guid\n");
        Print(L"[F5]:Create a new variable\n");
        Print(L"[F6]:Delete a variable\n");
        Print(L"[Esc]:Exit\n");

        gBS->WaitForEvent(1, &gST->ConIn->WaitForKey, NULL);
        gST->ConIn->ReadKeyStroke(gST->ConIn, &Key);

        if (Key.ScanCode == SCAN_ESC) break;
        switch (Key.ScanCode) {
            case SCAN_F1: DumpAllVariables(); break;
            case SCAN_F2: case SCAN_F3: SearchVariable(2); break;
            case SCAN_F4: SearchVariable(4); break;
            case SCAN_F5: CreateVariable(); break;
            case SCAN_F6: DeleteVariable(); break;
        }
    }
    return EFI_SUCCESS;
}