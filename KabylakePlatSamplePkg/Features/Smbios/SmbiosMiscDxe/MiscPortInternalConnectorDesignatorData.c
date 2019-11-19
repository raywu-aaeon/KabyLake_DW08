/** @file
  Type 8: Port Connector Information.
  This driver will determine the default value settting for SMBIOS_TABLE_TYPE8.

  @par Revision Reference:
  SMBIOS Specification version 2.8.0 from DMTF: http://www.dmtf.org/standards/smbios
  Intel Framework Specifications, all available at: http://www.intel.com/technology/framework/spec.htm
   - SMBUS Host Controller Protocol Specification
  Unified Extensible Firmware Interface (UEFI) Specifications: http://www.uefi.org/specs/

 @copyright
  INTEL CONFIDENTIAL
  Copyright 2013 - 2016 Intel Corporation.

  The source code contained or described herein and all documents related to the
  source code ("Material") are owned by Intel Corporation or its suppliers or
  licensors. Title to the Material remains with Intel Corporation or its suppliers
  and licensors. The Material may contain trade secrets and proprietary and
  confidential information of Intel Corporation and its suppliers and licensors,
  and is protected by worldwide copyright and trade secret laws and treaty
  provisions. No part of the Material may be used, copied, reproduced, modified,
  published, uploaded, posted, transmitted, distributed, or disclosed in any way
  without Intel's prior express written permission.

  No license under any patent, copyright, trade secret or other intellectual
  property right is granted to or conferred upon you by disclosure or delivery
  of the Materials, either expressly, by implication, inducement, estoppel or
  otherwise. Any license under such intellectual property rights must be
  express and approved by Intel in writing.

  Unless otherwise agreed by Intel in writing, you may not remove or alter
  this notice or any other notice embedded in Materials by Intel or
  Intel's suppliers or licensors in any way.

  This file contains a 'Sample Driver' and is licensed as such under the terms
  of your license agreement with Intel or your vendor. This file may be modified
  by the user, subject to the additional terms of the license agreement.

@par Specification Reference:
**/

#include "CommonHeader.h"
#include "SmbiosMisc.h"

//
// Static (possibly build generated) Bios Vendor data.
//
GLOBAL_REMOVE_IF_UNREFERENCED MISC_SMBIOS_TABLE_DATA(SMBIOS_TABLE_TYPE8, MiscPortKeyboard) = {
  {                                               // Hdr
    EFI_SMBIOS_TYPE_PORT_CONNECTOR_INFORMATION,   ///< Hdr.Type
    sizeof(SMBIOS_TABLE_TYPE8),                   ///< Hdr.Length
    0                                             ///< Hdr.Handle
  },
  SMBIOS_MISC_PORT_TOKEN_KEYBOARD,                ///< InternalConnectorDesignator
  PortConnectorTypeNone,                          ///< InternalConnectorType
  SMBIOS_MISC_STRING_2,                           ///< ExternalConnectorDesignator
  PortConnectorTypePS2,                           ///< ExternalConnectorType
  PortTypeKeyboard                                ///< PortType
};

GLOBAL_REMOVE_IF_UNREFERENCED MISC_SMBIOS_TABLE_DATA(SMBIOS_TABLE_TYPE8, MiscPortMouse) = {
  {                                               // Hdr
    EFI_SMBIOS_TYPE_PORT_CONNECTOR_INFORMATION,   ///< Hdr.Type
    sizeof(SMBIOS_TABLE_TYPE8),                   ///< Hdr.Length
    0                                             ///< Hdr.Handle
  },
  SMBIOS_MISC_PORT_TOKEN_MOUSE,                   ///< InternalConnectorDesignator
  PortConnectorTypeNone,                          ///< InternalConnectorType
  SMBIOS_MISC_STRING_2,                           ///< ExternalConnectorDesignator
  PortConnectorTypePS2,                           ///< ExternalConnectorType
  PortTypeMouse                                   ///< PortType
};

GLOBAL_REMOVE_IF_UNREFERENCED MISC_SMBIOS_TABLE_DATA(SMBIOS_TABLE_TYPE8, MiscPortCom1) = {
  {                                               // Hdr
    EFI_SMBIOS_TYPE_PORT_CONNECTOR_INFORMATION,   ///< Hdr.Type
    sizeof(SMBIOS_TABLE_TYPE8),                   ///< Hdr.Length
    0                                             ///< Hdr.Handle
  },
  SMBIOS_MISC_PORT_TOKEN_COM1,                    ///< InternalConnectorDesignator
  PortConnectorTypeOther,                         ///< InternalConnectorType
  SMBIOS_MISC_STRING_2,                           ///< ExternalConnectorDesignator
  PortConnectorTypeNone,                          ///< ExternalConnectorType
  PortTypeSerial16550ACompatible                  ///< PortType
};

