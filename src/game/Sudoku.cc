
#include "Sudoku.hh"
#include <core_utils/Chrono.hh>

namespace {

unsigned levelToNumbers(const sudoku::Level &level) noexcept {
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

} // namespace

namespace sudoku {

Game::Game(const Level &level) noexcept
    : utils::CoreObject("board"),

      m_board(), m_level(level) {
  setService("sudoku");
}

unsigned Game::w() const noexcept { return m_board.w(); }

unsigned Game::h() const noexcept { return m_board.h(); }

const Board &Game::operator()() const noexcept { return m_board; }

void Game::clear() noexcept { m_board.reset(); }

void Game::initialize() noexcept {
  // Reset the board and generate it with a certain
  // amount of numbers visible still.
  m_board.reset();

  bool generated = false;
  withSafetyNet(
      [this, &generated]() {
        utils::ChronoMilliseconds c("Solving Sudoku", "solver");
        generated = m_board.generate(levelToNumbers(m_level));
      },
      "Board::generate");

  if (!generated) {
    error("Failed to generate sudoku");
  }
}

void Game::load(const std::string &file) { m_board.load(file); }

void Game::save(const std::string &file) const { m_board.save(file); }

bool Game::put(unsigned x, unsigned y, unsigned digit, const DigitKind &kind,
               ConstraintKind *reason) {
  if (digit != 0u && !m_board.canFit(x, y, digit, reason)) {
    return false;
  }

  DigitKind eKind = DigitKind::None;
  m_board.at(x, y, &eKind);
  if (eKind == DigitKind::Generated) {
    return false;
  }

  m_board.put(x, y, digit, kind);

  return true;
}

bool Game::solved() const noexcept { return m_board.solved(); }

} // namespace sudoku
