#!/bin/bash

set -e

echo renaming folders
rename 's/ /_/g' $@/*
rename 's/\(//g' $@/*
rename 's/\)//g' $@/*
