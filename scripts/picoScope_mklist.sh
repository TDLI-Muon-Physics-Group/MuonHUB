#!/bin/bash

set -e

TOP=$PWD

#echo renaming folders
#rename 's/ /_/g' $@/*
#rename 's/\(//g' $@/*
#rename 's/\)//g' $@/*

echo $(basename $TOP)

if [[ $(basename $TOP) != "MuonHUB" ]];
then
    echo please run the script at the base directory PWD/MuonHUB:
    echo ./scripts/picoScope_mklist.sh YOUR_FAVOURITE_DIRECTORY
    exit
fi

NAME=$(basename $@)

echo making list $@
echo output to $TOP/data/${NAME}_filelist.txt
ls -d $@/*/* > $TOP/data/${NAME}_filelist.txt
