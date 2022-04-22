
# sudoku

A simple app to play sudoku and solve them if needed. The application allows to generate new puzzles and a load/save mechanism is also provided.

The user can choose to solve the sudokus themselves, get some hints, or solve it automatically which is very fast but not that fun in the long run.

# Installation

- Clone the repo: `git clone git@github.com:Knoblauchpilze/sudoku.git`.
- Clone dependencies:
    * [core_utils](https://github.com/Knoblauchpilze/core_utils)
    * [maths_utils](https://github.com/Knoblauchpilze/maths_utils)
- Go to the project's directory `cd ~/path/to/the/repo`.
- Compile: `make run`.

Don't forget to add `/usr/local/lib` to your `LD_LIBRARY_PATH` to be able to load shared libraries at runtime. This is handled automatically when using the `make run` target (which internally uses the [run.sh](data/run.sh) script).
