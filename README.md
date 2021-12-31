# Advent of Code

My answers to https://adventofcode.com written in C++

Architecture is a bit wonky: the goal is to have an easy to compile / run process by using only one command line.

## Test / Compile / Run

- You can also compile with `make`.
- And run with `./main ([0-9]+)? ([0-9]+)?`
  - `./main`: Run last exercice of current year
  - `./main 6`: Run exercice 6 of current year
  - `./main 0`: Run all of current year
  - `./main 2020 16`: Run exercice 15 of year 2020

## Adding new days

- Add input files in `inputs/`
- `config.txt` : For each input, add a line `DAYID input_path part1_result part2_result`. See Config file for the format
- `src/YEAR/days.hpp` : add function declaration for `dayXX() ` and add in array the new function declaration.
- Create a `src/dayXX.cpp` file.


### Adding a new year

- Create a `src/YEAR/` folder
- Put a `days.hpp` file (copy another year)
- In `src/main.cpp`, include the `days.hpp` file and fill the `get_all_handlers()` function
- In the `main()` function, check if the current year (`const int year`) matches your expectation

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

- Generic Makefile distributed under the MIT License (see https://github.com/mbcrawfo/GenericMakefile for more information)

- The used md5 implementation licensied under the zlib license. See https://create.stephan-brumme.com/hash-library/

