#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/UefiApplicationEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/BaseLib.h>

CHAR16 GetKey (VOID) {
  EFI_STATUS    Status;
  EFI_INPUT_KEY Key;
  UINTN         Index;

  gBS->WaitForEvent (1, &gST->ConIn->WaitForKey, &Index);
  Status = gST->ConIn->ReadKeyStroke (gST->ConIn, &Key);
  if (EFI_ERROR (Status)) {
    return 0;
  }
  return Key.UnicodeChar;
}

VOID ReadString (CHAR16 *Str, UINTN MaxLen) {
  UINTN Index = 0;
  CHAR16 Key;

  while (Index < MaxLen - 1) {
    Key = GetKey();
    if (Key == L'\r' || Key == L'\n') {
      Print(L"\n");
      break;
    }
    
    if (Key == 0x08) { 
      if (Index > 0) {
        Index--;
        Print(L"\b \b");
      }
    } else if (Key >= L' ' && Key <= L'~') {
      Str[Index] = Key;
      Index++;
      Print(L"%c", Key);
    }
  }
  Str[Index] = L'\0';
}

VOID PrintMenu (VOID) {
  Print (L"CPUID & MSR\n\n");
  Print (L"  0. CPUID Function 0.\n");
  Print (L"  1. CPUID Function 1.\n");
  Print (L"  2. CPUID Function 2.\n");
  Print (L"  3. CPUID Function 3.\n");
  Print (L"  4. CPUID Function 4.\n");
  Print (L"  5. CPUID Function 5.\n");
  Print (L"  6. CPUID Function 6.\n");
  Print (L"  7. CPUID Function 7.\n");
  Print (L"  8. CPUID Function 8.\n");
  Print (L"  9. CPUID Function 9.\n");
  Print (L"  A. CPUID Function A.\n");
  Print (L"  B. CPUID Function B.\n");
  Print (L"  C. CPUID Function C.\n");
  Print (L"  D. CPUID Function D.\n");
  Print (L"  E. CPUID Extended Function 0x80000000.\n");
  Print (L"  F. CPUID Extended Function 0x80000001.\n");
  Print (L"  G. CPUID Extended Function 0x80000002.\n");
  Print (L"  H. CPUID Extended Function 0x80000003.\n");
  Print (L"  I. CPUID Extended Function 0x80000004.\n");
  Print (L"  J. CPUID Extended Function 0x80000005.\n");
  Print (L"  K. CPUID Extended Function 0x80000006.\n");
  Print (L"  L. CPUID Extended Function 0x80000007.\n");
  Print (L"  N. CPUID Extended Function 0x80000008.\n");
  Print (L"  M. MSR Read / Write.\n\n");
  Print (L"Press [Q] to quit.\n\n");
  Print (L"-> ");
}

VOID ExecuteFunction0 (VOID) {
  UINT32 Eax = 0, Ebx = 0, Ecx = 0, Edx = 0;
  CHAR8  Vendor[13];
  AsmCpuid (0, &Eax, &Ebx, &Ecx, &Edx);
  *(UINT32*)(&Vendor[0]) = Ebx; *(UINT32*)(&Vendor[4]) = Edx; *(UINT32*)(&Vendor[8]) = Ecx; Vendor[12] = '\0';
  Print (L"\nCPUID Standard Function 0\n\n");
  Print (L"EAX: %08X, EBX: %08X, ECX: %08X, EDX: %08X\n\n", Eax, Ebx, Ecx, Edx);
  Print (L"Largest Standard Function : %08X\n", Eax);
  Print (L"Processor Vendor ID       : %a\n\n", Vendor);
  Print (L"Press Any Key to Continue...\n");
  GetKey ();
}

