
# include "Board.hh"
# include <cmath>
# include <fstream>
# include <core_utils/RNG.hh>
# include "Definitions.hh"
# include "Definitions.hh"
# include "SudokuMatrix.hh"

namespace sudoku {
  namespace {

    /// @brief - A convenience structure representing a digit
    /// at a specific position.
    struct DigitAt {
      /// @brief - The value of the digit.
      unsigned value;

      /// @brief - The row in which the digit should be placed.
      unsigned x;

      /// @brief - The column in which the digit should be
      /// placed.
      unsigned y;

      /// @brief - The width of the board.
      unsigned width;

      /// @brief - The height of the board.
      unsigned height;
    };

    bool
    canFitInColumn(const std::vector<unsigned>& board, const DigitAt& digit) noexcept {
      if (digit.x >= counting::columnsCount || digit.y >= counting::rowsCount) {
        return false;
      }

      unsigned y = 0u;
      while (y < counting::rowsCount) {
        if (board[y * digit.width + digit.x] == digit.value) {
          return false;
        }

        ++y;
      }

      return true;
    }

    bool
    canFitInRow(const std::vector<unsigned>& board, const DigitAt& digit) noexcept {
      if (digit.x >= counting::columnsCount || digit.y >= counting::rowsCount) {
        return false;
      }

      unsigned x = 0u;
      while (x < counting::columnsCount) {
        if (board[digit.y * digit.width + x] == digit.value) {
          return false;
        }

        ++x;
      }

      return true;
    }

    bool
    canFitInBox(const std::vector<unsigned>& board, const DigitAt& digit) noexcept {
      if (digit.x >= counting::columnsCount || digit.y >= counting::rowsCount) {
        return false;
      }

      unsigned bx = digit.x / counting::boxesXCount;
      unsigned by = digit.y / counting::boxesYCount;

      unsigned p = 0u;
      while (p < counting::boxCellsCount) {
        unsigned yOffset = counting::boxYCellsCount * by + p / counting::boxYCellsCount;
        unsigned xOffset = counting::boxXCellsCount * bx + p % counting::boxXCellsCount;

        if (board[yOffset * digit.width + xOffset] == digit.value) {
          return false;
        }

        ++p;
      }

      return true;
    }

  }

  std::string
  toString(const ConstraintKind& constraint) noexcept {
    switch (constraint) {
      case ConstraintKind::Row:
        return "row";
      case ConstraintKind::Column:
        return "column";
      case ConstraintKind::Box:
        return "box";
      case ConstraintKind::None:
        return "none";
      default:
        return "unknown";
    }
  }

  Board::Board() noexcept:
    utils::CoreObject("board"),

    m_width(9u),
    m_height(9u),

    m_board(w() * h(), 0u),
    m_kinds(w() * h(), DigitKind::None)
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
  Board::at(unsigned x, unsigned y, DigitKind* kind) const {
    if (x >= m_width || y >= m_height) {
      error(
        "Failed to fetch board number",
        "Invalid coordinate " + std::to_string(x) + "x" + std::to_string(y)
      );
    }

    if (kind != nullptr) {
      *kind = m_kinds[linear(x, y)];
    }

    return m_board[linear(x, y)];
  }

  bool
  Board::canFit(unsigned x,
                unsigned y,
                unsigned digit,
                ConstraintKind* reason) const
  {
    if (x >= m_width || y >= m_height) {
      error(
        "Failed to fetch digit status",
        "Invalid coordinate " + std::to_string(x) + "x" + std::to_string(y)
      );
    }

    DigitAt d{digit, x, y, m_width, m_height};

    if (!canFitInColumn(m_board, d)) {
      log(
        "Digit " + std::to_string(digit) + " doesn't fit in column " + std::to_string(x),
        utils::Level::Verbose
      );

      if (reason != nullptr) {
        *reason = ConstraintKind::Column;
      }

      return false;
    }

    if (!canFitInRow(m_board, d)) {
      log(
        "Digit " + std::to_string(digit) + " doesn't fit in row " + std::to_string(y),
        utils::Level::Verbose
      );

      if (reason != nullptr) {
        *reason = ConstraintKind::Row;
      }

      return false;
    }

    if (!canFitInBox(m_board, d)) {
      log(
        "Digit " + std::to_string(digit) + " doesn't fit in box " + std::to_string(1u + x / 3u) + "x" + std::to_string(1u + y / 3u),
        utils::Level::Verbose
      );

      if (reason != nullptr) {
        *reason = ConstraintKind::Box;
      }

      return false;
    }

    if (reason != nullptr) {
      *reason = ConstraintKind::None;
    }

    return true;
  }

