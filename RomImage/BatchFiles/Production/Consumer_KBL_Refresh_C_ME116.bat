cd RomImage
cd Fitc11_8_Consumer
Fit -o ../../Build/AmiCrbMeRoms/KBL_Refresh_C_Cons_Prod_ME116.bin -f ../XmlFiles/Production/Consumer_KBL_Refresh_C_ME116.xml -b
del fit.log
cd ..
cd ..
if exist Build\AmiCrbMeRoms\Int RD /S /Q Build\AmiCrbMeRoms\Int