VOID ExecuteFunction1 (VOID) {
  UINT32 Eax = 0, Ebx = 0, Ecx = 0, Edx = 0;
  UINT32 ExtFamily, BaseFamily, Family, ExtModel, BaseModel, Model, Stepping, Type;
  AsmCpuid (1, &Eax, &Ebx, &Ecx, &Edx);
  ExtFamily  = (Eax >> 20) & 0xFF; BaseFamily = (Eax >> 8) & 0x0F; Family = ExtFamily + BaseFamily;
  ExtModel   = (Eax >> 16) & 0x0F; BaseModel  = (Eax >> 4) & 0x0F; Model  = (ExtModel << 4) + BaseModel;
  Stepping   = Eax & 0x0F; Type = (Eax >> 12) & 0x03;
  Print (L"\nCPUID Standard Function 1- Feature Information\n\n");
  Print (L"EAX: %08X, EBX: %08X, ECX: %08X, EDX: %08X\n\n", Eax, Ebx, Ecx, Edx);
  Print (L"Family   : 0x%X\n", Family); Print (L"Model    : 0x%X\n", Model); Print (L"Stepping : 0x%X\n", Stepping);
  Print (L"Type     : ");
  switch (Type) {
    case 0: Print (L"Original OEM processor\n"); break; case 1: Print (L"Intel OverDrive processor\n"); break;
    case 2: Print (L"Dual processor\n"); break;         case 3: Print (L"Intel reserved\n"); break;
    default: Print (L"Unknown\n"); break;
  }
  Print (L"\nVendor-ID and Largest Standard Function\n\n");
  Print (L"Press Any Key to Continue...\n");
  GetKey ();
}

VOID ExecuteFunction2 (VOID) {
  UINT32 Eax = 0, Ebx = 0, Ecx = 0, Edx = 0;
  AsmCpuid (2, &Eax, &Ebx, &Ecx, &Edx);
  Print (L"\nCPUID Standard Function 2- Cache Descriptors\n\n");
  Print (L"EAX: %08X, EBX: %08X, ECX: %08X, EDX: %08X\n\n", Eax, Ebx, Ecx, Edx);
  Print (L"Press Any Key to Continue...\n");
  GetKey ();
}

VOID ExecuteFunction3 (VOID) {
  UINT32 Eax = 0, Ebx = 0, Ecx = 0, Edx = 0;
  Print (L"\nCPUID Standard Function 3- Processor Serial Number\n\n");
  AsmCpuid (1, &Eax, &Ebx, &Ecx, &Edx);
  if ((Edx & (1 << 18)) != 0) {
    AsmCpuid (3, &Eax, &Ebx, &Ecx, &Edx);
    Print (L"EAX: %08X, EBX: %08X, ECX: %08X, EDX: %08X\n\n", Eax, Ebx, Ecx, Edx);
  } else {
    Print (L"Unsupport!!\n\n");
  }
  Print (L"Press Any Key to Continue...\n");
  GetKey ();
}

VOID ExecuteFunction4 (VOID) {
  UINT32 Eax = 0, Ebx = 0, Ecx = 0, Edx = 0;
  UINT32 CacheIndex = 0, Cores = 0, CacheType = 0;
  Print (L"\nCPUID Standard Function 4- Deterministic Cache Parameters\n\n");
  AsmCpuidEx (4, 0, &Eax, &Ebx, &Ecx, &Edx);
  Print (L"EAX: %08X, EBX: %08X, ECX: %08X, EDX: %08X\n\n", Eax, Ebx, Ecx, Edx);
  Cores = ((Eax >> 26) & 0x3F) + 1;
  Print (L"The number of cores: %d\n", Cores);
  while (TRUE) {
    AsmCpuidEx (4, CacheIndex, &Eax, &Ebx, &Ecx, &Edx);
    CacheType = Eax & 0x1F;
    Print (L"The Cache %d Information:\n", CacheIndex + 1);
    Print (L"EAX: %08X, EBX: %08X, ECX: %08X, EDX: %08X\n\n", Eax, Ebx, Ecx, Edx);
    if (CacheType == 0) {
      Print (L"Cache Type: Null, no more caches\n\n");
      break; 
    }
    CacheIndex++;
  }
  Print (L"Press Any Key to Continue...\n");
  GetKey ();
}

