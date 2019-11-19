/** @file
  Intel ACPI Reference Code for Dynamic Platform & Thermal Framework

 @copyright
  INTEL CONFIDENTIAL
  Copyright 2013 - 2018 Intel Corporation.

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

    Name (SPLX, Package()
    {
      0,                      // Revision
      Package()
      {
       0x80000000,            // DomainType,         09h: Module; 07h: WiFi/WLAN; 0Fh: WWAN; 10h WiGig; 14h: RFEM
       0x80000000,            // DefaultPowerLimit,  mW
       0x80000000             // DefaultTimeWindow,  ms
      },
      Package()
      {
       0x80000000,            // DomainType,         09h: Module; 07h: WiFi/WLAN; 0Fh: WWAN; 10h WiGig; 14h: RFEM
       0x80000000,            // DefaultPowerLimit,  mW
       0x80000000             // DefaultTimeWindow,  ms
      },
      Package()
      {
       0x80000000,            // DomainType,         09h: Module; 07h: WiFi/WLAN; 0Fh: WWAN; 10h WiGig; 14h: RFEM
       0x80000000,            // DefaultPowerLimit,  mW
       0x80000000             // DefaultTimeWindow,  ms
      }
    }) // End of SPLC object

    // SPLC (Static Power Limit Configuration)
    //
    // The SPLC object evaluates to a package of packages that indicates to each core as well as to
    // the Power Sharing Manager the static power limit capabilities to be used as default when
    // no dynamic power limit controls are available. For PSM object, the SPLC object can have more
    // than package with different domain types to provide default values for various combinations
    // of cores. For example, there can be two packages with each package having a DomainType
    // value of 1 and 8 respectively to provide different power limit values for the NGFF module and
    // the R-FEM module separately.
    //
    // Arguments: (0)
    //   None
    // Return Value:
    //
    // Name (SPLC, Package()
    // { // Field Name           Field Type
    //   Revision,               // DWordConst
    //   Package()               // Default Configuration 1
    //   {
    //     DomainType,           // 09h: Module; 07h: WiFi/WLAN; 0Fh: WWAN; 10h WiGig; 14h: RFEM
    //     DefaultPowerLimit,    // mW
    //     DefaultTimeWindow     // ms
    //   },
    //   Package()               // Default Configuration 2
    //   {
    //     DomainType,           // 09h: Module; 07h: WiFi/WLAN; 0Fh: WWAN; 10h WiGig; 14h: RFEM
    //     DefaultPowerLimit,    // mW
    //     DefaultTimeWindow     // ms
    //   },
    //   Package()               // Default Configuration 3
    //   {
    //     DomainType,           // 09h: Module; 07h: WiFi/WLAN; 0Fh: WWAN; 10h WiGig; 14h: RFEM
    //     DefaultPowerLimit,    // mW
    //     DefaultTimeWindow     // ms
    //   }
    //  }) // End of SPLC object
    //
    Method(SPLC,0,Serialized)
    {
      Store(\DOM1,Index (DeRefOf (Index (SPLX, 1)), 0)) // DomainType
      Store(\LIM1,Index (DeRefOf (Index (SPLX, 1)), 1)) // PowerLimit
      Store(\TIM1,Index (DeRefOf (Index (SPLX, 1)), 2)) // TimeWindow

      Store(\DOM2,Index (DeRefOf (Index (SPLX, 2)), 0)) // DomainType
      Store(\LIM2,Index (DeRefOf (Index (SPLX, 2)), 1)) // PowerLimit
      Store(\TIM2,Index (DeRefOf (Index (SPLX, 2)), 2)) // TimeWindow

      Store(\DOM3,Index (DeRefOf (Index (SPLX, 3)), 0)) // DomainType
      Store(\LIM3,Index (DeRefOf (Index (SPLX, 3)), 1)) // PowerLimit
      Store(\TIM3,Index (DeRefOf (Index (SPLX, 3)), 2)) // TimeWindow

      Return(SPLX)
    }

    PowerResource(WRST, 5, 0)
    {
      // Define the PowerResource for PCIe slot
      // PowerResource expects to have _STA, _ON and _OFF Method per ACPI Spec. Not having one of them will cause BSOD

      // Method: Dummy _STA() to comply with ACPI Spec
      Method(_STA)
      {
        ADBG("PXSX _STA")
        return(1)
      }

      // Method: Dummy _ON() to comply with ACPI Spec
      Method(_ON, 0)
      {
        ADBG("PXSX _ON")
      }

      // Method: Dummy _OFF() to comply with ACPI Spec
      Method(_OFF, 0)
      {
        ADBG("PXSX _OFF")
      }

      // Method: _RST()
      Method(_RST, 0, NotSerialized)
      {
        ADBG("PXSX _RST")
        If(And(DCAP, 0x10000000))
        {
          Store(DCTR, Local0)
          Or(Local0, 0x8000, Local0)
          Store(Local0, DCTR)
        }
      }
    } // End WRST
    Name(_PRR, Package(){WRST})

    Name (WANX, Package()
    {
      0,                      // Revision, DWordConst
      Package()               // Default Configuration 1
      {
        0x80000000,           // AntennaID, 0:Antenna A, 1:Antenna B
        0x80000000,           // TxRx Delay, possible values 1-100 in 0.1ns resolution (0.1 to 10ns)
        0x80000000            // Antenna CableLength, in 1cm resolution, possible values 1-100
      },
      Package()               // Default Configuration 1
      {
        0x80000000,           // AntennaID, 0:Antenna A, 1:Antenna B
        0x80000000,           // TxRx Delay, possible values 1-100 in 0.1ns resolution (0.1 to 10ns)
        0x80000000            // Antenna CableLength, in 1cm resolution, possible values 1-100
      }
    }) // End of WAND object

    // WAND (Wireless Antenna Diversity)
    //
    // Description of the WiFi antennas on the module
    //
    // Arguments: (0)
    //   None
    // Return Value:
    //
    // Name (WAND, Package()
    // { // Field Name           Field Type
    //   Revision,               // DWordConst
    //   Package()               // Default Configuration 1
    //   {
    //     AntennaID,            // 0:Antenna A, 1:Antenna B
    //     TRxDelay,             // nSecDelay, possible values 1-100 in 0.1ns resolution (0.1 to 10ns).
    //     TRxCableLength        // cm ; possible values 1-100cm in 1cm resolution.
    //   },
    //   Package()               // Default Configuration 1
    //   {
    //     AntennaID,            // 0:Antenna A, 1:Antenna B
    //     TRxDelay,             // nSecDelay, possible values 1-100 in 0.1ns resolution (0.1 to 10ns).
    //     TRxCableLength        // cm ; possible values 1-100cm in 1cm resolution.
    //   }
    // }) // End of WAND object
    //
    Method(WAND,0,Serialized)
    {
      Store(0,Index (DeRefOf (Index (WANX, 1)), 0)) // AntennaID 0
      Store(\TRD0,Index (DeRefOf (Index (WANX, 1)), 1)) // TRxDelay 0
      Store(\TRL0,Index (DeRefOf (Index (WANX, 1)), 2))  // TRxCableLength 0

      Store(1,Index (DeRefOf (Index (WANX, 2)), 0)) // AntennaID 1
      Store(\TRD1,Index (DeRefOf (Index (WANX, 2)), 1)) // TRxDelay 1
      Store(\TRL1,Index (DeRefOf (Index (WANX, 2)), 2))  // TRxCableLength 1

      Return(WANX)
    }

    Name (WRDX, Package()
    {
      0,                      // Revision, [0-255]
      Package()               // Configuration for WiFi or WiGig
      {
        0x80000000,           // DomainType, 0x7:WiFi Core, 0x10:WiGig Core
        0x8000,               // RegulatoryDomain, Country identifier as defined in ISO/IEC 3166-1 Alpha 2 code
      },
      Package()               // Configuration for WiFi or WiGig
      {
        0x80000000,           // DomainType, 0x7:WiFi Core, 0x10:WiGig Core
        0x8000,               // RegulatoryDomain, Country identifier as defined in ISO/IEC 3166-1 Alpha 2 code
      }
    })                        // End of WRDD object

    // WRDD (Wireless Regulatory Domain Description)
    //
    // WiFi has regulatory limitations which prohibit or allow usage of certain bands or channels as well as limiting the Tx power.
    // Those settings are different per country (or groups of countries). In order to reduce the number of
    // HW SKUs the direction is now to include platform level ACPI setting that determines the country.
    // WiFi driver shall read the WRDD object during initialization process and shall configure uCode to apply the right regulatory limits.
    //
    // Arguments: (0)
    //   None
    // Return Value:
    //
    // Name (WRDD, Package()
    // {
    //   Revision,               // Revision, [0-255]
    //   Package()               // Configuration for WiFi or WiGig
    //   {
    //     DomainType,           // 0x7:WiFi  Core, 0x10:WiGig Core
    //     RegulatoryDomain,     // Country identifier as defined in ISO/IEC 3166-1 Alpha 2 code
    //   },
    //   Package()               // Configuration for WiFi or WiGig
    //   {
    //     DomainType,           // 0x7:WiFi Core, 0x10:WiGig Core
    //     RegulatoryDomain,     // Country identifier as defined in ISO/IEC 3166-1 Alpha 2 code
    //   }
    // })                        // End of WRDD object
    //
    Method(WRDD,0,Serialized)
    {
      Store(\WDM1,Index (DeRefOf (Index (WRDX, 1)), 0)) // DomainType
      Store(\CID1,Index (DeRefOf (Index (WRDX, 1)), 1)) // Country identifier

      Store(\WDM2,Index (DeRefOf (Index (WRDX, 2)), 0)) // DomainType
      Store(\CID2,Index (DeRefOf (Index (WRDX, 2)), 1)) // Country identifier

      Return(WRDX)
    }

    Name (WRDY, Package()
    {
      0,                      // Revision, DWordConst
      Package()               // Configuration for WiFi
      {
        0x07,                 // DomainType, 0x7:WiFi
        0x80,                 // WiFi SAR Enable/Disable
        0x80,                 // WiFi SAR Tx Power Limit for 2400 MHz Chain A
        0x80,                 // WiFi SAR Tx Power Limit for 5150-5350 MHz Chain A
        0x80,                 // WiFi SAR Tx Power Limit for 5350-5470 MHz Chain A
        0x80,                 // WiFi SAR Tx Power Limit for 5470-5725 MHz Chain A
        0x80,                 // WiFi SAR Tx Power Limit for 5725-5950 MHz Chain A
        0x80,                 // WiFi SAR Tx Power Limit for 2400 MHz Chain B
        0x80,                 // WiFi SAR Tx Power Limit for 5150-5350 MHz Chain B
        0x80,                 // WiFi SAR Tx Power Limit for 5350-5470 MHz Chain B
        0x80,                 // WiFi SAR Tx Power Limit for 5470-5725 MHz Chain B
        0x80,                 // WiFi SAR Tx Power Limit for 5725-5950 MHz Chain B
      },
      Package()               // Configuration for WiGig
      {
        0x10,                 // DomainType, 0x10:WiGig Core
        0x80,                 // Wigig RFE Enable/Disable
        0x80,                 // RFE WiGig Ch1
        0x80,                 // RFE WiGig Ch2
        0x80,                 // RFE WiGig Ch3
        0x80,                 // RFE WiGig Ch4
      }
    })                        // End of WRDD object

    // WRDS (Wireless Regulatory Domain SAR)
    //
    // WiFi has regulatory limitations which prohibit or allow usage of certain bands or channels as well as limiting the Tx power.
    // This method is used to modify the WiFi SAR Tx Power Limit to properly control it
    //
    // Arguments: (0)
    //   None
    // Return Value:
    //
    // Name (WRDS, Package()
    // {
    //   Revision,                // DWordConst
    //   Package()                // Configuration for WiFi
    //   {
    //     DomainType,            // 0x7:WiFi
    //     WiFiSarEnable,         // Configures the source location of the WiFi SAR table to be used; 0:Device ignores BIOS configuration; 1:Device uses BIOS configuration
    //     WiFiSarTxPowerLimit1,  // WiFi SAR Tx Power Limit for 2400 MHz Chain A
    //     WiFiSarTxPowerLimit2,  // WiFi SAR Tx Power Limit for 5150-5350 MHz Chain A
    //     WiFiSarTxPowerLimit3,  // WiFi SAR Tx Power Limit for 5350-5470 MHz Chain A
    //     WiFiSarTxPowerLimit4,  // WiFi SAR Tx Power Limit for 5470-5725 MHz Chain A
    //     WiFiSarTxPowerLimit5,  // WiFi SAR Tx Power Limit for 5725-5950 MHz Chain A
    //     WiFiSarTxPowerLimit6,  // WiFi SAR Tx Power Limit for 2400 MHz Chain B
    //     WiFiSarTxPowerLimit7,  // WiFi SAR Tx Power Limit for 5150-5350 MHz Chain B
    //     WiFiSarTxPowerLimit8,  // WiFi SAR Tx Power Limit for 5350-5470 MHz Chain B
    //     WiFiSarTxPowerLimit9,  // WiFi SAR Tx Power Limit for 5470-5725 MHz Chain B
    //     WiFiSarTxPowerLimit10, // WiFi SAR Tx Power Limit for 5725-5950 MHz Chain B
    //   },
    //   Package()                // Configuration for Wigig
    //   {
    //     DomainType,            // 0x10:WiGig Core
    //     WigigRfe,              // Define the mode of RFE control to be used. 0:Tx power shall be mandated by device OTP RFE Class: 1:Tx power shall be the minimum between BIOS RFE table and OTP RFE Class
    //     WiGigRfeCh1,           // RFE WiGig Ch1
    //     WiGigRfeCh2,           // RFE WiGig Ch2
    //     WiGigRfeCh3,           // RFE WiGig Ch3
    //     WiGigRfeCh4,           // RFE WiGig Ch4
    //   }
    // })                         // End of WRDS object
    //
    Method(WRDS,0,Serialized)
    {
      Store(\STXE,Index (DeRefOf (Index (WRDY, 1)), 1))  // WiFi SAR Enable/Disable
      Store(\STX0,Index (DeRefOf (Index (WRDY, 1)), 2))  // WiFi SAR Tx Power Limit for 2400 MHz Chain A
      Store(\STX1,Index (DeRefOf (Index (WRDY, 1)), 3))  // WiFi SAR Tx Power Limit for 5150-5350 MHz Chain A
      Store(\STX2,Index (DeRefOf (Index (WRDY, 1)), 4))  // WiFi SAR Tx Power Limit for 5350-5470 MHz Chain A
      Store(\STX3,Index (DeRefOf (Index (WRDY, 1)), 5))  // WiFi SAR Tx Power Limit for 5470-5725 MHz Chain A
      Store(\STX4,Index (DeRefOf (Index (WRDY, 1)), 6))  // WiFi SAR Tx Power Limit for 5725-5950 MHz Chain A
      Store(\STX5,Index (DeRefOf (Index (WRDY, 1)), 7))  // WiFi SAR Tx Power Limit for 2400 MHz Chain B
      Store(\STX6,Index (DeRefOf (Index (WRDY, 1)), 8))  // WiFi SAR Tx Power Limit for 5150-5350 MHz Chain B
      Store(\STX7,Index (DeRefOf (Index (WRDY, 1)), 9))  // WiFi SAR Tx Power Limit for 5350-5470 MHz Chain B
      Store(\STX8,Index (DeRefOf (Index (WRDY, 1)), 10)) // WiFi SAR Tx Power Limit for 5470-5725 MHz Chain B
      Store(\STX9,Index (DeRefOf (Index (WRDY, 1)), 11)) // WiFi SAR Tx Power Limit for 5725-5950 MHz Chain B

      Store(\WRFE,Index (DeRefOf (Index (WRDY, 2)), 1))  // Wigig RFE Enable/Disable
      Store(\WRC1,Index (DeRefOf (Index (WRDY, 2)), 2))  // RFE WiGig Ch1
      Store(\WRC2,Index (DeRefOf (Index (WRDY, 2)), 3))  // RFE WiGig Ch2
      Store(\WRC3,Index (DeRefOf (Index (WRDY, 2)), 4))  // RFE WiGig Ch3
      Store(\WRC4,Index (DeRefOf (Index (WRDY, 2)), 5))  // RFE WiGig Ch4

      Return(WRDY)
    }

    //
    // Name: AWVC (AWV[Antenna Weight Vectors] Class Index)
    // Description: Potential Beamforming gain may be applied, by utilizing a dedicated beamforming schemes that will match a specific platform characteristics (RFEM type, RFEM location, Chassis materiel etc).
    // The appropriate AWV class to be filled in platform specific BIOS, allowing WiGig driver to optimize its beamforming scheme accordingly
    // Input: Nothing
    // Return: AWV Class Index data
    //
    Method(AWVC,0,Serialized)
    {
      Return(AWVI)
    }

    //
    // Name: WOWG (Wake on WiGig)
    // Description: This method is consumed by WiGig driver to Enable\Disable S3-S4 Wake on WiGig Docking Support. We basically controling using user Bios Setup Option.
    // Input: Nothing
    // Return: User selected value in Bios Setup Menu 0 : Disable 1 : Enable
    //
    Method(WOWG,0,Serialized)
    {
      Return(WGWS)
    }

    Name (WGDY, Package()
    {
      0,                      // Revision, DWordConst
      Package()               // Configuration for WiFi
      {
        0x07,                 // DomainType, 0x7:WiFi
        0x80,                 // WiFi output power delta for Group 1 FCC 2400 MHZ Max Allowed
        0x80,                 // WiFi output power delta for Group 1 FCC 2400 Chain A Offset
        0x80,                 // WiFi output power delta for Group 1 FCC 2400 Chain B Offset
        0x80,                 // WiFi output power delta for Group 1 FCC 5200 MHZ Max Allowed
        0x80,                 // WiFi output power delta for Group 1 FCC 5200 Chain A Offset
        0x80,                 // WiFi output power delta for Group 1 FCC 5200 Chain B Offset
        0x80,                 // WiFi output power delta for Group 2 EC Japan 2400 MHZ Max Allowed
        0x80,                 // WiFi output power delta for Group 2 EC Japan 2400 Chain A Offset
        0x80,                 // WiFi output power delta for Group 2 EC Japan 2400 Chain B Offset
        0x80,                 // WiFi output power delta for Group 2 EC Japan 5200 MHZ Max Allowed
        0x80,                 // WiFi output power delta for Group 2 EC Japan 5200 Chain A Offset
        0x80,                 // WiFi output power delta for Group 2 EC Japan 5200 Chain B Offset
        0x80,                 // WiFi output power delta for Group 3 ROW 2400 MHZ Max Allowed
        0x80,                 // WiFi output power delta for Group 3 ROW 2400 Chain A Offset
        0x80,                 // WiFi output power delta for Group 3 ROW 2400 Chain B Offset
        0x80,                 // WiFi output power delta for Group 3 ROW 5200 MHZ Max Allowed
        0x80,                 // WiFi output power delta for Group 3 ROW 5200 Chain A Offset
        0x80,                 // WiFi output power delta for Group 3 ROW 5200 Chain B Offset
      }
    })                        // End of WGDY object

    // WGDS (Wireless Geo Delta Settings)
    //
    // Defines the output power delta of the current SAR set, this value will allow an increased Tx power
    // compared to the default (Canada) BIOS SAR settings in case DRS mechanism
    // has detected country that is compatible with Europe regulations
    //
    //
    // Arguments: (0)
    //   None
    // Return Value:
    //
    // Name (WGDY, Package()
    // {
    //  Revision,                                      // Revision, DWordConst
    //  Package()                                      // Configuration for WiFi
    //  {
    //    DomainType,                                  // DomainType, 0x7:WiFi
    //    WgdsWiFiSarDeltaGroup1PowerMax1,             // WiFi output power delta for Group 1 FCC 2400 MHZ Max Allowed
    //    WgdsWiFiSarDeltaGroup1PowerChainA1,          // WiFi output power delta for Group 1 FCC 2400 Chain A Offset
    //    WgdsWiFiSarDeltaGroup1PowerChainB1,          // WiFi output power delta for Group 1 FCC 2400 Chain B Offset
    //    WgdsWiFiSarDeltaGroup1PowerMax2,             // WiFi output power delta for Group 1 FCC 5200 MHZ Max Allowed
    //    WgdsWiFiSarDeltaGroup1PowerChainA2,          // WiFi output power delta for Group 1 FCC 5200 Chain A Offset
    //    WgdsWiFiSarDeltaGroup1PowerChainB2,          // WiFi output power delta for Group 1 FCC 5200 Chain B Offset
    //    WgdsWiFiSarDeltaGroup2PowerMax1,             // WiFi output power delta for Group 2 EC Japan 2400 MHZ Max Allowed
    //    WgdsWiFiSarDeltaGroup2PowerChainA1,          // WiFi output power delta for Group 2 EC Japan 2400 Chain A Offset
    //    WgdsWiFiSarDeltaGroup2PowerChainB1,          // WiFi output power delta for Group 2 EC Japan 2400 Chain B Offset
    //    WgdsWiFiSarDeltaGroup2PowerMax2,             // WiFi output power delta for Group 2 EC Japan 5200 MHZ Max Allowed
    //    WgdsWiFiSarDeltaGroup2PowerChainA2,          // WiFi output power delta for Group 2 EC Japan 5200 Chain A Offset
    //    WgdsWiFiSarDeltaGroup2PowerChainB2,          // WiFi output power delta for Group 2 EC Japan 5200 Chain B Offset
    //    WgdsWiFiSarDeltaGroup3PowerMax1,             // WiFi output power delta for Group 3 ROW 2400 MHZ Max Allowed
    //    WgdsWiFiSarDeltaGroup3PowerChainA1,          // WiFi output power delta for Group 3 ROW 2400 Chain A Offset
    //    WgdsWiFiSarDeltaGroup3PowerChainB1,          // WiFi output power delta for Group 3 ROW 2400 Chain B Offset
    //    WgdsWiFiSarDeltaGroup3PowerMax2,             // WiFi output power delta for Group 3 ROW 5200 MHZ Max Allowed
    //    WgdsWiFiSarDeltaGroup3PowerChainA2,          // WiFi output power delta for Group 3 ROW 5200 Chain A Offset
    //    WgdsWiFiSarDeltaGroup3PowerChainB2,          // WiFi output power delta for Group 3 ROW 5200 Chain B Offset
    //  }
    //})                        // End of WGDS object
    //
    Method(WGDS,0,Serialized)
    {
      Store(\SD11,Index (DeRefOf (Index (WGDY, 1)), 1)) // WiFi output power delta for Group 1 FCC 2400 MHZ Max Allowed
      Store(\SD12,Index (DeRefOf (Index (WGDY, 1)), 2)) // WiFi output power delta for Group 1 FCC 2400 Chain A Offset
      Store(\SD13,Index (DeRefOf (Index (WGDY, 1)), 3)) // WiFi output power delta for Group 1 FCC 2400 Chain B Offset
      Store(\SD14,Index (DeRefOf (Index (WGDY, 1)), 4)) // WiFi output power delta for Group 1 FCC 5200 MHZ Max Allowed
      Store(\SD15,Index (DeRefOf (Index (WGDY, 1)), 5)) // WiFi output power delta for Group 1 FCC 5200 Chain A Offset
      Store(\SD16,Index (DeRefOf (Index (WGDY, 1)), 6)) // WiFi output power delta for Group 1 FCC 5200 Chain B Offset
      Store(\SD21,Index (DeRefOf (Index (WGDY, 1)), 7)) // WiFi output power delta for Group 2 EC Japan 2400 MHZ Max Allowed
      Store(\SD22,Index (DeRefOf (Index (WGDY, 1)), 8)) // WiFi output power delta for Group 2 EC Japan 2400 Chain A Offset
      Store(\SD23,Index (DeRefOf (Index (WGDY, 1)), 9)) // WiFi output power delta for Group 2 EC Japan 2400 Chain B Offset
      Store(\SD24,Index (DeRefOf (Index (WGDY, 1)), 10)) // WiFi output power delta for Group 2 EC Japan 5200 MHZ Max Allowed
      Store(\SD25,Index (DeRefOf (Index (WGDY, 1)), 11)) // WiFi output power delta for Group 2 EC Japan 5200 Chain A Offset
      Store(\SD26,Index (DeRefOf (Index (WGDY, 1)), 12)) // WiFi output power delta for Group 2 EC Japan 5200 Chain B Offset
      Store(\SD31,Index (DeRefOf (Index (WGDY, 1)), 13)) // WiFi output power delta for Group 3 ROW 2400 MHZ Max Allowed
      Store(\SD32,Index (DeRefOf (Index (WGDY, 1)), 14)) // WiFi output power delta for Group 3 ROW 2400 Chain A Offset
      Store(\SD33,Index (DeRefOf (Index (WGDY, 1)), 15)) // WiFi output power delta for Group 3 ROW 2400 Chain B Offset
      Store(\SD34,Index (DeRefOf (Index (WGDY, 1)), 16)) // WiFi output power delta for Group 3 ROW 5200 MHZ Max Allowed
      Store(\SD35,Index (DeRefOf (Index (WGDY, 1)), 17)) // WiFi output power delta for Group 3 ROW 5200 Chain A Offset
      Store(\SD36,Index (DeRefOf (Index (WGDY, 1)), 18)) /// WiFi output power delta for Group 3 ROW 5200 Chain B Offset

      Return(WGDY)
    }
