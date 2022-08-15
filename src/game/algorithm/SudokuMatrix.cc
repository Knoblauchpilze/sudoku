
# include "SudokuMatrix.hh"
# include "Definitions.hh"

// https://gieseanw.wordpress.com/2011/06/16/solving-sudoku-revisited/

namespace {

/// @brief - Offset in the matrix for row constraints section.
constexpr auto ROW_OFFSET = 0;
/// @brief - Offset in the matrix for column constraints section.
constexpr auto COL_OFFSET = 81;
/// @brief - Offset in the matrix for cell constraints section.
constexpr auto CELL_OFFSET = 162;
/// @brief - Offset in the matrix for box constraints section.
constexpr auto BOX_OFFSET = 243;

/// @brief - Size of the matrix (i.e. number of digits).
constexpr auto MATRIX_SIZE = 9;
/// @brief - Maximum number of constraints.
constexpr auto MAX_COLS = 324;
/// @brief - Maximum number of combination to satisfy a constrain.
constexpr auto MAX_ROWS = 729;

}

namespace sudoku::algorithm {

  SudokuMatrix::SudokuMatrix():
    utils::CoreObject("SudokuMatrix"),

    m_root(std::make_unique<Node>()),
    m_matrix(),
    m_workingSolution(),

    m_solved(false),

    totalCompetition()
  {}

  SudokuMatrix::~SudokuMatrix() {
    for (unsigned id = 0u ; id < m_matrix.size() ; ++id) {
      delete m_matrix[id];
    }
  }

  void
  SudokuMatrix::print(bool verbose) const {
    //simply print matrix to stdout
    if (m_root == nullptr) {
      log("Empty suoku matrix");
      return;
    }

    int count = 0;
    int colCount = 0;
    Node* printNode = m_root->right();
    Node* printNodeNext = m_root->right();

    while(printNodeNext != m_root.get()) {
      colCount++;

      printNode = printNodeNext->bottom();

      while(!printNode->header()) {
        // Print the matrix in column-dominant order.
        if (verbose) {
          log(
            std::to_string(printNode->row()) + ", " +
            std::to_string(printNode->column()) + ", " +
            std::to_string(printNode->value())
          );
        }

        printNode = printNode->bottom();
        count++;
      }

      printNodeNext = printNodeNext->right();
    }

    log(
      "Counted " + std::to_string(count) + " values, " +
      std::to_string(colCount) + " column headers"
     );
  }

  bool
  SudokuMatrix::empty() {
    return m_root->right() == m_root.get();
  }

  namespace {
    enum class Type {
      Row,
      Column,
      Cell,
      Box
    };

    unsigned
    indexFromTriplet(unsigned row,
                     unsigned column,
                     unsigned value,
                     Type kind)
    {
      switch (kind) {
        case Type::Row:
          return ROW_OFFSET + row * MATRIX_SIZE + value;
        case Type::Column:
          return COL_OFFSET + column * MATRIX_SIZE + value;
        case Type::Cell:
          return CELL_OFFSET + row * MATRIX_SIZE + column;
        case Type::Box:
          // default is a box.
        default:
          return
            BOX_OFFSET
            + (row / ROW_BOX_DIVISOR + column / COL_BOX_DIVISOR * COL_BOX_DIVISOR) * MATRIX_SIZE
            + value;
      }
    }

    unsigned
    linearIndex(unsigned row, unsigned column, unsigned value) {
      return row * COL_OFFSET + column * MATRIX_SIZE + value;
    }

