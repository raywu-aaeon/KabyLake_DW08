!IFNDEF PS

!IFNDEF FV_MAIN
!ERROR FV_MAIN Undefined
!ELSE
FV_MAIN = $(FV_MAIN) \
          VideoRom \
          RaidOR \
          RsteOR \
          PxeOR
!ENDIF

!ELSE

!INCLUDE $(EFI_SOURCE)\tokens.mak

PKGMAK =  $(EFI_SOURCE)\Build\Package.mak
DST_DIR = $(OUTPUT_DIR)\$(TEMP_DIR)\$(PROCESSOR)

#=================GUID Section==========================
VideoRom_FILE_GUID = 6A5F1EF9-C478-42ba-A4DB-C7846CC7DB57
RaidOR_FILE_GUID = 2BB1381D-AE4F-4b37-B999-812F93811282
RsteOR_FILE_GUID = BF3CE2C0-FCEE-11E1-A21F-0800200C9A66
PxeOR_FILE_GUID = 58531F11-5C10-4dea-A54C-0A017FA03425
#=======================================================

$(DST_DIR)\VideoRom.ffs: $(DST_DIR)\VideoRom.pkg
  $(GENSECTION) -I $(EFI_SOURCE)\$(PLATFORM_PACKAGE)\Binaries\LegacyRom\VideoRom\LegacyVideoRom.bin -O $*.sec -S EFI_SECTION_RAW
  $(GENFFSFILE) -B $(DST_DIR) -P1 $*.pkg -V -o $@

$(DST_DIR)\RaidOR.ffs: $(DST_DIR)\RaidOR.pkg
  $(GENSECTION) -I $(EFI_SOURCE)\$(PLATFORM_PACKAGE)\Binaries\LegacyRom\RaidRom\RaidOrom.bin -O $*.sec -S EFI_SECTION_RAW
  $(GENFFSFILE) -B $(DST_DIR) -P1 $*.pkg -V -o $@

$(DST_DIR)\RsteOR.ffs: $(DST_DIR)\RsteOR.pkg
  $(GENSECTION) -I $(EFI_SOURCE)\$(PLATFORM_PACKAGE)\Binaries\LegacyRom\RaidRom\RsteRaidOrom.bin -O $*.sec -S EFI_SECTION_RAW
  $(GENFFSFILE) -B $(DST_DIR) -P1 $*.pkg -V -o $@

$(DST_DIR)\PxeOR.ffs: $(DST_DIR)\PxeOR.pkg
  $(GENSECTION) -I $(EFI_SOURCE)\$(PLATFORM_PACKAGE)\InternalOnly\LegacyRom\PxeUndiRom\PxeUndiRom.BIN -O $*.sec -S EFI_SECTION_RAW
  $(GENFFSFILE) -B $(DST_DIR) -P1 $*.pkg -V -o $@

OpRomFfs:   $(DST_DIR)\VideoRom.ffs \
            $(DST_DIR)\RaidOR.ffs \
            $(DST_DIR)\RsteOR.ffs \
            $(DST_DIR)\PxeOR.ffs

$(DST_DIR)\VideoRom.pkg: $(EFI_SOURCE)\$(PLATFORM_PACKAGE)\Binaries\LegacyRom\VideoRom\LegacyVideoRom.bin
  nmake -f $(PKGMAK) \
                    GENCRC32SECTION=$(GENCRC32SECTION) \
                    COMPONENT_TYPE=LEGACY16 \
                    PACKAGE_FILENAME=$*.pkg \
                    DEST_DIR=$(DST_DIR) \
                    BASE_NAME=$(*B) \
                    FILE_GUID=$(VideoRom_FILE_GUID)

$(DST_DIR)\RaidOR.pkg: $(EFI_SOURCE)\$(PLATFORM_PACKAGE)\Binaries\LegacyRom\RaidRom\RaidOrom.bin
  nmake -f $(PKGMAK) \
                    GENCRC32SECTION=$(GENCRC32SECTION) \
                    COMPONENT_TYPE=LEGACY16 \
                    PACKAGE_FILENAME=$*.pkg \
                    DEST_DIR=$(DST_DIR) \
                    BASE_NAME=$(*B) \
                    FILE_GUID=$(RaidOR_FILE_GUID)

$(DST_DIR)\RsteOR.pkg: $(EFI_SOURCE)\$(PLATFORM_PACKAGE)\Binaries\LegacyRom\RaidRom\RsteRaidOrom.bin
  nmake -f $(PKGMAK) \
                    GENCRC32SECTION=$(GENCRC32SECTION) \
                    COMPONENT_TYPE=LEGACY16 \
                    PACKAGE_FILENAME=$*.pkg \
                    DEST_DIR=$(DST_DIR) \
                    BASE_NAME=$(*B) \
                    FILE_GUID=$(RsteOR_FILE_GUID)

$(DST_DIR)\PxeOR.pkg: $(EFI_SOURCE)\$(PLATFORM_PACKAGE)\InternalOnly\LegacyRom\PxeUndiRom\PxeUndiRom.BIN
  nmake -f $(PKGMAK) \
                    GENCRC32SECTION=$(GENCRC32SECTION) \
                    COMPONENT_TYPE=LEGACY16 \
                    PACKAGE_FILENAME=$*.pkg \
                    DEST_DIR=$(DST_DIR) \
                    BASE_NAME=$(*B) \
                    FILE_GUID=$(PxeOR_FILE_GUID)

!ENDIF