  void
  Board::put(unsigned x,
             unsigned y,
             unsigned digit,
             const DigitKind& kind)
  {
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
    m_kinds[linear(x, y)] = (digit == 0u ? DigitKind::None : kind);
  }

  void
  Board::reset() noexcept {
    m_board = std::vector<unsigned>(w() * h(), 0u);
    m_kinds = std::vector<DigitKind>(w() * h(), DigitKind::None);
  }

  bool
  Board::generate(unsigned digits) noexcept {
    if (digits > counting::cellsCount) {
      return false;
    }

    utils::RNG rng;

    // Put a random digit somewhere to initialize the
    // board. This will prevent identical sudokus to
    // be generated.
    unsigned digit = rng.rndInt(1, counting::candidates);
    unsigned x = rng.rndInt(0u, counting::columnsCount - 1u);
    unsigned y = rng.rndInt(0u, counting::rowsCount - 1u);

    log(
      "Starting with seed " + std::to_string(digit) + " at " +
      std::to_string(x) + "x" + std::to_string(y)
    );

    m_board[y * m_width + x] = digit;

    // Solve the sudoku.
    std::stack<sudoku::algorithm::MatrixNode> nodes;
    sudoku::algorithm::SudokuMatrix solver;
    nodes = solver.solve(*this);

    if (nodes.empty()) {
      error("Failed to generate sudoku");
    }

    // Fill the board.
    while (!nodes.empty()) {
      sudoku::algorithm::MatrixNode node = nodes.top();
      nodes.pop();

      put(
        node.column(),
        node.row(),
        node.value(),
        sudoku::DigitKind::Solved
      );
    }

    // Now remove digits randomly until we reach the amount
    // of digits we want to keep.
    unsigned toRemove = counting::cellsCount - digits;
    unsigned removed = 0u;

    unsigned failures = 0u;
    unsigned totalFailures = 0u;
    // A large amount representing how many failures we can
    // tolerate when generating the sudoku.
    constexpr auto maxFailues = 81;

    while (removed < toRemove && failures <= maxFailues) {
      unsigned x = rng.rndInt(0u, counting::columnsCount - 1u);
      unsigned y = rng.rndInt(0u, counting::rowsCount - 1u);

      // Remove the digit.
      DigitKind kind;
      digit = at(x, y, &kind);

      put(x, y, 0u, DigitKind::None);

      // Check if the sudoku is still solvable.
      sudoku::algorithm::SudokuMatrix solver;
      if (solver.solvable(*this)) {
        ++removed;
        // Reset the failures.
        log("Generated digit " + std::to_string(digit) + " after " + std::to_string(failures) + " failure(s)");
        failures = 0u;
      }
      else {
        // Restore the digit.
        put(x, y, digit, kind);
        ++failures;
        ++totalFailures;
      }
    }

    info(
      "Generated sudoku with " + std::to_string(digits) +
      " after " + std::to_string(totalFailures) + " failure(s)"
    );

    return true;
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

    std::vector<char> kBuf;
    unsigned kSize = sizeof(std::underlying_type<DigitKind>::type);
    kBuf.resize(kSize);
    const char* kRaw = kBuf.data();

    // Save the dimensions of the board.
    buf = m_width;
    out.write(raw, size);

    buf = m_height;
    out.write(raw, size);

    // Save the content board.
    for (unsigned id = 0u ; id < m_board.size() ; ++id) {
      buf = m_board[id];
      out.write(raw, size);

      kRaw = reinterpret_cast<char*>(&m_kinds[id]);
      out.write(kRaw, kSize);
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
    m_kinds = std::vector<DigitKind>(m_width * m_height, DigitKind::None);

    for (unsigned id = 0u ; id < m_board.size() ; ++id) {
      out.read(reinterpret_cast<char*>(&m_board[id]), sizeof(unsigned));
      out.read(reinterpret_cast<char*>(&m_kinds[id]), sizeof(std::underlying_type<DigitKind>::type));
    }

    info("Loaded board with dimensions " + std::to_string(m_width) + "x" + std::to_string(m_height));
  }

  inline
  unsigned
  Board::linear(unsigned x, unsigned y) const noexcept {
    return y * m_width + x;
  }

}
