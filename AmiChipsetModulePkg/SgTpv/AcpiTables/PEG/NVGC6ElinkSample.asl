//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**        5555 Oakbrook Pkwy, Suite 200, Norcross, GA 30093         **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

//<AMI_PHDR_START>
//------------------------------------------------------------------------
//
// Procedure:    G6EL
//
// Description:  Sample code for OEM to modify for their requirement when enter/exit GC6I/GC6O, 
//               and replace the path in the TOKEN "GC6_ELINK_METHOD_PATH"      
//               
// Input:
// Arg0: 0: Enter GC6I
//       1: Exit GC6I
//       2: Enter GC6O
//       3: Exit GC6O
// Output:
//  None 
//<AMI_PHDR_END>
Method(G6EL,1)
{
  If (LEqual(Arg0,0)){
  // Enter GC6I
  // Porting by OEM.  
  }
  ElseIf (LEqual(Arg0,1)){
  // Exit GC6I
  // Porting by OEM.
  } 
  ElseIf (LEqual(Arg0,2)){
  // Enter GC6O
  // Porting by OEM.
  }  
  ElseIf (LEqual(Arg0,3)){
  // Exit GC6O
  // Porting by OEM.
  }                       
}
//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**        5555 Oakbrook Pkwy, Suite 200, Norcross, GA 30093         **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
//**********************************************************************