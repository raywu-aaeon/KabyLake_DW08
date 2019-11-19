/** @file
  PostCode status code definition.

 @copyright
  INTEL CONFIDENTIAL
  Copyright 2010 - 2016 Intel Corporation.

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

  This file contains 'Framework Code' and is licensed as such under the terms
  of your license agreement with Intel or your vendor. This file may not be 
  modified, except as allowed by additional terms of your license agreement.

@par Specification Reference:
**/

#ifndef __PLATFORM_STATUS_CODES_INTERNAL_H__
#define __PLATFORM_STATUS_CODES_INTERNAL_H__

#include <Pi/PiStatusCode.h>

typedef struct{
  EFI_STATUS_CODE_VALUE Value;
  UINT32                Data;
} STATUS_CODE_TO_DATA_MAP;

// AMI_OVERRIDE_START >>> Sync to StatusCodes.h
//AMI Status Code definitions
#define AMI_STATUS_CODE_CLASS EFI_OEM_SPECIFIC //0x8000

// Host Processor AMI-specific codes
#define AMI_CU_HP_PC_DXE_CPU_INIT (AMI_STATUS_CODE_CLASS | 0x00000000)
#define AMI_CU_HP_EC_DXE_CPU_ERROR (AMI_STATUS_CODE_CLASS | 0x00000001)
//Chipset AMI-specific codes
#define AMI_CHIPSET_PC_PEI_CAR_SB_INIT (AMI_STATUS_CODE_CLASS | 0x00000000)
#define AMI_CHIPSET_PC_PEI_CAR_NB_INIT (AMI_STATUS_CODE_CLASS | 0x00000001)
//PEIM AMI-specific codes
#define AMI_PEIM_PC_S3_STARTED (AMI_STATUS_CODE_CLASS | 0x00000000)
#define AMI_PEIM_PC_VIDEO_REPOST (AMI_STATUS_CODE_CLASS | 0x00000003)
//Boot Services AMI-specific codes
#define AMI_BS_PC_NVRAM_INIT (AMI_STATUS_CODE_CLASS | 0x00000000)
//DXE Driver AMI-specific codes
#define AMI_DXE_BS_PC_ACPI_INIT (AMI_STATUS_CODE_CLASS | 0x00000000)
#define AMI_DXE_BS_PC_CSM_INIT (AMI_STATUS_CODE_CLASS | 0x00000001)
//Boot Services AMI-specific codes
#define AMI_BS_PC_NVRAM_GC (AMI_STATUS_CODE_CLASS | 0x00000001)
#define AMI_BS_PC_CONFIG_RESET (AMI_STATUS_CODE_CLASS | 0x00000002)
// AMI_OVERRIDE_END <<< Sync to StatusCodes.h

//                       
// Enable PEI/DXE status code
//                         
#define PEI_STATUS_CODE 1  
#define DXE_STATUS_CODE 1  
  
#define STATUS_CODE_TYPE(Type)                ((Type)&EFI_STATUS_CODE_TYPE_MASK)
#define STATUS_CODE_CLASS(Value)              ((Value)&EFI_STATUS_CODE_CLASS_MASK)

//Progress/Error codes
#define PEI_CORE_STARTED                      (EFI_SOFTWARE_PEI_CORE | EFI_SW_PEI_CORE_PC_ENTRY_POINT)         
#define PEI_RESET_NOT_AVAILABLE               (EFI_SOFTWARE_PEI_CORE | EFI_SW_PS_EC_RESET_NOT_AVAILABLE)       
#define PEI_DXEIPL_NOT_FOUND                  (EFI_SOFTWARE_PEI_CORE | EFI_SW_PEI_CORE_EC_DXEIPL_NOT_FOUND)    
#define PEI_DXE_CORE_NOT_FOUND                (EFI_SOFTWARE_PEI_CORE | EFI_SW_PEI_CORE_EC_DXE_CORRUPT)         
#define PEI_S3_RESUME_ERROR	                  (EFI_SOFTWARE_PEI_CORE | EFI_SW_PEI_EC_S3_RESUME_FAILED)         
#define PEI_RECOVERY_FAILED                   (EFI_SOFTWARE_PEI_CORE | EFI_SW_PEI_EC_RECOVERY_FAILED)          
#define DXE_CORE_STARTED                      (EFI_SOFTWARE_DXE_CORE | EFI_SW_DXE_CORE_PC_ENTRY_POINT)
      
