cd RomImage
cd Fitc11_8_Consumer
Fit -o ../../Build/AmiCrbMeRoms/RVP8_D_Cons_Prod.bin -f ../XmlFiles/Production/Consumer_RVP8_D.xml -b
copy ../../Build/AmiCrbMeRoms/RVP8_D_Cons_Prod.bin ../../%1.bin
del fit.log
cd ..
cd ..
if exist Build\AmiCrbMeRoms\Int RD /S /Q Build\AmiCrbMeRoms\Int
