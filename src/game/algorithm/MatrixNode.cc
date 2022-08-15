
# include "MatrixNode.hh"

namespace sudoku::algorithm {

  Node::Node():
    m_top(nullptr),
    m_bottom(nullptr),
    m_left(nullptr),
    m_right(nullptr),

    m_row(-1),
    m_column(-1),
    m_value(-1),

    m_header(false),
    m_colHeader(nullptr)
  {}

  Node::Node(int row, int column, int value):
    m_top(nullptr),
    m_bottom(nullptr),
    m_left(nullptr),
    m_right(nullptr),

    m_row(row),
    m_column(column),
    m_value(value),

    m_header(false),
    m_colHeader(nullptr)
  {}

  int
  Node::row() const noexcept {
    return m_row;
  }

  int
  Node::column() const noexcept {
    return m_column;
  }

  int
  Node::value() const noexcept {
    return m_value;
  }

  bool
  Node::header() const noexcept {
    return m_header;
  }

  void
  Node::makeHeader() noexcept {
    m_header = true;
  }

  Node*
  Node::top() const noexcept {
    return m_top;
  }

  Node*
  Node::bottom() const noexcept {
    return m_bottom;
  }

  Node*
  Node::left() const noexcept {
    return m_left;
  }

  Node*
  Node::right() const noexcept {
    return m_right;
  }

  Node*
  Node::headerNode() const noexcept {
    return m_colHeader;
  }

  void
  Node::linkTop(Node* top) noexcept {
    m_top = top;
  }

  void
  Node::linkBottom(Node* bottom) noexcept {
    m_bottom = bottom;
  }

  void
  Node::linkLeft(Node* left) noexcept {
    m_left = left;
  }

  void
  Node::linkRight(Node* right) noexcept {
    m_right = right;
  }

  void
  Node::setHeader(Node* header) noexcept {
    m_colHeader = header;
  }

}