VOID ExecuteFunction5 (VOID) {
  UINT32 Eax = 0, Ebx = 0, Ecx = 0, Edx = 0;
  AsmCpuid (5, &Eax, &Ebx, &Ecx, &Edx);
  Print (L"\nCPUID Standard Function 5- MONITOR / MWAIT Parameters\n\n");
  Print (L"EAX: %08X, EBX: %08X, ECX: %08X, EDX: %08X\n\n", Eax, Ebx, Ecx, Edx);
  Print (L"Smallest monitor line size: %d Byte\n", Eax & 0xFFFF);
  Print (L"Largest monitor line size : %d Byte\n", Ebx & 0xFFFF);
  Print (L"Support for treating interrupts as break-events for MWAIT: %s\n", (Ecx & 0x02) ? L"Yes" : L"No");
  Print (L"MONITOR / MWAIT Extensions supported: %s\n", (Ecx & 0x01) ? L"Yes" : L"No");
  Print (L"Number of C4 sub-states supported using MONITOR / MWAIT: %d\n", (Edx >> 16) & 0x0F);
  Print (L"Number of C3 sub-states supported using MONITOR / MWAIT: %d\n", (Edx >> 12) & 0x0F);
  Print (L"Number of C2 sub-states supported using MONITOR / MWAIT: %d\n", (Edx >> 8) & 0x0F);
  Print (L"Number of C1 sub-states supported using MONITOR / MWAIT: %d\n", (Edx >> 4) & 0x0F);
  Print (L"Number of C0 sub-states supported using MONITOR / MWAIT: %d\n\n", Edx & 0x0F);
  Print (L"Press Any Key to Continue...\n");
  GetKey ();
}

VOID ExecuteFunction6 (VOID) {
  UINT32 Eax = 0, Ebx = 0, Ecx = 0, Edx = 0;
  AsmCpuid (6, &Eax, &Ebx, &Ecx, &Edx);
  Print (L"\nCPUID Standard Function 6- Digital Thermal Sensor & Power Management Parameters\n\n");
  Print (L"EAX: %08X, EBX: %08X, ECX: %08X, EDX: %08X\n\n", Eax, Ebx, Ecx, Edx);
  Print (L"Digital Thermal Sensor Capability: %s\n", (Eax & 0x01) ? L"Yes" : L"No");
  Print (L"Number of Interrupt Thresholds: %02d\n", Ebx & 0x0F);
  Print (L"Hardware Coordination Feedback Capability: %s\n\n", (Ecx & 0x01) ? L"Yes" : L"No");
  Print (L"Press Any Key to Continue...\n");
  GetKey ();
}

VOID ExecuteFunction7 (VOID) {
  UINT32 Eax = 0, Ebx = 0, Ecx = 0, Edx = 0;
  AsmCpuidEx (7, 0, &Eax, &Ebx, &Ecx, &Edx);
  Print (L"\nCPUID Standard Function 7- Structured Extended Feature Flags Enumeration\n\n");
  Print (L"EAX: %08X, EBX: %08X, ECX: %08X, EDX: %08X\n\n", Eax, Ebx, Ecx, Edx);
  if (Eax == 0) Print (L"No sub-leaves are supported\n");
  Print (L"Press Any Key to Continue...\n");
  GetKey ();
}

VOID ExecuteFunction8 (VOID) {
  UINT32 Eax = 0, Ebx = 0, Ecx = 0, Edx = 0;
  AsmCpuid (8, &Eax, &Ebx, &Ecx, &Edx);
  Print (L"\nCPUID Standard Function 8- Reserved\n\n");
  Print (L"EAX: %08X, EBX: %08X, ECX: %08X, EDX: %08X\n\n", Eax, Ebx, Ecx, Edx);
  Print (L"Press Any Key to Continue...\n");
  GetKey ();
}

