/** @file

  Touch Input Filter Driver
  TouchInputFilterDriver.c

  Functionality:
  Driver reponsible for filtering all data that comes out of Touch Panel Devices
  If touch FINGER_DOWN event lasts long enough to gather more then 3 samples filtration begins
  Swipe actions are detected and stored inside of Touch Input Filter Device Context

  Driver hooks on Absolute Pointer Get State and replaces its orignal call with new custom implementation
  Inside of Custem Get State function Input filters are applied once the orignal GetState is executed

  Filtering algorithm:
  Fixed-size buffer (#define BUFFER_LENGTH) used as a circular queue
  Oldest samples are replaced with new samples.
  Data in buffer is averaged before passed back to Absolute Pointer Get State caller (like Virtual Keyboard)

  //

 @copyright
  INTEL CONFIDENTIAL
  Copyright 2016 - 2017 Intel Corporation.

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

#include "TouchInputFilterDriver.h"

//
// Driver Support EFI Version Protocol instance
//
GLOBAL_REMOVE_IF_UNREFERENCED
EFI_DRIVER_SUPPORTED_EFI_VERSION_PROTOCOL gTouchInputFilterDriverSupportedEfiVersion = {
  sizeof (EFI_DRIVER_SUPPORTED_EFI_VERSION_PROTOCOL),
  0x0002001E
};

//
// Driver Binding Protocol instance
//
GLOBAL_REMOVE_IF_UNREFERENCED EFI_DRIVER_BINDING_PROTOCOL gTouchInputFilterDriverBinding = {
  TouchInputFilterDriverBindingSupported,
  TouchInputFilterDriverBindingStart,
  TouchInputFilterDriverBindingStop,
  TOUCH_INPUT_FILTER_VERSION,
  NULL,
  NULL
};


/**
  Checks if current FINGER_DOWN actions result within the range of SwipeDetectMin Distance
  If true one of 4 swipe actions is detected

  @param[in, out]  TouchInputFilterDev     Device Context.
  @param[in]       State                   Absolute Pointer State X, Y, Z and button data

**/
VOID
SwipeDetect (
  IN OUT TOUCH_INPUT_FILTER_DEV                *TouchInputFilterDev
  )
{
  INT64       DistanceX;
  INT64       DistanceY;
  INT64       ScoreX;
  INT64       ScoreY;

  DistanceX = TouchInputFilterDev->CurrentX - TouchInputFilterDev->PreviousX;
  DistanceY = TouchInputFilterDev->CurrentY - TouchInputFilterDev->PreviousY;

  ScoreX = (SQUARE (DistanceX)/TouchInputFilterDev->SwipeDetectMinXDistance) - TouchInputFilterDev->SwipeDetectMinXDistance;
  ScoreY = (SQUARE (DistanceY)/TouchInputFilterDev->SwipeDetectMinYDistance) - TouchInputFilterDev->SwipeDetectMinYDistance;

  if (!(ScoreX > 1 || ScoreY > 1)) {
    TouchInputFilterDev->SwipeDirection = SWIPE_UNKNOWN;
    return;
  }
  if (ScoreX > ScoreY) {
    if (DistanceX < (-1 * TouchInputFilterDev->SwipeDetectMinXDistance)) {
      TouchInputFilterDev->SwipeDirection = SWIPE_LEFT;
      return;
    } else if (DistanceX > TouchInputFilterDev->SwipeDetectMinXDistance) {
      TouchInputFilterDev->SwipeDirection = SWIPE_RIGHT;
      return;
    }
  } else {
    if (DistanceY < (-1 * TouchInputFilterDev->SwipeDetectMinYDistance)) {
      TouchInputFilterDev->SwipeDirection = SWIPE_UP;
      return;
    } else if (DistanceY > TouchInputFilterDev->SwipeDetectMinYDistance) {
      TouchInputFilterDev->SwipeDirection = SWIPE_DOWN;
      return;
    }
  }

  TouchInputFilterDev->SwipeDirection = SWIPE_UNKNOWN;
}


