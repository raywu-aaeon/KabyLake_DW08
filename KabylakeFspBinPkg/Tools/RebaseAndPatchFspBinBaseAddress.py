## @ RebaseAndPatchFspBinBaseAddress.py
#
# Copyright (c) 2016, Intel Corporation. All rights reserved.<BR>
# This program and the accompanying materials are licensed and made available under
# the terms and conditions of the BSD License that accompanies this distribution.
# The full text of the license may be found at
# http://opensource.org/licenses/bsd-license.php.
#
# THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
# WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
#
##
import os
import sys
import re
import subprocess

if len(sys.argv) != 7:
  print "RebaseAndPatchFspBinBaseAddress.py - Error in number of arguments received"
  print "Usage - RebaseAndPatchFspBinBaseAddress.py <Build Target - DEBUG/RELEASE> <FlashMap file path> <FspBinPkg Folder> <Fsp.fd file name> <Dsc file path to be patched> <pad_offset for Fsp-S Base Address>"
# AMI_OVERRIDE_START - For py2exe
#  exit(1)
  sys.exit(1)
# AMI_OVERRIDE_END - For py2exe

buildTarget       = sys.argv[1].upper()
flashMapName      = sys.argv[2]
fspBinPath        = sys.argv[3]
fspBinFile        = sys.argv[4]
targetDscFile     = sys.argv[5]
fvOffset          = long(sys.argv[6], 16)
fspBinFileRebased = "Fsp_Rebased.fd"
# AMI_OVERRIDE_START - Output Build folder
fspRebasedBinPath = "Build"
# AMI_OVERRIDE_END - Output Build folder

#
# Make sure argument passed or valid
#
if not (buildTarget in "DEBUG" or buildTarget in "RELEASE") :
  print "WARNING!  " + str(buildTarget) + " is not a valid build target"
# AMI_OVERRIDE_START - For py2exe
#  exit(1)
  sys.exit(1)
# AMI_OVERRIDE_END - For py2exe
if not os.path.exists(flashMapName):
  print "WARNING!  " + str(flashMapName) + " is not found."
# AMI_OVERRIDE_START - For py2exe
#  exit(1)
  sys.exit(1)
# AMI_OVERRIDE_END - For py2exe
fspBinFilePath = fspBinPath + "\\" + fspBinFile
if not os.path.exists(fspBinFilePath):
  print "WARNING!  " + str(fspBinFilePath) + " is not found."
# AMI_OVERRIDE_START - For py2exe
#  exit(1)
  sys.exit(1)
# AMI_OVERRIDE_END - For py2exe
if not os.path.exists(targetDscFile):
  print "WARNING!  " + str(targetDscFile) + " is not found."
# AMI_OVERRIDE_START - For py2exe
#  exit(1)
  sys.exit(1)
# AMI_OVERRIDE_END - For py2exe
ext_file = str(os.path.splitext(targetDscFile)[-1]).lower()
if ext_file != ".dsc":
  print "WARNING!  " + str(targetDscFile) + " is not a dsc file"
# AMI_OVERRIDE_START - For py2exe
#  exit(1)
  sys.exit(1)
# AMI_OVERRIDE_END - For py2exe

#
# Get the FSP-S / FSP-M-T FV Base Address from Flash Map
#
file = open (flashMapName, "r")
data = file.read ()

# Get the Flash Base Address
# AMI_OVERRIDE_START - For AMI platform.fdf
#flashBase = long(data.split("FLASH_BASE")[1].split("=")[1].split()[0], 16)
flashBase = long(data.split("BaseAddress")[1].split("=")[1].split()[0], 16)
# AMI_OVERRIDE_END - For AMI platform.fdf

# Based on Build Target, select the section in the FlashMap file
# AMI_OVERRIDE_START - Don't use
#if buildTarget in "DEBUG": 
#  flashmap = data.rsplit("!if $(TARGET) == DEBUG", 1)[1].split("!else")[0]
#else:
#  flashmap = data.rsplit("!else", 1)[1]
# AMI_OVERRIDE_END - Don't use

# Get FSP-S & FSP-M-T offset & calculate the base
# AMI_OVERRIDE_START - For AMI platform.fdf
#for line in flashmap.split("\n"):
for line in data.split("\n"):
# AMI_OVERRIDE_END - For AMI platform.fdf
  if "PcdFlashFvFspsOffset" in line:
    fspSBaseOffset = long(line.split("=")[1].split()[0], 16)
  if "PcdFlashFvFspmtOffset" in line:
    fspMBaseOffset = long(line.split("=")[1].split()[0], 16)
