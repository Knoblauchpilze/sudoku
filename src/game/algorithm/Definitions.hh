#ifndef    DEFINITIONS_HH
# define   DEFINITIONS_HH

namespace sudoku::counting {

  // Amount of solutions for a constraint.
  constexpr auto candidates = 9;
  constexpr auto rowsCount = 9;
  constexpr auto columnsCount = 9;
  constexpr auto boxesXCount = 3;
  constexpr auto boxesYCount = 3;
  constexpr auto cellsCount = rowsCount * columnsCount;
  constexpr auto boxXCellsCount = columnsCount / boxesXCount;
  constexpr auto boxYCellsCount = rowsCount / boxesYCount;
  constexpr auto boxCellsCount = boxXCellsCount * boxYCellsCount;

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

  inline
  int
  boxIDFromRowAndColumn(int row, int column) noexcept {
    return (row / boxesYCount) * boxesXCount + column / boxesXCount;
  }

}

#endif    /* DEFINITIONS_HH */