/**
  Fills both X and Y buffers with gathered averaged samples

  @param[in, out]  TouchInputFilterDev     Device Context.
  @param[in]       State                   Absolute Pointer State X, Y, Z and button data

**/
VOID
FillBuffers (
  IN OUT TOUCH_INPUT_FILTER_DEV         *TouchInputFilterDev,
  IN     EFI_ABSOLUTE_POINTER_STATE     *State
  )
{
  UINT16     Index;
  UINT64     TempX;
  UINT64     TempY;

  TouchInputFilterDev->FingerDownEvent = FALSE;
  TempX = (UINT64) (TouchInputFilterDev->Buffer.TotalX / TouchInputFilterDev->SampleRate);
  TempY = (UINT64) (TouchInputFilterDev->Buffer.TotalY / TouchInputFilterDev->SampleRate);

  for (Index = 0; Index < TouchInputFilterDev->Buffer.Length; Index++) {
    TouchInputFilterDev->Buffer.X[Index] = TempX;
    TouchInputFilterDev->Buffer.Y[Index] = TempY;
  }

  TouchInputFilterDev->Buffer.TotalX = TouchInputFilterDev->Buffer.Length * TempX;
  TouchInputFilterDev->Buffer.TotalY = TouchInputFilterDev->Buffer.Length * TempY;

  TouchInputFilterDev->SwipeDirection   = SWIPE_UNKNOWN;
}

/**
  Resets Touch Input Filter Device for new Touch Event

  @param[in, out]  TouchInputFilterDev     Device Context.

**/
VOID
FingerUpEventCleanUp (
  IN OUT TOUCH_INPUT_FILTER_DEV         *TouchInputFilterDev
  )
{
  TouchInputFilterDev->FingerDownEvent  = TRUE;
  TouchInputFilterDev->Buffer.TotalX    = 0;
  TouchInputFilterDev->Buffer.TotalY    = 0;
  TouchInputFilterDev->Buffer.Index     = 0;
  TouchInputFilterDev->SamplesCollected = 0;
}

/**
  Gathers Samples for filtration

  @param[in, out]  TouchInputFilterDev     Device Context.

  @retval   EFI_NOT_READY    Case 1: Polling too slow, no more samples to collect (Finger UP event)
  @retval   EFI_NOT_READY    Case 2: Samples collected but device is not ready to start any filtering actions (not enough samples)

**/
EFI_STATUS
GatherSamples (
  IN OUT TOUCH_INPUT_FILTER_DEV         *TouchInputFilterDev,
  IN     EFI_ABSOLUTE_POINTER_STATE     *State
  )
{
  if (State->ActiveButtons == (UINT8) FINGER_UP) {
    FingerUpEventCleanUp (TouchInputFilterDev);
    return EFI_NOT_READY;
  }

  if (TouchInputFilterDev->SamplesCollected == 0) {
    TouchInputFilterDev->PreviousX      = State->CurrentX;
    TouchInputFilterDev->PreviousY      = State->CurrentY;
    TouchInputFilterDev->SwipeDirection = SWIPE_UNKNOWN;
  }

  TouchInputFilterDev->Buffer.TotalX   += State->CurrentX;
  TouchInputFilterDev->Buffer.TotalY   += State->CurrentY;
  TouchInputFilterDev->SamplesCollected++;

  return EFI_NOT_READY;
}