GLOBAL_REMOVE_IF_UNREFERENCED MISC_SMBIOS_TABLE_DATA(SMBIOS_TABLE_TYPE8, MiscPortVideo) = {
  {                                               // Hdr
    EFI_SMBIOS_TYPE_PORT_CONNECTOR_INFORMATION,   ///< Hdr.Type
    sizeof(SMBIOS_TABLE_TYPE8),                   ///< Hdr.Length
    0                                             ///< Hdr.Handle
  },
  SMBIOS_MISC_PORT_TOKEN_VIDEO,                   ///< InternalConnectorDesignator
  PortConnectorTypeOther,                         ///< InternalConnectorType
  SMBIOS_MISC_STRING_2,                           ///< ExternalConnectorDesignator
  PortConnectorTypeDB15Female,                    ///< ExternalConnectorType
  PortTypeVideoPort                               ///< PortType
};

GLOBAL_REMOVE_IF_UNREFERENCED MISC_SMBIOS_TABLE_DATA(SMBIOS_TABLE_TYPE8, MiscPortHDMI) = {
  {                                               // Hdr
    EFI_SMBIOS_TYPE_PORT_CONNECTOR_INFORMATION,   ///< Hdr.Type
    sizeof(SMBIOS_TABLE_TYPE8),                   ///< Hdr.Length
    0                                             ///< Hdr.Handle
  },
  SMBIOS_MISC_PORT_TOKEN_HDMI,                    ///< InternalConnectorDesignator
  PortConnectorTypeOther,                         ///< InternalConnectorType
  SMBIOS_MISC_STRING_2,                           ///< ExternalConnectorDesignator
  PortConnectorTypeNone,                          ///< ExternalConnectorType
  PortTypeVideoPort                               ///< PortType
};

GLOBAL_REMOVE_IF_UNREFERENCED MISC_SMBIOS_TABLE_DATA(SMBIOS_TABLE_TYPE8, MiscPortUsb1) = {
  {                                               // Hdr
    EFI_SMBIOS_TYPE_PORT_CONNECTOR_INFORMATION,   ///< Hdr.Type
    sizeof(SMBIOS_TABLE_TYPE8),                   ///< Hdr.Length
    0                                             ///< Hdr.Handle
  },
  SMBIOS_MISC_PORT_TOKEN_USB1,                    ///< InternalConnectorDesignator
  PortConnectorTypeNone,                          ///< InternalConnectorType
  SMBIOS_MISC_STRING_2,                           ///< ExternalConnectorDesignator
  PortConnectorTypeUsb,                           ///< ExternalConnectorType
  PortTypeUsb                                     ///< PortType
};

GLOBAL_REMOVE_IF_UNREFERENCED MISC_SMBIOS_TABLE_DATA(SMBIOS_TABLE_TYPE8, MiscPortUsb2) = {
  {                                               // Hdr
    EFI_SMBIOS_TYPE_PORT_CONNECTOR_INFORMATION,   ///< Hdr.Type
    sizeof(SMBIOS_TABLE_TYPE8),                   ///< Hdr.Length
    0                                             ///< Hdr.Handle
  },
  SMBIOS_MISC_PORT_TOKEN_USB2,                    ///< InternalConnectorDesignator
  PortConnectorTypeNone,                          ///< InternalConnectorType
  SMBIOS_MISC_STRING_2,                           ///< ExternalConnectorDesignator
  PortConnectorTypeUsb,                           ///< ExternalConnectorType
  PortTypeUsb                                     ///< PortType
};

GLOBAL_REMOVE_IF_UNREFERENCED MISC_SMBIOS_TABLE_DATA(SMBIOS_TABLE_TYPE8, MiscPortUsb3) = {
  {                                               // Hdr
    EFI_SMBIOS_TYPE_PORT_CONNECTOR_INFORMATION,   ///< Hdr.Type
    sizeof(SMBIOS_TABLE_TYPE8),                   ///< Hdr.Length
    0                                             ///< Hdr.Handle
  },
  SMBIOS_MISC_PORT_TOKEN_USB3,                    ///< InternalConnectorDesignator
  PortConnectorTypeNone,                          ///< InternalConnectorType
  SMBIOS_MISC_STRING_2,                           ///< ExternalConnectorDesignator
  PortConnectorTypeUsb,                           ///< ExternalConnectorType
  PortTypeUsb                                     ///< PortType
};

GLOBAL_REMOVE_IF_UNREFERENCED MISC_SMBIOS_TABLE_DATA(SMBIOS_TABLE_TYPE8, MiscPortUsb4) = {
  {                                               // Hdr
    EFI_SMBIOS_TYPE_PORT_CONNECTOR_INFORMATION,   ///< Hdr.Type
    sizeof(SMBIOS_TABLE_TYPE8),                   ///< Hdr.Length
    0                                             ///< Hdr.Handle
  },
  SMBIOS_MISC_PORT_TOKEN_USB4,                    ///< InternalConnectorDesignator
  PortConnectorTypeNone,                          ///< InternalConnectorType
  SMBIOS_MISC_STRING_2,                           ///< ExternalConnectorDesignator
  PortConnectorTypeUsb,                           ///< ExternalConnectorType
  PortTypeUsb                                     ///< PortType
};

