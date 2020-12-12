#!/bin/bash
set -e

files=$(find . -type f -name *.cpp)
g++ $files -o main -std=c++2a -Wall -Wextra -Wpedantic -g3
./main $1
