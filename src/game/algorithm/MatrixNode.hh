#ifndef    MATRIX_NODE_HH
# define   MATRIX_NODE_HH

namespace sudoku::algorithm {

  class Node {
    public:

    Node();

    Node(int row, int column, int value);

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

    Node*
    top() const noexcept;

    Node*
    bottom() const noexcept;

    Node*
    left() const noexcept;

    Node*
    right() const noexcept;

    Node*
    headerNode() const noexcept;

    void
    linkTop(Node* top) noexcept;

    void
    linkBottom(Node* bottom) noexcept;

    void
    linkLeft(Node* left) noexcept;

    void
    linkRight(Node* right) noexcept;

    void
    setHeader(Node* header) noexcept;

    private:

      Node* m_top;
      Node* m_bottom;
      Node* m_left;
      Node* m_right;

      int m_row;
      int m_column;
      int m_value;

      bool m_header;
      Node* m_colHeader;
  };

}

#endif    /* MATRIX_NODE_HH */