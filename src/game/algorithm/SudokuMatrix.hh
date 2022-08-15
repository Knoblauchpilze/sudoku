#ifndef    SUDOKU_MATRIX_HH
# define   SUDOKU_MATRIX_HH

# include <stack>
# include <memory>
# include <core_utils/CoreObject.hh>
# include "Board.hh"
# include "MatrixNode.hh"

namespace sudoku::algorithm {

  class SudokuMatrix: public utils::CoreObject {
    public:

      SudokuMatrix();

      ~SudokuMatrix();

      void
      print(bool verbose = false) const;

      /**
       * @brief - Attempt to solve the board provided as input and
       *          return the solution as a stack of node defining
       *          the list of nodes to assign to the missing cells
       *          of the input puzzle.
       * @param board - the puzzle to solve.
       * @return - the solution to apply to empty cells.
       */
      std::stack<Node>
      solve(const Board& board);

    private:

      /**
       * @brief - Whether root is the only node in the matrix.
       * @return - `true` in case the root is the only node.
       */
      bool
      empty();

      void
      createColumnLinks(std::vector<Node*> nodes, Node* root);

      /**
       * @brief - Assuming root is not null, creates the DLX layout
       *          for the blank sudoku puzzle.
       */
      void
      initialize();

      /**
       * @brief - Hides all nodes in node's row, as well as all the
       *          columns node's row covers, and all the rows contained
       *          in those columns.
       * @param node - the row to cover.
       */
      void
      cover(Node* node);

      /**
       * @brief - Unhides node from the rest of matrix. node is assumed
       *          to be a column header.
       * @param node - the header to uncover.
       */
      void
      uncover(Node* node);

      /**
       * @brief - Searches for the input node.
       * @param node - the node to find.
       * @return - the first node found or null.
       */
      Node*
      find(Node* node);

      /**
       * @brief - Initialize the matrix of constraints with the data
       *          from the input board and return the list of nodes
       *          which were covered.
       * @param board - the puzzle to initialize the matrix with.
       * @return - a stack of nodes that were covered.
       */
      std::stack<Node*>
      initializePuzzle(const Board& board);

      /**
       * @brief - Recursive function performing algorithm X as per
       *          the description found here:
       *          https://en.wikipedia.org/wiki/Knuth%27s_Algorithm_X
       * @return - if the algorithm terminated successfully.
       */
      bool
      solve();

      /**
       * @brief - Returns a pointer to a column in the matrix that
       *          has the fewest nodes in its row this is used as
       *          the heuristic for choosing the next constraint to
       *          satisfy in dancing links.
       * @param count - the next column to pick.
       * @return - the node corresponding to the index.
       */
      Node*
      chooseNextColumn(int& count);

    private:

      /**
       * @brief - Points to the first column header of the matrix.
       */
      std::unique_ptr<Node> m_root;

      /**
       * @brief - The matrix represented as an array of elements.
       */
      std::vector<Node*> m_matrix;

      /**
       * @brief - The partial or full solution to the current puzzle.
       */
      std::stack<Node> m_workingSolution;

      /**
       * @brief - `true` if a solution was found.
       */
      bool m_solved;

      int totalCompetition;
  };

}

#endif    /* SUDOKU_MATRIX_HH */