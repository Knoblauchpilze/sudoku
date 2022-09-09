#ifndef    SUDOKU_MATRIX_HH
# define   SUDOKU_MATRIX_HH

# include <stack>
# include <unordered_set>
# include <core_utils/CoreObject.hh>
# include "Board.hh"
# include "MatrixNode.hh"

namespace sudoku::algorithm {

  class SudokuMatrix: public utils::CoreObject {
    public:

      SudokuMatrix();

      std::stack<MatrixNode>
      solve(const Board& board);

      bool
      solvable(const Board& board);

    private:

      /// @brief - A partial step for the solution.
      struct SolutionStep {
        int column{-1};
        int row{-1};
        int value{-1};

        bool
        valid() const noexcept;
      };

      /// @brief - Convenience structure helping to solve the exact
      /// cover problem for the sudoku.
      class Solver: public utils::CoreObject {
        public:
          Solver();

          int
          chooseColumn(const std::vector<int>& matrix) const;

          int
          chooseRow(const std::vector<int>& matrix, int column) const;

          SolutionStep
          fromRowIndex(int row) const noexcept;

          int
          toRowIndex(int column, int row, int value) const noexcept;

          void
          cover(int column, std::vector<int>& matrix);

          std::stack<MatrixNode>
          buildSolution() const noexcept;

        public:
          /// @brief - The list of available columns to pick.
          std::unordered_set<int> columns{};

          /// @brief - The list of rows available to pick.
          std::unordered_set<int> rows{};

          /// @brief - The steps taken for the solution.
          std::vector<SolutionStep> steps{};
      };

      void
      initialize();

      void
      verifyMatrix() const;

      Solver
      initializePuzzle(const Board& board);

      bool
      solve(Solver& helper);

    private:

      friend class Solver;

      std::vector<int> m_matrix;

      bool m_solved;
  };

}

#endif    /* SUDOKU_MATRIX_HH */
