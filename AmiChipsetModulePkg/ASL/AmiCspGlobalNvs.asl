//*************************************************************************
//**                                                                     **
//**   Copyright (c) 1985-2019, American Megatrends International LLC.   **
//**                                                                     **
//**      All rights reserved. Subject to AMI licensing agreement.       **
//**                                                                     **
//*************************************************************************
/** @file AmiCspGlobalNvs.asl
   
  This file contains ACPI methods implementation for AMI CSP Global NVS.
  
*/

Name (CNVB, 0xFFFF0000)  // AMI CSP Global NVS Base address
Name (CNVL, 0xAA55)      // AMI CSP Global NVS Length

OperationRegion(CPNV, SystemMemory, CNVB, CNVL)
Field(CPNV, AnyAcc, Lock, Preserve)
{  
    Offset(0),    NTPB, 8,  // Offset(0),   NotifyPowerBtn. Determine if notify power button in Sx Transition.
}