VOID ExecuteFunction9 (VOID) {
  UINT32 Eax = 0, Ebx = 0, Ecx = 0, Edx = 0;
  Print (L"\nCPUID Standard Function 9- Direct Cache Access (DCA) Parameters\n\n");
  AsmCpuid (1, &Eax, &Ebx, &Ecx, &Edx);
  if ((Ecx & (1 << 18)) != 0) {
    AsmCpuid (9, &Eax, &Ebx, &Ecx, &Edx);
    Print (L"EAX: %08X, EBX: %08X, ECX: %08X, EDX: %08X\n\n", Eax, Ebx, Ecx, Edx);
  } else {
    Print (L"Unsupport!!\n\n");
  }
  Print (L"Press Any Key to Continue...\n");
  GetKey ();
}

VOID ExecuteFunctionA (VOID) {
  UINT32 Eax = 0, Ebx = 0, Ecx = 0, Edx = 0;
  AsmCpuid (0xA, &Eax, &Ebx, &Ecx, &Edx);
  Print (L"\nCPUID Standard Function A- Architectural Performance Monitor Features\n\n");
  Print (L"EAX: %08X, EBX: %08X, ECX: %08X, EDX: %08X\n\n", Eax, Ebx, Ecx, Edx);
  Print (L"Press Any Key to Continue...\n");
  GetKey ();
}

VOID ExecuteFunctionB (VOID) {
  UINT32 Eax = 0, Ebx = 0, Ecx = 0, Edx = 0, Level = 0, Type = 0;
  Print (L"\nCPUID Standard Function B- x2APIC Features / Processor Topology\n\n");
  while (TRUE) {
    AsmCpuidEx (0xB, Level, &Eax, &Ebx, &Ecx, &Edx);
    Type = (Ecx >> 8) & 0xFF;
    if (Type == 0) break; 
    else if (Type == 1) Print (L"Thread Level Processor Topology\n");
    else if (Type == 2) Print (L"Core Level Processor Topology\n");
    
    Print (L"EAX: %08X, EBX: %08X, ECX: %08X, EDX: %08X\n\n", Eax, Ebx, Ecx, Edx);
    Level++;
  }
  Print (L"Press Any Key to Continue...\n");
  GetKey ();
}

VOID ExecuteFunctionC (VOID) {
  UINT32 Eax = 0, Ebx = 0, Ecx = 0, Edx = 0;
  AsmCpuid (0xC, &Eax, &Ebx, &Ecx, &Edx);
  Print (L"\nCPUID Standard Function C- Reserved\n\n");
  Print (L"EAX: %08X, EBX: %08X, ECX: %08X, EDX: %08X\n\n", Eax, Ebx, Ecx, Edx);
  Print (L"Press Any Key to Continue...\n");
  GetKey ();
}

VOID ExecuteFunctionD (VOID) {
  UINT32 Eax = 0, Ebx = 0, Ecx = 0, Edx = 0;
  AsmCpuid (0xD, &Eax, &Ebx, &Ecx, &Edx);
  Print (L"\nCPUID Standard Function D- XSAVE Features\n\n");
  Print (L"EAX: %08X, EBX: %08X, ECX: %08X, EDX: %08X\n\n", Eax, Ebx, Ecx, Edx);
  Print (L"Press Any Key to Continue...\n");
  GetKey ();
}

VOID ExecuteFunctionE (VOID) {
  UINT32 Eax = 0, Ebx = 0, Ecx = 0, Edx = 0;
  AsmCpuid (0x80000000, &Eax, &Ebx, &Ecx, &Edx);
  Print (L"\nCPUID Extended Function 0x80000000- Largest Extended Function\n\n");
  Print (L"EAX: %08X, EBX: %08X, ECX: %08X, EDX: %08X\n\n", Eax, Ebx, Ecx, Edx);
  Print (L"Largest Extended Function: %08X\n\n", Eax);
  Print (L"Press Any Key to Continue...\n");
  GetKey ();
}

VOID ExecuteFunctionF (VOID) {
  UINT32 Eax = 0, Ebx = 0, Ecx = 0, Edx = 0;
  AsmCpuid (0x80000001, &Eax, &Ebx, &Ecx, &Edx);
  Print (L"\nCPUID Extended Function 0x80000001- Extended Feature Bits\n\n");
  Print (L"EAX: %08X, EBX: %08X, ECX: %08X, EDX: %08X\n\n", Eax, Ebx, Ecx, Edx);
  Print (L"Press Any Key to Continue...\n");
  GetKey ();
}

