cd RomImage
cd Fitc11_8_Corporate
Fit -o ../../Build/AmiCrbMeRoms/RVP3_C_Corp_Prod_ME116.bin -f ../XmlFiles/Production/Corporate_RVP3_C_ME116.xml -b
del fit.log
cd ..
cd ..
if exist Build\AmiCrbMeRoms\Int RD /S /Q Build\AmiCrbMeRoms\Int
