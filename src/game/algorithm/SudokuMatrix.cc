//  SudokuMatrix.cpp
//  Author: Andy Giese
//  Date: June 2011
//  Purpose: Implements the SudokuMatrix ADT defined in SudokuMatrix.h

# include "SudokuMatrix.hh"

namespace {
  //these are offsets into the matrix that specify the sections of columns pertaining to
  //row,column,cell,and box constraints respectively

  constexpr auto ROW_OFFSET = 0;
  constexpr auto COL_OFFSET = 81;
  constexpr auto CELL_OFFSET = 162;
  constexpr auto BOX_OFFSET = 243;

  //these ultimately define the proportions of our matrix
  constexpr auto MATRIX_SIZE = 9;
  constexpr auto MAX_COLS = 324;
  constexpr auto MAX_ROWS = 729;

  //number of boxes in a column
  constexpr auto COL_BOX_DIVISOR = 3;
  //number of boxes in a row
  constexpr auto ROW_BOX_DIVISOR = 3;
}

namespace sudoku::algorithm {

  SudokuMatrix::SudokuMatrix():
    utils::CoreObject("SudokuMatrix"),
    Root(new MatrixNode()),

    m_workingSolution(),
    m_solved(false)
  {
    // points to upper left corner of matrix
    Root->makeHeader();
    // Root points to itself in all directions
    Root->linkRight(Root);
    Root->linkLeft(Root);
    Root->linkTop(Root);
    Root->linkBottom(Root);
  }

  SudokuMatrix::~SudokuMatrix() {
    deleteMatrix();
    delete Root;
  }

  void
  SudokuMatrix::print() const noexcept {
    if (Root == nullptr) {
      return;
    }

    int count = 0;
    int colCount = 0;
    MatrixNode* printNode = Root->right();
    MatrixNode* printNodeNext = Root->right();
    while(printNodeNext != Root) {
      colCount++;
      printNode = printNodeNext->bottom();

      while(!printNode->header()) {
        //std::cout << printNode->row << ", " << printNode->column << ", " << printNode->val << endl;
        printNode=printNode->bottom();
        count++;
      }

      printNodeNext = printNodeNext->right();
    }

    log("Counted " + std::to_string(count) + " values, " + std::to_string(colCount) + " Column Headers");
  }

  bool
  SudokuMatrix::AddColumn(MatrixNode* newNode) {
    if (!newNode->header()) {
      return false;
    }
    else {
      return AddColumnHelp(newNode,Root);
    }
  }

  bool
  SudokuMatrix::AddColumnHelp(MatrixNode* newNode, MatrixNode* r) {
    //disallow duplicate pointer insertion
    if (r->right() == Root && r != newNode) {
      //add column to the end of the column list
      r->right()->linkLeft(newNode);
      newNode->linkRight(r->right());
      newNode->linkLeft(r);
      r->linkRight(newNode);
      return true;
    }
    else if (r == newNode) {
      return false;
    }
    else {
      return AddColumnHelp(newNode,r->right());
    }
  }

  void
  SudokuMatrix::deleteMatrix() {
    MatrixNode* deleteNextCol = Root->right();
    MatrixNode* deleteNextRow;
    MatrixNode* temp;
    while(deleteNextCol != Root)
    {
      deleteNextRow = deleteNextCol->bottom();
      while(!deleteNextRow->header())
      {
        temp = deleteNextRow->bottom();
        delete deleteNextRow;
        deleteNextRow = temp;
      }
      temp = deleteNextCol->right();
      delete deleteNextCol;
      deleteNextCol = temp;
    }
  }