/**
  Gathers data samples, applies filtering algorithm.


  @param[in, out]  TouchInputFilterDev     Device Context.
  @param[in]       State                   Absolute Pointer State X, Y, Z and button data

  @retval  EFI_NOT_READY  When Samples are not yet collected
           EFI_SUCCESS    During FINGER_DOWN Event after collecting samples
           EFI_SUCCESS    After FINGER_UP Event and collected samples

**/
EFI_STATUS
InputFilter (
  IN OUT TOUCH_INPUT_FILTER_DEV         *TouchInputFilterDev,
  IN     EFI_ABSOLUTE_POINTER_STATE     *State
  )
{
  //
  // Gather samples
  //
  if (TouchInputFilterDev->SamplesCollected < TouchInputFilterDev->SampleRate) {
    return GatherSamples (TouchInputFilterDev, State);
  }

  //
  //  Initial Key Down Event
  //
  if (TouchInputFilterDev->FingerDownEvent && (State->ActiveButtons == (UINT8) FINGER_DOWN)) {
    FillBuffers (TouchInputFilterDev, State);
  }

  //
  //  Reset the Index buffer when it reaches the end
  //
  if (TouchInputFilterDev->Buffer.Index == TouchInputFilterDev->Buffer.Length) {
    TouchInputFilterDev->Buffer.Index = 0;
  }

  //
  //  Decrease Totals
  //  Remove first entries from the buffered sums
  //
  TouchInputFilterDev->Buffer.TotalX -= TouchInputFilterDev->Buffer.X[TouchInputFilterDev->Buffer.Index];
  if (TouchInputFilterDev->Buffer.TotalX <= 0) {
    TouchInputFilterDev->Buffer.TotalX = 0;
  }
  TouchInputFilterDev->Buffer.TotalY -= TouchInputFilterDev->Buffer.Y[TouchInputFilterDev->Buffer.Index];
  if (TouchInputFilterDev->Buffer.TotalY <= 0) {
    TouchInputFilterDev->Buffer.TotalY = 0;
  }

  //
  //  Replace new X and Y with the least significant (swap with the oldest sample)
  //
  TouchInputFilterDev->Buffer.X[TouchInputFilterDev->Buffer.Index] = State->CurrentX;
  TouchInputFilterDev->Buffer.Y[TouchInputFilterDev->Buffer.Index] = State->CurrentY;

  //
  //  Increase Totals and update Index
  //
  TouchInputFilterDev->Buffer.TotalX += State->CurrentX;
  TouchInputFilterDev->Buffer.TotalY += State->CurrentY;

  TouchInputFilterDev->Buffer.Index++;

  //
  //  Assign new averaged values to the result filtered X and Y
  //
  TouchInputFilterDev->CurrentX = (UINT64)(TouchInputFilterDev->Buffer.TotalX / TouchInputFilterDev->Buffer.Length);
  TouchInputFilterDev->CurrentY = (UINT64)(TouchInputFilterDev->Buffer.TotalY / TouchInputFilterDev->Buffer.Length);

  //
  //  Handle FINGER_DOWN Event
  //
  if (State->ActiveButtons != (UINT8) FINGER_UP) {
    SwipeDetect (TouchInputFilterDev);
    return EFI_SUCCESS;
  }

  FingerUpEventCleanUp (TouchInputFilterDev);

  return EFI_SUCCESS;
}

/**
  Validatas the outcome of the applied filters.

  If the result data is FINGER_DOWN and swipe is not detected output data is not changed.
  In this case EFI_NOT_READY is returned.
  All FINGER_UP Events come through and output data is modified with the filtered results.

  @param[in, out]  TouchInputFilterDev     Device Context.
  @param[in, out]  State                   Absolute Pointer State X, Y, Z and button data

  @retval   EFI_NOT_READY  Filtered data did not pass validation
  @retval   EFI_SUCCESS    Data validation success

**/
EFI_STATUS
ValidateData (
  IN OUT TOUCH_INPUT_FILTER_DEV               *TouchInputFilterDev,
  IN OUT EFI_ABSOLUTE_POINTER_STATE           *State
  )
{

  TouchInputFilterDev->PreviousX = TouchInputFilterDev->CurrentX;
  TouchInputFilterDev->PreviousY = TouchInputFilterDev->CurrentY;

  if ((State->ActiveButtons == (UINT8) FINGER_UP) || (TouchInputFilterDev->SwipeDirection >= (UINT8) SWIPE_LEFT && TouchInputFilterDev->SwipeDirection <= (UINT8) SWIPE_DOWN))
  {

    State->CurrentX = TouchInputFilterDev->CurrentX;
    State->CurrentY = TouchInputFilterDev->CurrentY;

    //
    //  Clear Previous X and Y when button released
    //
    if (State->ActiveButtons == (UINT8) FINGER_UP) {
      TouchInputFilterDev->PreviousX = 0;
      TouchInputFilterDev->PreviousY = 0;
    }
    return EFI_SUCCESS;
  }

  //
  //  Return EFI_NOT_READY if filtered data did not pass validation
  //
  return EFI_NOT_READY;
}

