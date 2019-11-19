#!/bin/bash

for var in $(cat $1)
do
	echo "Test Vfr Parser"
	echo $var
	./KabylakePlatSamplePkg/Setup/VfrParser/linux/VfrParser.exe $var -o$3 -i$2 -c
done