//#define DXE_EXIT_BOOT_SERVICES_BEGIN 0xF8
#define DXE_EXIT_BOOT_SERVICES_END            (EFI_SOFTWARE_EFI_BOOT_SERVICE | EFI_SW_BS_PC_EXIT_BOOT_SERVICES)
                                              
// Reported by CPU PEIM                       
#define PEI_CAR_CPU_INIT                      (EFI_COMPUTING_UNIT_HOST_PROCESSOR | EFI_CU_HP_PC_POWER_ON_INIT)
                                              
// Reported by NB PEIM                        
//#define PEI_CAR_NB_INIT                       (EFI_COMPUTING_UNIT_CHIPSET | EFI_CU_CHIPSET_NORTH_INIT)
                                              
// Reported by SB PEIM                        
//#define PEI_CAR_SB_INIT                       (EFI_COMPUTING_UNIT_CHIPSET | EFI_CU_CHIPSET_PC_SOUTH_INIT)

// AMI_OVERRIDE_START >>> Sync to StatusCodes.h
#define PEI_CAR_NB_INIT                       (EFI_COMPUTING_UNIT_CHIPSET | AMI_CHIPSET_PC_PEI_CAR_NB_INIT)
#define PEI_CAR_SB_INIT                       (EFI_COMPUTING_UNIT_CHIPSET | AMI_CHIPSET_PC_PEI_CAR_SB_INIT)
#define PEI_S3_STARTED                        (EFI_SOFTWARE_PEI_MODULE | AMI_PEIM_PC_S3_STARTED)
#define PEI_S3_VIDEO_REPOST                   (EFI_SOFTWARE_PEI_MODULE | AMI_PEIM_PC_VIDEO_REPOST)

#define DXE_NVRAM_INIT                        (EFI_SOFTWARE_EFI_BOOT_SERVICE | AMI_BS_PC_NVRAM_INIT)
#define DXE_CPU_INIT                          (EFI_COMPUTING_UNIT_HOST_PROCESSOR | AMI_CU_HP_PC_DXE_CPU_INIT)
#define DXE_ACPI_INIT                         (EFI_SOFTWARE_DXE_BS_DRIVER | AMI_DXE_BS_PC_ACPI_INIT)
#define DXE_CSM_INIT                          (EFI_SOFTWARE_DXE_BS_DRIVER | AMI_DXE_BS_PC_CSM_INIT)
#define DXE_BDS_CONNECT_DRIVERS               (EFI_SOFTWARE_DXE_BS_DRIVER | EFI_SW_DXE_BS_PC_BEGIN_CONNECTING_DRIVERS)
#define DXE_SETUP_VERIFYING_PASSWORD          (EFI_SOFTWARE_DXE_BS_DRIVER | EFI_SW_DXE_BS_PC_VERIFYING_PASSWORD)
#define DXE_NVRAM_CLEANUP                     (EFI_SOFTWARE_EFI_BOOT_SERVICE | AMI_BS_PC_NVRAM_GC)
#define DXE_CONFIGURATION_RESET               (EFI_SOFTWARE_EFI_BOOT_SERVICE | AMI_BS_PC_CONFIG_RESET)
#define DXE_CPU_ERROR                         (EFI_COMPUTING_UNIT_HOST_PROCESSOR | AMI_CU_HP_EC_DXE_CPU_ERROR)
// AMI_OVERRIDE_END <<< Sync to StatusCodes.h

