#ifndef    SUDOKU_MATRIX_V2_HH
# define   SUDOKU_MATRIX_V2_HH

# include <stack>
# include <core_utils/CoreObject.hh>
# include "Board.hh"
# include "MatrixNode.hh"

namespace sudoku::algorithm {

  class SudokuMatrixV2: public utils::CoreObject {
    public:

      SudokuMatrixV2();

      std::stack<MatrixNode>
      solve(const Board& board);

    private:

      void
      initialize();

      void
      verifyMatrix() const;

      void
      initializePuzzle(const Board& board);

      bool
      solve();

      std::stack<MatrixNode>
      buildSolution();

    private:

      std::vector<int> m_matrix;

      bool m_solved;
  };

}

#endif    /* SUDOKU_MATRIX_V2_HH */