GLOBAL_REMOVE_IF_UNREFERENCED MISC_SMBIOS_TABLE_DATA(SMBIOS_TABLE_TYPE8, MiscPortUsb5) = {
  {                                               // Hdr
    EFI_SMBIOS_TYPE_PORT_CONNECTOR_INFORMATION,   ///< Hdr.Type
    sizeof(SMBIOS_TABLE_TYPE8),                   ///< Hdr.Length
    0                                             ///< Hdr.Handle
  },
  SMBIOS_MISC_PORT_TOKEN_USB5,                    ///< InternalConnectorDesignator
  PortConnectorTypeNone,                          ///< InternalConnectorType
  SMBIOS_MISC_STRING_2,                           ///< ExternalConnectorDesignator
  PortConnectorTypeUsb,                           ///< ExternalConnectorType
  PortTypeUsb                                     ///< PortType
};

GLOBAL_REMOVE_IF_UNREFERENCED MISC_SMBIOS_TABLE_DATA(SMBIOS_TABLE_TYPE8, MiscPortUsb201) = {
  {                                               // Hdr
    EFI_SMBIOS_TYPE_PORT_CONNECTOR_INFORMATION,   ///< Hdr.Type
    sizeof(SMBIOS_TABLE_TYPE8),                   ///< Hdr.Length
    0                                             ///< Hdr.Handle
  },
  SMBIOS_MISC_PORT_TOKEN_USB201,                  ///< InternalConnectorDesignator
  PortConnectorTypeNone,                          ///< InternalConnectorType
  SMBIOS_MISC_STRING_2,                           ///< ExternalConnectorDesignator
  PortConnectorTypeUsb,                           ///< ExternalConnectorType
  PortTypeUsb                                     ///< PortType
};

GLOBAL_REMOVE_IF_UNREFERENCED MISC_SMBIOS_TABLE_DATA(SMBIOS_TABLE_TYPE8, MiscPortUsb202) = {
  {                                               // Hdr
    EFI_SMBIOS_TYPE_PORT_CONNECTOR_INFORMATION,   ///< Hdr.Type
    sizeof(SMBIOS_TABLE_TYPE8),                   ///< Hdr.Length
    0                                             ///< Hdr.Handle
  },
  SMBIOS_MISC_PORT_TOKEN_USB202,                  ///< InternalConnectorDesignator
  PortConnectorTypeNone,                          ///< InternalConnectorType
  SMBIOS_MISC_STRING_2,                           ///< ExternalConnectorDesignator
  PortConnectorTypeUsb,                           ///< ExternalConnectorType
  PortTypeUsb                                     ///< PortType
};

GLOBAL_REMOVE_IF_UNREFERENCED MISC_SMBIOS_TABLE_DATA(SMBIOS_TABLE_TYPE8, MiscPortNetwork) = {
  {                                               // Hdr
    EFI_SMBIOS_TYPE_PORT_CONNECTOR_INFORMATION,   ///< Hdr.Type
    sizeof(SMBIOS_TABLE_TYPE8),                   ///< Hdr.Length
    0                                             ///< Hdr.Handle
  },
  SMBIOS_MISC_PORT_TOKEN_NETWORK,                 ///< InternalConnectorDesignator
  PortConnectorTypeNone,                          ///< InternalConnectorType
  SMBIOS_MISC_STRING_2,                           ///< ExternalConnectorDesignator
  PortConnectorTypeRJ45,                          ///< ExternalConnectorType
  PortTypeNetworkPort                             ///< PortType
};


GLOBAL_REMOVE_IF_UNREFERENCED MISC_SMBIOS_TABLE_DATA(SMBIOS_TABLE_TYPE8, MiscPortIde1) = {
  {                                               // Hdr
    EFI_SMBIOS_TYPE_PORT_CONNECTOR_INFORMATION,   ///< Hdr.Type
    sizeof(SMBIOS_TABLE_TYPE8),                   ///< Hdr.Length
    0                                             ///< Hdr.Handle
  },
  SMBIOS_MISC_PORT_TOKEN_IDE1,                    ///< InternalConnectorDesignator
  PortConnectorTypeOnboardIde,                    ///< InternalConnectorType
  SMBIOS_MISC_STRING_2,                           ///< ExternalConnectorDesignator
  PortConnectorTypeNone,                          ///< ExternalConnectorType
  PortTypeOther                                   ///< PortType
};

