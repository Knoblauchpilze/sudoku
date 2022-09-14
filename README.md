
# sudoku

A simple app to play sudoku and solve them if needed. The application allows to generate new puzzles and a load/save mechanism is also provided.

The user can choose to solve the sudokus themselves, or solve it automatically which is very fast but not that fun in the long run.

# Installation

- Clone the repo: `git clone git@github.com:Knoblauchpilze/sudoku.git`.
- Clone dependencies:
    * [core_utils](https://github.com/Knoblauchpilze/core_utils)
    * [maths_utils](https://github.com/Knoblauchpilze/maths_utils)
- Go to the project's directory `cd ~/path/to/the/repo`.
- Compile: `make run`.

Don't forget to add `/usr/local/lib` to your `LD_LIBRARY_PATH` to be able to load shared libraries at runtime. This is handled automatically when using the `make run` target (which internally uses the [run.sh](data/run.sh) script).

# General principle

The application is structured in various screens:
* the home screen.
* the load game screen
* the game mode screen
* the difficulty selector screen
* the game screen

![Home screen](resources/home_screen.png)

Each screen serves a purpose and the user can navigate through one to the other to configure the app and choose what they want to do.

# Game mode
When selecting `New Game` the user then reaches the game mode selection screen. The game mode can be one of two things:
* Play a regular sudoku and try to solve it.
* Enter a sudoku and solve it automatically.

![Game mode](resources/game_mode_screen.png)

There's also the possibility to come back to the main menu.

# Interaction

For the solver mode or for the regular mode, the user can interact with the grid in different ways.

At any given time, the position of the active cell is displayed using a blue overlay. Different actions are possible based on which cell is active.

![Interactions](resources/interactions.png)

## Generated numbers

Generated numbers are displayed in light green and can't be changed by the user. They correspond to the numbers that have been created when generating a new sudoku for example. No action is possible when the active cell is one of those.

## Solved numbers

Solved numbers are displayed in orange and correspond to the numbers which are generated by the solver. These numbers are certain and should usually not be modified. The user has the possibility to delete them though.

## User generated numbers

These numbers are displayed in cyan. They can be modified or deleted as the user wants. They are not replaced by the solver: when applying it, the app assumes that the user knows what they are doing.

## Interacting with the numbers.

The user can:
* press any digit on the numeric keypad and add the corresponding number in a cell (if possible).
* press the `del` button and remove the digit currently in the active cell (if possible).
* left click with the mouse, placing (if possible) the next digit in the active cell compared to the active one (so if a 6 is in the cell, a 7 will be placed, then 8, etc.). Once `9` is reached the app cycles back to `1`.
* right click with the mouse, deleting the current digit in the active cell (if possible).

Each interaction is filtered by the app and only performed if it is possible. For example if there's already an `8` in the box where the user wants to put an `8` this value will be considered invalid.

This means that if the user presses `8` on the numeric keypad, nothing will happen. Similarly, if the user left clicks on the mouse in any cell in the box, the `8` value will be skipped.

We don't detect 'complex' invalid numbers, just the ones which can easily be detected as invalid because of existing digits. The number which will be possible to place in a cell are exactly the ones which would appear in the main game view as a hint (see [here](###bottom-banner).

## Controlling the grid

The user can save the current grid at any time by hitting the `s` key. A file will be generated and saved in the [saves](data/saves/) with a uniquely generated name.

The user can also exit the application at any time using the `Esc` key.

# Play mode

## Difficulty selection

When the user chooses to play, they must then choose the difficulty level. There are three choices (easy, medium and hard) and each one corresponds to more or less digits initially present on the board.

![Difficulty selection](resources/difficulty_screen.png)

## Playing a game.

When the user picked the difficulty level, the main screen appears:

![Game screen](resources/game_screen.png)

There are multiple information available to the user to help solving the sudoku.

### Top banner

The top banner is always visible and displays two kind of information.

On the left side the number of digits of a certain value is displayed: the background is light yellow in case there are some missing and light green in case all of them have been placed in the grid already. Note that this doesn't mean that they are all correct.

On the right side are two buttons: the `Solve` one allows to solve the current sudoku (might be impossible in case some digits are misplaced) and the `Reset` one allows to generate a new sudoku, losing all the progress already made.

### Bottom banner

The bottom banner is only visible after a small delay when the user waits on an active cell. This displays the list of digits that can be placed on the current cell. As explained in the [interaction](#interaction) part, only the digit shown as valid here will be used in the interaction process.

### Finishing the game

When the user is done with the game (either in a legit way or by clicking the `Solve` button), an alert is displayed for a short time, before bringing the user back to the main screen, where more games can be played.

# Solver mode

If the user chooses the solver mode, the app now displays an empty grid (except if the user loaded a save game before).

![Solver overview](resources/solver_empty.png)

The user can move around in the grid and add numbers as described in the relevant [section](#interaction).

When the user is ready, they can click on the `Solve` button. There are then three possibilities:
* the grid is solvable in which case the solver will find the solution.
* the grid is not uniquely solvable: the solver won't work and report an error.
* the grid is not solvable: the solver will report an error.

When the solver succeeds, an alert is displayed like so:

![Solved alert](resources/solved_alert.png)

When the solver fails, an alert is displayed like so:

![Unsolvable alert](resources/unsolvable_alert.png)

Not that the user can always come back to the main screen using the back button at the bottom of the screen.