VOID ExecuteFunctionG (VOID) {
  UINT32 Eax = 0, Ebx = 0, Ecx = 0, Edx = 0;
  CHAR8  AsciiStr[17]; 
  AsmCpuid (0x80000002, &Eax, &Ebx, &Ecx, &Edx);
  *(UINT32*)(&AsciiStr[0]) = Eax; *(UINT32*)(&AsciiStr[4]) = Ebx; *(UINT32*)(&AsciiStr[8]) = Ecx; *(UINT32*)(&AsciiStr[12]) = Edx; AsciiStr[16] = '\0';
  Print (L"\nCPUID Extended Function 0x80000002- Processor Name / Brand String\n\n");
  Print (L"EAX: %08X, EBX: %08X, ECX: %08X, EDX: %08X\n\n", Eax, Ebx, Ecx, Edx);
  Print (L"Processor Name Part 1: \"%a\"\n\n", AsciiStr);
  Print (L"Press Any Key to Continue...\n");
  GetKey ();
}

VOID ExecuteFunctionH (VOID) {
  UINT32 Eax = 0, Ebx = 0, Ecx = 0, Edx = 0;
  CHAR8  AsciiStr[17]; 
  AsmCpuid (0x80000003, &Eax, &Ebx, &Ecx, &Edx);
  *(UINT32*)(&AsciiStr[0]) = Eax; *(UINT32*)(&AsciiStr[4]) = Ebx; *(UINT32*)(&AsciiStr[8]) = Ecx; *(UINT32*)(&AsciiStr[12]) = Edx; AsciiStr[16] = '\0';
  Print (L"\nCPUID Extended Function 0x80000003- Processor Name / Brand String\n\n");
  Print (L"EAX: %08X, EBX: %08X, ECX: %08X, EDX: %08X\n\n", Eax, Ebx, Ecx, Edx);
  Print (L"Processor Name Part 2: \"%a\"\n\n", AsciiStr);
  Print (L"Press Any Key to Continue...\n");
  GetKey ();
}

VOID ExecuteFunctionI (VOID) {
  UINT32 Eax = 0, Ebx = 0, Ecx = 0, Edx = 0;
  CHAR8  PartStr[17]; 
  CHAR8  FullName[49];
  
  AsmCpuid (0x80000004, &Eax, &Ebx, &Ecx, &Edx);
  *(UINT32*)(&PartStr[0]) = Eax; *(UINT32*)(&PartStr[4]) = Ebx; *(UINT32*)(&PartStr[8]) = Ecx; *(UINT32*)(&PartStr[12]) = Edx; PartStr[16] = '\0';
  Print (L"\nCPUID Extended Function 0x80000004- Processor Name / Brand String\n\n");
  Print (L"EAX: %08X, EBX: %08X, ECX: %08X, EDX: %08X\n\n", Eax, Ebx, Ecx, Edx);
  Print (L"Processor Name Part 3: \"%a\"\n\n", PartStr);

  AsmCpuid (0x80000002, &Eax, &Ebx, &Ecx, &Edx);
  *(UINT32*)(&FullName[0]) = Eax; *(UINT32*)(&FullName[4]) = Ebx; *(UINT32*)(&FullName[8]) = Ecx; *(UINT32*)(&FullName[12]) = Edx;
  AsmCpuid (0x80000003, &Eax, &Ebx, &Ecx, &Edx);
  *(UINT32*)(&FullName[16]) = Eax; *(UINT32*)(&FullName[20]) = Ebx; *(UINT32*)(&FullName[24]) = Ecx; *(UINT32*)(&FullName[28]) = Edx;
  AsmCpuid (0x80000004, &Eax, &Ebx, &Ecx, &Edx);
  *(UINT32*)(&FullName[32]) = Eax; *(UINT32*)(&FullName[36]) = Ebx; *(UINT32*)(&FullName[40]) = Ecx; *(UINT32*)(&FullName[44]) = Edx;
  FullName[48] = '\0';
  
  Print (L"Processor Name: \"%a\"\n\n", FullName);
  Print (L"Press Any Key to Continue...\n");
  GetKey ();
}

