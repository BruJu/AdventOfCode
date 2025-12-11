# Advent of Code

My answers to https://adventofcode.com written in C++

## Test / Compile / Run

- Go to one of the year folder
- Compile with `make`.
- Run with `./main ([0-9]+)?` (runs both examples and the real input to solve)
  - `./main`: Run last exercise
  - `./main 6`: Run exercise 6
  - `./main 0`: Run all

## Adding new days

- Add input files in `inputs/`
- `config.txt` : For each input, add a line `DAYID input_path part1_result part2_result`. See Config file for the format
- `src/YEAR/days.hpp` : add function declaration for `dayXX() ` and add in array the new function declaration.
  - In the recent years, it is now useless as functions are marked as `__attribute__ ((weak))` to link missing days with no function.
- Create a `src/dayXX.cpp` file.


## Adding a new year

- Create `YEAR/src` and `YEAR/input` folders
- Create `main.cpp` file (copy another year) and empty the list of days.
- Copy the `MAKEFILE` from another year
- Create an empty `YEAR/src/config.txt` file

## Config file

The config file lists all the files with inputs, both for examples and for the real input to solve.

Format for each line: `Day file ResultForPartA ResultForPartB`

Possible values for the result :
- A raw value: It will be considered as a unit test and the given value is expected as an output
- `_`: This test won't be displayed, and sometimes (depending on the implementation of the day) not ran. It is useful if the input would be too long or isn't suited for a part
- `?`: The value is unknown and should be looked for. If a value is specified (`?123`) and this value isn't returned, this test will be displayed in red instead of blue.
- `inline`: Multiple unit tests are in the file. The file should follow this format :
    - On odd lines: A one line input
    - On even lines: The unit test results. `_` means it is ignored. It is possible to only specify the result of the first part.
- `Number\`output`: Means number will be passed as an extra parameter through `DayExtraInfo`. It is used for inputs that only loops several times on the algorithm (for example after 4 operations this example has this result).

## License

WTFPL ¯\\\_( ツ )\_/¯

- The used md5 implementation is licensed under the zlib license. See https://create.stephan-brumme.com/hash-library/

