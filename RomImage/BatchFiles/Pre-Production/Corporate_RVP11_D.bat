cd RomImage
cd Fitc11_8_Corporate
Fit -o ../../Build/AmiCrbMeRoms/RVP11_D_Corp.bin -f ../XmlFiles/Pre-Production/Corporate_RVP11_D.xml -b
del fit.log
cd ..
cd ..
if exist Build\AmiCrbMeRoms\Int RD /S /Q Build\AmiCrbMeRoms\Int
