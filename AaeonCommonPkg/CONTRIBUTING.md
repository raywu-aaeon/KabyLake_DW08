PORTING NOTE
======

GENERAL
======

1. Add following code into .veb

```
"AaeonCommonPkg\AaeonCommonPkg.cif"= Private
"AaeonCommonPkg\AaeonBfpi\AaeonBfpi.cif"= Private
"AaeonCommonPkg\AaeonBfpi\AaeonBfpiReportCapability\AaeonBfpiReportCapability.cif"= Private
"AaeonCommonPkg\F75111PeiPkg\F75111PeiPkg.cif"= Private
"AaeonCommonPkg\Ptn3460PeiPkg\Ptn3460PeiPkg.cif"= Private
"AaeonCommonPkg\SecDecodePkg\SecDecodePkg.cif"= Private
"AaeonCommonPkg\AaeonSetup\AaeonSetup.cif"= Private
"AaeonCommonPkg\AaeonSetup\LibraryInstances\LibraryInstances.cif"= Private
"AaeonCommonPkg\AaeonSetup\SignOnMessage\SignOnMessage.cif"= Private
"AaeonCommonPkg\AaeonDioPkg\AaeonDioPkg.cif"= Private
"AaeonCommonPkg\AaeonDio2Pkg\AaeonDio2Pkg.cif"= Private
"AaeonCommonPkg\AaeonDio3\AaeonDio3.cif"= Private
"AaeonCommonPkg\AaeonDio3\AaeonDio3ReportCapability\AaeonDio3ReportCapability.cif"= Private
"AaeonCommonPkg\AaeonMagicWordPkg\AaeonMagicWordPkg.cif"= Private
"AaeonCommonPkg\AaeonUartModePkg\AaeonUartModePkg.cif"= Private
"AaeonCommonPkg\AaeonUartMode2Pkg\AaeonUartMode2Pkg.cif"= Private
"AaeonCommonPkg\AaeonMultiVbiosPkg\AaeonMultiVbiosPkg.cif"= Private
"AaeonCommonPkg\AaeonOemBeepMap\AaeonOemBeepMap.cif"= Private
"AaeonCommonPkg\AaeonRtcWakePkg\AaeonRtcWakePkg.cif"= Private
"AaeonCommonPkg\AaeonPowerMode\AaeonPowerMode.cif"= Private
"AaeonCommonPkg\AaeonGpioIdPkg\AaeonGpioIdPkg.cif"= Private
"AaeonCommonPkg\PTN3460Cfg\PTN3460Cfg.cif"= Private
"AaeonCommonPkg\PTN3460Cfg\PTN3460CfgSetup.cif"= Private
"AaeonCommonPkg\PTN3460Cfg\PTN3460OemHooks/PTN3460OemHooks.cif"= Private
"AaeonCommonPkg\AutoLoadDefault\AutoLoadDefault.cif"= Private
"AaeonCommonPkg\IPCBU_BiosInfoPeiPkg\BiosInfoPei.cif"= Private
"AaeonCommonPkg\HiManagerUUID\HiManagerUUID.cif"= Private
"AaeonCommonPkg\AaeonUefiLanPkg\AaeonLanUefiDriver.cif"= Private
"AaeonCommonPkg\SioLdnDefaultDisable\SioLdnDefaultDisable.cif"= Private
"AaeonCommonPkg\MacToUUIDPkg\MacToUUID.cif"= Private
"AaeonCommonPkg\AaeonDefaultPwd\AaeonDefaultPwd.cif"= Private
```

2. **Important:**
   * **AaeonCommonPkg** build process should be before than **AaeonIOPkg**


DEPEDENCIES
=====

## **AaeonSetup**

Dependent Component Reference Name: **AmiCompatibilityPkg.Setup**

Dependent Source Control Label: **Setup_05**

Dependent Source Control Location: **SS: AptioV;$/AptioV/Source/Modules/Setup**


CHILD MODULES
=====

## **AaeonPowerMode**

Configure TOKEN **_CRB_USE_VAR_STATEAFTERG3_** according your CRB code.
```
	Name = "CRB_USE_VAR_STATEAFTERG3"
	Help = "1: CRB code provide StateAfterG3\0: CRB provide LastState"
```

## **AaeonUefiLan**

Configure TOKEN **Select LAN Chip Intel/Realtek**
```
	Name  = "Lan_Option"
	Value  = "1"
	Help  = "\0-IntelGigabitLan,\1-RealtekLan"
```

## **AaeonBfpi**

Check porting guide in readme.md.

* [AaeonCommonPkg\AaeonBfpi\Readme.md](AaeonBfpi/Readme.md)

## **AaeonDio3**

Check porting guide in readme.md.

* [AaeonCommonPkg\AaeonDio3\Readme.md](AaeonDio3/Readme.md)