/**

  Hook function that replaces prevoius Source Get State
  All filtering actions are invoked here

  @param[in]       This       Absolute Pointer Protocol
  @param[in, out]  State      Absolute Pointer State X, Y, Z and button data

  @retval   EFI_SUCCESS       Returned even when filtering did not happen.
                              For example in case when there was not enough samples collected.
  @retval   Others            Source Absolute Pointer Get State Failed
  @retval   EFI_NOT_READY     Unable to retrieve device context

**/
EFI_STATUS
EFIAPI
CustomAbsolutePointerGetState (
  IN      EFI_ABSOLUTE_POINTER_PROTOCOL  *This,
  IN OUT  EFI_ABSOLUTE_POINTER_STATE     *State
  )
{
  EFI_STATUS                        Status;
  TOUCH_INPUT_FILTER_NULL_PROTOCOL  *NullProtocol;
  TOUCH_INPUT_FILTER_DEV            *TouchInputFilterDev;

  Status = gBS->LocateProtocol (
                  &gTouchInputFilterNullProtocolGuid,
                  NULL,
                  (VOID **) &NullProtocol
                  );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "CustomAbsolutePointerGetState LocateProtocol Failed Status=%r\n", Status));
    return Status;
  }

  TouchInputFilterDev = TOUCH_INPUT_FILTER_CONTEXT_FROM_NULL_PROTOCOL (NullProtocol);

  if (TouchInputFilterDev == NULL) {
    DEBUG ((DEBUG_ERROR, "CustomAbsolutePointerGetState Error NULL Context\n"));
    ASSERT (FALSE);
    return EFI_NOT_READY;
  }

  Status = TouchInputFilterDev->SourceAbsolutePointerGetState (This, State);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = InputFilter (TouchInputFilterDev, State);
  //
  // Expected Status if samples are not collected yet
  // or Finger Event was too short to start Swipe Detection or any filtering actions
  //
  if (Status == EFI_NOT_READY) {
    return EFI_SUCCESS;
  } else if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = ValidateData (TouchInputFilterDev, State);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_INFO, "CustomAbsolutePointerGetState ValidateData filtered data did not pass validation\n"));
    return EFI_SUCCESS;
  }

  return Status;
}


/**
  This is the declaration of an EFI image entry point. This entry point is
  the same for UEFI Applications, UEFI OS Loaders, and UEFI Drivers including
  both device drivers and bus drivers.

  @param  ImageHandle           The firmware allocated handle for the UEFI image.
  @param  SystemTable           A pointer to the EFI System Table.

  @retval EFI_SUCCESS           The operation completed successfully.
  @retval Others                An unexpected error occurred.
**/
EFI_STATUS
EFIAPI
TouchInputFilterDriverEntryPoint (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS    Status;

  Status = EFI_SUCCESS;
  //
  // Install UEFI Driver Model protocol(s).
  //
  Status = EfiLibInstallDriverBindingComponentName2 (
             ImageHandle,
             SystemTable,
             &gTouchInputFilterDriverBinding,
             ImageHandle,
             &gTouchInputFilterDriverComponentName,
             &gTouchInputFilterDriverComponentName2
             );
  ASSERT_EFI_ERROR (Status);

  //
  // Install Driver Supported EFI Version Protocol onto ImageHandle
  //
  Status = gBS->InstallMultipleProtocolInterfaces (
                  &ImageHandle,
                  &gEfiDriverSupportedEfiVersionProtocolGuid,
                  &gTouchInputFilterDriverSupportedEfiVersion,
                  NULL
                  );
  ASSERT_EFI_ERROR (Status);

  return Status;
}

