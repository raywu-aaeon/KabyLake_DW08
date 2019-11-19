cd RomImage
cd Fitc11_7_Corporate
Fit -o ../../Build/AmiCrbMeRoms/KBL_Z370_D_Corp_Prod_ME11_7.bin -f ../XmlFiles/Production/Corporate_KBL_Z370_D.xml -b
del fit.log
cd ..
cd ..
if exist Build\AmiCrbMeRoms\Int RD /S /Q Build\AmiCrbMeRoms\Int
