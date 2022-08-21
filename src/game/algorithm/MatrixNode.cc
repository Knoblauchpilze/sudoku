
# include "MatrixNode.hh"

namespace sudoku::algorithm {

  MatrixNode::MatrixNode():
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

  MatrixNode::MatrixNode(int row, int column, int value):
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
  MatrixNode::row() const noexcept {
    return m_row;
  }

  int
  MatrixNode::column() const noexcept {
    return m_column;
  }

  int
  MatrixNode::value() const noexcept {
    return m_value;
  }

  bool
  MatrixNode::header() const noexcept {
    return m_header;
  }

  void
  MatrixNode::makeHeader() noexcept {
    m_header = true;
  }

  MatrixNode*
  MatrixNode::top() const noexcept {
    return m_top;
  }

  MatrixNode*
  MatrixNode::bottom() const noexcept {
    return m_bottom;
  }

  MatrixNode*
  MatrixNode::left() const noexcept {
    return m_left;
  }

  MatrixNode*
  MatrixNode::right() const noexcept {
    return m_right;
  }

  MatrixNode*
  MatrixNode::headerNode() const noexcept {
    return m_colHeader;
  }

  void
  MatrixNode::linkTop(MatrixNode* top) noexcept {
    m_top = top;
  }

  void
  MatrixNode::linkBottom(MatrixNode* bottom) noexcept {
    m_bottom = bottom;
  }

  void
  MatrixNode::linkLeft(MatrixNode* left) noexcept {
    m_left = left;
  }

  void
  MatrixNode::linkRight(MatrixNode* right) noexcept {
    m_right = right;
  }

  void
  MatrixNode::setHeader(MatrixNode* header) noexcept {
    m_colHeader = header;
  }

  std::string
  MatrixNode::toString() const noexcept {
    std::string out = "[";

    if (m_header) {
      out += "HEAD ";
    }

    out += "row: ";
    out += std::to_string(m_row);
    out += ", ";

    out += "column: ";
    out += std::to_string(m_column);
    out += ", ";

    out += "value: ";
    out += std::to_string(m_value);

    out += "]";

    return out;
  }

}
