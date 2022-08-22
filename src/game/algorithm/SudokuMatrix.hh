#ifndef    SUDOKU_MATRIX_HH
# define   SUDOKU_MATRIX_HH

# include <stack>
# include <core_utils/CoreObject.hh>
# include "Board.hh"
# include "MatrixNode.hh"

namespace sudoku::algorithm {

  class SudokuMatrix: public utils::CoreObject {
    public:

      SudokuMatrix();

      ~SudokuMatrix();

      void
      print(bool verbose = false) const noexcept;

      //pre: Root is not null
      //post: creates the DLX structure for the blank sudoku puzzle
      //    This function should be called after the data structure is initialized
      //    if it returns false, then intializing the matrix failed
      //    if it returns true, then initializing the matrix succeeded
      bool
      initialize();

      //pre: newNode is a column header
      //post: newNode is added to the end of the column headers list in our matrix
      bool
      AddColumn(MatrixNode* newNode);

      //pre: the matrix has been initialized
      //post: will solve the puzzle in the given file and return the solution in the stack structure
      //    A stack structure was chosen so that one could see the order in which the puzzle was solved
      //    with the last entries to the solution found on top
      std::stack<MatrixNode>
      solve(const Board& board);

    private:

      //helper function for AddColumn
      bool
      AddColumnHelp(MatrixNode* newNode, MatrixNode* r);

      //returns whether Root is only node in the matrix
      bool
      isEmpty();

      //helper function for destructor
      void
      deleteMatrix();

      //hides all nodes in r's row, as well as the columns r's row covers,
      //and all the rows contained in those columns
      void
      cover(MatrixNode* r);

      //unhides r from rest of matrix. r is assumed to be a column header
      void
      uncover(MatrixNode* r);

      // searches for find, if found, returns first found node, else returns nullptr
      MatrixNode*
      find(MatrixNode* find);

      //recursively called function that performs Algorithm X
      /* Algorithm X:
        if the matrix is empty, terminate successfully
        else choose a column c with the least 1s
        if (least number of 1s in a column is 0, terminate unsuccessfully)
        for each row r in c
          add r to partial solution
          cover r
          if (recurse on reduced matrix == unsuccessfull)
          uncover r, remove from partial solution
          else
            return successful, solution

      */
      bool
      solve();

      //returns a pointer to a column in the matrix
      //that has the fewest nodes in its row
      //this is used as the heuristic for choosing the next constraint to satisfy
      //in dancing links
      MatrixNode*
      chooseNextColumn(int& count);

    private:

      /**
       * @brief - Points to first column header of the solution
       *          matrix.
       */
      MatrixNode* m_root;

      /**
       * @brief - The partial or full solution to the current
       *          puzzle.
       */
      std::stack<MatrixNode> m_workingSolution;

      /**
       * @brief - Whether the puzzle has been solved.
       */
      bool m_solved;

      int totalCompetition;
  };

}

#endif    /* SUDOKU_MATRIX_HH */