GLOBAL_REMOVE_IF_UNREFERENCED MISC_SMBIOS_TABLE_DATA(SMBIOS_TABLE_TYPE8, MiscPortSata0) = {
  {                                               // Hdr
    EFI_SMBIOS_TYPE_PORT_CONNECTOR_INFORMATION,   ///< Hdr.Type
    sizeof(SMBIOS_TABLE_TYPE8),                   ///< Hdr.Length
    0                                             ///< Hdr.Handle
  },
  SMBIOS_MISC_PORT_TOKEN_SATA0,                   ///< InternalConnectorDesignator
  PortConnectorTypeNone,                          ///< InternalConnectorType
  SMBIOS_MISC_STRING_2,                           ///< ExternalConnectorDesignator
  PortConnectorTypeSasSata,                       ///< ExternalConnectorType
  PortTypeSata                                    ///< PortType
};

GLOBAL_REMOVE_IF_UNREFERENCED MISC_SMBIOS_TABLE_DATA(SMBIOS_TABLE_TYPE8, MiscPorteSata4) = {
  {                                               // Hdr
    EFI_SMBIOS_TYPE_PORT_CONNECTOR_INFORMATION,   ///< Hdr.Type
    sizeof(SMBIOS_TABLE_TYPE8),                   ///< Hdr.Length
    0                                             ///< Hdr.Handle
  },
  SMBIOS_MISC_PORT_TOKEN_ESATA4,                  ///< InternalConnectorDesignator
  PortConnectorTypeNone,                          ///< InternalConnectorType
  SMBIOS_MISC_STRING_2,                           ///< ExternalConnectorDesignator
  PortConnectorTypeSasSata,                       ///< ExternalConnectorType
  PortTypeSata                                    ///< PortType
};

GLOBAL_REMOVE_IF_UNREFERENCED MISC_SMBIOS_TABLE_DATA(SMBIOS_TABLE_TYPE8, MiscPorteSata3) = {
  {                                               // Hdr
    EFI_SMBIOS_TYPE_PORT_CONNECTOR_INFORMATION,   ///< Hdr.Type
    sizeof(SMBIOS_TABLE_TYPE8),                   ///< Hdr.Length
    0                                             ///< Hdr.Handle
  },
  SMBIOS_MISC_PORT_TOKEN_ESATA3,                  ///< InternalConnectorDesignator
  PortConnectorTypeNone,                          ///< InternalConnectorType
  SMBIOS_MISC_STRING_2,                           ///< ExternalConnectorDesignator
  PortConnectorTypeSasSata,                       ///< ExternalConnectorType
  PortTypeSata                                    ///< PortType
};

GLOBAL_REMOVE_IF_UNREFERENCED MISC_SMBIOS_TABLE_DATA(SMBIOS_TABLE_TYPE8, MiscPortSata2) = {
  {                                               // Hdr
    EFI_SMBIOS_TYPE_PORT_CONNECTOR_INFORMATION,   ///< Hdr.Type
    sizeof(SMBIOS_TABLE_TYPE8),                   ///< Hdr.Length
    0                                             ///< Hdr.Handle
  },
  SMBIOS_MISC_PORT_TOKEN_SATA2,                   ///< InternalConnectorDesignator
  PortConnectorTypeSasSata,                       ///< InternalConnectorType
  SMBIOS_MISC_STRING_2,                           ///< ExternalConnectorDesignator
  PortConnectorTypeNone,                          ///< ExternalConnectorType
  PortTypeSata                                    ///< PortType
};

GLOBAL_REMOVE_IF_UNREFERENCED MISC_SMBIOS_TABLE_DATA(SMBIOS_TABLE_TYPE8, MiscPortSata1) = {
  {                                               // Hdr
    EFI_SMBIOS_TYPE_PORT_CONNECTOR_INFORMATION,   ///< Hdr.Type
    sizeof(SMBIOS_TABLE_TYPE8),                   ///< Hdr.Length
    0                                             ///< Hdr.Handle
  },
  SMBIOS_MISC_PORT_TOKEN_SATA1,                   ///< InternalConnectorDesignator
  PortConnectorTypeSasSata,                       ///< InternalConnectorType
  SMBIOS_MISC_STRING_2,                           ///< ExternalConnectorDesignator
  PortConnectorTypeNone,                          ///< ExternalConnectorType
  PortTypeSata                                    ///< PortType
};

GLOBAL_REMOVE_IF_UNREFERENCED MISC_SMBIOS_TABLE_DATA(SMBIOS_TABLE_TYPE8, MiscPortACIN) = {
  {                                               // Hdr
    EFI_SMBIOS_TYPE_PORT_CONNECTOR_INFORMATION,   ///< Hdr.Type
    sizeof(SMBIOS_TABLE_TYPE8),                   ///< Hdr.Length
    0                                             ///< Hdr.Handle
  },
  SMBIOS_MISC_PORT_TOKEN_ACIN,                    ///< InternalConnectorDesignator
  PortConnectorTypeNone,                          ///< InternalConnectorType
  SMBIOS_MISC_STRING_2,                           ///< ExternalConnectorDesignator
  PortConnectorTypeOther,                         ///< ExternalConnectorType
  PortTypeOther                                   ///< PortType
};

