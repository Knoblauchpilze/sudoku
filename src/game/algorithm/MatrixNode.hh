#ifndef    MATRIX_NODE_HH
# define   MATRIX_NODE_HH

# include <string>

namespace sudoku::algorithm {

  class MatrixNode {
    public:

    MatrixNode();

    MatrixNode(int row, int column, int value);

    int
    row() const noexcept;

    int
    column() const noexcept;

    int
    value() const noexcept;

    bool
    header() const noexcept;

    void
    makeHeader() noexcept;

    MatrixNode*
    top() const noexcept;

    MatrixNode*
    bottom() const noexcept;

    MatrixNode*
    left() const noexcept;

    MatrixNode*
    right() const noexcept;

    MatrixNode*
    headerNode() const noexcept;

    void
    linkTop(MatrixNode* top) noexcept;

    void
    linkBottom(MatrixNode* bottom) noexcept;

    void
    linkLeft(MatrixNode* left) noexcept;

    void
    linkRight(MatrixNode* right) noexcept;

    void
    setHeader(MatrixNode* header) noexcept;

    std::string
    toString() const noexcept;

    private:

      MatrixNode* m_top;
      MatrixNode* m_bottom;
      MatrixNode* m_left;
      MatrixNode* m_right;

      int m_row;
      int m_column;
      int m_value;

      bool m_header;
      MatrixNode* m_colHeader;
  };

}

#endif    /* MATRIX_NODE_HH */