file.close()

#
# Get FSP-M Size, in order to calculate the FSP-T Base. Used SplitFspBin.py script 
# to dump the header, and get the ImageSize in FSP-M section
#
# AMI_OVERRIDE_START - Change for py2exe
#Process = subprocess.Popen("python IntelFsp2Pkg\Tools\SplitFspBin.py info -f" + fspBinFilePath, stdout=subprocess.PIPE)
Process = subprocess.Popen("IntelFsp2Pkg\Tools\SplitFspBin.exe info -f" + fspBinFilePath, stdout=subprocess.PIPE)
# AMI_OVERRIDE_END - Change for py2exe
Output = Process.communicate()[0]
FsptInfo = Output.rsplit("FSP_M", 1);
for line in FsptInfo[1].split("\n"):
  if "ImageSize" in line:
    fspMSize = long(line.split("=")[1], 16)
    break

# Calculate FSP-S/M/T base address, to which re-base has to be done
fspSBaseAddress = flashBase + fspSBaseOffset + fvOffset
fspMBaseAddress = flashBase + fspMBaseOffset
fspTBaseAddress = flashBase + fspMBaseOffset + fspMSize

#
# Re-base FSP bin file to new address and save it as fspBinFileRebased using SplitFspBin.py
#
# AMI_OVERRIDE_START - Change for py2exe and output build folder
#rebaseArguments = fspBinFilePath + " -c s m t -b " + str(hex(fspSBaseAddress).rstrip("L")) + " " + str(hex(fspMBaseAddress).rstrip("L")) + " " + str(hex(fspTBaseAddress).rstrip("L")) + " -o" + fspBinPath + " -n " + fspBinFileRebased
rebaseArguments = fspBinFilePath + " -c s m t -b " + str(hex(fspSBaseAddress).rstrip("L")) + " " + str(hex(fspMBaseAddress).rstrip("L")) + " " + str(hex(fspTBaseAddress).rstrip("L")) + " -o" + fspRebasedBinPath + " -n " + fspBinFileRebased
#os.system("python IntelFsp2Pkg\Tools\SplitFspBin.py rebase -f" + rebaseArguments)
os.system("IntelFsp2Pkg\Tools\SplitFspBin.exe rebase -f" + rebaseArguments)
# AMI_OVERRIDE_END - Change for py2exe and output build folder

#
# Split FSP bin to FSP-S/M/T segments
#
# AMI_OVERRIDE_START - Change for py2exe and output build folder
#splitArguments = fspBinPath +"\\" + fspBinFileRebased + " -o " + fspBinPath + " -n Fsp_Rebased.fd"
splitArguments = fspRebasedBinPath +"\\" + fspBinFileRebased + " -o " + fspRebasedBinPath + " -n Fsp_Rebased.fd"
#os.system("python IntelFsp2Pkg\Tools\SplitFspBin.py split -f" + splitArguments)
os.system("IntelFsp2Pkg\Tools\SplitFspBin.exe split -f" + splitArguments)
# AMI_OVERRIDE_END - Change for py2exe and output build folder

#
# Patch dsc file with the re-based FSP-S/M/T address, so internally build will use the same.
#
DscFile  = open(targetDscFile, "r")
DscLines = DscFile.readlines()
DscFile.close()
DscContent = []
for line in DscLines:
  DscContent.append(line)
DscFile = open(targetDscFile,"w")
for index in range(len(DscContent)):
  DscLine = DscContent[index]
  Match = re.match("([\s_a-zA-Z0-9]+).Pcd(Fspt|Fspm|Fsps)BaseAddress",DscLine)
  if Match:
    DscLine = Match.group(1) + ".Pcd" + Match.group(2) + "BaseAddress|0x"
    if Match.group(2) == 'Fspt':
      BaseAddrStr = str(hex(fspTBaseAddress)[2:]).zfill(8).upper().rstrip('L')
    elif Match.group(2) == 'Fspm':
      BaseAddrStr = str(hex(fspMBaseAddress)[2:]).zfill(8).upper().rstrip('L')
    elif Match.group(2) == 'Fsps':
      BaseAddrStr = str(hex(fspSBaseAddress)[2:]).zfill(8).upper().rstrip('L')
    DscLine = DscLine + BaseAddrStr + "\n"
  DscFile.writelines(DscLine)
DscFile.close()
# AMI_OVERRIDE_START - For py2exe
#exit(0)
sys.exit(0)
# AMI_OVERRIDE_END - For py2exe