GLOBAL_REMOVE_IF_UNREFERENCED MISC_SMBIOS_TABLE_DATA(SMBIOS_TABLE_TYPE8, MiscPortPCHJTAG) = {
  {                                               // Hdr
    EFI_SMBIOS_TYPE_PORT_CONNECTOR_INFORMATION,   ///< Hdr.Type
    sizeof(SMBIOS_TABLE_TYPE8),                   ///< Hdr.Length
    0                                             ///< Hdr.Handle
  },
  SMBIOS_MISC_PORT_TOKEN_PCHJTAG,                 ///< InternalConnectorDesignator
  PortConnectorTypeOther,                         ///< InternalConnectorType
  SMBIOS_MISC_STRING_2,                           ///< ExternalConnectorDesignator
  PortConnectorTypeNone,                          ///< ExternalConnectorType
  PortTypeOther                                   ///< PortType
};

GLOBAL_REMOVE_IF_UNREFERENCED MISC_SMBIOS_TABLE_DATA(SMBIOS_TABLE_TYPE8, MiscPortPORT80) = {
  {                                               // Hdr
    EFI_SMBIOS_TYPE_PORT_CONNECTOR_INFORMATION,   ///< Hdr.Type
    sizeof(SMBIOS_TABLE_TYPE8),                   ///< Hdr.Length
    0                                             ///< Hdr.Handle
  },
  SMBIOS_MISC_PORT_TOKEN_PORT80,                  ///< InternalConnectorDesignator
  PortConnectorTypeOther,                         ///< InternalConnectorType
  SMBIOS_MISC_STRING_2,                           ///< ExternalConnectorDesignator
  PortConnectorTypeNone,                          ///< ExternalConnectorType
  PortTypeOther                                   ///< PortType
};

GLOBAL_REMOVE_IF_UNREFERENCED MISC_SMBIOS_TABLE_DATA(SMBIOS_TABLE_TYPE8, MiscPort2X8HEADER) = {
  {                                               // Hdr
    EFI_SMBIOS_TYPE_PORT_CONNECTOR_INFORMATION,   ///< Hdr.Type
    sizeof(SMBIOS_TABLE_TYPE8),                   ///< Hdr.Length
    0                                             ///< Hdr.Handle
  },
  SMBIOS_MISC_PORT_TOKEN_2X8HEADER,               ///< InternalConnectorDesignator
  PortConnectorTypeOther,                         ///< InternalConnectorType
  SMBIOS_MISC_STRING_2,                           ///< ExternalConnectorDesignator
  PortConnectorTypeNone,                          ///< ExternalConnectorType
  PortTypeOther                                   ///< PortType
};

GLOBAL_REMOVE_IF_UNREFERENCED MISC_SMBIOS_TABLE_DATA(SMBIOS_TABLE_TYPE8, MiscPort8PINHEADER) = {
  {                                               // Hdr
    EFI_SMBIOS_TYPE_PORT_CONNECTOR_INFORMATION,   ///< Hdr.Type
    sizeof(SMBIOS_TABLE_TYPE8),                   ///< Hdr.Length
    0                                             ///< Hdr.Handle
  },
  SMBIOS_MISC_PORT_TOKEN_8PINHEADER,              ///< InternalConnectorDesignator
  PortConnectorTypeOther,                         ///< InternalConnectorType
  SMBIOS_MISC_STRING_2,                           ///< ExternalConnectorDesignator
  PortConnectorTypeNone,                          ///< ExternalConnectorType
  PortTypeOther,                                  ///< PortType
};

GLOBAL_REMOVE_IF_UNREFERENCED MISC_SMBIOS_TABLE_DATA(SMBIOS_TABLE_TYPE8, MiscPortHDAHDMI) = {
  {                                               // Hdr
    EFI_SMBIOS_TYPE_PORT_CONNECTOR_INFORMATION,   ///< Hdr.Type
    sizeof(SMBIOS_TABLE_TYPE8),                   ///< Hdr.Length
    0                                             ///< Hdr.Handle
  },
  SMBIOS_MISC_PORT_TOKEN_HDAHDMI,                 ///< InternalConnectorDesignator
  PortConnectorTypeOther,                         ///< InternalConnectorType
  SMBIOS_MISC_STRING_2,                           ///< ExternalConnectorDesignator
  PortConnectorTypeNone,                          ///< ExternalConnectorType
  PortTypeOther                                   ///< PortType
};

