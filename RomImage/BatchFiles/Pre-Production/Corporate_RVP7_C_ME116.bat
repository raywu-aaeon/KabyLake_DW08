cd RomImage
cd Fitc11_8_Corporate
Fit -o ../../Build/AmiCrbMeRoms/RVP7_C_Corp_ME116.bin -f ../XmlFiles/Pre-Production/Corporate_RVP7_C_ME116.xml -b
del fit.log
cd ..
cd ..
if exist Build\AmiCrbMeRoms\Int RD /S /Q Build\AmiCrbMeRoms\Int
