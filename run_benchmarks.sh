#!/bin/bash

# Constants for coloring output
RED='\033[0;31m'
GREEN='\033\e[0;32m'
NO_COLOR='\033[0m' 

# List of necessary python packages for CI/CD
packages=('pandas' 'matplotlib')

# Iterate over packages and check their installation
for package in ${packages[@]}
do
    pip3 show $package 1>/dev/null 
    if [ $? == 0 ]
    then
        echo -e ${GREEN}OK${NO_COLOR}: $package is installed!
    else
        echo -e ${RED}ERROR${NO_COLOR}: $package is not installed! ; exit 1
    fi
done

# Clone basic version of the algorithm
mkdir ./basiclian && git clone https://github.com/cyb3r-b4stard/basicLIAN.git basiclian/

# Create directories for logs
mkdir ./benchmark/out
rm -r ./benchmark/out/logs_basic    ; mkdir ./benchmark/out/logs_basic
rm -r ./benchmark/out/logs_modified ; mkdir ./benchmark/out/logs_modified

# Create directories for resources
# mkdir ./benchmark/resources/xml_basic ; mkdir ./benchmark/resources/xml_modified ; python3 benchmark/generate_missions.py
 
if [ ! -d  './benchmark/resources/xml_basic' ] || [ ! -d './benchmark/resources/xml_modified' ]
then
    if [ ! -d  './benchmark/resources/xml_basic' ]
    then
        mkdir ./benchmark/resources/xml_basic
    fi
    if [ ! -d './benchmark/resources/xml_modified' ]
    then 
        mkdir ./benchmark/resources/xml_modified
    fi
    python3 benchmark/generate_missions.py
fi

# Build algorithms
if [ ! -f './basiclian/LianSearch' ]
then
    cd ./basiclian ; cmake . ; make ; cd ..
fi &
cmake . ; make &
wait

function benchmark(){
    if [ $1 == 'basic' ] # Iterate over missions for basic LIAN
    then
        for file in ./benchmark/resources/xml_basic/*.xml
        do
            ./basiclian/LianSearch $file
        done
    else # Iterate over missions for modified LIAN
        for file in ./benchmark/resources/xml_modified/*.xml
        do
            ./LianSearch $file
        done
    fi
}

# Run benchmarks in separate processes
benchmark 'basic'    & 
benchmark 'modified' &
wait

# Move logs into benchmark/out directory
mv ./benchmark/resources/xml_basic/*_log.xml ./benchmark/out/logs_basic/
mv ./benchmark/resources/xml_modified/*_log.xml ./benchmark/out/logs_modified/

# Analyse data and export results in pdf
python3 ./benchmark/data_analysis.py