GLOBAL_REMOVE_IF_UNREFERENCED MISC_SMBIOS_TABLE_DATA(SMBIOS_TABLE_TYPE8, MiscPortMKEYBOARD) = {
  {                                               // Hdr
    EFI_SMBIOS_TYPE_PORT_CONNECTOR_INFORMATION,   ///< Hdr.Type
    sizeof(SMBIOS_TABLE_TYPE8),                   ///< Hdr.Length
    0                                             ///< Hdr.Handle
  },
  SMBIOS_MISC_PORT_TOKEN_MKEYBOARD,               ///< InternalConnectorDesignator
  PortConnectorTypeOther,                         ///< InternalConnectorType
  SMBIOS_MISC_STRING_2,                           ///< ExternalConnectorDesignator
  PortConnectorTypeNone,                          ///< ExternalConnectorType
  PortTypeOther                                   ///< PortType
};

GLOBAL_REMOVE_IF_UNREFERENCED MISC_SMBIOS_TABLE_DATA(SMBIOS_TABLE_TYPE8, MiscPortSPI) = {
  {                                               // Hdr
    EFI_SMBIOS_TYPE_PORT_CONNECTOR_INFORMATION,   ///< Hdr.Type
    sizeof(SMBIOS_TABLE_TYPE8),                   ///< Hdr.Length
    0                                             ///< Hdr.Handle
  },
  SMBIOS_MISC_PORT_TOKEN_SPI,                     ///< InternalConnectorDesignator
  PortConnectorTypeOther,                         ///< InternalConnectorType
  SMBIOS_MISC_STRING_2,                           ///< ExternalConnectorDesignator
  PortConnectorTypeNone,                          ///< ExternalConnectorType
  PortTypeOther                                   ///< PortType
};

GLOBAL_REMOVE_IF_UNREFERENCED MISC_SMBIOS_TABLE_DATA(SMBIOS_TABLE_TYPE8, MiscPortLPCDOCKING) = {
  {                                               // Hdr
    EFI_SMBIOS_TYPE_PORT_CONNECTOR_INFORMATION,   ///< Hdr.Type
    sizeof(SMBIOS_TABLE_TYPE8),                   ///< Hdr.Length
    0                                             ///< Hdr.Handle
  },
  SMBIOS_MISC_PORT_TOKEN_LPCDOCKING,              ///< InternalConnectorDesignator
  PortConnectorTypeOther,                         ///< InternalConnectorType
  SMBIOS_MISC_STRING_2,                           ///< ExternalConnectorDesignator
  PortConnectorTypeNone,                          ///< ExternalConnectorType
  PortTypeOther                                   ///< PortType
};

GLOBAL_REMOVE_IF_UNREFERENCED MISC_SMBIOS_TABLE_DATA(SMBIOS_TABLE_TYPE8, MiscPortSIDEBAND) = {
  {                                               // Hdr
    EFI_SMBIOS_TYPE_PORT_CONNECTOR_INFORMATION,   ///< Hdr.Type
    sizeof(SMBIOS_TABLE_TYPE8),                   ///< Hdr.Length
    0                                             ///< Hdr.Handle
  },
  SMBIOS_MISC_PORT_TOKEN_SIDEBAND,                ///< InternalConnectorDesignator
  PortConnectorTypeOther,                         ///< InternalConnectorType
  SMBIOS_MISC_STRING_2,                           ///< ExternalConnectorDesignator
  PortConnectorTypeNone,                          ///< ExternalConnectorType
  PortTypeOther                                   ///< PortType
};

GLOBAL_REMOVE_IF_UNREFERENCED MISC_SMBIOS_TABLE_DATA(SMBIOS_TABLE_TYPE8, MiscPortLPCSLOT) = {
  {                                               // Hdr
    EFI_SMBIOS_TYPE_PORT_CONNECTOR_INFORMATION,   ///< Hdr.Type
    sizeof(SMBIOS_TABLE_TYPE8),                   ///< Hdr.Length
    0                                             ///< Hdr.Handle
  },
  SMBIOS_MISC_PORT_TOKEN_LPCSLOT,                 ///< InternalConnectorDesignator
  PortConnectorTypeOther,                         ///< InternalConnectorType
  SMBIOS_MISC_STRING_2,                           ///< ExternalConnectorDesignator
  PortConnectorTypeNone,                          ///< ExternalConnectorType
  PortTypeOther                                   ///< PortType
};

GLOBAL_REMOVE_IF_UNREFERENCED MISC_SMBIOS_TABLE_DATA(SMBIOS_TABLE_TYPE8, MiscPortPCHXDP) = {
  {                                               // Hdr
    EFI_SMBIOS_TYPE_PORT_CONNECTOR_INFORMATION,   ///< Hdr.Type
    sizeof(SMBIOS_TABLE_TYPE8),                   ///< Hdr.Length
    0                                             ///< Hdr.Handle
  },
  SMBIOS_MISC_PORT_TOKEN_PCHXDP,                  ///< InternalConnectorDesignator
  PortConnectorTypeOther,                         ///< InternalConnectorType
  SMBIOS_MISC_STRING_2,                           ///< ExternalConnectorDesignator
  PortConnectorTypeNone,                          ///< ExternalConnectorType
  PortTypeOther                                   ///< PortType
};

