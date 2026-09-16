#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/UefiApplicationEntryPoint.h>

#pragma pack(push, 1)

typedef struct {
    UINT32 Signature;               // "$PIR" (4 Bytes)
    UINT8  MinorVersion;            // Minor Version (1 Byte)
    UINT8  MajorVersion;            // Major Version (1 Byte)
    UINT16 TableSize;               // Table Size (2 Bytes)
    UINT8  RouterBus;               // PCI Interrupt Router's Bus (1 Byte)
    UINT8  RouterDevFunc;           // PCI Interrupt Router's DevFunc (1 Byte)
    UINT16 ExclusiveIRQs;           // PCI Exclusive IRQs (2 Bytes)
    UINT16 CompatibleVendorID;      // Compatible PCI Interrupt Router Vendor ID (2 Bytes)
    UINT16 CompatibleDeviceID;      // Compatible PCI Interrupt Router Device ID (2 Bytes)
    UINT32 MiniportData;            // Miniport Data (4 Bytes)
    UINT8  Reserved[11];            // Reserved (11 Bytes)
    UINT8  Checksum;                // Checksum (1 Byte)
} PIRQ_TABLE_HEADER;

typedef struct {
    UINT8  PciBus;                  // PCI Bus Number (1 Byte)
    UINT8  PciDevice;               // PCI Device Number (in upper five bits) (1 Byte)
    UINT8  LinkINTA;                // Link Value for INTA# (1 Byte)
    UINT16 BitmapINTA;              // IRQ Bitmap for INTA# (2 Bytes)
    UINT8  LinkINTB;                // Link Value for INTB# (1 Byte)
    UINT16 BitmapINTB;              // IRQ Bitmap for INTB# (2 Bytes)
    UINT8  LinkINTC;                // Link Value for INTC# (1 Byte)
    UINT16 BitmapINTC;              // IRQ Bitmap for INTC# (2 Bytes)
    UINT8  LinkINTD;                // Link Value for INTD# (1 Byte)
    UINT16 BitmapINTD;              // IRQ Bitmap for INTD# (2 Bytes)
    UINT8  SlotNumber;              // Slot Number (1 Byte)
    UINT8  Reserved;                // Reserved (1 Byte)
} PIRQ_SLOT_ENTRY;

#pragma pack(pop)

PIRQ_TABLE_HEADER* FindPirqTable (VOID) {
    UINT8 *Addr;
    UINT8 Checksum;
    UINT16 Size;
    UINT16 i;
    PIRQ_TABLE_HEADER *Header;
    
    for (Addr = (UINT8*)(UINTN)0xF0000; Addr < (UINT8*)(UINTN)0xFFFFF; Addr += 16) {
        if (*(UINT32*)Addr == 0x52495024) {
            Header = (PIRQ_TABLE_HEADER*)Addr;
            Size = Header->TableSize;
            if (Size < sizeof(PIRQ_TABLE_HEADER) || Size > 4096) {
                continue;
            }
            Checksum = 0;
            for (i = 0; i < Size; i++) {
                Checksum += Addr[i];
            }
            if (Checksum == 0) {
                return Header; 
            }
        }
    }
    return NULL; 
}

VOID WaitForKey (EFI_SYSTEM_TABLE *SystemTable) {
    UINTN      Index;
    EFI_INPUT_KEY Key;
    SystemTable->BootServices->WaitForEvent(1, &SystemTable->ConIn->WaitForKey, &Index);
    SystemTable->ConIn->ReadKeyStroke(SystemTable->ConIn, &Key);
}

VOID DumpTableHeader (PIRQ_TABLE_HEADER *Header, EFI_SYSTEM_TABLE *SystemTable) {
    PIRQ_TABLE_HEADER Dummy = {0};
    CHAR16 SigStr[5] = {0};
    UINT8 i;

    if (Header == NULL) {
        Header = &Dummy;
    }

    SystemTable->ConOut->ClearScreen(SystemTable->ConOut);
    Print(L"\nPIRQ Routing Table Header\n\n");
    
    SigStr[0] = (CHAR16)(Header->Signature & 0xFF);
    SigStr[1] = (CHAR16)((Header->Signature >> 8) & 0xFF);
    SigStr[2] = (CHAR16)((Header->Signature >> 16) & 0xFF);
    SigStr[3] = (CHAR16)((Header->Signature >> 24) & 0xFF);
    
    Print(L"Signature                               = %s\n", SigStr[0] ? SigStr : L"");
    Print(L"Minor Version                           = 0x%02X\n", Header->MinorVersion);
    Print(L"Major Version                           = 0x%02X\n", Header->MajorVersion);
    Print(L"Table Size                              = %d Byte\n", Header->TableSize);
    Print(L"PCI Interrupt Router's Bus              = 0x%02X\n", Header->RouterBus);
    Print(L"PCI Interrupt Router's Device Number    = 0x%02X\n", (Header->RouterDevFunc >> 3) & 0x1F);
    Print(L"PCI Interrupt Router's Function Number  = 0x%02X\n", Header->RouterDevFunc & 0x07);
    Print(L"PCI Exclusive IRQs                      = 0x%04X\n", Header->ExclusiveIRQs);
    Print(L"Compatible PCI Interrupt Router Vendor ID= 0x%04X\n", Header->CompatibleVendorID);
    Print(L"Compatible PCI Interrupt Router Device ID= 0x%04X\n", Header->CompatibleDeviceID);
    Print(L"Miniport Data                           = 0x%08X\n", Header->MiniportData);
    Print(L"Reserved                                = ");
    for (i = 0; i < 11; i++) {
        Print(L"%02X ", Header->Reserved[i]);
    }
    Print(L"\n");
    Print(L"Checksum                                = 0x%02X\n\n", Header->Checksum);
    Print(L"Press any key to continuous..\n");
    WaitForKey(SystemTable);
}

