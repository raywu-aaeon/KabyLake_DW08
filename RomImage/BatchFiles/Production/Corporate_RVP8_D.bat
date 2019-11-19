cd RomImage
cd Fitc11_8_Corporate
Fit -o ../../Build/AmiCrbMeRoms/RVP8_D_Corp_Prod.bin -f ../XmlFiles/Production/Corporate_RVP8_D.xml -b
del fit.log
cd ..
cd ..
if exist Build\AmiCrbMeRoms\Int RD /S /Q Build\AmiCrbMeRoms\Int