GLOBAL_REMOVE_IF_UNREFERENCED MISC_SMBIOS_TABLE_DATA(SMBIOS_TABLE_TYPE8, MiscPortSATAPOWER) = {
  {                                               // Hdr
    EFI_SMBIOS_TYPE_PORT_CONNECTOR_INFORMATION,   ///< Hdr.Type
    sizeof(SMBIOS_TABLE_TYPE8),                   ///< Hdr.Length
    0                                             ///< Hdr.Handle
  },
  SMBIOS_MISC_PORT_TOKEN_SATAPOWER,               ///< InternalConnectorDesignator
  PortConnectorTypeOther,                         ///< InternalConnectorType
  SMBIOS_MISC_STRING_2,                           ///< ExternalConnectorDesignator
  PortConnectorTypeNone,                          ///< ExternalConnectorType
  PortTypeOther                                   ///< PortType
};

GLOBAL_REMOVE_IF_UNREFERENCED MISC_SMBIOS_TABLE_DATA(SMBIOS_TABLE_TYPE8, MiscPortFPHEADER) = {
  {                                               // Hdr
    EFI_SMBIOS_TYPE_PORT_CONNECTOR_INFORMATION,   ///< Hdr.Type
    sizeof(SMBIOS_TABLE_TYPE8),                   ///< Hdr.Length
    0                                             ///< Hdr.Handle
  },
  SMBIOS_MISC_PORT_TOKEN_FPHEADER,                ///< InternalConnectorDesignator
  PortConnectorTypeOther,                         ///< InternalConnectorType
  SMBIOS_MISC_STRING_2,                           ///< ExternalConnectorDesignator
  PortConnectorTypeNone,                          ///< ExternalConnectorType
  PortTypeOther                                   ///< PortType
};

GLOBAL_REMOVE_IF_UNREFERENCED MISC_SMBIOS_TABLE_DATA(SMBIOS_TABLE_TYPE8, MiscPortATXPOWER) = {
  {                                               // Hdr
    EFI_SMBIOS_TYPE_PORT_CONNECTOR_INFORMATION,   ///< Hdr.Type
    sizeof(SMBIOS_TABLE_TYPE8),                   ///< Hdr.Length
    0                                             ///< Hdr.Handle
  },
  SMBIOS_MISC_PORT_TOKEN_ATXPOWER,                ///< InternalConnectorDesignator
  PortConnectorTypeOther,                         ///< InternalConnectorType
  SMBIOS_MISC_STRING_2,                           ///< ExternalConnectorDesignator
  PortConnectorTypeNone,                          ///< ExternalConnectorType
  PortTypeOther                                   ///< PortType
};

GLOBAL_REMOVE_IF_UNREFERENCED MISC_SMBIOS_TABLE_DATA(SMBIOS_TABLE_TYPE8, MiscPortAVMC) = {
  {                                               // Hdr
    EFI_SMBIOS_TYPE_PORT_CONNECTOR_INFORMATION,   ///< Hdr.Type
    sizeof(SMBIOS_TABLE_TYPE8),                   ///< Hdr.Length
    0                                             ///< Hdr.Handle
  },
  SMBIOS_MISC_PORT_TOKEN_AVMC,                    ///< InternalConnectorDesignator
  PortConnectorTypeOther,                         ///< InternalConnectorType
  SMBIOS_MISC_STRING_2,                           ///< ExternalConnectorDesignator
  PortConnectorTypeNone,                          ///< ExternalConnectorType
  PortTypeOther                                   ///< PortType
};

GLOBAL_REMOVE_IF_UNREFERENCED MISC_SMBIOS_TABLE_DATA(SMBIOS_TABLE_TYPE8, MiscPortBATTB) = {
  {                                               // Hdr
    EFI_SMBIOS_TYPE_PORT_CONNECTOR_INFORMATION,   ///< Hdr.Type
    sizeof(SMBIOS_TABLE_TYPE8),                   ///< Hdr.Length
    0                                             ///< Hdr.Handle
  },
  SMBIOS_MISC_PORT_TOKEN_BATTB,                   ///< InternalConnectorDesignator
  PortConnectorTypeOther,                         ///< InternalConnectorType
  SMBIOS_MISC_STRING_2,                           ///< ExternalConnectorDesignator
  PortConnectorTypeNone,                          ///< ExternalConnectorType
  PortTypeOther                                   ///< PortType
};