//Reported by Memory Detection PEIM
#define PEI_MEMORY_SPD_READ                   (EFI_COMPUTING_UNIT_MEMORY | EFI_CU_MEMORY_PC_SPD_READ)
#define PEI_MEMORY_PRESENCE_DETECT            (EFI_COMPUTING_UNIT_MEMORY | EFI_CU_MEMORY_PC_PRESENCE_DETECT)
#define PEI_MEMORY_TIMING                     (EFI_COMPUTING_UNIT_MEMORY | EFI_CU_MEMORY_PC_TIMING)
#define PEI_MEMORY_CONFIGURING                (EFI_COMPUTING_UNIT_MEMORY | EFI_CU_MEMORY_PC_CONFIGURING)
#define PEI_MEMORY_OPTIMIZING                 (EFI_COMPUTING_UNIT_MEMORY | EFI_CU_MEMORY_PC_OPTIMIZING)
#define PEI_MEMORY_INIT                       (EFI_COMPUTING_UNIT_MEMORY | EFI_CU_MEMORY_PC_INIT)
#define PEI_MEMORY_TEST                       (EFI_COMPUTING_UNIT_MEMORY | EFI_CU_MEMORY_PC_TEST)
#define PEI_MEMORY_INVALID_TYPE               (EFI_COMPUTING_UNIT_MEMORY | EFI_CU_MEMORY_EC_INVALID_TYPE)
#define PEI_MEMORY_INVALID_SPEED              (EFI_COMPUTING_UNIT_MEMORY | EFI_CU_MEMORY_EC_INVALID_SPEED)
#define PEI_MEMORY_SPD_FAIL                   (EFI_COMPUTING_UNIT_MEMORY | EFI_CU_MEMORY_EC_SPD_FAIL)
#define PEI_MEMORY_INVALID_SIZE               (EFI_COMPUTING_UNIT_MEMORY | EFI_CU_MEMORY_EC_INVALID_SIZE)
#define PEI_MEMORY_MISMATCH                   (EFI_COMPUTING_UNIT_MEMORY | EFI_CU_MEMORY_EC_MISMATCH)
#define PEI_MEMORY_S3_RESUME_FAILED           (EFI_COMPUTING_UNIT_MEMORY | EFI_CU_MEMORY_EC_S3_RESUME_FAIL)
#define PEI_MEMORY_NOT_DETECTED               (EFI_COMPUTING_UNIT_MEMORY | EFI_CU_MEMORY_EC_NONE_DETECTED)
#define PEI_MEMORY_NONE_USEFUL                (EFI_COMPUTING_UNIT_MEMORY | EFI_CU_MEMORY_EC_NONE_USEFUL)
#define PEI_MEMORY_ERROR                      (EFI_COMPUTING_UNIT_MEMORY | EFI_CU_EC_NON_SPECIFIC)
#define PEI_MEMORY_INSTALLED                  (EFI_SOFTWARE_PEI_SERVICE  | EFI_SW_PS_PC_INSTALL_PEI_MEMORY)
#define PEI_MEMORY_NOT_INSTALLED              (EFI_SOFTWARE_PEI_SERVICE  | EFI_SW_PEI_CORE_EC_MEMORY_NOT_INSTALLED)
#define PEI_MEMORY_INSTALLED_TWICE            (EFI_SOFTWARE_PEI_SERVICE  | EFI_SW_PS_EC_MEMORY_INSTALLED_TWICE)

//Reported by CPU PEIM
#define PEI_CPU_INIT                          (EFI_COMPUTING_UNIT_HOST_PROCESSOR | EFI_CU_PC_INIT_BEGIN)
#define PEI_CPU_CACHE_INIT                    (EFI_COMPUTING_UNIT_HOST_PROCESSOR | EFI_CU_HP_PC_CACHE_INIT)
#define PEI_CPU_BSP_SELECT                    (EFI_COMPUTING_UNIT_HOST_PROCESSOR | EFI_CU_HP_PC_BSP_SELECT)
#define PEI_CPU_AP_INIT                       (EFI_COMPUTING_UNIT_HOST_PROCESSOR | EFI_CU_HP_PC_AP_INIT)
#define PEI_CPU_SMM_INIT                      (EFI_COMPUTING_UNIT_HOST_PROCESSOR | EFI_CU_HP_PC_SMM_INIT)
#define PEI_CPU_INVALID_TYPE                  (EFI_COMPUTING_UNIT_HOST_PROCESSOR | EFI_CU_HP_EC_INVALID_TYPE)
#define PEI_CPU_INVALID_SPEED                 (EFI_COMPUTING_UNIT_HOST_PROCESSOR | EFI_CU_HP_EC_INVALID_SPEED)
#define PEI_CPU_MISMATCH                      (EFI_COMPUTING_UNIT_HOST_PROCESSOR | EFI_CU_HP_EC_MISMATCH)
#define PEI_CPU_SELF_TEST_FAILED              (EFI_COMPUTING_UNIT_HOST_PROCESSOR | EFI_CU_HP_EC_SELF_TEST)
#define PEI_CPU_CACHE_ERROR                   (EFI_COMPUTING_UNIT_HOST_PROCESSOR | EFI_CU_HP_EC_CACHE)
#define PEI_CPU_MICROCODE_UPDATE_FAILED       (EFI_COMPUTING_UNIT_HOST_PROCESSOR | EFI_CU_HP_EC_MICROCODE_UPDATE)
#define PEI_CPU_NO_MICROCODE                  (EFI_COMPUTING_UNIT_HOST_PROCESSOR | EFI_CU_HP_EC_NO_MICROCODE_UPDATE)
//If non of the errors above apply use this one
#define PEI_CPU_INTERNAL_ERROR                (EFI_COMPUTING_UNIT_HOST_PROCESSOR | EFI_CU_HP_EC_INTERNAL)
//Generic CPU error. It should only be used if non of the errors above apply
#define PEI_CPU_ERROR                         (EFI_COMPUTING_UNIT_HOST_PROCESSOR | EFI_CU_EC_NON_SPECIFIC)
                                              
