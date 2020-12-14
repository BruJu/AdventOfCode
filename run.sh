#!/bin/bash
set -e

files=$(find . -type f -name *.cpp)
g++ $files -o main -std=c++2a -Wall -Wextra -Wpedantic -O0
./main $1