  std::stack<MatrixNode>
  SudokuMatrix::solve(const Board& board) {
    //read in filename
    //find nodes corresponding to entries in puzzle, and add them to the partial solution
      //cover those nodes

    //perform algorithm X:
    /*
      if the matrix is empty, terminate successfully
      else choose a column c with the least 1s
      if (least number of 1s in a column is 0, terminate unsuccessfully)
      add c to partial solution
      cover c
      if (recurse on reduced matrix == unsuccessfull)
        uncover c, remove from partial solution
      else
        return successful, solution

    */
    m_solved = false;

    totalCompetition = 0;
    while(!m_workingSolution.empty()) {
      m_workingSolution.pop();
    }

    unsigned nextVal;

    MatrixNode* toFind = nullptr;
    MatrixNode* insertNext = nullptr;
    std::stack<MatrixNode*> puzzleNodes;

    // List of nodes that were covered as part of reading in puzzle.
    MatrixNode* colNode,*nextRowInCol,*rowNode;

    // Iterates through rows.
    for (int i = 0 ; i < MATRIX_SIZE ; ++i) {
      // Iterates through columns.
      for (int j = 0 ; j < MATRIX_SIZE ; ++j) {
        nextVal = board.at(j, i);

        if (nextVal > MATRIX_SIZE) {
          log("Invalid Sudoku Puzzle specified", utils::Level::Error);
          return {};
        }

        if (nextVal != 0) {
          toFind = new MatrixNode(i,j,nextVal-1);
          insertNext = find(toFind);

          if (insertNext == nullptr) {
            log("Error in Sudoku Puzzle " + std::to_string(i) + ", " + std::to_string(j) + " val= " + std::to_string(nextVal), utils::Level::Error);
            return {};
          }

          colNode = insertNext->headerNode();

          //print();

          nextRowInCol = insertNext;
          cover(colNode);

          rowNode = nextRowInCol->right();
          while(rowNode != nextRowInCol) {
            cover(rowNode->headerNode());
            rowNode = rowNode->right();
          }

          puzzleNodes.push(insertNext);
          m_workingSolution.push(*insertNext);
          delete toFind;
        }
      }
    }

    log("Solving...");

    if (solve()) {
      log("Puzzle solved successfully!");
    }
    else {
      log("Puzzle not solveable!", utils::Level::Error);
    }

    //print();

    // Uncover the nodes that we covered earlier, so that we
    // can solve a different puzzle in the future.
    while(!puzzleNodes.empty()) {
      colNode = puzzleNodes.top()->headerNode();
      nextRowInCol = (puzzleNodes.top());


      rowNode = nextRowInCol->right();
      while(rowNode != nextRowInCol) {
        uncover(rowNode->headerNode());
        rowNode = rowNode->right();
      }

      uncover(colNode);
      puzzleNodes.pop();
    }

    std::stack<MatrixNode> out;
    out.swap(m_workingSolution);
    return out;
  }

  void
  SudokuMatrix::cover(MatrixNode* r) {
    MatrixNode *RowNode, *RightNode,*ColNode=r->headerNode();
    ColNode->right()->linkLeft(ColNode->left());
    ColNode->left()->linkRight(ColNode->right());

    for(RowNode = ColNode->bottom(); RowNode!=ColNode; RowNode = RowNode->bottom()) {
      for(RightNode = RowNode->right(); RightNode!=RowNode; RightNode = RightNode->right()) {
        RightNode->top()->linkBottom(RightNode->bottom());
        RightNode->bottom()->linkTop(RightNode->top());
      }
    }
  }

  void
  SudokuMatrix::uncover(MatrixNode* r) {
    MatrixNode *RowNode, *LeftNode,*ColNode=r->headerNode();

    for(RowNode = ColNode->top(); RowNode!=ColNode; RowNode = RowNode->top()) {
      for(LeftNode = RowNode->left(); LeftNode!=RowNode; LeftNode = LeftNode->left()) {
        LeftNode->top()->linkBottom(LeftNode);
        LeftNode->bottom()->linkTop(LeftNode);
      }
    }

    ColNode->right()->linkLeft(ColNode);
    ColNode->left()->linkRight(ColNode);
  }

  bool
  SudokuMatrix::isEmpty() {
    return Root->right() == Root;
  }

  bool
  SudokuMatrix::solve() {
    if (isEmpty()) {
      // Matrix is empty, solution has been found.
      return true;
    }

    int numCols;
    MatrixNode* nextCol = NULL;
    nextCol = chooseNextColumn(numCols);

    if (numCols < 1) {
      return false;
    }

    totalCompetition += numCols;

    MatrixNode* nextRowInCol = nextCol->bottom();
    MatrixNode* rowNode;
    cover(nextCol);

    //need check for solved so that matrix is successfully uncovered after solve, for memory management purposes
    while(nextRowInCol != nextCol && !m_solved) {
      m_workingSolution.push(*nextRowInCol);

      rowNode = nextRowInCol->right();
      while(rowNode != nextRowInCol) {
        cover(rowNode->headerNode());
        rowNode = rowNode->right();
      }

      m_solved = solve();
      if (!m_solved) {
        m_workingSolution.pop();
      }

      rowNode = nextRowInCol->right();
      while(rowNode != nextRowInCol) {
        uncover(rowNode->headerNode());
        rowNode = rowNode->right();
      }

      nextRowInCol = nextRowInCol->bottom();
    }

    uncover(nextCol);

    return m_solved;
  }

  MatrixNode*
  SudokuMatrix::chooseNextColumn(int& count) {
    MatrixNode* currentBest = Root->right();
    int best = -1;
    int tempCount = 0;

    //iterate through currentBest and count nodes, then iterate through currentBest's neighbors and count their nodes

    MatrixNode* next = currentBest->bottom();
    MatrixNode* nextCol = currentBest;
    while(nextCol != Root) {
      next = nextCol->bottom();
      tempCount = 0;

      while(next != nextCol) {
        if (next == next->bottom()) {
          error("Err! Link is its own bottom");
        }

        tempCount++;
        next = next->bottom();
      }

      if (tempCount < best || best == -1) {
        currentBest = nextCol;
        best = tempCount;
      }

      nextCol = nextCol->right();
    }

    if (currentBest == Root) {
      // This is a problem.
      error("Attempted to choose column from empty matrix!");
    }

    count = best;

    return currentBest;
  }

