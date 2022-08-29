
# include "SudokuMatrixV2.hh"
# include <fstream>

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
        out << "row " << row;
        if (row < 10) {
          out << "  ";
        }
        else if (row < 100) {
          out << " ";
        }
        out << ": ";

        for (int column = 0u ; column < counting::constraints ; ++column)
        {
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
          for (int column = 0 ; column < counting::rowsCount ; ++column) {
            int cellOffset = (row * counting::columnsCount + column) * counting::constraints;

            // First the row constraint.
            int constraintOffset = counting::rowOffset + row * counting::candidates + value;
            matrix[digitOffset + cellOffset + constraintOffset] = 1;

            // The column constraint.
            constraintOffset = counting::columnOffset + column * counting::candidates + value;
            matrix[digitOffset + cellOffset + constraintOffset] = 1;

            // The box constraint.
            constraintOffset = counting::boxOffset + counting::boxIDFromRowAndColumn(row, column);
            matrix[digitOffset + cellOffset + constraintOffset] = 1;

            // The digit constraint.
            constraintOffset = counting::cellOffset + row * counting::candidates + column;
            matrix[digitOffset + cellOffset + constraintOffset] = 1;
          }
        }
      }
    }
  }

  SudokuMatrixV2::SudokuMatrixV2():
    utils::CoreObject("SudokuMatrixV2"),

    m_matrix(),

    m_solved(false)
  {
    initialize();
  }

  std::stack<MatrixNode>
  SudokuMatrixV2::solve(const Board& board) {
    m_solved = false;

    initializePuzzle(board);

    if (!solve()) {
      log("Puzzle not solveable!", utils::Level::Error);
      return {};
    }

    log("Puzzle solved successfully!");

    return buildSolution();
  }

  void
  SudokuMatrixV2::initialize() {
    m_matrix.resize(counting::choices * counting::constraints, 0);

    if (!initializeMatrix(m_matrix)) {
      error("Failed to initialize Sudoku matrix");
    }

    verifyMatrix();
    print(m_matrix, "matrix.txt");
  }

  void
  SudokuMatrixV2::verifyMatrix() const {
    for (int row = 0u ; row < counting::rowsCount ; ++row) {
      int count = 0;

      for (int column = 0 ; column < counting::constraints ; ++column) {
        count += m_matrix[row * counting::columnsCount + column];
      }

      if (count != counting::constraintTypes) {
        error(
          "Sudoku matrix line " + std::to_string(row) + " only statisfies " + std::to_string(count) + " constraint(s)"
        );
      }
    }
  }

  void
  SudokuMatrixV2::initializePuzzle(const Board& /*board*/) {
  }

  bool
  SudokuMatrixV2::solve() {
    log("Solving...");
    return true;
  }

  std::stack<MatrixNode>
  SudokuMatrixV2::buildSolution() {
    return {};
  }

}
