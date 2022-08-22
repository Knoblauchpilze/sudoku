
# include "Board.hh"
# include <cmath>
# include <fstream>

namespace sudoku {

  Board::Board() noexcept:
    utils::CoreObject("board"),

    m_width(9u),
    m_height(9u),

    m_board(w() * h(), 0u)
  {
    setService("sudoku");
  }

  unsigned
  Board::w() const noexcept {
    return m_width;
  }

  unsigned
  Board::h() const noexcept {
    return m_height;
  }

  bool
  Board::empty(unsigned x, unsigned y) const {
    if (x >= m_width || y >= m_height) {
      error(
        "Failed to fetch board number",
        "Invalid coordinate " + std::to_string(x) + "x" + std::to_string(y)
      );
    }

    return m_board[linear(x, y)] == 0u;
  }

  unsigned
  Board::at(unsigned x, unsigned y) const {
    if (x >= m_width || y >= m_height) {
      error(
        "Failed to fetch board number",
        "Invalid coordinate " + std::to_string(x) + "x" + std::to_string(y)
      );
    }

    return m_board[linear(x, y)];
  }

  bool
  Board::canFit(unsigned x, unsigned y, unsigned /*digit*/) const {
    if (x >= m_width || y >= m_height) {
      error(
        "Failed to fetch digit status",
        "Invalid coordinate " + std::to_string(x) + "x" + std::to_string(y)
      );
    }

    /// TODO: Determine if a digit can fit.
    return true;
  }

  void
  Board::put(unsigned x, unsigned y, unsigned digit) {
    if (x >= m_width || y >= m_height) {
      error(
        "Failed to put number on board",
        "Invalid coordinate " + std::to_string(x) + "x" + std::to_string(y)
      );
    }
    if (digit >= 10u) {
      error(
        "Failed to put number on board",
        "Invalid digit " + std::to_string(digit) + " not in range [0; 9]"
      );
    }

    m_board[linear(x, y)] = digit;
  }

  void
  Board::reset() noexcept {
    m_board = std::vector<unsigned>(w() * h(), 0u);
  }

  bool
  Board::generate(unsigned digits) noexcept {
    /// TODO: Generate a valid sudoku.
    warn("Should generate game with " + std::to_string(digits) + " digit(s)");

    unsigned id = 0u;
    while (id < digits) {
      unsigned x = std::rand() % 9u;
      unsigned y = std::rand() % 9u;

      m_board[linear(x, y)] = 1u + std::rand() % 9u;

      ++id;
    }

    return false;
  }

  void
  Board::save(const std::string& file) const
  {
    // Open the file and verify that it is valid.
    std::ofstream out(file.c_str());
    if (!out.good()) {
      error(
        "Failed to save board to \"" + file + "\"",
        "Failed to open file"
      );
    }

    unsigned buf, size = sizeof(unsigned);
    const char* raw = reinterpret_cast<const char*>(&buf);

    // Save the dimensions of the board.
    buf = m_width;
    out.write(raw, size);

    buf = m_height;
    out.write(raw, size);

    // Save the content board.
    for (unsigned id = 0u ; id < m_board.size() ; ++id) {
      buf = m_board[id];
      out.write(raw, size);
    }

    info(
      "Saved content of board with dimensions " +
      std::to_string(m_width) + "x" + std::to_string(m_height) +
      " to \"" + file + "\""
    );
  }

  void
  Board::load(const std::string& file) {
    // Open the file and verify that it is valid.
    std::ifstream out(file.c_str());
    if (!out.good()) {
      error(
        "Failed to load board to \"" + file + "\"",
        "Failed to open file"
      );
    }

    out.read(reinterpret_cast<char*>(&m_width), sizeof(unsigned));
    out.read(reinterpret_cast<char*>(&m_height), sizeof(unsigned));

    // Consistency check.
    if (m_width == 0u || m_height == 0u) {
      error(
        "Failed to load board from file \"" + file + "\"",
        "Invalid board of size " + std::to_string(m_width) + "x" +
        std::to_string(m_height)
      );
    }

    // Read the content of the board.
    m_board = std::vector<unsigned>(m_width * m_height, 0u);

    for (unsigned id = 0u ; id < m_board.size() ; ++id) {
      out.read(reinterpret_cast<char*>(&m_board[id]), sizeof(unsigned));
    }

    info("Loaded board with dimensions " + std::to_string(m_width) + "x" + std::to_string(m_height));
  }

  inline
  unsigned
  Board::linear(unsigned x, unsigned y) const noexcept {
    return y * m_width + x;
  }

}