/**
  Tests to see if this driver supports a given controller. If a child device is provided,
  it further tests to see if this driver supports creating a handle for the specified child device.

  This function checks to see if the driver specified by This supports the device specified by
  ControllerHandle. Drivers will typically use the device path attached to
  ControllerHandle and/or the services from the bus I/O abstraction attached to
  ControllerHandle to determine if the driver supports ControllerHandle. This function
  may be called many times during platform initialization. In order to reduce boot times, the tests
  performed by this function must be very small, and take as little time as possible to execute. This
  function must not change the state of any hardware devices, and this function must be aware that the
  device specified by ControllerHandle may already be managed by the same driver or a
  different driver. This function must match its calls to AllocatePages() with FreePages(),
  AllocatePool() with FreePool(), and OpenProtocol() with CloseProtocol().
  Because ControllerHandle may have been previously started by the same driver, if a protocol is
  already in the opened state, then it must not be closed with CloseProtocol(). This is required
  to guarantee the state of ControllerHandle is not modified by this function.

  @param[in]  This                 A pointer to the EFI_DRIVER_BINDING_PROTOCOL instance.
  @param[in]  ControllerHandle     The handle of the controller to test. This handle
                                   must support a protocol interface that supplies
                                   an I/O abstraction to the driver.
  @param[in]  RemainingDevicePath  A pointer to the remaining portion of a device path.  This
                                   parameter is ignored by device drivers, and is optional for bus
                                   drivers. For bus drivers, if this parameter is not NULL, then
                                   the bus driver must determine if the bus controller specified
                                   by ControllerHandle and the child controller specified
                                   by RemainingDevicePath are both supported by this
                                   bus driver.

  @retval EFI_SUCCESS              The device specified by ControllerHandle and
                                   RemainingDevicePath is supported by the driver specified by This.
  @retval EFI_ALREADY_STARTED      The device specified by ControllerHandle and
                                   RemainingDevicePath is already being managed by the driver
                                   specified by This.
  @retval EFI_ACCESS_DENIED        The device specified by ControllerHandle and
                                   RemainingDevicePath is already being managed by a different
                                   driver or an application that requires exclusive access.
                                   Currently not implemented.
  @retval EFI_UNSUPPORTED          The device specified by ControllerHandle and
                                   RemainingDevicePath is not supported by the driver specified by This.
**/
EFI_STATUS
EFIAPI
TouchInputFilterDriverBindingSupported (
  IN EFI_DRIVER_BINDING_PROTOCOL  *This,
  IN EFI_HANDLE                   ControllerHandle,
  IN EFI_DEVICE_PATH_PROTOCOL     *RemainingDevicePath OPTIONAL
  )
{
  TOUCH_INPUT_FILTER_NULL_PROTOCOL  *NullProtocol;
  EFI_ABSOLUTE_POINTER_PROTOCOL     *AbsolutePointerProtocol;
  EFI_STATUS                        Status;

  //
  // Verify that this driver is not already running on this device
  //
  Status = gBS->OpenProtocol (
                  ControllerHandle,
                  &gTouchInputFilterNullProtocolGuid,
                  (VOID **)&NullProtocol,
                  NULL,
                  NULL,
                  EFI_OPEN_PROTOCOL_TEST_PROTOCOL
                  );
  if (!EFI_ERROR (Status)) {
    return EFI_ALREADY_STARTED;
  }

  //
  // Determine if the pointer protocol is available.
  // This should be installed in touch driver.
  //
  Status = gBS->OpenProtocol (
                  ControllerHandle,
                  &gEfiAbsolutePointerProtocolGuid,
                  (VOID**)&AbsolutePointerProtocol,
                  This->DriverBindingHandle,
                  ControllerHandle,
                  EFI_OPEN_PROTOCOL_TEST_PROTOCOL
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  DEBUG (( DEBUG_INFO, "TouchInputFilterDriverBindingSupported AbsolutePointerProtocol found\n"));

  Status = gBS->OpenProtocol (
                  ControllerHandle,
                  &gEfiTouchPanelGuid,
                  NULL,
                  This->DriverBindingHandle,
                  ControllerHandle,
                  EFI_OPEN_PROTOCOL_TEST_PROTOCOL
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  DEBUG ((DEBUG_INFO, "TouchInputFilterDriverBindingSupported TouchPanel GUID found \n"));

  DEBUG ((DEBUG_INFO, "TouchInputFilterDriverBindingSupported Touch Supported \n"));

  return Status;
}


/**
  Starts a device controller or a bus controller.

  The Start() function is designed to be invoked from the EFI boot service ConnectController().
  As a result, much of the error checking on the parameters to Start() has been moved into this
  common boot service. It is legal to call Start() from other locations,
  but the following calling restrictions must be followed, or the system behavior will not be deterministic.
  1. ControllerHandle must be a valid EFI_HANDLE.
  2. If RemainingDevicePath is not NULL, then it must be a pointer to a naturally aligned
     EFI_DEVICE_PATH_PROTOCOL.
  3. Prior to calling Start(), the Supported() function for the driver specified by This must
     have been called with the same calling parameters, and Supported() must have returned EFI_SUCCESS.

  @param[in]  This                 A pointer to the EFI_DRIVER_BINDING_PROTOCOL instance.
  @param[in]  ControllerHandle     The handle of the controller to start. This handle
                                   must support a protocol interface that supplies
                                   an I/O abstraction to the driver.
  @param[in]  RemainingDevicePath  A pointer to the remaining portion of a device path.  This
                                   parameter is ignored by device drivers, and is optional for bus
                                   drivers. For a bus driver, if this parameter is NULL, then handles
                                   for all the children of Controller are created by this driver.
                                   If this parameter is not NULL and the first Device Path Node is
                                   not the End of Device Path Node, then only the handle for the
                                   child device specified by the first Device Path Node of
                                   RemainingDevicePath is created by this driver.
                                   If the first Device Path Node of RemainingDevicePath is
                                   the End of Device Path Node, no child handle is created by this
                                   driver.

  @retval EFI_SUCCESS              The device was started.
  @retval EFI_DEVICE_ERROR         The device could not be started due to a device error.Currently not implemented.
  @retval EFI_OUT_OF_RESOURCES     The request could not be completed due to a lack of resources.
  @retval Others                   The driver failded to start the device.

**/
EFI_STATUS
EFIAPI
TouchInputFilterDriverBindingStart (
  IN EFI_DRIVER_BINDING_PROTOCOL    *This,
  IN EFI_HANDLE                     ControllerHandle,
  IN EFI_DEVICE_PATH_PROTOCOL       *RemainingDevicePath OPTIONAL
  )
{
  TOUCH_INPUT_FILTER_DEV            *TouchInputFilterDev;
  EFI_ABSOLUTE_POINTER_PROTOCOL     *AbsPtrProtocol;
  UINT8                             Index;
  EFI_STATUS                        Status;

  DEBUG ((DEBUG_INFO, "TouchInputFilterDriverBindingStart Start\n"));

  TouchInputFilterDev = AllocateZeroPool (sizeof (TOUCH_INPUT_FILTER_DEV));
  if (TouchInputFilterDev == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  TouchInputFilterDev->Signature         = TOUCH_INPUT_FILTER_SIGNATURE;
  TouchInputFilterDev->CurrentX          = 0;
  TouchInputFilterDev->CurrentY          = 0;
  TouchInputFilterDev->PreviousX         = 0;
  TouchInputFilterDev->PreviousY         = 0;
  TouchInputFilterDev->SwipeDirection    = SWIPE_UNKNOWN;
  TouchInputFilterDev->SamplesCollected  = 0;
  TouchInputFilterDev->FingerDownEvent   = TRUE;

  AddUnicodeString2 ("eng", gTouchInputFilterDriverComponentName.SupportedLanguages,  &TouchInputFilterDev->ControllerNameTable, L"Touch Input Filter Driver", TRUE);
  AddUnicodeString2 ("en",  gTouchInputFilterDriverComponentName2.SupportedLanguages, &TouchInputFilterDev->ControllerNameTable, L"Touch Input Filter Driver", FALSE);

  TouchInputFilterDev->Buffer.Length = sizeof (TouchInputFilterDev->Buffer.X)/sizeof(UINT64);
  TouchInputFilterDev->Buffer.Index  = 0;
  TouchInputFilterDev->Buffer.TotalX = 0;
  TouchInputFilterDev->Buffer.TotalY = 0;

  TouchInputFilterDev->SwipeDetectMinDistance  = 20;
  //
  //  Adjust to aspect ratio
  //
  TouchInputFilterDev->SwipeDetectMinXDistance = 16 * TouchInputFilterDev->SwipeDetectMinDistance;
  TouchInputFilterDev->SwipeDetectMinYDistance = 10 * TouchInputFilterDev->SwipeDetectMinDistance;
  TouchInputFilterDev->SampleRate              = 3;

  for (Index = 0; Index < TouchInputFilterDev->Buffer.Length; Index++) {
    TouchInputFilterDev->Buffer.X[Index] = 0;
    TouchInputFilterDev->Buffer.Y[Index] = 0;
  }

  //
  // Install only TouchInputFilterNull Protocol
  //
  Status = gBS->InstallMultipleProtocolInterfaces (
                  &ControllerHandle,
                  &gTouchInputFilterNullProtocolGuid,
                  &TouchInputFilterDev->NullProtocol,
                  NULL,
                  NULL
                  );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "TouchInputFilterDriverBindingStart failed to install upper protocol, Status: %r\n", Status));
    TouchInputFilterDriverCleanUp (TouchInputFilterDev);
    return Status;
  }

  Status = gBS->HandleProtocol (ControllerHandle, &gEfiAbsolutePointerProtocolGuid, (VOID **) &AbsPtrProtocol);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "TouchInputFilterDriverBindingStart HandleProtocol Status=%r\n", Status));
    TouchInputFilterDriverCleanUp (TouchInputFilterDev);
    return Status;
  }

  //
  // Hook to Absolute Pointer GetState
  //
  TouchInputFilterDev->SourceAbsolutePointerGetState  = AbsPtrProtocol->GetState;
  AbsPtrProtocol->GetState                            = CustomAbsolutePointerGetState;

  //
  // Create ExitBootServices/LegacyBootEvent for calling the IntegratedTouchDriverBindingStop procedure
  //
  Status = gBS->CreateEventEx (
                  EVT_NOTIFY_SIGNAL,
                  TPL_NOTIFY,
                  TouchInputFilterDriverExitEvent,
                  TouchInputFilterDev,
                  &gEfiEventExitBootServicesGuid,
                  &TouchInputFilterDev->ExitEvent
                  );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "TouchInputFilterDriverBindingStart Create Exit Events Status=%r\n", Status));
    TouchInputFilterDriverCleanUp (TouchInputFilterDev);
    return Status;
  }

  Status = gBS->CreateEventEx (
                  EVT_NOTIFY_SIGNAL,
                  TPL_NOTIFY,
                  TouchInputFilterDriverExitEvent,
                  TouchInputFilterDev,
                  &gEfiEventLegacyBootGuid,
                  &TouchInputFilterDev->ExitEvent
                  );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "TouchInputFilterDriverBindingStart Create Exit Events Status=%r\n", Status));
    TouchInputFilterDriverCleanUp (TouchInputFilterDev);
    return Status;
  }

  DEBUG ((DEBUG_INFO, "TouchInputFilterDriverBindingStart End\n"));

  return Status;
}