VOID ExecuteFunctionJ (VOID) {
  UINT32 Eax = 0, Ebx = 0, Ecx = 0, Edx = 0;
  AsmCpuid (0x80000005, &Eax, &Ebx, &Ecx, &Edx);
  Print (L"\nCPUID Extended Function 0x80000005- Reserved\n\n");
  Print (L"EAX: %08X, EBX: %08X, ECX: %08X, EDX: %08X\n\n", Eax, Ebx, Ecx, Edx);
  Print (L"Press Any Key to Continue...\n");
  GetKey ();
}

VOID ExecuteFunctionK (VOID) {
  UINT32 Eax = 0, Ebx = 0, Ecx = 0, Edx = 0;
  UINT32 AssocType;
  
  AsmCpuid (0x80000006, &Eax, &Ebx, &Ecx, &Edx);
  Print (L"\nCPUID Extended Function 0x80000006- Extended L2 Cache Features\n\n");
  Print (L"EAX: %08X, EBX: %08X, ECX: %08X, EDX: %08X\n\n", Eax, Ebx, Ecx, Edx);
  
  AssocType = (Ecx >> 12) & 0x0F;
  Print (L"L2 Cache Size: %d KB\n", (Ecx >> 16) & 0xFFFF);
  Print (L"Associativity: ");
  switch(AssocType) {
    case 0x00: Print(L"Disabled\n"); break;
    case 0x01: Print(L"1-Way\n"); break;
    case 0x02: Print(L"2-Way\n"); break;
    case 0x04: Print(L"4-Way\n"); break;
    case 0x06: Print(L"8-Way\n"); break;
    case 0x08: Print(L"16-Way\n"); break;
    case 0x0A: Print(L"32-Way\n"); break;
    case 0x0B: Print(L"48-Way\n"); break;
    case 0x0C: Print(L"64-Way\n"); break;
    case 0x0D: Print(L"96-Way\n"); break;
    case 0x0E: Print(L"128-Way\n"); break;
    case 0x0F: Print(L"Fully associative\n"); break;
    default:   Print(L"Unknown\n"); break;
  }
  Print (L"L2 Cache Line Size: %d Byte\n\n", Ecx & 0xFF);
  Print (L"Press Any Key to Continue...\n");
  GetKey ();
}

VOID ExecuteFunctionL (VOID) {
  UINT32 Eax = 0, Ebx = 0, Ecx = 0, Edx = 0;
  AsmCpuid (0x80000007, &Eax, &Ebx, &Ecx, &Edx);
  Print (L"\nCPUID Extended Function 0x80000007- Reserved\n\n");
  Print (L"EAX: %08X, EBX: %08X, ECX: %08X, EDX: %08X\n\n", Eax, Ebx, Ecx, Edx);
  Print (L"Press Any Key to Continue...\n");
  GetKey ();
}

VOID ExecuteFunctionN (VOID) {
  UINT32 Eax = 0, Ebx = 0, Ecx = 0, Edx = 0;
  AsmCpuid (0x80000008, &Eax, &Ebx, &Ecx, &Edx);
  Print (L"\nCPUID Extended Function 0x80000008- Virtual and Physical Address Sizes\n\n");
  Print (L"EAX: %08X, EBX: %08X, ECX: %08X, EDX: %08X\n\n", Eax, Ebx, Ecx, Edx);
  Print (L"Virtual Address Size: %d bits\n", (Eax >> 8) & 0xFF);
  Print (L"Physical Address Size: %d bits\n\n", Eax & 0xFF);
  Print (L"Press Any Key to Continue...\n");
  GetKey ();
}

