/** @file
  ACPI DSDT table

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

// HD Audio I2S codecs
#define HDAC_I2S_DISABLED 0
#define HDAC_I2S_ALC298   1
#define HDAC_I2S_ALC286S  2
#define HDAC_I2S_SSM4567  3

// I2C Controller Number
#define SERIAL_IO_INDEX_I2C0 0
#define SERIAL_IO_INDEX_I2C1 1

Scope(\_SB.PCI0) {

  Name(HIDG, ToUUID("3CDFF6F7-4267-4555-AD05-B30A3D8938DE")) //GUID for HID-over-I2C
  Name(TP7G, ToUUID("EF87EB82-F951-46DA-84EC-14871AC6F84B")) //GUID for Touch device support in Windows7

  Method(HIDD, 5, Serialized) {
    // Common _DSM method for HID I2C devices
    // Arg0..3 - original arguments to _DSM method, as passed by OS
    // Arg4 - HID descriptor address to be returned from Function1
    If(LEqual(Arg0, HIDG)) {
      If(LEqual(Arg2, Zero)) { // Function 0 : Query Function
        If(LEqual(Arg1, One)) { // Revision 1
          Return(Buffer(One) { 0x03 })
        }
      }
      If(LEqual(Arg2, One)) { // Function 1 : HID Function
        Return (Arg4) // HID Descriptor Address (IHV Specific)
      }
    }
    return (Buffer(1){0})
  }

  Method(TP7D, 6, Serialized) {
    // Common _DSM method for touch devices support in Win7
    // Arg0..3 - original arguments to _DSM method, as passed by OS
    // Arg4..5 - buffers containing resources (such as I2c bus, interrupt) to be returned from Function1
     If(LEqual(Arg0, TP7G)) {
      If(LEqual(Arg2, Zero)) { // Function 0 : Query Function
        If(LEqual(Arg1, One)) { // Revision 1
          Return(Buffer(One) { 0x03 })
        }
      }
      If(LEqual(Arg2, One)) { // Function 1 : Report resources
          Return (ConcatenateResTemplate(Arg4, Arg5))
      }
    }
    Return(Buffer(1){0})
  }
}

Scope(\_SB.PCI0.I2C0) {
//------------------------
// Touch Pads on I2C0
// Note: instead of adding more touch devices, parametrize this one with appropriate _HID value and GPIO numbers
//------------------------
  Device (TPD0) {

    Name(HID2,0)
    Name (SBFB, ResourceTemplate () {
      I2cSerialBus (0x20,ControllerInitiated,400000,AddressingMode7Bit,"\\_SB.PCI0.I2C0",,,I2CB)
    })
    Name (SBFI, ResourceTemplate () {
      Interrupt(ResourceConsumer, Level, ActiveLow, ExclusiveAndWake,,,INTI) {0}
    })
    Name (SBFG, ResourceTemplate () {
      GpioInt (Level, ActiveLow, ExclusiveAndWake, PullDefault, 0x0000, "\\_SB.PCI0.GPI0", 0x00, ResourceConsumer, INTG) {0}
    })
    CreateWordField(SBFB,I2CB._ADR,BADR)
    CreateDWordField(SBFB,I2CB._SPE,SPED)
    CreateWordField(SBFG,INTG._PIN,INT1)
    CreateDWordField(SBFI,INTI._INT,INT2)

    Method(_INI) {
      if(LLess(OSYS,2012)) { SRXO(GPDI,1) }
      Store(GNUM(GPDI),INT1)
      Store(INUM(GPDI),INT2)
      If(LEqual(SDM0,0)) {
        SHPO(GPDI,1) // configure gpio pad in gpio driver mode
      }
      if(LEqual(SDS0,1)) { // Synaptics precision touchpad
        store("SYNA2393",_HID)
        store(0x20,HID2)
        return
      }
      if (LEqual(SDS0,2)) { // Synaptics forcepad
        store("06CB2846",_HID)
        store(0x20,HID2)
        return
      }
      If (LEqual(SDS0, 6)) { // ClickPad
        Store ("ALPS0000", _HID)
        Store (0x20, HID2)
        Store (0x2C, BADR)
        Return
      }
      if(LEqual(SDS0,5)) { // Custom touchpad
        store("CUST0001",_HID)
        store(TPDH,HID2)
        store(TPDB,BADR)
        if (LEqual(TPDS,0)) { store( 100000,SPED) }
        if (LEqual(TPDS,1)) { store( 400000,SPED) }
        if (LEqual(TPDS,2)) { store(1000000,SPED) }
        return
      }
    }

    Name (_HID, "XXXX0000")
    Name (_CID, "PNP0C50")
    Name (_S0W, 3)          // PTP will be in D3hot during CS, and wake capable
    Method(_DSM, 0x4, Serialized) {
      If(LEqual(Arg0,HIDG)) {Return(HIDD(Arg0,Arg1,Arg2,Arg3,HID2))}
      If(LEqual(Arg0,TP7G)) {Return(TP7D(Arg0,Arg1,Arg2,Arg3,SBFB,SBFG))}
      Return(Buffer(1){0})
    }

    Method (_STA) {
      If(LOr(LEqual(SDS0,5),LOr(LEqual(SDS0,1),LOr(LEqual(SDS0,2),LEqual(SDS0,6))))) { Return (0x0F) }
      Return (0x00)
    }

    Method (_CRS, 0, NotSerialized) {
      if(LLess(OSYS,2012)) { Return (SBFI) }
      if(LEqual(SDM0,0)) { Return (ConcatenateResTemplate(SBFB, SBFG)) }
      Return (ConcatenateResTemplate(SBFB, SBFI))
    }
  }  // Device (TPD0)


  //-----------------------------------
  //  HD Audio I2S Codec device
  //  Disabled               (I2SC = 0)
  //  Realtek ALC298         (I2SC = 1)
  //  Realtek ALC286S        (I2SC = 2)
  //  Analog Devices SSM4567 (I2SC = 3)
  //-----------------------------------
  Device (HDAC)
  {
    Name (_HID, "INT0000")
    Name (_CID, "INT0000")
    Name (_DDN, "Intel(R) Smart Sound Technology Audio Codec")
    Name (_UID, 1)
    Name (CADR, 0) // Codec I2C address

    Method(_INI) {
      If (LOr(LEqual(I2SC, HDAC_I2S_ALC298), LEqual(I2SC, HDAC_I2S_ALC286S))) { // ALC298/ALC286S
        Store ("INT343A", _HID)
        Store ("INT343A", _CID)
        Store (0x1C, CADR)
        Return
      }

      If (LEqual(I2SC, HDAC_I2S_SSM4567)) { // ADI SSM4567
        Store ("INT343B", _HID)
        Store ("INT343B", _CID)
        Store (0x34, CADR)
        Return
      }
    }

    Method (_CRS, 0, NotSerialized) {
      Name (SBFB, ResourceTemplate () {
        I2cSerialBus (0, ControllerInitiated, 400000, AddressingMode7Bit, "\\_SB.PCI0.I2C0",,,I2CA)
      })

      Name (SBFI, ResourceTemplate () {
        Interrupt (ResourceConsumer, Level, ActiveLow, ExclusiveAndWake,,,INTI) {0}
      })

      // Update I2C connection descriptor
      CreateWordField (SBFB, I2CA._ADR, ADR)
      Store (CADR, ADR)

      // Update APIC interrupt descriptor (GPP_E22 for jack detection)
      CreateDWordField (SBFI, INTI._INT, AINT)
      Store (INUM(GPIO_SKL_LP_GPP_E22), AINT)

      If (LEqual( HAID, 1)) { Return (SBFB) }

      Return (ConcatenateResTemplate(SBFB, SBFI))
    }

    Method (_STA, 0, NotSerialized) {
      If (LNotEqual(I2SC, HDAC_I2S_DISABLED)) {
        Return(0xF)  // I2S Codec Enabled
      }
      Return(0x0)  // I2S Codec Diabled
    }
  }  // Device (HDAC)

  // Expose UCMC devices under I2C0 based on setup option setting.
  Device (UCM1) {
    Name (_HID, "INT3515")
    Name (_UID, Zero)

    Method(_CRS, 0x0, NotSerialized) {
      Name (SBFB, ResourceTemplate () {
        // Port A TPS65982; I2C_ADD: GND; DEBUG_CTL1: High; DEBUG_CTL2: High; Address: 0x70/0x71 (0x38 if specifying the upper 7 bits)
        I2CSerialBus (0x38, ControllerInitiated, 400000, AddressingMode7Bit, "\\_SB.PCI0.I2C0", 0, ResourceConsumer,,)
      })

      Name (SBFI, ResourceTemplate () {
        Interrupt(ResourceConsumer, Level, ActiveLow, Exclusive, , , UCMI) {0}  //GPP_B_0_CORE_VID_0 INTSEL = 0x30
      })

      // Update APIC interrupt descriptor (GPP_B0)
      CreateDWordField (SBFI, UCMI._INT, GINT)
      Store (INUM(UCG1), GINT)

      Return (ConcatenateResTemplate(SBFB, SBFI))
    } // Method (_CRS)

    Method(_STA, 0x0, NotSerialized) {
      If(LEqual(UCSI, 1)) {
        Return (0x0F)
      } Else {
        Return (0x00)
      }
    } // Method (STA)
  } //Device (UCM1)

  // Expose UCMC devices under I2C0 based on setup option setting.
  Device (UCM2) {
    Name (_HID, "INT3515")
    Name (_UID, One)

    Method(_CRS, 0x0, NotSerialized) {
      Name (SBFB, ResourceTemplate () {
        // Port B TPS65982; I2C_ADD: GND; DEBUG_CTL1: High; DEBUG_CTL2: High; Address: 0x7E/0x7F (0x3F if specifying the upper 7 bits)
        I2CSerialBus (0x3F, ControllerInitiated, 400000, AddressingMode7Bit, "\\_SB.PCI0.I2C0", 0, ResourceConsumer,,)
      })

      Name (SBFI, ResourceTemplate () {
        Interrupt(ResourceConsumer, Level, ActiveLow, Exclusive, , , UCMI) {0}  //GPP_B_0_CORE_VID_1 INTSEL = 0x31
      })

      // Update APIC interrupt descriptor (GPP_B1)
      CreateDWordField (SBFI, UCMI._INT, GINT)
      Store (INUM(UCG2), GINT)

      Return (ConcatenateResTemplate(SBFB, SBFI))
    } // Method (_CRS)

    Method(_STA, 0x0, NotSerialized) {
      If(LEqual(UCSI, 1)) {
        Return (0x0F)
      } Else {
        Return (0x00)
      }
    } // Method (STA)
  } //Device (UCM2)
}

Scope(\_SB.PCI0.I2C1) {
//------------------------
// Touch Panels on I2C1
// Note: instead of adding more touch panels, parametrize this one with appropriate _HID value and GPIO numbers
//------------------------
  Device (TPL1) {
    Name(HID2,0)
    Name (SBFB, ResourceTemplate () {
      I2cSerialBus (0x4C,ControllerInitiated,400000,AddressingMode7Bit,"\\_SB.PCI0.I2C1",,,I2CB)
    })
    Name (SBFG, ResourceTemplate () {
      GpioInt (Level, ActiveLow, Exclusive, PullDefault, 0x0000, "\\_SB.PCI0.GPI0", 0x00, ResourceConsumer,INTG ) { 0 }
    })
    Name (SBFI, ResourceTemplate () {
      Interrupt(ResourceConsumer, Level, ActiveLow, Exclusive,,,INTI) {0}
    })
    CreateWordField(SBFB,I2CB._ADR,BADR)
    CreateDWordField(SBFB,I2CB._SPE,SPED)
    CreateDWordField(SBFI,INTI._INT,INT2)
    CreateWordField (SBFG,INTG._PIN,INT1)

    Method(_INI) {
      if(LLess(OSYS,2012)) { SRXO(GPLI,1) }
      Store(GNUM(GPLI),INT1)
      Store(INUM(GPLI),INT2)
      If(LEqual(SDM1,0)) {
        SHPO(GPLI,1) // configure gpio pad in gpio driver mode
      }
      if(LEqual(SDS1,1)) { // Atmel touchpanel
        store("ATML3432",_HID)
        store(0,HID2)
        store(0x4C,BADR)
        store(400000,SPED)
        return
      }
      if(LEqual(SDS1,2)) { // Atmel touchpanel
        store("ATML2952",_HID)
        store(0,HID2)
        store(0x4A,BADR)
        store(400000,SPED)
        return
      }
      if(LEqual(SDS1,3)) { // Elan touchpanel
        store("ELAN2097",_HID)
        store(1,HID2)
        store(0x10,BADR)
        store(400000,SPED)
        return
      }
      if (LEqual(SDS1,4)) { // NTRIG Samsung touch panel (Ntrig-BDW)
        store("NTRG0001",_HID)
        store(1,HID2)
        store(0x07,BADR)
        store(400000,SPED)
        return
      }
      if (LEqual(SDS1,5)) { // NTRIG Sharp touch panel (Ntrig-SKL)
        store("NTRG0002",_HID)
        store(1,HID2)
        store(0x64,BADR)
        store(400000,SPED)
        return
      }
      if(LEqual(SDS1,6)) { // WACOM 9015 touch panel
        store("WCOM508E",_HID)
        store(1,HID2)
        store(0x0A,BADR)
        if (LEqual(TPLS,0)) { store( 400000,SPED) } // Fast mode
        if (LEqual(TPLS,1)) { store(1000000,SPED) } // Fast mode+
        return
      }
      if(LEqual(SDS1,7)) { // Custom touchpanel
        store("CUST0000",_HID)
        store(TPLH,HID2)
        store(TPLB,BADR)
        if (LEqual(TPLS,0)) { store( 100000,SPED) }
        if (LEqual(TPLS,1)) { store( 400000,SPED) }
        if (LEqual(TPLS,2)) { store(1000000,SPED) }
        return
      }
// AMI_OVERRIDE_START - For Skylake MRD support
#if defined(ASL_SkylakeMRD_SUPPORT) && (ASL_SkylakeMRD_SUPPORT == 1) 
      if(LEqual(SDS1,7)) { // eKTH3912 
        store("ELAN0732",_HID)
        store(1,HID2)
        store(0x10,BADR)
        store(400000,SPED)
        return
      }
#endif
// AMI_OVERRIDE_END - For Skylake MRD support
    }

    Name (_HID, "XXXX0000")
    Name (_CID, "PNP0C50")
    Name (_S0W, 4)
    Method(_DSM, 0x4, Serialized) {
      If(LEqual(Arg0,HIDG)) {Return(HIDD(Arg0,Arg1,Arg2,Arg3,HID2))}
      If(LEqual(Arg0,TP7G)) {Return(TP7D(Arg0,Arg1,Arg2,Arg3,SBFB,SBFG))}
      Return(Buffer(1){0})
    }

    Method (_STA) {
      If(LNotEqual(SDS1,0)) { Return (0x0F) }
      Return (0x00)
    }

    Method (_CRS, 0, NotSerialized) {
      If(LLess(OSYS,2012)) { return (SBFI) } // For Windows 7 only report Interrupt; it doesn't support ACPI5.0 and wouldn't understand GpioInt nor I2cBus
      If(LEqual(SDM1,0)) { Return (ConcatenateResTemplate(SBFB, SBFG)) }
      Return (ConcatenateResTemplate(SBFB, SBFI))
    }
  } // Device (TPL0)

//------------------------
// RFID on I2C1
//------------------------
  Device (IMP3)
  {
    Name (_ADR, Zero)               // _ADR: Address
    Name (_HID, "IMPJ0003")         // _HID: Hardware ID
    Name (_CID, "IMPJ0003")         // _CID: Compatible ID
    Name (_UID, One)                // _UID: Unique ID

    Method(_STA, 0x0, NotSerialized) {
      If(LEqual(HAID,1)) { Return(0x0F) }
      Return(0x00)
    }

    Method (_CRS, 0, NotSerialized) {
      Name (SBUF, ResourceTemplate () {
        I2cSerialBus (0x6E,ControllerInitiated,400000,AddressingMode7Bit,"\\_SB.PCI0.I2C1",0x00,ResourceConsumer,,)
      })
      Return (SBUF)
    }
  } // Device (IMP3)
}

//
// AdvancedFeaturesBegin
//

//-----------------------------
//  I2C Power Meter Controller
//-----------------------------

if (LEqual(PMTC, SERIAL_IO_INDEX_I2C0)) {
  Scope(\_SB.PCI0.I2C0) {
#ifdef I2CS
#undef I2CS
#endif
#ifdef PAC_01_I2C_SLAVE_ADDRESS
#undef PAC_01_I2C_SLAVE_ADDRESS
#endif
#ifdef PAC_02_I2C_SLAVE_ADDRESS
#undef PAC_02_I2C_SLAVE_ADDRESS
#endif
#ifdef PAC_03_I2C_SLAVE_ADDRESS
#undef PAC_03_I2C_SLAVE_ADDRESS
#endif

#define I2CS "\\_SB.PCI0.I2C0"          // I2CS: I2C ACPI PATH String
#define PAC_01_I2C_SLAVE_ADDRESS 0x18   // PAC193x Microchip Device 01 7-bit Slave Address
#define PAC_02_I2C_SLAVE_ADDRESS 0x19   // PAC193x Microchip Device 02 7-bit Slave Address
#define PAC_03_I2C_SLAVE_ADDRESS 0x1a   // PAC193x Microchip Device 03 7-bit Slave Address

    Name (IBUS, SERIAL_IO_INDEX_I2C0)   // IBUS: I2C BUS Number

    if (LEqual(P193, 1)) {
      //
      // PAC193x Microchip As Power Meter
      //
      //AMI_OVERRIDE_START - Fix GCC Build Failed
      //Include ("PAC193x.asl")
      Include ("PAC193x.ASL")
      //AMI_OVERRIDE_END - Fix GCC Build Failed
    }
#undef I2CS
#undef PAC_01_I2C_SLAVE_ADDRESS
#undef PAC_02_I2C_SLAVE_ADDRESS
#undef PAC_03_I2C_SLAVE_ADDRESS
  }
}

if (LEqual(PMTC, SERIAL_IO_INDEX_I2C1)) {
  Scope(\_SB.PCI0.I2C1) {
#ifdef I2CS
#undef I2CS
#endif
#ifdef PAC_01_I2C_SLAVE_ADDRESS
#undef PAC_01_I2C_SLAVE_ADDRESS
#endif
#ifdef PAC_02_I2C_SLAVE_ADDRESS
#undef PAC_02_I2C_SLAVE_ADDRESS
#endif
#ifdef PAC_03_I2C_SLAVE_ADDRESS
#undef PAC_03_I2C_SLAVE_ADDRESS
#endif

#define I2CS "\\_SB.PCI0.I2C1"          // I2CS: I2C ACPI PATH String
#define PAC_01_I2C_SLAVE_ADDRESS 0x18   // PAC193x Microchip Device 01 7-bit Slave Address
#define PAC_02_I2C_SLAVE_ADDRESS 0x19   // PAC193x Microchip Device 02 7-bit Slave Address
#define PAC_03_I2C_SLAVE_ADDRESS 0x1a   // PAC193x Microchip Device 03 7-bit Slave Address

    Name (IBUS, SERIAL_IO_INDEX_I2C1)   // IBUS: I2C BUS Number

    if (LEqual(P193, 1)) {
      //
      // PAC193x Microchip As Power Meter
      //
      //AMI_OVERRIDE_START - Fix GCC Build Failed
      //Include ("PAC193x.asl")
      Include ("PAC193x.ASL")
      //AMI_OVERRIDE_END - Fix GCC Build Failed
    }
#undef I2CS
#undef PAC_01_I2C_SLAVE_ADDRESS
#undef PAC_02_I2C_SLAVE_ADDRESS
#undef PAC_03_I2C_SLAVE_ADDRESS
  }
}

//
// AdvancedFeaturesEnd
//

//-----------------------------
//  Serial IO SPI1 Controller
//-----------------------------
Scope(\_SB.PCI0.SPI1)
{
  Device(FPNT) {
    Method(_HID) {
      // Return FPS HID based on BIOS Setup
      if(LEqual(SDS7,1)){ Return("FPC1011") }
      if(LEqual(SDS7,2)){ Return("FPC1020") }
      if(LEqual(SDS7,3)){ Return("VFSI6101") }
      if(LEqual(SDS7,4)){ Return("VFSI7500") }
      if(LEqual(SDS7,5)){ Return("EGIS0300") }
      if(LEqual(SDS7,6)){ Return("FPC1021") }
      Return("FPNT_DIS")
    }
    Method(_INI) {
      SHPO(GFPI,1) // configure gpio pad in gpio driver mode
      SHPO(GFPS,1) // configure gpio pad in gpio driver mode
    }
    Method(_STA) {
      // Is SerialIo SPI1 FPS enabled in BIOS Setup?
      If(LNotEqual(SDS7, 0)) {
        Return(0x0F)
      }
      Return(0x00)
    }
    Method(_CRS, 0x0, Serialized) {
      Name(BBUF,ResourceTemplate () {
        SPISerialBus(0x00,PolarityLow,FourWireMode,8,ControllerInitiated,10000000,ClockPolarityLow,ClockPhaseFirst,"\\_SB.PCI0.SPI1",,,SPIR)
        GpioIo(Exclusive, PullDefault, 0, 0, IoRestrictionOutputOnly, "\\_SB.PCI0.GPI0",,,GSLP) {8}
      })
      Name(IBUF,ResourceTemplate () {
        Interrupt(ResourceConsumer, Level, ActiveLow, ExclusiveAndWake,,,IINT) {0}
      })
      Name (GBUF, ResourceTemplate () {
        GpioInt (Level, ActiveLow, ExclusiveAndWake, PullDefault, 0x0000, "\\_SB.PCI0.GPI0", 0x00, ResourceConsumer,GINT ) { 0 }
      })
      Name(UBUF,ResourceTemplate () {
        GpioIo(Exclusive, PullDefault, 0, 0, IoRestrictionInputOnly, "\\_SB.PCI0.GPI0",,,GIRQ) {0}
      })

      CreateDWordField(BBUF, SPIR._SPE, SPEX)     // Connection Speed
      CreateByteField(BBUF, SPIR._PHA, PHAX)      // Clock Phase
      CreateWordField(BBUF,GSLP._PIN,SPIN)
      CreateWordField(GBUF,GINT._PIN,GPIN)
      CreateDWordField(IBUF,IINT._INT,IPIN)
      CreateBitField(IBUF,IINT._LL,ILVL)
      CreateBitField(IBUF,IINT._HE,ITRG)
      CreateField(GBUF,GINT._POL,2,GLVL)
      CreateBitField(GBUF,GINT._MOD,GTRG)
      CreateBitField(BBUF,SPIR._DPL,SCSP)  // chipselect polarity
      CreateWordField(UBUF,GIRQ._PIN,UPIN)

      Store(GNUM(GFPS),SPIN)
      Store(GNUM(GFPI),GPIN)
      Store(INUM(GFPI),IPIN)
      Store(GNUM(GFPI),UPIN)
      if(LOr(LEqual(SDS7,2), LEqual(SDS7,6))) { //FPC1020/FPC1021 has rising-edge interrupts, instead of the default level-low
        Store(0,ILVL) // interrupt active high
        Store(1,ITRG) // interrupt edge triggered
        Store(0,GLVL) // gpioint active high
        Store(1,GTRG) // gpioint edge triggered
      }
      if(LEqual(SDS7,4)) { //VFS7500 has rising-edge interrupts, instead of the default level-low
        Store(0,ILVL) // interrupt active high
        Store(1,ITRG) // interrupt edge triggered
      }
      //Store(?,SCSP) // update chipselect polarity per fingerprint's version, once we receive information which FPS requires which polarity

      // Update SPI interface settings based on BIOS Setup
      Switch(ToInteger(SDS7)) {
        Case (1) { // FPC1011
          Store(10000000, SPEX) // Connection Speed
          Store(0, PHAX) // First Phase
        }
        Case (2) { // FPC1020
          Store(3000000, SPEX) // Connection Speed 0x2Dc6C0
          Store(0, PHAX) // First Phase
        }
        Case (3) { // VFS6101
          Store(8000000, SPEX) // Connection Speed 0x7A1200
          Store(1, PHAX) // Second Phase
        }
        Case (4) { // VFS7500
          Store(8000000, SPEX) // Connection Speed 0x7A1200
          Store(0, PHAX) // First Phase
        }
        Case (5) { // EGIS0300
          Store(16000000, SPEX) // Connection Speed 0xF42400
          Store(0, PHAX) // First Phase
        }
        Case (6) { // FPC1021
          Store(3000000, SPEX) // Connection Speed 0x2Dc6C0
          Store(0, PHAX) // First Phase
        }
        Default {
        }
      }

      if(LEqual(SDS7,1)) { Return (BBUF) } // FPC1011 doesn't use interrupts
      if(LAnd(LEqual(SDS7,4), LEqual(SDM7,0))) {  Return (ConcatenateResTemplate(BBUF, ConcatenateResTemplate(UBUF, GBUF))) } // VFS7500, GpioInt
      if(LAnd(LEqual(SDS7,4), LNotEqual(SDM7,0))) { Return (ConcatenateResTemplate(BBUF, ConcatenateResTemplate(UBUF, IBUF))) } // VFS7500, Interrupt
      if(LEqual(SDM7,0)) { Return (ConcatenateResTemplate(BBUF, GBUF)) } // GpioInt
      Return (ConcatenateResTemplate(BBUF, IBUF))  //Interrupt
    }
  } // Device (FPNT)
}

//-----------------------------
//  Serial IO UART0 Controller
//-----------------------------
Scope(\_SB.PCI0.UA00)
{
  //
  // BroadCom Bluetooth controller using serial interface
  //
  Device(BTH0){
    Method(_HID) {
      // Return HID based on BIOS Setup
      if(LEqual(SDS8,1)){ // Snowfield Peak
        Return("INT33E1")
        }
      if(LEqual(SDS8,2)){ Return("BCM2E40") } // Broadcom BRCM43241, 2E40 means Rev4
      Return("INT33E1")
    }

    Method(_INI) {
      SHPO(GBTI,1) // configure gpio pad in gpio driver mode; Bluetooth Interrupt
      SHPO(GBTW,1) // configure gpio pad in gpio driver mode; Bluetooth Wake
      SHPO(GBTK,1) // configure gpio pad in gpio driver mode; Bluetooth RF-Kill
    }

    Method(_CRS, 0x0, NotSerialized){
      Name(SBFG, ResourceTemplate (){
        UARTSerialBus(115200,,,0xc0,,,FlowControlHardware,32,32,"\\_SB.PCI0.UA00" )
        GpioIo(Exclusive, PullDefault, 0, 0, IoRestrictionOutputOnly, "\\_SB.PCI0.GPI0",,,WAK1 ) {0}
        GpioIo(Exclusive, PullDefault, 0, 0, IoRestrictionOutputOnly, "\\_SB.PCI0.GPI0",,,KIL1 ) {0}
        GpioInt (Edge, ActiveLow, ExclusiveAndWake, PullDefault, 0x0000, "\\_SB.PCI0.GPI0", 0x00, ResourceConsumer, INT1) { 0 }
      })
      CreateWordField(SBFG,INT1._PIN,INT3)
      CreateWordField(SBFG,WAK1._PIN,WAK3)
      CreateWordField(SBFG,KIL1._PIN,KIL3)
      Store(GNUM(GBTI),INT3)
      Store(GNUM(GBTW),WAK3)
      Store(GNUM(GBTK),KIL3)

      Name(SBFI, ResourceTemplate (){
        UARTSerialBus(115200,,,0xc0,,,FlowControlHardware,32,32,"\\_SB.PCI0.UA00" )
        GpioIo(Exclusive, PullDefault, 0, 0, IoRestrictionOutputOnly, "\\_SB.PCI0.GPI0",,,WAK2 ) {0}
        GpioIo(Exclusive, PullDefault, 0, 0, IoRestrictionOutputOnly, "\\_SB.PCI0.GPI0",,,KIL2 ) {0}
        Interrupt(ResourceConsumer, Edge, ActiveLow, ExclusiveAndWake,,,INT2 ) {0}
      })
      CreateDWordField(SBFI,INT2._INT,INT4)
      CreateWordField(SBFI,WAK2._PIN,WAK4)
      CreateWordField(SBFI,KIL2._PIN,KIL4)
      Store(INUM(GBTI),INT4)
      Store(GNUM(GBTW),WAK4)
      Store(GNUM(GBTK),KIL4)

      If(LEqual(SDM8,0)) {
        Return (SBFG)
      } Else {
        Return (SBFI)
      }
    }

    Method (_STA, 0x0, NotSerialized){
      If(LNotEqual(SDS8,0)) { Return (0x0F) }
      Return (0x00)
    }
    Name (_S0W, 2)                            // required to put the device to D2 during S0 idle
  } // Device BTH0

}  //  end Scope(\_SB.PCI0.UART0)

Scope(\_SB.PCI0) {
  Device(GNSS) {
    Name(_HID, "INT33A2")

    Method(_CRS, 0x0, NotSerialized){
      Name(BUF1, ResourceTemplate (){
        UARTSerialBus(115200,DataBitsEight,StopBitsOne,0xc0,LittleEndian,ParityTypeNone,FlowControlHardware,64,64,"\\_SB.PCI0.UA01" )
      })
      Name(BUF2, ResourceTemplate(){
        GpioIo(Exclusive, PullDefault, 0, 0, IoRestrictionOutputOnly, "\\_SB.PCI0.GPI0",,,RSTP ) {0}
      })

      CreateWordField(BUF2,RSTP._PIN,RPIN)
      Store(GNUM(GGNR),RPIN)

      if (LEqual(GNSC,1)) { // GNSS connected to SerialIO UART - need to report UART bus as one of resources
        Return (ConcatenateResTemplate(BUF1,BUF2))
      } else { // GNSS connected to ISH UART - no need to report UART bus
        Return (BUF2)
      }
    }

    Method (_STA, 0x0, NotSerialized){
      If(LEqual(GNSC,0)) { Return (0x00) }
      Return (0x0F)
    }
  }
} //scope UART01

// GPIO expander

Scope(\_SB.PCI0) {
  Device(GEXP) {

    Name(_ADR,1)
    Name(_STA,0xB)

    OperationRegion(BAR0, SystemMemory, SB04, 0x208)
    Field(BAR0, DWordAcc, NoLock, Preserve) {
      Offset(0x0),  ICON, 32,
      Offset(0x4),  TAR,  32,
      Offset(0x10), DATA, 32,
      Offset(0x14), HCNT, 32,
      Offset(0x18), LCNT, 32,
      Offset(0x2C),     , 5,  ABRT,1,
      Offset(0x40), RBCK, 32,
      Offset(0x54), CLR,  32,
      Offset(0x6C), ENB , 1,
      Offset(0x70), ACTV, 1,  TFNF,1,  ,1, RFNE,1,
      Offset(0x7C), HOLD, 32,
      Offset(0x9C), ENSB, 1,
      Offset(0x204), RST, 32
    }

    //
    // There are only 4 public functions that may be called by external code:
    // SGEP(ctrl number, expander number, pin number, state)     - set pin value(0 low, 1 high)
    // SGED(ctrl number, expander number, pin number, direction) - set pin direction (0 output, 1 input)
    // GEPS(ctrl number, expander number, pin number)            - get pin value
    // INVC()                                                    - invalidate cache
    // Everything else inside GEXP is internal implementation, not to be called from outside
    //

    // SGEP - method for setting expander's pin state.
    // Arg0=expander number, Arg1=pin number, Arg2=state
    Method(SGEP, 3, Serialized) {
      CSER(GEXN,Arg0,Arg1,Arg2,0)
    }

    // SGED - method for setting expander's pin direction.
    // Arg0=expander number, Arg1=pin number, Arg2=direction(0=out, 1=in)
    Method(SGED, 3, Serialized) {
      CSER(GEXN,Arg0,Arg1,Arg2,1)
    }

    // GEPS - method for reading expander's pin state
    // Arg0=expander number, Arg1=pin number
    Method(GEPS, 2, Serialized) {
      return(CSER(GEXN,Arg0,Arg1,0,2))
    }

    // SGEI - method for setting expander's pin inversion.
    // Arg0=expander number, Arg1=pin number, Arg2=polarity(0=no inversion)
    Method(SGEI, 3, Serialized) {
      CSER(GEXN,Arg0,Arg1,Arg2,3)
    }

    // INVC - method for invalidating cache.
    // Useful after S3/S4 when hardware state becomes different that what was remembered in cache
    Method(INVC, 0) {
      Store(0,Local0)
      while(LLess(Local0,16)) {
        // if cache state is 2(cached), make it 1(not initialized)
        if(LEqual(R3DC(0,Local0,0),2)) { W3DC(0,Local0,0,1) } // Expander1
        if(LEqual(R3DC(1,Local0,0),2)) { W3DC(1,Local0,0,1) } // Expander2
        Add(Local0,1,Local0)
      }
    }

    //
    // Internal GEXP implementation - do not call these methods from outside
    //
    Name(PPR,8)                           // pins per register
    Name(INR,  Package(3){0x0, 0x1, 0x2}) // input registers
    Name(OUTR, Package(3){0x4, 0x5, 0x6}) // output registers
    Name(CFGR, Package(3){0xC, 0xD, 0xE}) // config registers
    Name(POLR, Package(3){0x8, 0x9, 0xA}) // polarity registers
    Name(EXPA, 0x22)                      // expander's i2c address
    Name(UCCH, 1)                         // UseCaCHe, 0=no, 1=yes
    Name(END,0x200)                       // end transfer
    Name(READ,0x100)                      // read transfer
    Name(TEMP,0)                          // scratch variable that does not cause compiler warnings as writes to Localx variables do.

    //cached register values
    //cache indexed by their parent expander and register address
    //each register is described by its cacheability state and cached value
    //state = 0(volatile), 1(cacheable but not initialized), 2(cached), 3(does not exist)
    Name(CACH, Package(2) {
      Package(16) {      //for expander0
        Package(2) {0,0},//input0
        Package(2) {0,0},//input1
        Package(2) {0,0},//input2
        Package(2) {3,0},//na
        Package(2) {1,0},//output0
        Package(2) {1,0},//output1
        Package(2) {1,0},//output2
        Package(2) {3,0},//na
        Package(2) {1,0},//polarity0
        Package(2) {1,0},//polarity1
        Package(2) {1,0},//polarity2
        Package(2) {3,0},//na
        Package(2) {1,0},//config0
        Package(2) {1,0},//config1
        Package(2) {1,0},//config2
        Package(2) {3,0}//na
      },
      Package(16) {      //for expander1
        Package(2) {0,0},//input0
        Package(2) {0,0},//input1
        Package(2) {0,0},//input2
        Package(2) {3,0},//na
        Package(2) {1,0},//output0
        Package(2) {1,0},//output1
        Package(2) {1,0},//output2
        Package(2) {3,0},//na
        Package(2) {1,0},//polarity0
        Package(2) {1,0},//polarity1
        Package(2) {1,0},//polarity2
        Package(2) {3,0},//na
        Package(2) {1,0},//config0
        Package(2) {1,0},//config1
        Package(2) {1,0},//config2
        Package(2) {3,0}//na
      },
    })

    Method(W3DC,4) {
    //Write 3-Dimensional Cache table
    //Arg0-2: indexes, Arg3-value
      Store(Arg3, Index(DeRefOf(Index(DeRefOf(Index(CACH,Arg0)),Arg1)),Arg2)) //CACH[Arg0][Arg1][Arg2] := Arg3
    }

    Method(R3DC,3) {
    //Read 3-Dimensional Cache table
    //Arg0-2: indexes
      return(DeRefOf(Index(DeRefOf(Index(DeRefOf(Index(CACH,Arg0)),Arg1)),Arg2))) //return CACH[Arg0][Arg1][Arg2]
    }

    // WriteREGister, Arg0=BAR0, Arg1=Expander number, Arg2=register address, Arg3=value to write
    Method(WREG,4,Serialized) {
      Store(Add(Timer(),50000),Local1) // Local0 = cutoff time(5ms)
      Store(7,RST)
      Store(0,ENB)
      Store(RBCK,TEMP)
      Store(CLR,TEMP)
      Store(0x1C001C, HOLD)
      Store(528,HCNT)
      Store(640,LCNT)
      Store(Add(EXPA,Arg1),TAR)
      Store(0x65,ICON)
      Store(1,ENB)
      while(LNotEqual(ENSB,0x1)) {           // poll for enable complete
        if(LGreater(Timer(),Local1)){return(0)} // abort if it takes too long.
      }
      Store(Arg2,DATA)
      Store(Add(END,Arg3),DATA)
      while(LNotEqual(ACTV,0x0)) {           // poll for transfer finished
        if(LGreater(Timer(),Local1)){return(0)} // abort if it takes too long.
      }
      Store(0,ENB)
      while(LNotEqual(ENSB,0x0)) {           // poll for disable complete
        if(LGreater(Timer(),Local1)){return(0)} // abort if it takes too long.
      }
      //update cache
      W3DC(Arg1,Arg2,1,Arg3)
      If(LEqual(R3DC(Arg1,Arg2,0),1)) { W3DC(Arg1,Arg2,0,2) }
      return(0)
    }

    // ReadREGister, Arg0=BAR0, Arg1=Expander number, Arg2=register address
    Method(RREG,3,Serialized) {
      if(LEqual(UCCH,1)){ //use cache?
        If(LEqual(R3DC(Arg1,Arg2,0),2)) {return(R3DC(Arg1,Arg2,1))} //check if requested value is available in cache
      }
      Store(Add(Timer(),50000),Local1) // Local0 = cutoff time(5ms)
      Store(7,RST)
      Store(0,ENB)
      Store(RBCK,TEMP)
      Store(CLR,TEMP)
      Store(0x1C001C, HOLD)
      Store(528,HCNT)
      Store(640,LCNT)
      Store(Add(EXPA,Arg1),TAR)
      Store(0x65,ICON)
      Store(1,ENB)
      while(LNotEqual(ENSB,0x1)) {           // poll for enable complete
        if(LGreater(Timer(),Local1)){return(0)} // abort if it takes too long.
      }
      Store(Arg2,DATA)
      Store(Add(END,READ),DATA)
      while(LNotEqual(ACTV,0x0)) {           // poll for transfer finished
        if(LGreater(Timer(),Local1)){return(0)} // abort if it takes too long.
      }
      Store(DATA,Local0)
      Store(0,ENB)
      while(LNotEqual(ENSB,0x0)) {           // poll for disable complete
        if(LGreater(Timer(),Local1)){return(0)} // abort if it takes too long.
      }
      return (Local0)
    }

    //PS0 - method for putting IoExpander's controller in D0
    //Arg0=BAR1 of controller,
    Method(PS0, 1, Serialized) {
      OperationRegion(BAR1, SystemMemory, Arg0, 0x88)
      Field(BAR1, DWordAcc, NoLock, Preserve) {
        Offset(0x84), D0D3, 2
      }
      Store(0,D0D3)      // put controller in D0
      Store(D0D3,TEMP) // perform read after write to ensure bus coherency
    }

    Method(PS3, 1, Serialized) {
      OperationRegion(BAR1, SystemMemory, Arg0, 0x88)
      Field(BAR1, DWordAcc, NoLock, Preserve) {
        Offset(0x84), D0D3, 2
      }
      Store(3, D0D3)     // put controller in D3
      Store(D0D3,TEMP) // perform read after write to ensure bus coherency
    }

    // CSER - Common Set Expander register
    // Arg0=ctrl,Arg1=exp,Arg2=pin,Arg3=value,Arg4=function(set value,set direction,read,polarity)
    Method(CSER, 5, Serialized) {
      //constants
      //variables
      Name(SB1X,0) //BAR1 of selected controller
      Name(SB0X,0) //BAR0 of selected controller
      Name(SMDX,0) //MODE of selected controller
      Name(PINN,0) //pin number inside register
      Name(REGN,0) //register number inside register bank
      Name(REGA,0) //register address
      Name(OLDV,0) //old register value (retrieved from expander)
      Name(NEWV,0) //new register value (to be written to expander)
      Name(RETV,0) //return value

      If(LGreater(Arg0,5)) {return (0) } //incorrect controller number - abort

      // store BAR0, BAR1 and MODE to common variables
      If(LEqual(Arg0,0)){Store(SB10,SB1X);Store(SB00,SB0X);Store(SMD0,SMDX)}
      If(LEqual(Arg0,1)){Store(SB11,SB1X);Store(SB01,SB0X);Store(SMD1,SMDX)}
      If(LEqual(Arg0,2)){Store(SB12,SB1X);Store(SB02,SB0X);Store(SMD2,SMDX)}
      If(LEqual(Arg0,3)){Store(SB13,SB1X);Store(SB03,SB0X);Store(SMD3,SMDX)}
      If(LEqual(Arg0,4)){Store(SB14,SB1X);Store(SB04,SB0X);Store(SMD4,SMDX)}
      If(LEqual(Arg0,5)){Store(SB15,SB1X);Store(SB05,SB0X);Store(SMD5,SMDX)}

      // check argument correctness
      if(LGreater(Arg0,5)) {return (0)}
      if(LGreater(Arg1,1)) {return (0)}
      if(LGreater(Arg2,23)){return (0)}
      if(LGreater(Arg3,1)) {return (0)}
      if(LGreater(Arg4,2)) {return (0)}

      if(LNotEqual(SMDX,3)){return (0)} // host controller in wrong state, abort

      // select register bank
      if(LEqual(Arg4,0)) {Store(OUTR,Local0)}
      if(LEqual(Arg4,1)) {Store(CFGR,Local0)}
      if(LEqual(Arg4,2)) {Store(INR, Local0)}
      if(LEqual(Arg4,3)) {Store(POLR,Local0)}

      PS0(SB1X)
      Divide(Arg2,PPR,PINN,REGN)                    // calculate pin number (PINN) and register number
      Store(DeRefOf(Index(Local0,REGN)),REGA)       // calculate register address based on register bank and register number

      Store(RREG(SB0X,Arg1,REGA),OLDV)              // get old register value
      if(LEqual(Arg4,2)) {
        Store (And(0x1,ShiftRight(OLDV,PINN)),RETV) //if this is read access, return single bit value
      } else {
        And(OLDV,Not(ShiftLeft(0x1,PINN)),NEWV)     // clear affected bit
        Or(ShiftLeft(Arg3,PINN),NEWV,NEWV)          // if Arg1=1, set affected bit
        If(LNotEqual(NEWV,OLDV)) {
          WREG(SB0X,Arg1,REGA,NEWV)                 // write new register value only if it is different from old one
        }
      }
      PS3(SB1X)
      return (RETV)
    }

  }
}