/**
  Stops a device controller or a bus controller.

  The Stop() function is designed to be invoked from the EFI boot service DisconnectController().
  As a result, much of the error checking on the parameters to Stop() has been moved
  into this common boot service. It is legal to call Stop() from other locations,
  but the following calling restrictions must be followed, or the system behavior will not be deterministic.
  1. ControllerHandle must be a valid EFI_HANDLE that was used on a previous call to this
     same driver's Start() function.
  2. The first NumberOfChildren handles of ChildHandleBuffer must all be a valid
     EFI_HANDLE. In addition, all of these handles must have been created in this driver's
     Start() function, and the Start() function must have called OpenProtocol() on
     ControllerHandle with an Attribute of EFI_OPEN_PROTOCOL_BY_CHILD_CONTROLLER.

  @param[in]  This              A pointer to the EFI_DRIVER_BINDING_PROTOCOL instance.
  @param[in]  ControllerHandle  A handle to the device being stopped. The handle must
                                support a bus specific I/O protocol for the driver
                                to use to stop the device.
  @param[in]  NumberOfChildren  The number of child device handles in ChildHandleBuffer.
  @param[in]  ChildHandleBuffer An array of child handles to be freed. May be NULL
                                if NumberOfChildren is 0.

  @retval EFI_SUCCESS           The device was stopped.
  @retval EFI_DEVICE_ERROR      The device could not be stopped due to a device error.

**/
EFI_STATUS
EFIAPI
TouchInputFilterDriverBindingStop (
  IN EFI_DRIVER_BINDING_PROTOCOL  *This,
  IN EFI_HANDLE                   ControllerHandle,
  IN UINTN                        NumberOfChildren,
  IN EFI_HANDLE                   *ChildHandleBuffer OPTIONAL
  )
{
  TOUCH_INPUT_FILTER_NULL_PROTOCOL  *NullProtocol;
  TOUCH_INPUT_FILTER_DEV            *TouchInputFilterDev;
  EFI_STATUS                        Status;

  DEBUG ((DEBUG_INFO, "TouchInputFilterDriverBindingStop Start\n"));

  //
  // Check if IntegratedTouchNullProtocol is installed
  //
  Status = gBS->OpenProtocol (
                  ControllerHandle,
                  &gTouchInputFilterNullProtocolGuid,
                  (VOID **) &NullProtocol,
                  This->DriverBindingHandle,
                  ControllerHandle,
                  EFI_OPEN_PROTOCOL_GET_PROTOCOL
                  );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "  TouchInputFilterDriverBindingStop Failed to get TouchInputFilterNullProtocol, Status: %r\n", Status));
    return EFI_UNSUPPORTED;
  }

  //
  // Get iTouchDevice context from Null Protocol
  //
  TouchInputFilterDev = TOUCH_INPUT_FILTER_CONTEXT_FROM_NULL_PROTOCOL (NullProtocol);

  Status = gBS->UninstallMultipleProtocolInterfaces (
                    ControllerHandle,
                    &gTouchInputFilterNullProtocolGuid,
                    &TouchInputFilterDev->NullProtocol,
                    NULL
                    );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_WARN, "  TouchInputFilterDriverBindingStop Failed to Uninstall IntegratedTouchNullProtocol, Status: %r\n", Status));
  }

  //
  // Free all the structures that were allocated on behalf of ControllerHandle in Start().
  //
  TouchInputFilterDriverCleanUp (TouchInputFilterDev);

  DEBUG ((DEBUG_INFO, "TouchInputFilterDriverBindingStop End\n"));

  return Status;
}

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
  )
{
  TOUCH_INPUT_FILTER_DEV     *TouchInputFilterDev;

  TouchInputFilterDev = Context;

  DEBUG ((DEBUG_INFO, "TouchInputFilterDriverExitEvent Start \n"));

  TouchInputFilterDriverCleanUp (TouchInputFilterDev);

  if (Event != NULL) {
    gBS->CloseEvent (Event);
  }

  DEBUG ((DEBUG_INFO, "TouchInputFilterDriverExitEvent End \n"));
  return;
}


