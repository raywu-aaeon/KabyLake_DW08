cd RomImage
cd Fitc11_8_Corporate
Fit -o ../../Build/AmiCrbMeRoms/KBL_uDIMM_D_Corp.bin -f ../XmlFiles/Pre-Production/Corporate_KBL_uDIMM_D.xml -b
del fit.log
cd ..
cd ..
if exist Build\AmiCrbMeRoms\Int RD /S /Q Build\AmiCrbMeRoms\Int
