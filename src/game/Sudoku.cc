
# include "Sudoku.hh"

namespace {

  unsigned
  levelToNumbers(const sudoku::Level& level) noexcept {
    switch (level) {
      case sudoku::Level::Medium:
        return 20u;
      case sudoku::Level::Hard:
        return 15u;
      case sudoku::Level::Easy:
      default:
        // Similar to the easy mode.
        return 25u;
    }
  }

}

namespace sudoku {

  Game::Game(const Level& level) noexcept:
    utils::CoreObject("board"),

    m_board(),
    m_level(level)
  {
    setService("sudoku");

    initialize();
  }

  unsigned
  Game::w() const noexcept {
    return m_board.w();
  }

  unsigned
  Game::h() const noexcept {
    return m_board.h();
  }

  const Board&
  Game::operator()() const noexcept {
    return m_board;
  }

  void
  Game::clear() noexcept {
    m_board.reset();
  }

  void
  Game::initialize() noexcept {
    // Reset the board and generate it with a certain
    // amount of numbers visible still.
    m_board.reset();
    m_board.generate(levelToNumbers(m_level));
  }

  void
  Game::load(const std::string& file) {
    m_board.load(file);
  }

  void
  Game::save(const std::string& file) const {
    m_board.save(file);
  }

}