// Reported by NB PEIM                        
#define PEI_MEM_NB_INIT                       (EFI_COMPUTING_UNIT_CHIPSET | EFI_CHIPSET_PC_PEI_MEM_NB_INIT)
// Reported by SB PEIM                        
#define PEI_MEM_SB_INIT                       (EFI_COMPUTING_UNIT_CHIPSET | EFI_CHIPSET_PC_PEI_MEM_SB_INIT)

//Reported by PEIM which detected forced or auto recovery condition
#define PEI_RECOVERY_AUTO                     (EFI_SOFTWARE_PEI_MODULE | EFI_SW_PEI_PC_RECOVERY_AUTO)
#define PEI_RECOVERY_USER                     (EFI_SOFTWARE_PEI_MODULE | EFI_SW_PEI_PC_RECOVERY_USER)
                                              
//Reported by DXE IPL                         
#define PEI_RECOVERY_PPI_NOT_FOUND            (EFI_SOFTWARE_PEI_MODULE | EFI_SW_PEI_EC_RECOVERY_PPI_NOT_FOUND)
#define PEI_S3_RESUME_PPI_NOT_FOUND           (EFI_SOFTWARE_PEI_MODULE | EFI_SW_PEI_EC_S3_RESUME_PPI_NOT_FOUND)
#define PEI_S3_RESUME_FAILED                  (EFI_SOFTWARE_PEI_MODULE | EFI_SW_PEI_EC_S3_RESUME_FAILED)

//Reported by Recovery PEIM
#define PEI_RECOVERY_STARTED                  (EFI_SOFTWARE_PEI_MODULE | EFI_SW_PEI_PC_RECOVERY_BEGIN)
#define PEI_RECOVERY_CAPSULE_FOUND            (EFI_SOFTWARE_PEI_MODULE | EFI_SW_PEI_PC_CAPSULE_LOAD)
#define PEI_RECOVERY_NO_CAPSULE               (EFI_SOFTWARE_PEI_MODULE | EFI_SW_PEI_EC_NO_RECOVERY_CAPSULE)
#define PEI_RECOVERY_CAPSULE_LOADED           (EFI_SOFTWARE_PEI_MODULE | EFI_SW_PEI_PC_CAPSULE_START)
#define PEI_RECOVERY_INVALID_CAPSULE          (EFI_SOFTWARE_PEI_MODULE | EFI_SW_PEI_EC_INVALID_CAPSULE_DESCRIPTOR)
                                              
//Reported by S3 Resume PEIM                  
#define PEI_S3_BOOT_SCRIPT                    (EFI_SOFTWARE_PEI_MODULE | EFI_SW_PEI_PC_S3_BOOT_SCRIPT)
#define PEI_S3_OS_WAKE                        (EFI_SOFTWARE_PEI_MODULE | EFI_SW_PEI_PC_OS_WAKE)
#define PEI_S3_BOOT_SCRIPT_ERROR              (EFI_SOFTWARE_PEI_MODULE | EFI_SW_PEI_EC_S3_BOOT_SCRIPT_ERROR)
#define PEI_S3_OS_WAKE_ERROR                  (EFI_SOFTWARE_PEI_MODULE | EFI_SW_PEI_EC_S3_OS_WAKE_ERROR)
                                              
#define PEI_PEIM_STARTED                      (EFI_SOFTWARE_PEI_CORE | EFI_SW_PC_INIT_BEGIN)
#define PEI_PEIM_ENDED                        (EFI_SOFTWARE_PEI_CORE | EFI_SW_PC_INIT_END)
                                              
