
# include "SudokuMatrixV2.hh"
# include <fstream>
# include <limits>

#include <iostream>

// https://gieseanw.wordpress.com/2011/06/16/solving-sudoku-revisited/
// https://en.wikipedia.org/wiki/Exact_cover#Sudoku
// https://en.wikipedia.org/wiki/Knuth%27s_Algorithm_X

namespace sudoku::algorithm {
  namespace counting {

    // Amount of solutions for a constraint.
    constexpr auto candidates = 9;
    constexpr auto rowsCount = 9;
    constexpr auto columnsCount = 9;
    constexpr auto boxesXCount = 3;
    constexpr auto boxesYCount = 3;
    constexpr auto cellsCount = rowsCount * columnsCount;

    constexpr auto constraintTypes = 4;

    // We have 9x9 cells each one potentiallly containing
    // 9 values: that's a total of 9x9x9 = 729 possible
    // choices to make.
    constexpr auto choices = rowsCount * columnsCount * candidates;

    // On the other hand, we know that each row must have
    // 9 values, same for the rows and the boxes. We also
    // know that there are 9 row, 9 columns and 9 boxes.
    // That's a total of 9x9 + 9x9 + 9x9 constraints.
    // On top of that, we also need to enforce that each
    // intersection of a row and a column (in other words
    // each cell) contains only one digit: that's another
    // 81 constraints for a total of 324.
    constexpr auto constraints =
      rowsCount * candidates +
      columnsCount * candidates +
      boxesXCount * boxesYCount * candidates +
      rowsCount * columnsCount;

    // First in the matrix.
    constexpr auto rowOffset = 0;
    // After the 81 rows.
    constexpr auto columnOffset = rowsCount * candidates;
    // Comes after the columns.
    constexpr auto boxOffset = columnOffset + columnsCount * candidates;
    // Comes after the boxes.
    constexpr auto cellOffset = boxOffset + boxesXCount * boxesYCount * candidates;

    int
    boxIDFromRowAndColumn(int row, int column) {
      return (row / boxesYCount) * boxesXCount + column / boxesXCount;
    }

  }

  namespace {
    void
    print(const std::vector<int>& matrix, const std::string& fileName) noexcept {
      if (matrix.empty()) {
        return;
      }

      std::ofstream out(fileName);

      for (int row = 0u ; row < counting::choices ; ++row) {
        for (int column = 0u ; column < counting::constraints ; ++column) {
          out << matrix[row * counting::constraints + column];
        }
        out << std::endl;
      }
    }

    bool
    initializeMatrix(std::vector<int>& matrix) noexcept {
      if (matrix.size() != counting::choices * counting::constraints) {
        return false;
      }

      // Initialize each of the choices: when putting a number
      // in a cell, we satisfy four constraints:
      // - the number is in a row
      // - the number is in a column
      // - the number is in a box
      // - the number is in a cell
      // So we have to put four 1s in the matrix.
      // Each choice is put one after the other and so in the
      // 1D array we have the first 324 values which are the
      // first choice, then the next 324 for the second choice
      // etc.
      // The first choice is a 1 in the first cell, the second
      // choice is a 1 in the second cell, the 82th choice is
      // a 2 in the first cell, etc.
      // For the constraints
      // - the first constraint is the fact that the first row
      //   should have a 1 somewhere.
      // - the second one is that the first row should have a 2
      //   somewhere.
      // - ...
      // - the 10th one is that the second row should have a 1
      //   somewhere.
      // - ...
      // - the 82nd is that the first column should have a 1
      //   somewhere.
      // - ...
      // - the 90th is that the second column should have a 1
      //   somewhere.
      // - ...
      // - the 162nd is that the first box should have a 1 in
      //   any location.
      // - ...
      // - the 243rd is that the first cell should have a number
      //   in it.
      // From there we just initialize which constraints are
      // satisfied for each new choice that we make.
      for (int value = 0 ; value < counting::candidates ; ++value) {
        // Try to put this value in all of the 81 possible cells
        // for the digit.
        // Each digit has 81 possible cells and there are 329
        // index in the matrix for a single row.
        int digitOffset = value * counting::cellsCount * counting::constraints;

        for (int row = 0 ; row < counting::rowsCount ; ++row) {
          for (int column = 0 ; column < counting::columnsCount ; ++column) {
            int cellOffset = (row * counting::columnsCount + column) * counting::constraints;

            // First the row constraint.
            int constraintOffset = counting::rowOffset + row * counting::candidates + value;
            matrix[digitOffset + cellOffset + constraintOffset] = 1;

            // The column constraint.
            constraintOffset = counting::columnOffset + column * counting::candidates + value;
            matrix[digitOffset + cellOffset + constraintOffset] = 1;

            // The box constraint.
            constraintOffset = counting::boxOffset + counting::boxIDFromRowAndColumn(row, column) * counting::candidates + value;
            matrix[digitOffset + cellOffset + constraintOffset] = 1;

            // The digit constraint.
            constraintOffset = counting::cellOffset + row * counting::candidates + column;
            matrix[digitOffset + cellOffset + constraintOffset] = 1;
          }
        }
      }

      return true;
    }
  }

