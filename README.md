# Advent of Code

My answers to https://adventofcode.com written in C++

Architecture is a bit wonky: the goal is to have an easy to compile / run process by using only one command line.

## Test / Compile / Run

**The bash way**
- `./run.sh` will run the last day
- `./run.sh 0` will run every day
- `./run.sh x` will run day x

If two integers are provided, the first one is the year and the second one is the day

**The standard way**
- You can also compile with `make`.
- And run with `./main ([0-9]+)?`

## Adding new days

- Add input files in `inputs/`
- `config.txt` : For each input, add a line `DAYID input_path part1_result part2_result`. Write ? instead of a number if unknown. Write `\_ for tests which result is unknwon and not required.
- `src/main.cpp` : add function declaration for `dayXX() ` and add in `dispatch()` the handler
- Create a `src/dayXX.cpp` file.

## Config file

Format for each line: `Day file ResultForPartA ResultForPartB`

Possible values for the resumt :
- A raw value: It will be considerd as a unit test
- `_`: This test won't be displayed, and sometimes (depending on the implementation of the day) not ran. It is usefull if the input would be too long or isn't suited for a part
- `?`: The value is unknown and should be looked for. If a value is specified (`?123`) and this value isn't returned, this test will be displayed in red instead of blue.
- `nope_`: Has the same effect as `_`
- `inline`: Multiple unit tests are in the file. The file should follow this format :
    - On odd lines: A one line input
    - On even lines: The unit test results. `_` means it is ignored. It is possible to only specify the result of the first part.
- `Number``something`: Means number will be passed as an extra parameter throught `DayExtraInfo`. It is used for inputs that only loops several times on the algorithm (for example after 4 operations this example has this result, apply 100 times the operation to the given input).

## License

WTFPL ¯\\\_( ツ )\_/¯

Generic Makefile distributed under the MIT License (see https://github.com/mbcrawfo/GenericMakefile for more information)
