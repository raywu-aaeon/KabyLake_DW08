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

#define MP_TABLE_IRQ_DATA { \
/*+-------------------+----------------+----------------+----------------+----------------+*/ \
/*| Device Location   | Int A Pin      | Int B Pin      | Int C Pin      | Int D Pin      |*/ \
/*+-------------------+----------------+----------------+----------------+----------------+-----------+*/ \
/*| PCI Bus  Device   | APIC    IO APIC| APIC    IO APIC| APIC    IO APIC| APIC    IO APIC| Reserved  |*/ \
/*| Number   Number   | Input#  ID#    | Input#  ID#    | Input#  ID#    | Input#  ID#    | must be 0 |*/ \
/*+-------------------+----------------+----------------+----------------+----------------+-----------+*/ \
  { 0x00,    0x02,      16,     0x02,    00,     0x00,    00,     0x00,    00,     0x00,    0 }, /* Device #0x02 :Pin0: I.G.D. ( Fn# 0x00)*/ \
  { 0x00,    0x03,      16,     0x02,    00,     0x00,    00,     0x00,    00,     0x00,    0 }, /* Device #0x03 :Pin0: SA Audio Device ( Fn# 0x00)*/ \
  { 0x00,    0x04,      16,     0x02,    00,     0x00,    00,     0x00,    00,     0x00,    0 }, /* Device #0x04 :Pin0: SA Thermal Device ( Fn# 0x00)*/ \
  { 0x00,    0x07,      16,     0x02,    00,     0x00,    00,     0x00,    00,     0x00,    0 }, /* Device #0x07 :Pin0: SA Chap Device ( Fn# 0x00)*/ \
  { 0x00,    0x13,      20,     0x02,    00,     0x00,    00,     0x00,    00,     0x00,    0 }, /* Device #0x13 :Pin0: Integrated Sensor Hub(ISH) ( Fn# 0x00)*/ \
  { 0x00,    0x14,      16,     0x02,    17,     0x02,    18,     0x02,    19,     0x02,    0 }, /* Device #0x14 :Pin0: USB 3.0 xHCI Controller ( Fn# 0x00) :Pin1: USB 3.0 xHCI Controller ( Fn# 0x00) :Pin2: USB 3.0 xHCI Controller ( Fn# 0x00) :Pin3: USB 3.0 xHCI Controller ( Fn# 0x00)*/ \
  { 0x00,    0x15,      16,     0x02,    17,     0x02,    18,     0x02,    19,     0x02,    0 }, /* Device #0x15 :Pin0: SERIALIO I2C Controller #0 ( Fn# 0x00) :Pin1: SERIALIO I2C Controller #0 ( Fn# 0x00) :Pin2: SERIALIO I2C Controller #0 ( Fn# 0x00) :Pin3: SERIALIO I2C Controller #0 ( Fn# 0x00)*/ \
  { 0x00,    0x16,      16,     0x02,    17,     0x02,    18,     0x02,    19,     0x02,    0 }, /* Device #0x16 :Pin0: ME ( Fn# 0x00) :Pin1: ME ( Fn# 0x00) :Pin2: ME ( Fn# 0x00) :Pin3: ME ( Fn# 0x00)*/ \
  { 0x00,    0x17,      16,     0x02,    00,     0x00,    00,     0x00,    00,     0x00,    0 }, /* Device #0x17 :Pin0: SATA Controller ( Fn# 0x00)*/ \
  { 0x00,    0x19,      32,     0x02,    33,     0x02,    34,     0x02,    35,     0x02,    0 }, /* Device #0x19 :Pin0: SERIALIO UART Controller #2 ( Fn# 0x00) :Pin1: SERIALIO UART Controller #2 ( Fn# 0x00) :Pin2: SERIALIO UART Controller #2 ( Fn# 0x00) :Pin3: SERIALIO UART Controller #2 ( Fn# 0x00)*/ \
  { 0x00,    0x1C,      16,     0x02,    17,     0x02,    18,     0x02,    19,     0x02,    0 }, /* Device #0x1C :Pin0: PCIE Root Port #1 ( Fn# 0x00) :Pin1: PCIE Root Port #1 ( Fn# 0x00) :Pin2: PCIE Root Port #1 ( Fn# 0x00) :Pin3: PCIE Root Port #1 ( Fn# 0x00)*/ \
  { 0x01,    0x00,      16,     0x02,    17,     0x02,    18,     0x02,    19,     0x02,    0 }, /* Device #0x00, Slot 33 */ \
  { 0x02,    0x00,      17,     0x02,    18,     0x02,    19,     0x02,    16,     0x02,    0 }, /* Device #0x00, Slot 34 */ \
  { 0x03,    0x00,      18,     0x02,    19,     0x02,    16,     0x02,    17,     0x02,    0 }, /* Device #0x00, Slot 08 */ \
  { 0x04,    0x00,      19,     0x02,    16,     0x02,    17,     0x02,    18,     0x02,    0 }, /* Device #0x00, Slot 09 */ \
  { 0x05,    0x00,      16,     0x02,    17,     0x02,    18,     0x02,    19,     0x02,    0 }, /* Device #0x00, Slot 10 */ \
  { 0x06,    0x00,      17,     0x02,    18,     0x02,    19,     0x02,    16,     0x02,    0 }, /* Device #0x00, Slot 16 */ \
  { 0x07,    0x00,      18,     0x02,    19,     0x02,    16,     0x02,    17,     0x02,    0 }, /* Device #0x00, Slot 17 */ \
  { 0x08,    0x00,      19,     0x02,    16,     0x02,    17,     0x02,    18,     0x02,    0 }, /* Device #0x00, Slot 18 */ \
  { 0x00,    0x1D,      16,     0x02,    17,     0x02,    18,     0x02,    19,     0x02,    0 }, /* Device #0x1D :Pin0: PCIE Root Port #9 ( Fn# 0x00) :Pin1: PCIE Root Port #9 ( Fn# 0x00) :Pin2: PCIE Root Port #9 ( Fn# 0x00) :Pin3: PCIE Root Port #9 ( Fn# 0x00)*/ \
  { 0x09,    0x00,      16,     0x02,    17,     0x02,    18,     0x02,    19,     0x02,    0 }, /* Device #0x00, Slot 00 */ \
  { 0x0A,    0x00,      17,     0x02,    18,     0x02,    19,     0x02,    16,     0x02,    0 }, /* Device #0x00, Slot 00 */ \
  { 0x0B,    0x00,      18,     0x02,    19,     0x02,    16,     0x02,    17,     0x02,    0 }, /* Device #0x00, Slot 00 */ \
  { 0x0C,    0x00,      19,     0x02,    16,     0x02,    17,     0x02,    18,     0x02,    0 }, /* Device #0x00, Slot 00 */ \
  { 0x00,    0x1E,      20,     0x02,    21,     0x02,    22,     0x02,    23,     0x02,    0 }, /* Device #0x1E :Pin0: SERIALIO(UART #0) ( Fn# 0x00) :Pin1: SERIALIO(UART #0) ( Fn# 0x00) :Pin2: SERIALIO(UART #0) ( Fn# 0x00) :Pin3: SERIALIO(UART #0) ( Fn# 0x00)*/ \
  { 0x00,    0x1F,      16,     0x02,    17,     0x02,    18,     0x02,    19,     0x02,    0 }, /* Device #0x1F :Pin0: LPC Bridge ( Fn# 0x00) :Pin1: LPC Bridge ( Fn# 0x00) :Pin2: LPC Bridge ( Fn# 0x00) :Pin3: LPC Bridge ( Fn# 0x00)*/ \
}

#define MP_TABLE_APIC_DATA { \
/*+----------------------+----------+*/ \
/*| IO APIC     IO APIC  | IO APIC  |*/ \
/*| Address     ID(BUILD)| ID(POST) |*/ \
/*+----------------------+----------+*/ \
  { 0xFEC00000, 0x02,      0x00 }, /* 000 : Int vector base. IO APIC to serve Legacy Interrupts */ \
}