//Reported by DXE IPL                         
#define PEI_DXE_IPL_STARTED                   (EFI_SOFTWARE_PEI_CORE | EFI_SW_PEI_CORE_PC_HANDOFF_TO_NEXT)

//Reported by PEIM which installs Reset PPI
#define PEI_RESET_SYSTEM                      (EFI_SOFTWARE_PEI_SERVICE | EFI_SW_PS_PC_RESET_SYSTEM)

//Reported by the PEIM or DXE driver which detected the error
#define GENERIC_MEMORY_CORRECTABLE_ERROR      (EFI_COMPUTING_UNIT_MEMORY | EFI_CU_MEMORY_EC_CORRECTABLE)
#define GENERIC_MEMORY_UNCORRECTABLE_ERROR    (EFI_COMPUTING_UNIT_MEMORY | EFI_CU_MEMORY_EC_UNCORRECTABLE)

//Reported by Flash Update DXE driver
#define DXE_FLASH_UPDATE_FAILED               (EFI_COMPUTING_UNIT_MEMORY | EFI_CU_MEMORY_EC_UPDATE_FAIL)

//Reported by the PEIM or DXE driver which detected the error
#define GENERIC_CPU_THERMAL_ERROR             (EFI_COMPUTING_UNIT_HOST_PROCESSOR | EFI_CU_HP_EC_THERMAL)
#define GENERIC_CPU_LOW_VOLTAGE               (EFI_COMPUTING_UNIT_HOST_PROCESSOR | EFI_CU_HP_EC_LOW_VOLTAGE)
#define GENERIC_CPU_HIGH_VOLTAGE              (EFI_COMPUTING_UNIT_HOST_PROCESSOR | EFI_CU_HP_EC_HIGH_VOLTAGE)
#define GENERIC_CPU_CORRECTABLE_ERROR         (EFI_COMPUTING_UNIT_HOST_PROCESSOR | EFI_CU_HP_EC_CORRECTABLE)
#define GENERIC_CPU_UNCORRECTABLE_ERROR       (EFI_COMPUTING_UNIT_HOST_PROCESSOR | EFI_CU_HP_EC_UNCORRECTABLE)
#define GENERIC_BAD_DATE_TIME_ERROR           (EFI_SOFTWARE_UNSPECIFIED | EFI_SW_EC_BAD_DATE_TIME)
#define GENERIC_MEMORY_SIZE_DECREASE          (EFI_COMPUTING_UNIT_MEMORY | EFI_CU_MEMORY_EC_MISMATCH)

//Reported by DXE Core
#define DXE_DRIVER_STARTED                    (EFI_SOFTWARE_EFI_DXE_SERVICE | EFI_SW_PC_INIT_BEGIN)
#define DXE_DRIVER_ENED                       (EFI_SOFTWARE_DXE_CORE | EFI_SW_PC_INIT_END)
#define DXE_ARCH_PROTOCOLS_AVAILABLE          (EFI_SOFTWARE_DXE_CORE | EFI_SW_DXE_CORE_PC_ARCH_READY)
#define DXE_DRIVER_CONNECTED                  (EFI_SOFTWARE_DXE_CORE | EFI_SW_DXE_CORE_PC_START_DRIVER)
#define DXE_ARCH_PROTOCOL_NOT_AVAILABLE       (EFI_SOFTWARE_DXE_CORE | EFI_SW_DXE_CORE_EC_NO_ARCH)

//Reported by DXE CPU driver
#define DXE_CPU_SELF_TEST_FAILED              (EFI_COMPUTING_UNIT_HOST_PROCESSOR | EFI_CU_HP_EC_SELF_TEST)

//Reported by PCI Host Bridge driver
#define DXE_NB_HB_INIT                        (EFI_COMPUTING_UNIT_CHIPSET | EFI_CHIPSET_PC_DXE_HB_INIT )

// Reported by NB Driver
#define DXE_NB_INIT                           (EFI_COMPUTING_UNIT_CHIPSET | EFI_CHIPSET_PC_DXE_NB_INIT )
#define DXE_NB_SMM_INIT                       (EFI_COMPUTING_UNIT_CHIPSET | EFI_CHIPSET_PC_DXE_NB_SMM_INIT )
#define DXE_NB_ERROR                          (EFI_COMPUTING_UNIT_CHIPSET | EFI_CHIPSET_EC_DXE_NB_ERROR )