/**
  Free Touch Input Filter Driver structure allocated in Start()
  Free allocate Unicde Strings

  @param[in]  iTouchDev       context of iTouch device

**/
VOID
EFIAPI
TouchInputFilterDriverCleanUp (
  IN TOUCH_INPUT_FILTER_DEV  *TouchInputFilterDev
  )
{

  DEBUG ((DEBUG_INFO, "TouchInputFilterDriverCleanUp Start \n"));

  //
  // Check if CleanUp is required
  //
  if (TouchInputFilterDev->Signature != TOUCH_INPUT_FILTER_SIGNATURE) {
    DEBUG ((DEBUG_INFO, "TouchInputFilterDriverCleanUp Touch Struct already freed. End \n"));
    return;
  }

  //
  // Done with the controller name table
  //
  FreeUnicodeStringTable (TouchInputFilterDev->ControllerNameTable);

  //
  //  Done with the driver context
  //
  ZeroMem (TouchInputFilterDev, sizeof (TOUCH_INPUT_FILTER_DEV));
  FreePool (TouchInputFilterDev);

  DEBUG ((DEBUG_INFO, "TouchInputFilterDriverCleanUp End \n"));
}

/**
  Unloads an image.

  @param  ImageHandle           Handle that identifies the image to be unloaded.

  @retval EFI_SUCCESS           The image has been unloaded.
  @retval EFI_INVALID_PARAMETER ImageHandle is not a valid image handle.

**/
EFI_STATUS
EFIAPI
TouchInputFilterDriverUnload (
  IN EFI_HANDLE  ImageHandle
  )
{
  EFI_STATUS  Status;
  EFI_HANDLE  *HandleBuffer;
  UINTN       HandleCount;
  UINTN       Index;

  Status = EFI_SUCCESS;

  //
  // Retrieve array of all handles in the handle database
  //
  Status = gBS->LocateHandleBuffer (
                  AllHandles,
                  NULL,
                  NULL,
                  &HandleCount,
                  &HandleBuffer
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // Disconnect the current driver from handles in the handle database
  //
  for (Index = 0; Index < HandleCount; Index++) {
    Status = gBS->DisconnectController (HandleBuffer[Index], ImageHandle, NULL);
  }

  //
  // Free the array of handles
  //
  FreePool (HandleBuffer);

  //
  // Uninstall the driver version protocol
  //
  Status = gBS->UninstallMultipleProtocolInterfaces (
                  ImageHandle,
                  &gEfiDriverSupportedEfiVersionProtocolGuid,
                  &gTouchInputFilterDriverSupportedEfiVersion,
                  NULL
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // Uninstall protocols installed in the driver entry point
  //
  Status = gBS->UninstallMultipleProtocolInterfaces (
                  ImageHandle,
                  &gEfiDriverBindingProtocolGuid,  &gTouchInputFilterDriverBinding,
                  &gEfiComponentNameProtocolGuid,  &gTouchInputFilterDriverComponentName,
                  &gEfiComponentName2ProtocolGuid, &gTouchInputFilterDriverComponentName2,
                  NULL
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }
  return EFI_SUCCESS;
}


