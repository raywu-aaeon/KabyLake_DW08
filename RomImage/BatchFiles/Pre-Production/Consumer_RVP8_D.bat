cd RomImage
cd Fitc11_8_Consumer
Fit -o ../../Build/AmiCrbMeRoms/RVP8_D_Cons.bin -f ../XmlFiles/Pre-Production/Consumer_RVP8_D.xml -b
del fit.log
cd ..
cd ..
if exist Build\AmiCrbMeRoms\Int RD /S /Q Build\AmiCrbMeRoms\Int