// Reported by SB Driver(s)
#define DXE_SBRUN_INIT                        (EFI_COMPUTING_UNIT_CHIPSET | EFI_CHIPSET_PC_DXE_SB_RT_INIT )
#define DXE_SB_INIT                           (EFI_COMPUTING_UNIT_CHIPSET | EFI_CHIPSET_PC_DXE_SB_INIT )
#define DXE_SB_SMM_INIT                       (EFI_COMPUTING_UNIT_CHIPSET | EFI_CHIPSET_PC_DXE_SB_SMM_INIT )
#define DXE_SB_DEVICES_INIT                   (EFI_COMPUTING_UNIT_CHIPSET | EFI_CHIPSET_PC_DXE_SB_DEVICES_INIT )
#define DXE_SB_BAD_BATTERY                    (EFI_COMPUTING_UNIT_CHIPSET | EFI_CHIPSET_EC_BAD_BATTERY)
#define DXE_SB_ERROR                          (EFI_COMPUTING_UNIT_CHIPSET | EFI_CHIPSET_EC_DXE_SB_ERROR )

//Reported by DXE Core
#define DXE_BDS_STARTED                       (EFI_SOFTWARE_DXE_CORE | EFI_SW_DXE_CORE_PC_HANDOFF_TO_NEXT)

//Reported by BDS
//#define DXE_BDS_CONNECT_DRIVERS             (EFI_SOFTWARE_DXE_BS_DRIVER | EFI_SW_DXE_BS_PC_BEGIN_CONNECTING_DRIVERS)

//Reported by Boot Manager
#define DXE_READY_TO_BOOT                     (EFI_SOFTWARE_DXE_BS_DRIVER | EFI_SW_DXE_BS_PC_READY_TO_BOOT_EVENT)

//Reported by DXE Core
#define DXE_EXIT_BOOT_SERVICES                (EFI_SOFTWARE_EFI_BOOT_SERVICE | EFI_SW_BS_PC_EXIT_BOOT_SERVICES)
#define DXE_EXIT_BOOT_SERVICES_EVENT          (EFI_SOFTWARE_DXE_BS_DRIVER | EFI_SW_DXE_BS_PC_EXIT_BOOT_SERVICES_EVENT)

//Reported by driver that installs Runtime AP
#define RT_SET_VIRTUAL_ADDRESS_MAP_BEGIN      (EFI_SOFTWARE_EFI_RUNTIME_SERVICE | EFI_SW_RS_PC_SET_VIRTUAL_ADDRESS_MAP)
#define RT_SET_VIRTUAL_ADDRESS_MAP_END        (EFI_SOFTWARE_DXE_BS_DRIVER | EFI_SW_DXE_BS_PC_VIRTUAL_ADDRESS_CHANGE_EVENT)

//Reported by CSM
#define DXE_LEGACY_OPROM_INIT                 (EFI_SOFTWARE_DXE_BS_DRIVER | EFI_SW_DXE_BS_PC_LEGACY_OPROM_INIT)
#define DXE_LEGACY_BOOT                       (EFI_SOFTWARE_DXE_BS_DRIVER | EFI_SW_DXE_BS_PC_LEGACY_BOOT_EVENT)
#define DXE_LEGACY_OPROM_NO_SPACE             (EFI_SOFTWARE_DXE_BS_DRIVER | EFI_SW_DXE_BS_EC_LEGACY_OPROM_NO_SPACE)

//Reported by SETUP
//#define DXE_SETUP_VERIFYING_PASSWORD        (EFI_SOFTWARE_DXE_BS_DRIVER | EFI_SW_DXE_BS_PC_VERIFYING_PASSWORD)
#define DXE_SETUP_START                       (EFI_SOFTWARE_DXE_BS_DRIVER | EFI_SW_PC_USER_SETUP)
#define DXE_SETUP_INPUT_WAIT                  (EFI_SOFTWARE_DXE_BS_DRIVER | EFI_SW_PC_INPUT_WAIT)
#define DXE_INVALID_PASSWORD                  (EFI_SOFTWARE_DXE_BS_DRIVER | EFI_SW_DXE_BS_EC_INVALID_PASSWORD)
#define DXE_INVALID_IDE_PASSWORD              (EFI_SOFTWARE_DXE_BS_DRIVER | EFI_SW_DXE_BS_EC_INVALID_IDE_PASSWORD)
#define DXE_BOOT_OPTION_LOAD_ERROR            (EFI_SOFTWARE_DXE_BS_DRIVER | EFI_SW_DXE_BS_EC_BOOT_OPTION_LOAD_ERROR)
#define DXE_BOOT_OPTION_FAILED                (EFI_SOFTWARE_DXE_BS_DRIVER | EFI_SW_DXE_BS_EC_BOOT_OPTION_FAILED)