  bool
  SudokuMatrixV2::SolutionStep::valid() const noexcept {
    return row >= 0 && row < counting::rowsCount &&
           column >= 0 && column < counting::columnsCount &&
           value >= 1 && value <= counting::candidates;
  }

  SudokuMatrixV2::Solver::Solver():
    utils::CoreObject("solver")
  {
    setService("sudoku");
  }

  int
  SudokuMatrixV2::Solver::chooseColumn(const std::vector<int>& matrix) const {
    int best = -1;
    int minOnes = std::numeric_limits<int>::max();

    for (auto column : columns) {
      int ones = 0;

      for (auto row : rows) {
        ones += matrix[row * counting::constraints + column];
      }

      if (ones < minOnes) {
        log("Found better constraint " + std::to_string(column + 1) + " only satisfied " + std::to_string(ones) + " time(s)");
        minOnes = ones;
        best = column;
      }
    }

    log("Best column is " + std::to_string(best + 1) + " only satisfied " + std::to_string(minOnes) + " time(s)");

    return best;
  }

  int
  SudokuMatrixV2::Solver::chooseRow(const std::vector<int>& matrix, int column) const {
    for (auto row : rows) {
      if (matrix[row * counting::constraints + column] == 1) {
        log("Picked row " + std::to_string(row + 1) + ", first satisfying constraint " + std::to_string(column + 1));

        return row;
      }
    }

    return -1;
  }

  SudokuMatrixV2::SolutionStep
  SudokuMatrixV2::Solver::fromRowIndex(int row) const noexcept {
    int outDigit = row / counting::cellsCount;

    int offset = outDigit * counting::cellsCount;
    int linearCell = row - offset;

    int outRow = linearCell / counting::columnsCount;
    int outColumn = linearCell % counting::columnsCount;

    return SolutionStep{outColumn, outRow, outDigit + 1};
  }

  void
  SudokuMatrixV2::Solver::cover(int rowToDelete, std::vector<int>& matrix) {
    if (rows.count(rowToDelete) == 0) {
      error("Cannot hide row " + std::to_string(rowToDelete) + " not available for picking anymore");
    }

    SudokuMatrixV2::SolutionStep step = fromRowIndex(rowToDelete);
    if (!step.valid()) {
      error("Cannot hide row " + std::to_string(rowToDelete) + ", failed to build solution step from it");
    }
    steps.push_back(step);
    log(
      "Adding digit " + std::to_string(step.value) + " at " +
       std::to_string(step.column + 1) + "x" + std::to_string(step.row + 1) +
       " as step " + std::to_string(steps.size() + 1)
    );

    rows.erase(rowToDelete);

    Solver tmp = *this;

    for (auto column : tmp.columns) {
      if (matrix[rowToDelete * counting::constraints + column] == 0) {
        continue;
      }

      for (auto row : tmp.rows) {
        if (matrix[row * counting::constraints + column] == 0) {
          continue;
        }

        log("Deleting row " + std::to_string(row + 1) + " sharing column " + std::to_string(column + 1) + " with row " + std::to_string(rowToDelete + 1));

        rows.erase(row);
      }

      log("Deleting column " + std::to_string(column + 1) + " as constraint is staisfied");

      columns.erase(column);
    }
  }

