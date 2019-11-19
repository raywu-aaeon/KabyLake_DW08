/**@file

  Touch Input Filter Driver
  TouchInputFilterDriver.h

  Functionality:
  Driver reponsible for filtering all data that comes out of Touch Panel Devices
  If touch FINGER_DOWN event lasts long enough to gather more then 3 samples filtration begins
  Swipe actions are dteceted and stored inside of Touch Input Filter Device Context

  Driver hooks on Absolute Pointer Get State and replaces its orignal call with new custom implementation
  Inside of Custem Get State function Input filters are applied once the orignal GetState is executed

  Filtering algorithm:
  Fixed-size buffer (#define BUFFER_LENGTH) used as a circular queue
  Oldest samples are replaced with new samples.
  Data in buffer is averaged before passed back to Absolute Pointer Get State caller (like Virtual Keyboard)

  //

 @copyright
  INTEL CONFIDENTIAL
  Copyright 2016 Intel Corporation.

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

  This file contains an 'Intel Peripheral Driver' and is uniquely identified as
  "Intel Reference Module" and is licensed for Intel CPUs and chipsets under
  the terms of your license agreement with Intel or your vendor. This file may
  be modified by the user, subject to additional terms of the license agreement.

@par Specification Reference:
**/

#ifndef _TOUCH_INPUT_FILTER_DRIVER_H_
#define _TOUCH_INPUT_FILTER_DRIVER_H_

#include <Uefi.h>

//
// Libraries
//
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Guid/EventGroup.h>
#include <Guid/EventLegacyBios.h>
//
// UEFI Driver Model Protocols
//
#include <Protocol/DriverBinding.h>
#include <Protocol/DriverSupportedEfiVersion.h>
#include <Protocol/ComponentName2.h>
#include <Protocol/ComponentName.h>

//
// Produced Protocols
//
#include <Protocol/AbsolutePointer.h>

//
// Protocol instances
//
extern EFI_DRIVER_BINDING_PROTOCOL  gTouchInputFilterDriverBinding;
extern EFI_COMPONENT_NAME2_PROTOCOL gTouchInputFilterDriverComponentName2;
extern EFI_COMPONENT_NAME_PROTOCOL  gTouchInputFilterDriverComponentName;
extern EFI_GUID                     gEfiTouchPanelGuid;
extern EFI_GUID                     gTouchInputFilterNullProtocolGuid;

#define BUFFER_LENGTH                10
#define SQUARE(X)                    ((X) * (X))

typedef struct TOUCH_INPUT_FILTER_NULL_PROTOCOL TOUCH_INPUT_FILTER_NULL_PROTOCOL;

/**
  Touch Input Filter Null protocol

**/
struct TOUCH_INPUT_FILTER_NULL_PROTOCOL {
  UINT8                             Revision; ///< Revision for the protocol structure
};

typedef enum
{
  FINGER_UP,
  FINGER_DOWN
} FINGER_EVENT;

typedef enum
{
  SWIPE_UNKNOWN = 0,
  SWIPE_LEFT,
  SWIPE_RIGHT,
  SWIPE_UP,
  SWIPE_DOWN
} SWIPE_DIRECTION;

typedef struct {
  UINT64    TotalX;
  UINT64    TotalY;
  UINT64    X[BUFFER_LENGTH];
  UINT64    Y[BUFFER_LENGTH];
  UINT8     Index;
  UINT8     Length;
} FILTER_BUFFER;

typedef struct {
  UINTN                            Signature;
  EFI_UNICODE_STRING_TABLE         *ControllerNameTable;
  TOUCH_INPUT_FILTER_NULL_PROTOCOL NullProtocol;
  EFI_EVENT                        ExitEvent;
  EFI_ABSOLUTE_POINTER_PROTOCOL    AbsolutePointerProtocol;
  EFI_ABSOLUTE_POINTER_GET_STATE   SourceAbsolutePointerGetState;
  UINT64                           CurrentX;
  UINT64                           CurrentY;
  UINT64                           PreviousX;
  UINT64                           PreviousY;
  UINT8                            SwipeDirection;
  UINT8                            SamplesCollected;
  BOOLEAN                          FingerDownEvent;
  FILTER_BUFFER                    Buffer;
  //
  // Settings
  //
  UINT8                            SampleRate;
  UINT8                            SwipeDetectMinDistance;
  UINT8                            SwipeDetectMinXDistance;
  UINT8                            SwipeDetectMinYDistance;
} TOUCH_INPUT_FILTER_DEV;

#define TOUCH_INPUT_FILTER_SIGNATURE SIGNATURE_32 ('T','C','H','F')
#define TOUCH_INPUT_FILTER_VERSION  0x00000000
#define TOUCH_INPUT_FILTER_CONTEXT_FROM_NULL_PROTOCOL(a)   CR (a, TOUCH_INPUT_FILTER_DEV, NullProtocol, TOUCH_INPUT_FILTER_SIGNATURE)

EFI_STATUS
EFIAPI
TouchInputFilterDriverBindingSupported (
  IN EFI_DRIVER_BINDING_PROTOCOL  *This,
  IN EFI_HANDLE                   ControllerHandle,
  IN EFI_DEVICE_PATH_PROTOCOL     *RemainingDevicePath OPTIONAL
  );

EFI_STATUS
EFIAPI
TouchInputFilterDriverBindingStart (
  IN EFI_DRIVER_BINDING_PROTOCOL  *This,
  IN EFI_HANDLE                   ControllerHandle,
  IN EFI_DEVICE_PATH_PROTOCOL     *RemainingDevicePath OPTIONAL
  );

EFI_STATUS
EFIAPI
TouchInputFilterDriverBindingStop (
  IN EFI_DRIVER_BINDING_PROTOCOL  *This,
  IN  EFI_HANDLE                  ControllerHandle,
  IN  UINTN                       NumberOfChildren,
  IN  EFI_HANDLE                  *ChildHandleBuffer OPTIONAL
  );

/**
  Function to unload Touch Input Filter Driver when
  gBS->ExitBootServices() called or LegacyBoot event.

  @param  Event                   Pointer to this event
  @param  Context                 Event hanlder private data

**/
VOID
EFIAPI
TouchInputFilterDriverExitEvent (
  IN EFI_EVENT     Event,
  IN VOID          *Context
  );

/**
  Free TouchInputFilter structure allocated in Start()

  @param[in]  iTouchDev       context of iTouch device

**/
VOID
EFIAPI
TouchInputFilterDriverCleanUp (
  IN TOUCH_INPUT_FILTER_DEV       *TouchInputFilterDev
  );

#endif