//Reported by a Driver that installs Reset AP
#define DXE_RESET_SYSTEM                      (EFI_SOFTWARE_EFI_RUNTIME_SERVICE | EFI_SW_RS_PC_RESET_SYSTEM)
#define DXE_RESET_NOT_AVAILABLE               (EFI_SOFTWARE_EFI_RUNTIME_SERVICE | EFI_SW_PS_EC_RESET_NOT_AVAILABLE)

// Reported by PCI bus driver
#define DXE_PCI_BUS_BEGIN                     (EFI_IO_BUS_PCI | EFI_IOB_PC_INIT)
#define DXE_PCI_BUS_ENUM                      (EFI_IO_BUS_PCI | EFI_IOB_PCI_BUS_ENUM)
#define DXE_PCI_BUS_HPC_INIT                  (EFI_IO_BUS_PCI | EFI_IOB_PCI_HPC_INIT)
#define DXE_PCI_BUS_REQUEST_RESOURCES         (EFI_IO_BUS_PCI | EFI_IOB_PCI_RES_ALLOC)
#define DXE_PCI_BUS_ASSIGN_RESOURCES          (EFI_IO_BUS_PCI | EFI_IOB_PC_ENABLE)
#define DXE_PCI_BUS_HOTPLUG                   (EFI_IO_BUS_PCI | EFI_IOB_PC_HOTPLUG)
#define DXE_PCI_BUS_OUT_OF_RESOURCES          (EFI_IO_BUS_PCI | EFI_IOB_EC_RESOURCE_CONFLICT)

// Reported by USB bus driver
#define DXE_USB_BEGIN                         (EFI_IO_BUS_USB | EFI_IOB_PC_INIT)
#define DXE_USB_RESET                         (EFI_IO_BUS_USB | EFI_IOB_PC_RESET)
#define DXE_USB_DETECT                        (EFI_IO_BUS_USB | EFI_IOB_PC_DETECT)
#define DXE_USB_ENABLE                        (EFI_IO_BUS_USB | EFI_IOB_PC_ENABLE)
#define DXE_USB_HOTPLUG                       (EFI_IO_BUS_USB | EFI_IOB_PC_HOTPLUG)

//Reported by IDE bus driver
#define DXE_IDE_BEGIN                         (EFI_IO_BUS_ATA_ATAPI | EFI_IOB_PC_INIT)
#define DXE_IDE_RESET                         (EFI_IO_BUS_ATA_ATAPI | EFI_IOB_PC_RESET)
#define DXE_IDE_DETECT                        (EFI_IO_BUS_ATA_ATAPI | EFI_IOB_PC_DETECT)
#define DXE_IDE_ENABLE                        (EFI_IO_BUS_ATA_ATAPI | EFI_IOB_PC_ENABLE)
#define DXE_IDE_SMART_ERROR                   (EFI_IO_BUS_ATA_ATAPI | EFI_IOB_ATA_BUS_SMART_OVERTHRESHOLD)
#define DXE_IDE_CONTROLLER_ERROR              (EFI_IO_BUS_ATA_ATAPI | EFI_IOB_EC_CONTROLLER_ERROR)
#define DXE_IDE_DEVICE_FAILURE                (EFI_IO_BUS_ATA_ATAPI | EFI_IOB_EC_INTERFACE_ERROR)

// Reported by SCSI bus driver
#define DXE_SCSI_BEGIN                        (EFI_IO_BUS_SCSI | EFI_IOB_PC_INIT)
#define DXE_SCSI_RESET                        (EFI_IO_BUS_SCSI | EFI_IOB_PC_RESET)
#define DXE_SCSI_DETECT                       (EFI_IO_BUS_SCSI | EFI_IOB_PC_DETECT)
#define DXE_SCSI_ENABLE                       (EFI_IO_BUS_SCSI | EFI_IOB_PC_ENABLE)

