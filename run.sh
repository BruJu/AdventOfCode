#!/bin/bash
set -e

# TODO : normalize $1
# TODO : if empty use the last available

folder=Day$1

cd $folder

g++ main.cpp -o main -std=c++2a
./main