  MatrixNode*
  SudokuMatrix::find(MatrixNode* find) {
    MatrixNode* rightNode,*bottomNode;
    rightNode = Root->right();

    // Iterate through column headers.
    while(rightNode != Root) {
      bottomNode = rightNode->bottom();

      // Iterate through columns.
      while(bottomNode != rightNode) {
        if (bottomNode->row() == find->row() && bottomNode->column() == find->column() && bottomNode->value() == find->value()) {
          return bottomNode;
        }

        bottomNode = bottomNode->bottom();
      }

      rightNode = rightNode->right();
    }

    return nullptr;
  }

  bool
  SudokuMatrix::initialize() {
    // Step 1: Construct maximum matrix (324x729)
    // Step 2: Use maximum matrix to build dancing links structure (matrix A)
    // Step 3: Read in initial sudoku grid of 1s and 0s
    // Step 4: Parse sudoku grid for {r,c,b,v}, and add those to the partial solution S* (removing from A)
    // r=row c=column b=box v=value (1,2,3,...,9)
    // Step 5: Perform Algorithm X on remaining A until exact cover is found --> S* is final solution
    // Step 6: Output final solution

    MatrixNode* matrix[MAX_ROWS][MAX_COLS];

    // Initialize matrix.
    for (int i = 0 ; i < MAX_ROWS ; ++i) {
      for (int j = 0 ; j < MAX_COLS ; ++j) {
        matrix[i][j] = nullptr;
      }
    }

    // Set appropriate non-null values in matrix.
    int row=0;
    MatrixNode *rowNode,*colNode,*cellNode,*boxNode;

    // Rows.
    for (int i = 0 ; i < MATRIX_SIZE ; ++i) {
      // Columns.
      for (int j = 0 ; j < MATRIX_SIZE ; ++j) {
        // Values.
        for (int k = 0 ; k < MATRIX_SIZE ; ++k) {
          row = i * COL_OFFSET + j * MATRIX_SIZE + k;

          // Each one of these 729 combinations of r,c,and v results in 4 constraints being satisfied in our grid
          // i.e. 4 nullptr values changed to Non-Null values
          // this is a *very* sparse matrix, which is why it will be converted to a DLX (Dancing Links) representation
          rowNode=matrix[row][ROW_OFFSET+(i*MATRIX_SIZE+k)] = new MatrixNode(i,j,k);
          colNode=matrix[row][COL_OFFSET+(j*MATRIX_SIZE+k)] = new MatrixNode(i,j,k);
          cellNode=matrix[row][CELL_OFFSET+(i*MATRIX_SIZE+j)] = new MatrixNode(i,j,k);
          boxNode=matrix[row][BOX_OFFSET+((i/ROW_BOX_DIVISOR + j/COL_BOX_DIVISOR * COL_BOX_DIVISOR)*MATRIX_SIZE+k)] = new MatrixNode(i,j,k);

          // Link the nodes we just created to save time later.
          rowNode->linkRight(colNode);
          rowNode->linkLeft(boxNode);
          colNode->linkLeft(rowNode);
          colNode->linkRight(cellNode);
          cellNode->linkLeft(colNode);
          cellNode->linkRight(boxNode);
          boxNode->linkLeft(cellNode);
          boxNode->linkRight(rowNode);
        }
      }
    }

    // Link columns together for dancing links.
    MatrixNode* nextColHeader;
    MatrixNode* nextColRow;

    for (int j = 0 ; j < MAX_COLS ; ++j) {
      nextColHeader = new MatrixNode();
      nextColHeader->makeHeader();

      // Link colHeader to self.
      nextColHeader->linkTop(nextColHeader);
      nextColHeader->linkBottom(nextColHeader);
      nextColHeader->linkLeft(nextColHeader);
      nextColHeader->linkRight(nextColHeader);
      nextColHeader->setHeader(nextColHeader);
      nextColRow = nextColHeader;

      for (int i = 0 ; i < MAX_ROWS ; ++i) {
        if (matrix[i][j] != nullptr) {
          // Search down rows to add to column.
          matrix[i][j]->linkTop(nextColRow);
          nextColRow->linkBottom(matrix[i][j]);

          matrix[i][j]->linkBottom(nextColHeader);
          nextColHeader->linkTop(matrix[i][j]);

          matrix[i][j]->setHeader(nextColHeader);
          nextColRow = matrix[i][j];
        }
      }

      if (nextColHeader->bottom() == nextColHeader) {
        log("Err! column has no rows! col:" + std::to_string(j), utils::Level::Error);
      }

      if(!AddColumn(nextColHeader)) {
        log("Error in adding column to matrix", utils::Level::Error);

        // Cleanup.
        for (int i = 0 ; i < MAX_ROWS ; ++i) {
          for (int j = 0 ; j < MAX_COLS ; ++j) {
            if (matrix[i][j] != nullptr) {
              delete matrix[i][j];
            }
          }
        }

        return false;
      }
    }

    return true;
  }

}