  SudokuMatrixV2::SudokuMatrixV2():
    utils::CoreObject("SudokuMatrixV2"),

    m_matrix(),

    m_solved(false)
  {
    setService("sudoku");
  }

  std::stack<MatrixNode>
  SudokuMatrixV2::solve(const Board& board) {
    m_solved = false;

    Solver helper = initializePuzzle(board);

    if (!solve(helper)) {
      log("Puzzle not solveable!", utils::Level::Error);
      return {};
    }

    log("Puzzle solved successfully!");

    return buildSolution(helper);
  }

  void
  SudokuMatrixV2::initialize() {
    m_matrix = std::vector<int>(counting::choices * counting::constraints, 0);

    if (!initializeMatrix(m_matrix)) {
      error("Failed to initialize Sudoku matrix");
    }

    print(m_matrix, "matrix.txt");
    verifyMatrix();
  }

  void
  SudokuMatrixV2::verifyMatrix() const {
    for (int row = 0u ; row < counting::choices ; ++row) {
      int count = 0;

      for (int column = 0 ; column < counting::constraints ; ++column) {
        count += m_matrix[row * counting::constraints + column];
      }

      if (count != counting::constraintTypes) {
        error(
          "Sudoku matrix row " + std::to_string(row) + " only statisfies " + std::to_string(count) + " constraint(s)"
        );
      }
    }

    for (int column = 0u ; column < counting::constraints ; ++column) {
      int count = 0;

      for (int row = 0u ; row < counting::choices ; ++row) {
        count += m_matrix[row * counting::constraints + column];
      }

      if (count == 0) {
        error(
          "Sudoku matrix column " + std::to_string(column) + " is never satisfied"
        );
      }
    }
  }

  SudokuMatrixV2::Solver
  SudokuMatrixV2::initializePuzzle(const Board& board) {
    initialize();

    Solver helper;
    for (int column = 0 ; column < counting::constraints ; ++column) {
      helper.columns.insert(column);
    }

    for (int row = 0 ; row < counting::choices; ++row) {
      helper.rows.insert(row);
    }

    for (int row = 0u ; row < counting::rowsCount ; ++row) {
      for (int column = 0u ; column < counting::columnsCount ; ++column) {
        int value = board.at(column, row);
        if (value == 0) {
          continue;
        }

        log(
          "Initial board has a " + std::to_string(value) +
          " at " + std::to_string(column + 1) + "x" + std::to_string(row + 1)
        );

        /// TODO: Cover this value.
      }
    }

    return helper;
  }

  bool
  SudokuMatrixV2::solve(Solver& helper) {
    if (helper.columns.empty()) {
      return true;
    }

    int column = helper.chooseColumn(m_matrix);
    if (column < 0) {
      error("Failed to pick a column while " + std::to_string(helper.columns.size()) + " are available");
    }
    log("Picked column " + std::to_string(column));

    int row = helper.chooseRow(m_matrix, column);
    if (row < 0) {
      error("Failed to pick a row while " + std::to_string(helper.rows.size()) + " are available");
    }
    log("Picked row " + std::to_string(row));

    log("Picked constraint " + std::to_string(column + 1) + "x" + std::to_string(row + 1));

    helper.cover(row, m_matrix);

    return solve(helper);
  }

  std::stack<MatrixNode>
  SudokuMatrixV2::buildSolution(Solver& helper) {
    std::stack<MatrixNode> out;

    log("Building solution containing " + std::to_string(helper.steps.size()) + " step(s)");

    for (const auto& step : helper.steps) {
      out.push(MatrixNode(step.row, step.column, step.value));
    }

    return out;
  }

}