VOID ExecuteMsrMenu (VOID) {
  CHAR16 Choice;
  CHAR16 InputStr[20];
  UINT64 MsrAddr, MsrValue;

  while (TRUE) {
    gST->ConOut->ClearScreen (gST->ConOut);
    Print (L"Model Specific Registers (MSR) Read / Write\n\n");
    Print (L"Press [R] to read MSR.\n");
    Print (L"Press [W] to write MSR.\n");
    Print (L"Press [Q] to return.\n");
    Print (L"-> ");
    
    Choice = GetKey ();
    Print (L"%c\n", Choice);

    if (Choice == L'q' || Choice == L'Q') break;
    else if (Choice == L'r' || Choice == L'R') {
      Print (L"Please input the MSR address: 0x");
      ReadString (InputStr, 20);
      MsrAddr = StrHexToUint64 (InputStr);
      MsrValue = AsmReadMsr64 ((UINT32)MsrAddr);
      Print (L"MSR[0x%x] Value: 0x%016lx\n\n", (UINT32)MsrAddr, MsrValue);
      Print (L"Press Any Key to Continue...\n");
      GetKey ();
    } else if (Choice == L'w' || Choice == L'W') {
      Print (L"Please input the MSR address: 0x");
      ReadString (InputStr, 20);
      MsrAddr = StrHexToUint64 (InputStr);
      Print (L"Please input the MSR Value: 0x");
      ReadString (InputStr, 20);
      MsrValue = StrHexToUint64 (InputStr);
      AsmWriteMsr64 ((UINT32)MsrAddr, MsrValue);
      Print (L"Write Success!\n\n");
      Print (L"Press Any Key to Continue...\n");
      GetKey ();
    } else {
      Print (L"Invalid Input!\nPress Any Key to Continue...\n");
      GetKey ();
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
  CHAR16 Choice;

  while (TRUE) {
    gST->ConOut->ClearScreen (gST->ConOut);
    PrintMenu ();
    Choice = GetKey ();
    Print (L"%c\n", Choice); 

    if (Choice == L'q' || Choice == L'Q') {
      break;
    } else if (Choice == L'0') {
      ExecuteFunction0 ();
    } else if (Choice == L'1') {
      ExecuteFunction1 ();
    } else if (Choice == L'2') {
      ExecuteFunction2 ();
    } else if (Choice == L'3') {
      ExecuteFunction3 ();
    } else if (Choice == L'4') {
      ExecuteFunction4 ();
    } else if (Choice == L'5') {
      ExecuteFunction5 ();
    } else if (Choice == L'6') {
      ExecuteFunction6 ();
    } else if (Choice == L'7') {
      ExecuteFunction7 ();
    } else if (Choice == L'8') {
      ExecuteFunction8 ();
    } else if (Choice == L'9') {
      ExecuteFunction9 ();
    } else if (Choice == L'a' || Choice == L'A') {
      ExecuteFunctionA ();
    } else if (Choice == L'b' || Choice == L'B') {
      ExecuteFunctionB ();
    } else if (Choice == L'c' || Choice == L'C') {
      ExecuteFunctionC ();
    } else if (Choice == L'd' || Choice == L'D') {
      ExecuteFunctionD ();
    } else if (Choice == L'e' || Choice == L'E') {
      ExecuteFunctionE ();
    } else if (Choice == L'f' || Choice == L'F') {
      ExecuteFunctionF ();
    } else if (Choice == L'g' || Choice == L'G') {
      ExecuteFunctionG ();
    } else if (Choice == L'h' || Choice == L'H') {
      ExecuteFunctionH ();
    } else if (Choice == L'i' || Choice == L'I') {
      ExecuteFunctionI ();
    } else if (Choice == L'j' || Choice == L'J') {
      ExecuteFunctionJ ();
    } else if (Choice == L'k' || Choice == L'K') {
      ExecuteFunctionK ();
    } else if (Choice == L'l' || Choice == L'L') {
      ExecuteFunctionL ();
    } else if (Choice == L'n' || Choice == L'N') {
      ExecuteFunctionN ();
    } else if (Choice == L'M' || Choice == L'm') {
      ExecuteMsrMenu ();
    } else {
      Print (L"\nInvalid Input!\nPress Any Key to Continue...\n");
      GetKey ();
    }
  }

  return EFI_SUCCESS;
}