// Reported by Super I/O driver
#define DXE_SIO_INIT                          (EFI_IO_BUS_LPC | EFI_IOB_PC_INIT)

// Reported by Keyboard driver
#define DXE_KEYBOARD_INIT                     (EFI_PERIPHERAL_KEYBOARD | EFI_P_PC_INIT)
#define DXE_KEYBOARD_RESET                    (EFI_PERIPHERAL_KEYBOARD | EFI_P_PC_RESET)
#define DXE_KEYBOARD_DISABLE                  (EFI_PERIPHERAL_KEYBOARD | EFI_P_PC_DISABLE)
#define DXE_KEYBOARD_DETECT                   (EFI_PERIPHERAL_KEYBOARD | EFI_P_PC_PRESENCE_DETECT)
#define DXE_KEYBOARD_ENABLE                   (EFI_PERIPHERAL_KEYBOARD | EFI_P_PC_ENABLE)
#define DXE_KEYBOARD_CLEAR_BUFFER             (EFI_PERIPHERAL_KEYBOARD | EFI_P_KEYBOARD_PC_CLEAR_BUFFER)
#define DXE_KEYBOARD_SELF_TEST                (EFI_PERIPHERAL_KEYBOARD | EFI_P_KEYBOARD_PC_SELF_TEST)

// Reported by Mouse driver
#define DXE_MOUSE_INIT                        (EFI_PERIPHERAL_MOUSE | EFI_P_PC_INIT)
#define DXE_MOUSE_RESET                       (EFI_PERIPHERAL_MOUSE | EFI_P_PC_RESET)
#define DXE_MOUSE_DISABLE                     (EFI_PERIPHERAL_MOUSE | EFI_P_PC_DISABLE)
#define DXE_MOUSE_DETECT                      (EFI_PERIPHERAL_MOUSE | EFI_P_PC_PRESENCE_DETECT)
#define DXE_MOUSE_ENABLE                      (EFI_PERIPHERAL_MOUSE | EFI_P_PC_ENABLE)

// Reported by Mass Storage drivers
#define DXE_FIXED_MEDIA_INIT                  (EFI_PERIPHERAL_FIXED_MEDIA | EFI_P_PC_INIT)
#define DXE_FIXED_MEDIA_RESET                 (EFI_PERIPHERAL_FIXED_MEDIA | EFI_P_PC_RESET)
#define DXE_FIXED_MEDIA_DISABLE               (EFI_PERIPHERAL_FIXED_MEDIA | EFI_P_PC_DISABLE)
#define DXE_FIXED_MEDIA_DETECT                (EFI_PERIPHERAL_FIXED_MEDIA | EFI_P_PC_PRESENCE_DETECT)
#define DXE_FIXED_MEDIA_ENABLE                (EFI_PERIPHERAL_FIXED_MEDIA | EFI_P_PC_ENABLE)
#define DXE_REMOVABLE_MEDIA_INIT              (EFI_PERIPHERAL_REMOVABLE_MEDIA | EFI_P_PC_INIT)
#define DXE_REMOVABLE_MEDIA_RESET             (EFI_PERIPHERAL_REMOVABLE_MEDIA | EFI_P_PC_RESET)
#define DXE_REMOVABLE_MEDIA_DISABLE           (EFI_PERIPHERAL_REMOVABLE_MEDIA | EFI_P_PC_DISABLE)
#define DXE_REMOVABLE_MEDIA_DETECT            (EFI_PERIPHERAL_REMOVABLE_MEDIA | EFI_P_PC_PRESENCE_DETECT)
#define DXE_REMOVABLE_MEDIA_ENABLE            (EFI_PERIPHERAL_REMOVABLE_MEDIA | EFI_P_PC_ENABLE)


// Reported by BDS
#define DXE_CON_OUT_CONNECT                   (EFI_PERIPHERAL_LOCAL_CONSOLE | EFI_P_PC_INIT)
#define DXE_CON_IN_CONNECT                    (EFI_PERIPHERAL_KEYBOARD | EFI_P_PC_INIT)
#define DXE_NO_CON_OUT                        (EFI_PERIPHERAL_LOCAL_CONSOLE | EFI_P_EC_NOT_DETECTED)
#define DXE_NO_CON_IN                         (EFI_PERIPHERAL_KEYBOARD | EFI_P_EC_NOT_DETECTED)

#endif
