#!/bin/bash

# Constants for coloring output
RED='\033[0;31m'
GREEN='\033\e[0;32m'
NC='\033[0m' 

# List of necessary python packages for CI/CD
packages=('pandas' 'matplotlib')

# Iterate over packages and check their installation
for package in ${packages[@]}
do
    pip3 show $package 1>/dev/null 
    if [ $? == 0 ]; 
    then
        echo -e ${GREEN}OK${NC}: $package is installed!
    else
        echo -e ${RED}ERROR${NC}: $package is not installed! ; exit 1
    fi
done

# Clone basic version of the algorithm
mkdir ./basiclian && git clone https://github.com/cyb3r-b4stard/basicLIAN.git basiclian/

# Create directories for logs
mkdir ./benchmark/out ; mkdir ./benchmark/out/logs_basic ; mkdir ./benchmark/out/logs_modified

# Create directories for resources
mkdir ./benchmark/resources/xml_basic ; mkdir ./benchmark/resources/xml_modified ; python3 benchmark/generate_missions.py

# Build algorithms
cd ./basiclian && cmake . && make && cd .. ; cmake . && make

# Iterate over missions for basic algorithm
for file in ./benchmark/resources/xml_basic/*.xml
do
    ./basiclian/LianSearch $file
done

# Iterate over missions for modified algorithm
for file in ./benchmark/resources/xml_modified/*.xml
do
    ./LianSearch $file
done

# Move logs into benchmark/out directory
mv ./benchmark/resources/xml_basic/*_log.xml ./benchmark/out/logs_basic/
mv ./benchmark/resources/xml_modified/*_log.xml ./benchmark/out/logs_modified/

# Analyse data and export results in pdf
python3 ./benchmark/data_analysis.py