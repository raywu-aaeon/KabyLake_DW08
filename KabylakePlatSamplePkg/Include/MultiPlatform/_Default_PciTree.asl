/** @file

Copyright (c) 2011, Intel Corporation. All rights reserved.<BR>
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

**/

//
// Automatically generated by ACE ver 1.59 PIRQTRANS 
// Please DO NOT modify !!!
//

//---------------------------------------------------------------------------
// Begin System Bus Scope
//---------------------------------------------------------------------------
Scope (\_SB) {
// Host Bridge
	Name(PR00, Package(){
// P.E.G. Root Port D1F0
// P.E.G. Root Port D1F1
// P.E.G. Root Port D1F2
		Package(){ 0x0001FFFF, 0, LNKA, 0 },
		Package(){ 0x0001FFFF, 1, LNKB, 0 },
		Package(){ 0x0001FFFF, 2, LNKC, 0 },
		Package(){ 0x0001FFFF, 3, LNKD, 0 },
// I.G.D.
		Package(){ 0x0002FFFF, 0, LNKA, 0 },
// SA Audio Device
		Package(){ 0x0003FFFF, 0, LNKA, 0 },
// SA Thermal Device
		Package(){ 0x0004FFFF, 0, LNKA, 0 },
// SA Chap Device
		Package(){ 0x0007FFFF, 0, LNKA, 0 },
// Integrated Sensor Hub(ISH)
		Package(){ 0x0013FFFF, 0, LNKA, 0 },
// USB 3.0 xHCI Controller
		Package(){ 0x0014FFFF, 0, LNKA, 0 },
		Package(){ 0x0014FFFF, 1, LNKB, 0 },
		Package(){ 0x0014FFFF, 2, LNKC, 0 },
		Package(){ 0x0014FFFF, 3, LNKD, 0 },
// ME
		Package(){ 0x0016FFFF, 0, LNKA, 0 },
		Package(){ 0x0016FFFF, 1, LNKB, 0 },
		Package(){ 0x0016FFFF, 2, LNKC, 0 },
		Package(){ 0x0016FFFF, 3, LNKD, 0 },
// SATA Controller
		Package(){ 0x0017FFFF, 0, LNKA, 0 },
// PCIE Root Port #17
		Package(){ 0x001BFFFF, 0, LNKA, 0 },
		Package(){ 0x001BFFFF, 1, LNKB, 0 },
		Package(){ 0x001BFFFF, 2, LNKC, 0 },
		Package(){ 0x001BFFFF, 3, LNKD, 0 },
// PCIE Root Port #1
		Package(){ 0x001CFFFF, 0, LNKA, 0 },
		Package(){ 0x001CFFFF, 1, LNKB, 0 },
		Package(){ 0x001CFFFF, 2, LNKC, 0 },
		Package(){ 0x001CFFFF, 3, LNKD, 0 },
// PCIE Root Port #9
		Package(){ 0x001DFFFF, 0, LNKA, 0 },
		Package(){ 0x001DFFFF, 1, LNKB, 0 },
		Package(){ 0x001DFFFF, 2, LNKC, 0 },
		Package(){ 0x001DFFFF, 3, LNKD, 0 },
// LPC Bridge
		Package(){ 0x001FFFFF, 0, LNKA, 0 },
		Package(){ 0x001FFFFF, 1, LNKB, 0 },
		Package(){ 0x001FFFFF, 2, LNKC, 0 },
		Package(){ 0x001FFFFF, 3, LNKD, 0 },
	})
// Host Bridge
	Name(AR00, Package(){
// P.E.G. Root Port D1F0
// P.E.G. Root Port D1F1
// P.E.G. Root Port D1F2
		Package(){ 0x0001FFFF, 0, 0, 16 },
		Package(){ 0x0001FFFF, 1, 0, 17 },
		Package(){ 0x0001FFFF, 2, 0, 18 },
		Package(){ 0x0001FFFF, 3, 0, 19 },
// I.G.D.
		Package(){ 0x0002FFFF, 0, 0, 16 },
// SA Audio Device
		Package(){ 0x0003FFFF, 0, 0, 16 },
// SA Thermal Device
		Package(){ 0x0004FFFF, 0, 0, 16 },
// SA Chap Device
		Package(){ 0x0007FFFF, 0, 0, 16 },
// Integrated Sensor Hub(ISH)
		Package(){ 0x0013FFFF, 0, 0, 20 },
// USB 3.0 xHCI Controller
		Package(){ 0x0014FFFF, 0, 0, 16 },
		Package(){ 0x0014FFFF, 1, 0, 17 },
		Package(){ 0x0014FFFF, 2, 0, 18 },
		Package(){ 0x0014FFFF, 3, 0, 19 },
// ME
		Package(){ 0x0016FFFF, 0, 0, 16 },
		Package(){ 0x0016FFFF, 1, 0, 17 },
		Package(){ 0x0016FFFF, 2, 0, 18 },
		Package(){ 0x0016FFFF, 3, 0, 19 },
// SATA Controller
		Package(){ 0x0017FFFF, 0, 0, 16 },
// PCIE Root Port #17
		Package(){ 0x001BFFFF, 0, 0, 16 },
		Package(){ 0x001BFFFF, 1, 0, 17 },
		Package(){ 0x001BFFFF, 2, 0, 18 },
		Package(){ 0x001BFFFF, 3, 0, 19 },
// PCIE Root Port #1
		Package(){ 0x001CFFFF, 0, 0, 16 },
		Package(){ 0x001CFFFF, 1, 0, 17 },
		Package(){ 0x001CFFFF, 2, 0, 18 },
		Package(){ 0x001CFFFF, 3, 0, 19 },
// PCIE Root Port #9
		Package(){ 0x001DFFFF, 0, 0, 16 },
		Package(){ 0x001DFFFF, 1, 0, 17 },
		Package(){ 0x001DFFFF, 2, 0, 18 },
		Package(){ 0x001DFFFF, 3, 0, 19 },
// LPC Bridge
		Package(){ 0x001FFFFF, 0, 0, 16 },
		Package(){ 0x001FFFFF, 1, 0, 17 },
		Package(){ 0x001FFFFF, 2, 0, 18 },
		Package(){ 0x001FFFFF, 3, 0, 19 },
	})
// P.E.G. Root Port D1F0
	Name(PR01, Package(){
// P.E.G. Port Slot x16
		Package(){ 0x0000FFFF, 0, LNKA, 0 },
		Package(){ 0x0000FFFF, 1, LNKB, 0 },
		Package(){ 0x0000FFFF, 2, LNKC, 0 },
		Package(){ 0x0000FFFF, 3, LNKD, 0 },
	})
// P.E.G. Root Port D1F0
	Name(AR01, Package(){
// P.E.G. Port Slot x16
		Package(){ 0x0000FFFF, 0, 0, 16 },
		Package(){ 0x0000FFFF, 1, 0, 17 },
		Package(){ 0x0000FFFF, 2, 0, 18 },
		Package(){ 0x0000FFFF, 3, 0, 19 },
	})
// P.E.G. Root Port D1F1
	Name(PR02, Package(){
// P.E.G. Port Slot x8
		Package(){ 0x0000FFFF, 0, LNKB, 0 },
		Package(){ 0x0000FFFF, 1, LNKC, 0 },
		Package(){ 0x0000FFFF, 2, LNKD, 0 },
		Package(){ 0x0000FFFF, 3, LNKA, 0 },
	})
// P.E.G. Root Port D1F1
	Name(AR02, Package(){
// P.E.G. Port Slot x8
		Package(){ 0x0000FFFF, 0, 0, 17 },
		Package(){ 0x0000FFFF, 1, 0, 18 },
		Package(){ 0x0000FFFF, 2, 0, 19 },
		Package(){ 0x0000FFFF, 3, 0, 16 },
	})
// P.E.G. Root Port D1F2
	Name(PR03, Package(){
// P.E.G. Port Slot x4
		Package(){ 0x0000FFFF, 0, LNKC, 0 },
		Package(){ 0x0000FFFF, 1, LNKD, 0 },
		Package(){ 0x0000FFFF, 2, LNKA, 0 },
		Package(){ 0x0000FFFF, 3, LNKB, 0 },
	})
// P.E.G. Root Port D1F2
	Name(AR03, Package(){
// P.E.G. Port Slot x4
		Package(){ 0x0000FFFF, 0, 0, 18 },
		Package(){ 0x0000FFFF, 1, 0, 19 },
		Package(){ 0x0000FFFF, 2, 0, 16 },
		Package(){ 0x0000FFFF, 3, 0, 17 },
	})
// PCIE Root Port #17
	Name(PR04, Package(){
// PCIE Port #17 Slot
		Package(){ 0x0000FFFF, 0, LNKA, 0 },
		Package(){ 0x0000FFFF, 1, LNKB, 0 },
		Package(){ 0x0000FFFF, 2, LNKC, 0 },
		Package(){ 0x0000FFFF, 3, LNKD, 0 },
	})
// PCIE Root Port #17
	Name(AR04, Package(){
// PCIE Port #17 Slot
		Package(){ 0x0000FFFF, 0, 0, 16 },
		Package(){ 0x0000FFFF, 1, 0, 17 },
		Package(){ 0x0000FFFF, 2, 0, 18 },
		Package(){ 0x0000FFFF, 3, 0, 19 },
	})
// PCIE Root Port #18
	Name(PR05, Package(){
// PCIE Port #18 Slot
		Package(){ 0x0000FFFF, 0, LNKB, 0 },
		Package(){ 0x0000FFFF, 1, LNKC, 0 },
		Package(){ 0x0000FFFF, 2, LNKD, 0 },
		Package(){ 0x0000FFFF, 3, LNKA, 0 },
	})
// PCIE Root Port #18
	Name(AR05, Package(){
// PCIE Port #18 Slot
		Package(){ 0x0000FFFF, 0, 0, 17 },
		Package(){ 0x0000FFFF, 1, 0, 18 },
		Package(){ 0x0000FFFF, 2, 0, 19 },
		Package(){ 0x0000FFFF, 3, 0, 16 },
	})
// PCIE Root Port #19
	Name(PR06, Package(){
// PCIE Port #19 Slot
		Package(){ 0x0000FFFF, 0, LNKC, 0 },
		Package(){ 0x0000FFFF, 1, LNKD, 0 },
		Package(){ 0x0000FFFF, 2, LNKA, 0 },
		Package(){ 0x0000FFFF, 3, LNKB, 0 },
	})
// PCIE Root Port #19
	Name(AR06, Package(){
// PCIE Port #19 Slot
		Package(){ 0x0000FFFF, 0, 0, 18 },
		Package(){ 0x0000FFFF, 1, 0, 19 },
		Package(){ 0x0000FFFF, 2, 0, 16 },
		Package(){ 0x0000FFFF, 3, 0, 17 },
	})
// PCIE Root Port #20
	Name(PR07, Package(){
// PCIE Port #20 Slot
		Package(){ 0x0000FFFF, 0, LNKD, 0 },
		Package(){ 0x0000FFFF, 1, LNKA, 0 },
		Package(){ 0x0000FFFF, 2, LNKB, 0 },
		Package(){ 0x0000FFFF, 3, LNKC, 0 },
	})
// PCIE Root Port #20
	Name(AR07, Package(){
// PCIE Port #20 Slot
		Package(){ 0x0000FFFF, 0, 0, 19 },
		Package(){ 0x0000FFFF, 1, 0, 16 },
		Package(){ 0x0000FFFF, 2, 0, 17 },
		Package(){ 0x0000FFFF, 3, 0, 18 },
	})
// PCIE Root Port #21
	Name(PR08, Package(){
// PCIE Port #21 Slot
		Package(){ 0x0000FFFF, 0, LNKA, 0 },
		Package(){ 0x0000FFFF, 1, LNKB, 0 },
		Package(){ 0x0000FFFF, 2, LNKC, 0 },
		Package(){ 0x0000FFFF, 3, LNKD, 0 },
	})
// PCIE Root Port #21
	Name(AR08, Package(){
// PCIE Port #21 Slot
		Package(){ 0x0000FFFF, 0, 0, 16 },
		Package(){ 0x0000FFFF, 1, 0, 17 },
		Package(){ 0x0000FFFF, 2, 0, 18 },
		Package(){ 0x0000FFFF, 3, 0, 19 },
	})
// PCIE Root Port #22
	Name(PR09, Package(){
// PCIE Port #22 Slot
		Package(){ 0x0000FFFF, 0, LNKB, 0 },
		Package(){ 0x0000FFFF, 1, LNKC, 0 },
		Package(){ 0x0000FFFF, 2, LNKD, 0 },
		Package(){ 0x0000FFFF, 3, LNKA, 0 },
	})
// PCIE Root Port #22
	Name(AR09, Package(){
// PCIE Port #22 Slot
		Package(){ 0x0000FFFF, 0, 0, 17 },
		Package(){ 0x0000FFFF, 1, 0, 18 },
		Package(){ 0x0000FFFF, 2, 0, 19 },
		Package(){ 0x0000FFFF, 3, 0, 16 },
	})
// PCIE Root Port #23
	Name(PR0A, Package(){
// PCIE Port #23 Slot
		Package(){ 0x0000FFFF, 0, LNKC, 0 },
		Package(){ 0x0000FFFF, 1, LNKD, 0 },
		Package(){ 0x0000FFFF, 2, LNKA, 0 },
		Package(){ 0x0000FFFF, 3, LNKB, 0 },
	})
// PCIE Root Port #23
	Name(AR0A, Package(){
// PCIE Port #23 Slot
		Package(){ 0x0000FFFF, 0, 0, 18 },
		Package(){ 0x0000FFFF, 1, 0, 19 },
		Package(){ 0x0000FFFF, 2, 0, 16 },
		Package(){ 0x0000FFFF, 3, 0, 17 },
	})
// PCIE Root Port #24
	Name(PR0B, Package(){
// PCIE Port #24 Slot
		Package(){ 0x0000FFFF, 0, LNKD, 0 },
		Package(){ 0x0000FFFF, 1, LNKA, 0 },
		Package(){ 0x0000FFFF, 2, LNKB, 0 },
		Package(){ 0x0000FFFF, 3, LNKC, 0 },
	})
// PCIE Root Port #24
	Name(AR0B, Package(){
// PCIE Port #24 Slot
		Package(){ 0x0000FFFF, 0, 0, 19 },
		Package(){ 0x0000FFFF, 1, 0, 16 },
		Package(){ 0x0000FFFF, 2, 0, 17 },
		Package(){ 0x0000FFFF, 3, 0, 18 },
	})
// PCIE Root Port #1
	Name(PR0C, Package(){
// PCIE Port #1 Slot
		Package(){ 0x0000FFFF, 0, LNKA, 0 },
		Package(){ 0x0000FFFF, 1, LNKB, 0 },
		Package(){ 0x0000FFFF, 2, LNKC, 0 },
		Package(){ 0x0000FFFF, 3, LNKD, 0 },
	})
// PCIE Root Port #1
	Name(AR0C, Package(){
// PCIE Port #1 Slot
		Package(){ 0x0000FFFF, 0, 0, 16 },
		Package(){ 0x0000FFFF, 1, 0, 17 },
		Package(){ 0x0000FFFF, 2, 0, 18 },
		Package(){ 0x0000FFFF, 3, 0, 19 },
	})
// PCIE Root Port #2
	Name(PR0D, Package(){
// PCIE Port #2 Slot
		Package(){ 0x0000FFFF, 0, LNKB, 0 },
		Package(){ 0x0000FFFF, 1, LNKC, 0 },
		Package(){ 0x0000FFFF, 2, LNKD, 0 },
		Package(){ 0x0000FFFF, 3, LNKA, 0 },
	})
// PCIE Root Port #2
	Name(AR0D, Package(){
// PCIE Port #2 Slot
		Package(){ 0x0000FFFF, 0, 0, 17 },
		Package(){ 0x0000FFFF, 1, 0, 18 },
		Package(){ 0x0000FFFF, 2, 0, 19 },
		Package(){ 0x0000FFFF, 3, 0, 16 },
	})
// PCIE Root Port #3
	Name(PR0E, Package(){
// PCIE Port #3 Slot
		Package(){ 0x0000FFFF, 0, LNKC, 0 },
		Package(){ 0x0000FFFF, 1, LNKD, 0 },
		Package(){ 0x0000FFFF, 2, LNKA, 0 },
		Package(){ 0x0000FFFF, 3, LNKB, 0 },
	})
// PCIE Root Port #3
	Name(AR0E, Package(){
// PCIE Port #3 Slot
		Package(){ 0x0000FFFF, 0, 0, 18 },
		Package(){ 0x0000FFFF, 1, 0, 19 },
		Package(){ 0x0000FFFF, 2, 0, 16 },
		Package(){ 0x0000FFFF, 3, 0, 17 },
	})
// PCIE Root Port #4
	Name(PR0F, Package(){
// PCIE Port #4 Slot
		Package(){ 0x0000FFFF, 0, LNKD, 0 },
		Package(){ 0x0000FFFF, 1, LNKA, 0 },
		Package(){ 0x0000FFFF, 2, LNKB, 0 },
		Package(){ 0x0000FFFF, 3, LNKC, 0 },
	})
// PCIE Root Port #4
	Name(AR0F, Package(){
// PCIE Port #4 Slot
		Package(){ 0x0000FFFF, 0, 0, 19 },
		Package(){ 0x0000FFFF, 1, 0, 16 },
		Package(){ 0x0000FFFF, 2, 0, 17 },
		Package(){ 0x0000FFFF, 3, 0, 18 },
	})
// PCIE Root Port #5
	Name(PR10, Package(){
// PCIE Port #5 Slot
		Package(){ 0x0000FFFF, 0, LNKA, 0 },
		Package(){ 0x0000FFFF, 1, LNKB, 0 },
		Package(){ 0x0000FFFF, 2, LNKC, 0 },
		Package(){ 0x0000FFFF, 3, LNKD, 0 },
	})
// PCIE Root Port #5
	Name(AR10, Package(){
// PCIE Port #5 Slot
		Package(){ 0x0000FFFF, 0, 0, 16 },
		Package(){ 0x0000FFFF, 1, 0, 17 },
		Package(){ 0x0000FFFF, 2, 0, 18 },
		Package(){ 0x0000FFFF, 3, 0, 19 },
	})
// PCIE Root Port #6
	Name(PR11, Package(){
// PCIE Port #6 Slot
		Package(){ 0x0000FFFF, 0, LNKB, 0 },
		Package(){ 0x0000FFFF, 1, LNKC, 0 },
		Package(){ 0x0000FFFF, 2, LNKD, 0 },
		Package(){ 0x0000FFFF, 3, LNKA, 0 },
	})
// PCIE Root Port #6
	Name(AR11, Package(){
// PCIE Port #6 Slot
		Package(){ 0x0000FFFF, 0, 0, 17 },
		Package(){ 0x0000FFFF, 1, 0, 18 },
		Package(){ 0x0000FFFF, 2, 0, 19 },
		Package(){ 0x0000FFFF, 3, 0, 16 },
	})
// PCIE Root Port #7
	Name(PR12, Package(){
// PCIE Port #7 Slot
		Package(){ 0x0000FFFF, 0, LNKC, 0 },
		Package(){ 0x0000FFFF, 1, LNKD, 0 },
		Package(){ 0x0000FFFF, 2, LNKA, 0 },
		Package(){ 0x0000FFFF, 3, LNKB, 0 },
	})
// PCIE Root Port #7
	Name(AR12, Package(){
// PCIE Port #7 Slot
		Package(){ 0x0000FFFF, 0, 0, 18 },
		Package(){ 0x0000FFFF, 1, 0, 19 },
		Package(){ 0x0000FFFF, 2, 0, 16 },
		Package(){ 0x0000FFFF, 3, 0, 17 },
	})
// PCIE Root Port #8
	Name(PR13, Package(){
// PCIE Port #8 Slot
		Package(){ 0x0000FFFF, 0, LNKD, 0 },
		Package(){ 0x0000FFFF, 1, LNKA, 0 },
		Package(){ 0x0000FFFF, 2, LNKB, 0 },
		Package(){ 0x0000FFFF, 3, LNKC, 0 },
	})
// PCIE Root Port #8
	Name(AR13, Package(){
// PCIE Port #8 Slot
		Package(){ 0x0000FFFF, 0, 0, 19 },
		Package(){ 0x0000FFFF, 1, 0, 16 },
		Package(){ 0x0000FFFF, 2, 0, 17 },
		Package(){ 0x0000FFFF, 3, 0, 18 },
	})
// PCIE Root Port #9
	Name(PR14, Package(){
// PCIE Port #9 Slot
		Package(){ 0x0000FFFF, 0, LNKA, 0 },
		Package(){ 0x0000FFFF, 1, LNKB, 0 },
		Package(){ 0x0000FFFF, 2, LNKC, 0 },
		Package(){ 0x0000FFFF, 3, LNKD, 0 },
	})
// PCIE Root Port #9
	Name(AR14, Package(){
// PCIE Port #9 Slot
		Package(){ 0x0000FFFF, 0, 0, 16 },
		Package(){ 0x0000FFFF, 1, 0, 17 },
		Package(){ 0x0000FFFF, 2, 0, 18 },
		Package(){ 0x0000FFFF, 3, 0, 19 },
	})
// PCIE Root Port #10
	Name(PR15, Package(){
// PCIE Port #10 Slot
		Package(){ 0x0000FFFF, 0, LNKB, 0 },
		Package(){ 0x0000FFFF, 1, LNKC, 0 },
		Package(){ 0x0000FFFF, 2, LNKD, 0 },
		Package(){ 0x0000FFFF, 3, LNKA, 0 },
	})
// PCIE Root Port #10
	Name(AR15, Package(){
// PCIE Port #10 Slot
		Package(){ 0x0000FFFF, 0, 0, 17 },
		Package(){ 0x0000FFFF, 1, 0, 18 },
		Package(){ 0x0000FFFF, 2, 0, 19 },
		Package(){ 0x0000FFFF, 3, 0, 16 },
	})
// PCIE Root Port #11
	Name(PR16, Package(){
// PCIE Port #11 Slot
		Package(){ 0x0000FFFF, 0, LNKC, 0 },
		Package(){ 0x0000FFFF, 1, LNKD, 0 },
		Package(){ 0x0000FFFF, 2, LNKA, 0 },
		Package(){ 0x0000FFFF, 3, LNKB, 0 },
	})
// PCIE Root Port #11
	Name(AR16, Package(){
// PCIE Port #11 Slot
		Package(){ 0x0000FFFF, 0, 0, 18 },
		Package(){ 0x0000FFFF, 1, 0, 19 },
		Package(){ 0x0000FFFF, 2, 0, 16 },
		Package(){ 0x0000FFFF, 3, 0, 17 },
	})
// PCIE Root Port #12
	Name(PR17, Package(){
// PCIE Port #12 Slot
		Package(){ 0x0000FFFF, 0, LNKD, 0 },
		Package(){ 0x0000FFFF, 1, LNKA, 0 },
		Package(){ 0x0000FFFF, 2, LNKB, 0 },
		Package(){ 0x0000FFFF, 3, LNKC, 0 },
	})
// PCIE Root Port #12
	Name(AR17, Package(){
// PCIE Port #12 Slot
		Package(){ 0x0000FFFF, 0, 0, 19 },
		Package(){ 0x0000FFFF, 1, 0, 16 },
		Package(){ 0x0000FFFF, 2, 0, 17 },
		Package(){ 0x0000FFFF, 3, 0, 18 },
	})
// PCIE Root Port #13
	Name(PR18, Package(){
// PCIE Port #13 Slot
		Package(){ 0x0000FFFF, 0, LNKA, 0 },
		Package(){ 0x0000FFFF, 1, LNKB, 0 },
		Package(){ 0x0000FFFF, 2, LNKC, 0 },
		Package(){ 0x0000FFFF, 3, LNKD, 0 },
	})
// PCIE Root Port #13
	Name(AR18, Package(){
// PCIE Port #13 Slot
		Package(){ 0x0000FFFF, 0, 0, 16 },
		Package(){ 0x0000FFFF, 1, 0, 17 },
		Package(){ 0x0000FFFF, 2, 0, 18 },
		Package(){ 0x0000FFFF, 3, 0, 19 },
	})
// PCIE Root Port #14
	Name(PR19, Package(){
// PCIE Port #14 Slot
		Package(){ 0x0000FFFF, 0, LNKB, 0 },
		Package(){ 0x0000FFFF, 1, LNKC, 0 },
		Package(){ 0x0000FFFF, 2, LNKD, 0 },
		Package(){ 0x0000FFFF, 3, LNKA, 0 },
	})
// PCIE Root Port #14
	Name(AR19, Package(){
// PCIE Port #14 Slot
		Package(){ 0x0000FFFF, 0, 0, 17 },
		Package(){ 0x0000FFFF, 1, 0, 18 },
		Package(){ 0x0000FFFF, 2, 0, 19 },
		Package(){ 0x0000FFFF, 3, 0, 16 },
	})
// PCIE Root Port #15
	Name(PR1A, Package(){
// PCIE Port #15 Slot
		Package(){ 0x0000FFFF, 0, LNKC, 0 },
		Package(){ 0x0000FFFF, 1, LNKD, 0 },
		Package(){ 0x0000FFFF, 2, LNKA, 0 },
		Package(){ 0x0000FFFF, 3, LNKB, 0 },
	})
// PCIE Root Port #15
	Name(AR1A, Package(){
// PCIE Port #15 Slot
		Package(){ 0x0000FFFF, 0, 0, 18 },
		Package(){ 0x0000FFFF, 1, 0, 19 },
		Package(){ 0x0000FFFF, 2, 0, 16 },
		Package(){ 0x0000FFFF, 3, 0, 17 },
	})
// PCIE Root Port #16
	Name(PR1B, Package(){
// PCIE Port #16 Slot
		Package(){ 0x0000FFFF, 0, LNKD, 0 },
		Package(){ 0x0000FFFF, 1, LNKA, 0 },
		Package(){ 0x0000FFFF, 2, LNKB, 0 },
		Package(){ 0x0000FFFF, 3, LNKC, 0 },
	})
// PCIE Root Port #16
	Name(AR1B, Package(){
// PCIE Port #16 Slot
		Package(){ 0x0000FFFF, 0, 0, 19 },
		Package(){ 0x0000FFFF, 1, 0, 16 },
		Package(){ 0x0000FFFF, 2, 0, 17 },
		Package(){ 0x0000FFFF, 3, 0, 18 },
	})
//---------------------------------------------------------------------------
// List of IRQ resource buffers compatible with _PRS return format.
//---------------------------------------------------------------------------
// Naming legend:
// RSxy, PRSy - name of the IRQ resource buffer to be returned by _PRS, "xy" - last two characters of IRQ Link name.
// Note. PRSy name is generated if IRQ Link name starts from "LNK".
// HLxy , LLxy - reference names, can be used to access bit mask of available IRQs. HL and LL stand for active High(Low) Level triggered Irq model.
//---------------------------------------------------------------------------
	Name(PRSA, ResourceTemplate(){	// Link name: LNKA
		IRQ(Level, ActiveLow, Shared, LLKA) {3,4,5,6,10,11,12,14,15}
	})
	Alias(PRSA,PRSB)	// Link name: LNKB
	Alias(PRSA,PRSC)	// Link name: LNKC
	Alias(PRSA,PRSD)	// Link name: LNKD
	Alias(PRSA,PRSE)	// Link name: LNKE
	Alias(PRSA,PRSF)	// Link name: LNKF
	Alias(PRSA,PRSG)	// Link name: LNKG
	Alias(PRSA,PRSH)	// Link name: LNKH
}

