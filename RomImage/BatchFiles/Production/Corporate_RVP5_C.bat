cd RomImage
cd Fitc11_8_Corporate
Fit -o ../../Build/AmiCrbMeRoms/RVP5_C_Corp_Prod.bin -f ../XmlFiles/Production/Corporate_RVP5_C.xml -b
del fit.log
cd ..
cd ..
if exist Build\AmiCrbMeRoms\Int RD /S /Q Build\AmiCrbMeRoms\Int
