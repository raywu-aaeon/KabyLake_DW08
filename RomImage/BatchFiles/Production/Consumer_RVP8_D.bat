cd RomImage
cd Fitc11_8_Consumer
Fit -o ../../Build/AmiCrbMeRoms/%1.bin -f ../XmlFiles/Production/Consumer_RVP8_D.xml -b
del fit.log
cd ..
cd ..
if exist Build\AmiCrbMeRoms\Int RD /S /Q Build\AmiCrbMeRoms\Int