    void
    createRowLinks(std::vector<Node*> nodes) {
      Node* rowNode = nullptr;
      Node* colNode = nullptr;
      Node* cellNode = nullptr;
      Node* boxNode = nullptr;

      // For each row.
      for (unsigned row = 0u ; row < MATRIX_SIZE ; ++row) {
        // For each column.
        for (unsigned column = 0u ; column < MATRIX_SIZE ; ++column) {
          // For each value.
          for (unsigned value = 0u ; value < MATRIX_SIZE ; ++value) {
            unsigned linear = linearIndex(row, column, value);

            // each one of these 729 combinations of row, column
            // and value results in 4 constraints being satisfied
            // in our grid i.e. 4 nullptr values changed to non-null
            // values.
            // This is a *very* sparse matrix, which is why it will
            // be converted to a DLX (Dancing Links) representation.
            unsigned id = linear * MAX_COLS + indexFromTriplet(row, column, value, Type::Row);
            rowNode = nodes[id] = new Node(row, column, value);
            id = linear * MAX_COLS + indexFromTriplet(row, column, value, Type::Column);
            colNode = nodes[id] = new Node(row, column, value);
            id = linear * MAX_COLS + indexFromTriplet(row, column, value, Type::Cell);
            cellNode = nodes[id] = new Node(row, column, value);
            id = linear * MAX_COLS + indexFromTriplet(row, column, value, Type::Box);
            boxNode = nodes[id] = new Node(row, column, value);

            //link the nodes we just created to save time later
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
    }

    bool
    addColumnWithHint(Node* newNode, Node* root, Node* trueRoot) {
      // Disable duplicate pointer insertion.
      if (root->right() == trueRoot && root != newNode) {
        // Add column to the end of the column list
        root->right()->linkLeft(newNode);
        newNode->linkRight(root->right());

        newNode->linkLeft(root);
        root->linkRight(newNode);

        return true;
      }
      else if (root == newNode) {
        return false;
      }
      else {
        return addColumnWithHint(newNode, root->right(), trueRoot);
      }
    }

    bool
    addColumn(Node* newNode, Node* trueRoot) {
      if (!newNode->header()) {
        return false;
      }

      return addColumnWithHint(newNode, trueRoot, trueRoot);
    }
  }

  void
  SudokuMatrix::createColumnLinks(std::vector<Node*> nodes, Node* root) {
    Node* nextColHeader;
    Node* nextColRow;

    for (unsigned column = 0u ; column < MAX_COLS ; ++column) {
      nextColHeader = new Node();
      nextColHeader->makeHeader();

      // Link colHeader to self.
      nextColHeader->linkTop(nextColHeader);
      nextColHeader->linkBottom(nextColHeader);
      nextColHeader->linkLeft(nextColHeader);
      nextColHeader->linkRight(nextColHeader);
      nextColHeader->setHeader(nextColHeader);

      nextColRow = nextColHeader;

      for (unsigned row = 0 ; row < MAX_ROWS ; ++row) {
        Node* node = nodes[row * MAX_COLS + column];
        if (node != nullptr) {
          // search down rows to add to column
          node->linkTop(nextColRow);
          nextColRow->linkBottom(node);

          node->linkBottom(nextColHeader);
          nextColHeader->linkTop(node);

          node->setHeader(nextColHeader);
          nextColRow = node;
        }
      }

      if (nextColHeader->bottom() == nextColHeader) {
        error("Column " + std::to_string(column) + " has no rows !");
      }

      if(!addColumn(nextColHeader, root)) {
        error("Failed to add column " + std::to_string(column) + " to matrix");
      }
    }
  }

  void
  SudokuMatrix::initialize() {
    m_matrix.resize(MAX_ROWS * MAX_COLS, nullptr);

    createRowLinks(m_matrix);
    createColumnLinks(m_matrix, m_root.get());
  }

  void
  SudokuMatrix::cover(Node* node) {
    Node* row = nullptr;
    Node* right = nullptr;
    Node* column = node->headerNode();
    column->right()->linkLeft(column->left());
    column->left()->linkRight(column->right());

    for(row = column->bottom() ; row != column ; row = row->bottom()) {
      for(right = row->right() ; right != row ; right = right->right()) {
        right->top()->linkBottom(right->bottom());
        right->bottom()->linkTop(right->top());
      }
    }
  }

  void
  SudokuMatrix::uncover(Node* node) {
    Node* row = nullptr;
    Node* left = nullptr;
    Node* column = node->headerNode();

    for(row = column->top() ; row != column ; row = row->top()) {
      for(left = row->left() ; left != row ; left = left->left()) {
        left->top()->linkBottom(left);
        left->bottom()->linkTop(left);
      }
    }

    column->right()->linkLeft(column);
    column->left()->linkRight(column);
  }

  Node*
  SudokuMatrix::find(Node* node) {
    Node* right = m_root->right();

    // Iterate through column headers.
    while(right != m_root.get()) {
      Node* bottom = right->bottom();

      // iterate through columns
      while(bottom != right) {
        if (bottom->row() == node->row() &&
            bottom->column() == node->column() &&
            bottom->value() == node->value())
        {
          return bottom;
        }

        bottom = bottom->bottom();
      }

      right = right->right();
    }

    // Not found.
    return nullptr;
  }

  std::stack<Node*>
  SudokuMatrix::initializePuzzle(const Board& board) {
    Node* insertNext = nullptr;

    // List of nodes that were covered as part of reading
    // in puzzle.
    std::stack<Node*> puzzleNodes;

    Node* colNode = nullptr;
    Node* nextRowInCol = nullptr;
    Node* rowNode = nullptr;

    for (int row = 0 ; row < MATRIX_SIZE ; ++row) {
      for (int column = 0 ; column < MATRIX_SIZE ; ++column) {
        unsigned value = board.at(column, row);

        if (value == 0u) {
          continue;
        }

        std::unique_ptr<Node> toFind = std::make_unique<Node>(row, column, value - 1u);

        insertNext = find(toFind.get());
        if (insertNext == nullptr) {
          error(
            "Failed to solve Sudoku puzzle",
            "Failed to find node with constraint [row: " +
            std::to_string(row) + ", column: " +
            std::to_string(column) + ", value: " +
            std::to_string(value) + "]"
          );
        }

        colNode = insertNext->headerNode();

        nextRowInCol = insertNext;
        cover(colNode);

        rowNode = nextRowInCol->right();
        while(rowNode != nextRowInCol) {
          cover(rowNode->headerNode());
          rowNode = rowNode->right();
        }

        puzzleNodes.push(insertNext);
        m_workingSolution.push(*insertNext);
      }
    }

    return puzzleNodes;
  }

  std::stack<Node>
  SudokuMatrix::solve(const Board& board) {
    m_solved = false;
    totalCompetition = 0;

    std::stack<Node> temp;
    m_workingSolution.swap(temp);

    std::stack<Node*> puzzleNodes = initializePuzzle(board);

    log("Solving sudoku...");

    if (solve()) {
      log(
        "Successfully solved sudoku ! (" + std::to_string(m_workingSolution.size()) +
        " node(s) were missing)",
        utils::Level::Info
      );
    }
    else {
      log("Failed to solve sudoku !", utils::Level::Error);
    }

    // Uncover the nodes that we covered earlier, so that we can
    // solve a different puzzle in the future.
    while(!puzzleNodes.empty()) {
      Node* header = puzzleNodes.top()->headerNode();
      Node* nextRowInCol = puzzleNodes.top();

      Node* rowNode = nextRowInCol->right();
      while(rowNode != nextRowInCol) {
        uncover(rowNode->headerNode());
        rowNode = rowNode->right();
      }

      uncover(header);
      puzzleNodes.pop();
    }

    // Build the output list of nodes.
    std::stack<Node> output;
    output.swap(m_workingSolution);

    return output;
  }

  bool
  SudokuMatrix::solve() {
    if (empty()) {
      // Matrix is empty, solution found.
      return true;
    }

    int numCols;
    Node* nextCol = nullptr;
    nextCol = chooseNextColumn(numCols);

    if (numCols < 1) {
      return false;
    }

    totalCompetition += numCols;

    Node* nextRowInCol = nextCol->bottom();
    Node* rowNode;
    cover(nextCol);

    // Need check for solved so that matrix is successfully
    // uncovered after solve, for memory management purposes.
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

    // Could not satisfy constraints of this column.
    return m_solved;
  }

  Node*
  SudokuMatrix::chooseNextColumn(int& count) {
    Node* currentBest = m_root->right();
    int best = -1;
    int tempCount = 0;

    // Iterate through currentBest and count nodes, then
    // iterate through currentBest's neighbors and count
    // their nodes.

    Node* next = currentBest->bottom();
    Node* nextCol = currentBest;

    while(nextCol != m_root.get()) {
      next = nextCol->bottom();
      tempCount = 0;

      while(next != nextCol) {
        if (next == next->bottom()) {
          error(
            "Failed to solve sudoku matrix",
            "Failed to find a suitable next candidate for sudoku solving"
          );
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

    // Then we have a problem.
    if (currentBest == m_root.get()) {
      error(
        "Failed to solve sudoku matrix",
        "Attempted to choose column from empty matrix!"
      );
    }

    count = best;
    return currentBest;
  }

}
