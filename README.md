# Advent of Code 2020

My answers to https://adventofcode.com/2020 written in C++

I don't actually compete in C++ when I first do the challenges, but I thought it would be nice to at least have a C++ public repository on Github.

Architecture is a bit wonky: the goal is to have an easy to compile / run process by using only one command line.

## Test / Compile / Run

- `./run.sh` will run the last day
- `./run.sh 0` will run every day
- `./run.sh x` will run day x

## Adding new days

- Add input files in `inputs/`
- `config.txt` : For each input, add a line `DAYID input_path part1_result part2_result`. Write ? instead of a number if unknown. Write `\_ for tests which result is unknwon and not required.
- `src/main.cpp` : add function declaration for `dayXX() ` and add in `dispatch()` the handler
- Create a `src/dayXX.cpp` file.

## License

WTFPL ¯\\\_( ツ )\_/¯
