cd RomImage
cd Fitc11_7_Consumer
Fit -o ../../Build/AmiCrbMeRoms/KBL_Z370_D_Cons_Prod_ME11_7.bin -f ../XmlFiles/Production/Consumer_KBL_Z370_D.xml -b
del fit.log
cd ..
cd ..
if exist Build\AmiCrbMeRoms\Int RD /S /Q Build\AmiCrbMeRoms\Int
