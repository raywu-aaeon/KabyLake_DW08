;------------------------------------------------------------------------------ ;
; Copyright (c) 2015 - 2017, Intel Corporation. All rights reserved.<BR>
; This program and the accompanying materials
; are licensed and made available under the terms and conditions of the BSD License
; which accompanies this distribution.  The full text of the license may be found at
; http://opensource.org/licenses/bsd-license.php.
;
; THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
; WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
;
; Module Name:
;
;   MpEqu.inc
;
; Abstract:
;
;   This is the equates file for Multiple Processor support
;
;-------------------------------------------------------------------------------

VacantFlag                    equ        00h
NotVacantFlag                 equ        0ffh

CPU_SWITCH_STATE_IDLE         equ        0
CPU_SWITCH_STATE_STORED       equ        1
CPU_SWITCH_STATE_LOADED       equ        2

LockLocation                  equ        (RendezvousFunnelProcEnd - RendezvousFunnelProcStart)
StackStartAddressLocation     equ        LockLocation + 04h
StackSizeLocation             equ        LockLocation + 08h
ApProcedureLocation           equ        LockLocation + 0Ch
GdtrLocation                  equ        LockLocation + 10h
IdtrLocation                  equ        LockLocation + 16h
BufferStartLocation           equ        LockLocation + 1Ch
ModeOffsetLocation            equ        LockLocation + 20h
NumApsExecutingLoction        equ        LockLocation + 24h
CodeSegmentLocation           equ        LockLocation + 28h
DataSegmentLocation           equ        LockLocation + 2Ch