GLOBAL_REMOVE_IF_UNREFERENCED MISC_SMBIOS_TABLE_DATA(SMBIOS_TABLE_TYPE8, MiscPortBATTA) = {
  {                                               // Hdr
    EFI_SMBIOS_TYPE_PORT_CONNECTOR_INFORMATION,   ///< Hdr.Type
    sizeof(SMBIOS_TABLE_TYPE8),                   ///< Hdr.Length
    0                                             ///< Hdr.Handle
  },
  SMBIOS_MISC_PORT_TOKEN_BATTA,                   ///< InternalConnectorDesignator
  PortConnectorTypeOther,                         ///< InternalConnectorType
  SMBIOS_MISC_STRING_2,                           ///< ExternalConnectorDesignator
  PortConnectorTypeNone,                          ///< ExternalConnectorType
  PortTypeOther                                   ///< PortType
};

GLOBAL_REMOVE_IF_UNREFERENCED MISC_SMBIOS_TABLE_DATA(SMBIOS_TABLE_TYPE8, MiscPortCPUFAN) = {
  {                                               // Hdr
    EFI_SMBIOS_TYPE_PORT_CONNECTOR_INFORMATION,   ///< Hdr.Type
    sizeof(SMBIOS_TABLE_TYPE8),                   ///< Hdr.Length
    0                                             ///< Hdr.Handle
  },
  SMBIOS_MISC_PORT_TOKEN_CPUFAN,                  ///< InternalConnectorDesignator
  PortConnectorTypeOther,                         ///< InternalConnectorType
  SMBIOS_MISC_STRING_2,                           ///< ExternalConnectorDesignator
  PortConnectorTypeNone,                          ///< ExternalConnectorType
  PortTypeOther                                   ///< PortType
};

GLOBAL_REMOVE_IF_UNREFERENCED MISC_SMBIOS_TABLE_DATA(SMBIOS_TABLE_TYPE8, MiscPortXDP) = {
  {                                               // Hdr
    EFI_SMBIOS_TYPE_PORT_CONNECTOR_INFORMATION,   ///< Hdr.Type
    sizeof(SMBIOS_TABLE_TYPE8),                   ///< Hdr.Length
    0                                             ///< Hdr.Handle
  },
  SMBIOS_MISC_PORT_TOKEN_XDP,                     ///< InternalConnectorDesignator
  PortConnectorTypeOther,                         ///< InternalConnectorType
  SMBIOS_MISC_STRING_2,                           ///< ExternalConnectorDesignator
  PortConnectorTypeNone,                          ///< ExternalConnectorType
  PortTypeOther                                   ///< PortType
};

GLOBAL_REMOVE_IF_UNREFERENCED MISC_SMBIOS_TABLE_DATA(SMBIOS_TABLE_TYPE8, MiscPortMEMORY1) = {
  {                                               // Hdr
    EFI_SMBIOS_TYPE_PORT_CONNECTOR_INFORMATION,   ///< Hdr.Type
    sizeof(SMBIOS_TABLE_TYPE8),                   ///< Hdr.Length
    0                                             ///< Hdr.Handle
  },
  SMBIOS_MISC_PORT_TOKEN_MEMORY1,                 ///< InternalConnectorDesignator
  PortConnectorTypeOther,                         ///< InternalConnectorType
  SMBIOS_MISC_STRING_2,                           ///< ExternalConnectorDesignator
  PortConnectorTypeNone,                          ///< ExternalConnectorType
  PortTypeOther                                   ///< PortType
};

GLOBAL_REMOVE_IF_UNREFERENCED MISC_SMBIOS_TABLE_DATA(SMBIOS_TABLE_TYPE8, MiscPortMEMORY2) = {
  {                                               // Hdr
    EFI_SMBIOS_TYPE_PORT_CONNECTOR_INFORMATION,   ///< Hdr.Type
    sizeof(SMBIOS_TABLE_TYPE8),                   ///< Hdr.Length
    0                                             ///< Hdr.Handle
  },
  SMBIOS_MISC_PORT_TOKEN_MEMORY2,                 ///< InternalConnectorDesignator
  PortConnectorTypeOther,                         ///< InternalConnectorType
  SMBIOS_MISC_STRING_2,                           ///< ExternalConnectorDesignator
  PortConnectorTypeNone,                          ///< ExternalConnectorType
  PortTypeOther                                   ///< PortType
};

GLOBAL_REMOVE_IF_UNREFERENCED MISC_SMBIOS_TABLE_DATA(SMBIOS_TABLE_TYPE8, MiscPortFANPWR) = {
  {                                               // Hdr
    EFI_SMBIOS_TYPE_PORT_CONNECTOR_INFORMATION,   ///< Hdr.Type
    sizeof(SMBIOS_TABLE_TYPE8),                   ///< Hdr.Length
    0                                             ///< Hdr.Handle
  },
  SMBIOS_MISC_PORT_TOKEN_FANPWR,                  ///< InternalConnectorDesignator
  PortConnectorTypeOther,                         ///< InternalConnectorType
  SMBIOS_MISC_STRING_2,                           ///< ExternalConnectorDesignator
  PortConnectorTypeNone,                          ///< ExternalConnectorType
  PortTypeOther                                   ///< PortType
};