VOID DumpSlotRouting (PIRQ_TABLE_HEADER *Header, EFI_SYSTEM_TABLE *SystemTable) {
    EFI_STATUS Status;
    UINTN      Index;
    EFI_INPUT_KEY Key;
    INTN       SlotCount = 0;
    INTN       CurrentSlot = 0;
    PIRQ_SLOT_ENTRY *Slots = NULL;
    PIRQ_SLOT_ENTRY DummySlot = {0};
    PIRQ_SLOT_ENTRY *Current;
    
    if (Header != NULL && Header->TableSize > sizeof(PIRQ_TABLE_HEADER)) {
        SlotCount = (Header->TableSize - sizeof(PIRQ_TABLE_HEADER)) / sizeof(PIRQ_SLOT_ENTRY);
        Slots = (PIRQ_SLOT_ENTRY*)((UINT8*)Header + sizeof(PIRQ_TABLE_HEADER));
    }

    while (TRUE) {
        SystemTable->ConOut->ClearScreen(SystemTable->ConOut);
        
        Current = &DummySlot;
        if (Slots != NULL && SlotCount > 0) {
            Current = &Slots[CurrentSlot];
        }

        Print(L"\n%02dth Slot IRQ Routing\n", CurrentSlot + 1);
        Print(L"PCI Bus Number         = 0x%02X\n", Current->PciBus);
        Print(L"PCI Device Number      = 0x%02X\n", (Current->PciDevice >> 3) & 0x1F);
        Print(L"Link Value for INTA#   = 0x%02X\n", Current->LinkINTA);
        Print(L"IRQ Bitmap for INTA#   = 0x%04X\n", Current->BitmapINTA);
        Print(L"Link Value for INTB#   = 0x%02X\n", Current->LinkINTB);
        Print(L"IRQ Bitmap for INTB#   = 0x%04X\n", Current->BitmapINTB);
        Print(L"Link Value for INTC#   = 0x%02X\n", Current->LinkINTC);
        Print(L"IRQ Bitmap for INTC#   = 0x%04X\n", Current->BitmapINTC);
        Print(L"Link Value for INTD#   = 0x%02X\n", Current->LinkINTD);
        Print(L"IRQ Bitmap for INTD#   = 0x%04X\n", Current->BitmapINTD);
        Print(L"Slot Number            = 0x%02X\n", Current->SlotNumber);
        Print(L"Reserved               = 0x%02X\n\n", Current->Reserved);
        Print(L"Press [Right][Left] to control slot page...\n");
        Print(L"Press [ESC] to quit...\n");

        SystemTable->BootServices->WaitForEvent(1, &SystemTable->ConIn->WaitForKey, &Index);
        Status = SystemTable->ConIn->ReadKeyStroke(SystemTable->ConIn, &Key);
        
        if (!EFI_ERROR(Status)) {
            if (Key.ScanCode == SCAN_ESC) {
                break;
            }
            if (Key.ScanCode == SCAN_RIGHT) {
                if (SlotCount > 0 && CurrentSlot < SlotCount - 1) {
                    CurrentSlot++;
                }
            }
            if (Key.ScanCode == SCAN_LEFT) {
                if (CurrentSlot > 0) {
                    CurrentSlot--;
                }
            }
        }
    }
}

EFI_STATUS
EFIAPI
UefiMain (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
    EFI_STATUS Status;
    UINTN      Index;
    EFI_INPUT_KEY Key;
    PIRQ_TABLE_HEADER *PirqTable;
    PirqTable = FindPirqTable();

    while (TRUE) {
        SystemTable->ConOut->ClearScreen(SystemTable->ConOut);
        
        Print(L"PIRQ Routing Table\n\n");
        Print(L"1. Get PIRQ Routing Table Header.\n");
        Print(L"2. Dump Slot IRQ Routing.\n\n");
        Print(L"Press [ESC] to Quit.\n-> ");

        SystemTable->BootServices->WaitForEvent(1, &SystemTable->ConIn->WaitForKey, &Index);
        Status = SystemTable->ConIn->ReadKeyStroke(SystemTable->ConIn, &Key);

        if (!EFI_ERROR(Status)) {
            if (Key.ScanCode == SCAN_ESC) {
                break;
            }
            if (Key.UnicodeChar == L'1') {
                DumpTableHeader(PirqTable, SystemTable);
            }
            if (Key.UnicodeChar == L'2') {
                DumpSlotRouting(PirqTable, SystemTable);
            }
        }
    }
    return EFI_SUCCESS;
}