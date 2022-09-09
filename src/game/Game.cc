
# include "Game.hh"
# include <cxxabi.h>
# include <core_utils/Chrono.hh>
# include "Menu.hh"
# include "SudokuMatrix.hh"

/// @brief - The height of the main menu.
# define STATUS_MENU_HEIGHT 50

/// @brief - The delay in milliseconds to display a hint.
# define HINT_DISPLAY_DELAY_MS 500

/// @brief - The duration of the alert prompting that
/// the player that the sudoku was solved or can't be
/// solved.
# define ALERT_DURATION_MS 3000

namespace {

  pge::MenuShPtr
  generateMenu(const olc::vi2d& pos,
               const olc::vi2d& size,
               const std::string& text,
               const std::string& name,
               const olc::Pixel& color,
               bool clickable = false,
               bool selectable = false)
  {
    pge::menu::MenuContentDesc fd = pge::menu::newMenuContent(text, "", size);
    fd.color = olc::BLACK;
    fd.hColor = olc::GREY;
    fd.align = pge::menu::Alignment::Center;

    return std::make_shared<pge::Menu>(
      pos,
      size,
      name,
      pge::menu::newColoredBackground(color),
      fd,
      pge::menu::Layout::Horizontal,
      clickable,
      selectable
    );
  }

  pge::MenuShPtr
  generateMessageBoxMenu(const olc::vi2d& pos,
                         const olc::vi2d& size,
                         const std::string& text,
                         const std::string& name,
                         bool alert)
  {
    pge::menu::MenuContentDesc fd = pge::menu::newMenuContent(text, "", size);
    fd.color = (alert ? olc::RED : olc::GREEN);
    fd.align = pge::menu::Alignment::Center;

    return std::make_shared<pge::Menu>(
      pos,
      size,
      name,
      pge::menu::newColoredBackground(alert ? olc::VERY_DARK_RED : olc::VERY_DARK_GREEN),
      fd,
      pge::menu::Layout::Horizontal,
      false,
      false
    );
  }

  const auto BUTTON_BG = olc::Pixel(185, 172, 159);
  const auto DISABLED_BUTTON_BG = olc::Pixel(92, 86, 78);

}

namespace pge {

  Game::Game():
    utils::CoreObject("game"),

    m_state(
      State{
        true,              // paused
        true,              // disabled
        false,             // terminated
        Mode::Solver,      // mode
        SolverStep::None,  // solverStep
      }
    ),

    m_menus(),

    m_board(std::make_shared<sudoku::Game>(sudoku::Level::Medium)),
    m_hint(HintData{
      -1,                      // x
      -1,                      // y
      1u,                      // digit
      utils::TimeStamp(),      // since
      false,                   // active
      std::vector<MenuShPtr>() // menus
    })
  {
    setService("game");
  }

  Game::~Game() {}

  std::vector<MenuShPtr>
  Game::generateMenus(float width,
                      float height)
  {
    olc::Pixel bg(250, 248, 239);

    // Generate the status menu.
    m_menus.status = generateMenu(olc::vi2d(), olc::vi2d(width, STATUS_MENU_HEIGHT), "", "status", bg);

    olc::vi2d pos;
    olc::vi2d dims(50, STATUS_MENU_HEIGHT);
    MenuShPtr mLabel = generateMenu(pos, dims, "Cell(s):", "cells_label", BUTTON_BG);

    // The list of remaining numbers
    m_menus.digits.resize(9u);

    m_menus.digits[0u] = generateMenu(pos, dims, "1s: 9", "ones", BUTTON_BG);
    m_menus.status->addMenu(m_menus.digits[0u]);
    m_menus.digits[1u] = generateMenu(pos, dims, "2s: 9", "twos", BUTTON_BG);
    m_menus.status->addMenu(m_menus.digits[1u]);
    m_menus.digits[2u] = generateMenu(pos, dims, "3s: 9", "threes", BUTTON_BG);
    m_menus.status->addMenu(m_menus.digits[2u]);
    m_menus.digits[3u] = generateMenu(pos, dims, "4s: 9", "fours", BUTTON_BG);
    m_menus.status->addMenu(m_menus.digits[3u]);
    m_menus.digits[4u] = generateMenu(pos, dims, "5s: 9", "fives", BUTTON_BG);
    m_menus.status->addMenu(m_menus.digits[4u]);
    m_menus.digits[5u] = generateMenu(pos, dims, "6s: 9", "sixes", BUTTON_BG);
    m_menus.status->addMenu(m_menus.digits[5u]);
    m_menus.digits[6u] = generateMenu(pos, dims, "7s: 9", "sevens", BUTTON_BG);
    m_menus.status->addMenu(m_menus.digits[6u]);
    m_menus.digits[7u] = generateMenu(pos, dims, "8s: 9", "eights", BUTTON_BG);
    m_menus.status->addMenu(m_menus.digits[7u]);
    m_menus.digits[8u] = generateMenu(pos, dims, "9s: 9", "nines", BUTTON_BG);
    m_menus.status->addMenu(m_menus.digits[8u]);

    MenuShPtr reset = generateMenu(pos, dims, "Reset", "reset", BUTTON_BG, true);
    reset->setSimpleAction(
      [](Game& g) {
        g.reset();
      }
    );
    m_menus.status->addMenu(reset);

    m_menus.hint = generateMenu(olc::vi2d(0, height - STATUS_MENU_HEIGHT), olc::vi2d(width, STATUS_MENU_HEIGHT), "", "hint", bg);

    dims = olc::vi2d(50, STATUS_MENU_HEIGHT);
    for (unsigned id = 0u ; id < 9u ; ++id) {
      std::string str = std::to_string(id);
      MenuShPtr d = generateMenu(pos, dims, str, "digit" + str, BUTTON_BG);
      d->setEnabled(false);
      m_hint.menus.push_back(d);
      m_menus.hint->addMenu(d);
    }

    // Generate the menus for the solver mode.
    m_menus.solve = generateMenu(pos, olc::vi2d(width, STATUS_MENU_HEIGHT), "Solve !", "solve", olc::DARK_APPLE_GREEN, true);
    m_menus.solve->setSimpleAction(
      [](Game& g) {
        g.solve();
      }
    );

    m_menus.solvedAlert.date = utils::TimeStamp();
    m_menus.solvedAlert.wasActive = false;
    m_menus.solvedAlert.duration = ALERT_DURATION_MS;

    m_menus.solvedAlert.menu = generateMessageBoxMenu(
      olc::vi2d((width - 300.0f) / 2.0f, (height - 150.0f) / 2.0f),
      olc::vi2d(300, 150),
      "Solved !",
      "solved_alert",
      false
    );
    m_menus.solvedAlert.menu->setVisible(false);

    m_menus.unsolvableAlert.date = utils::TimeStamp();
    m_menus.unsolvableAlert.wasActive = false;
    m_menus.unsolvableAlert.duration = ALERT_DURATION_MS;

    m_menus.unsolvableAlert.menu = generateMessageBoxMenu(
      olc::vi2d((width - 300.0f) / 2.0f, (height - 150.0f) / 2.0f),
      olc::vi2d(300, 150),
      "Unsolvable !",
      "unsolvable_alert",
      true
    );
    m_menus.unsolvableAlert.menu->setVisible(false);

    // Package menus for output.
    std::vector<MenuShPtr> menus;

    menus.push_back(m_menus.status);
    menus.push_back(m_menus.solve);

    menus.push_back(m_menus.hint);

    menus.push_back(m_menus.solvedAlert.menu);
    menus.push_back(m_menus.unsolvableAlert.menu);

    return menus;
  }

  void
  Game::performAction(float x, float y) {
    // Only handle actions when the game is not disabled.
    if (m_state.disabled) {
      log("Ignoring action while menu is disabled");
      return;
    }

    int ix = static_cast<int>(x);
    int iy = static_cast<int>(y);

    // Prevent invalid coordinates.
    if (ix < 0 || iy < 0 || ix >= 9 || iy >= 9) {
      return;
    }

    unsigned ux = static_cast<unsigned>(ix);
    unsigned uy = static_cast<unsigned>(iy);

    // Loop until we can put a valid number in the current
    // cell of the board.
    const sudoku::Board& b = (*m_board)();

    unsigned tries = 0u;
    bool put = false;
    while (tries < 10u && !put) {
      ++tries;

      if (m_hint.digit == 10u) {
        m_hint.digit = 0u;
      }

      if (b.at(ux, uy) == m_hint.digit) {
        ++m_hint.digit;
        continue;
      }

      sudoku::ConstraintKind reason;
      if (!b.canFit(ux, uy, m_hint.digit, &reason)) {
        log(
          "Digit " + std::to_string(m_hint.digit) +
          " doesn't fit at " + std::to_string(ux) + "x" + std::to_string(uy) +
          " due to " + sudoku::toString(reason),
          utils::Level::Verbose
        );

        ++m_hint.digit;
        continue;
      }

      if (!m_board->put(ux, uy, m_hint.digit, sudoku::DigitKind::UserGenerated)) {
        ++m_hint.digit;
        continue;
      }

      log(
        "Put " + std::to_string(m_hint.digit) + " at " +
        std::to_string(ux) + "x" + std::to_string(uy)
      );

      put = true;

      // Reset the solver step.
      if (m_state.mode == Mode::Solver) {
        m_state.solverStep = SolverStep::Preparing;
      }
    }
  }

  bool
  Game::step(float /*tDelta*/) {
    // When the game is paused it is not over yet.
    if (m_state.paused) {
      return true;
    }

    utils::Duration elapsed = utils::now() - m_hint.since;
    if (m_hint.x >= 0 && m_hint.y >= 0 && elapsed >= utils::toMilliseconds(HINT_DISPLAY_DELAY_MS)) {
      m_hint.active = true;
    }

    /// TODO: Handle step method of the game.
    updateUI();

    return true;
  }

  void
  Game::togglePause() {
    if (m_state.paused) {
      resume();
    }
    else {
      pause();
    }

    enable(!m_state.paused);
  }

  void
  Game::setMode(const Mode& mode) noexcept {
    m_state.mode = mode;
    m_state.solverStep = (mode == Mode::Interactive ? SolverStep::None : SolverStep::Preparing);
  }

  void
  Game::reset() {
    // Reset the sudoku game.
    m_board->initialize();
  }

  void
  Game::clear() {
    m_board->clear();
  }

  const sudoku::Board&
  Game::board() const noexcept {
    return (*m_board)();
  }

  void
  Game::load(const std::string& file) {
    // Load the board.
    m_board->load(file);
  }

  void
  Game::save(const std::string& file) const {
    // Save the file including the number of moves and the score.
    m_board->save(file);
  }

  void
  Game::setActiveCell(float x, float y) {
    // Do nothing in case the game is already running.
    if (m_state.paused) {
      return;
    }

    int ix = static_cast<int>(x);
    int iy = static_cast<int>(y);

    if (ix == m_hint.x && iy == m_hint.y) {
      return;
    }

    m_hint.x = ix;
    m_hint.y = iy;

    const sudoku::Board& b = (*m_board)();
    unsigned digit = b.at(ix, iy);
    m_hint.digit = (digit != 0u ? digit : 1u);

    m_hint.since = utils::now();

    m_hint.active = false;
  }

  void
  Game::resetActiveCell() {
    m_hint.x = -1;
    m_hint.y = -1;

    m_hint.since = utils::now();

    m_hint.active = false;
  }

  void
  Game::onDigitPressed(unsigned digit) {
    // Early return if the digit if the same.
    const sudoku::Board& b = (*m_board)();
    if (b.at(m_hint.x, m_hint.y) == digit) {
      return;
    }

    if (!m_board->put(m_hint.x, m_hint.y, digit, sudoku::DigitKind::UserGenerated)) {
      warn(
        "Failed to put digit " + std::to_string(digit) +
        " at " + std::to_string(m_hint.x) + "x" + std::to_string(m_hint.y)
      );
    }
    else {
      // Reset the solver step.
      if (m_state.mode == Mode::Solver) {
        m_state.solverStep = SolverStep::Preparing;
      }

      // And update the digit of the active cell.
      m_hint.digit = digit;
    }
  }

  void
  Game::setDifficultyLevel(const sudoku::Level& level) {
    m_board = std::make_shared<sudoku::Game>(level);
  }

  void
  Game::solve() {
    // If we're in solving mode.
    if (m_state.mode != Mode::Solver) {
      warn("Ignoring solve request, not in solver mode");
      return;
    }

    // The solver step should allow solving the sudoku.
    if (m_state.solverStep == SolverStep::Solved) {
      warn("Ignoring solve request, sudoku is already solved");
      return;
    }
    if (m_state.solverStep == SolverStep::Unsolvable) {
      warn("Ignoring solve request, sudoku can't be solved");
      return;
    }

    m_state.solverStep = SolverStep::Solving;

    std::stack<sudoku::algorithm::MatrixNode> nodes;

    const sudoku::Board& b = (*m_board)();
    withSafetyNet(
      [&nodes, &b]() {
        utils::ChronoMilliseconds c("Solving Sudoku", "solver");
        sudoku::algorithm::SudokuMatrix solver;
        nodes = solver.solve(b);
      },
      "SudokuMatrix::solve"
    );

    if (nodes.empty()) {
      m_state.solverStep = SolverStep::Unsolvable;
    }
    else {
      m_state.solverStep = SolverStep::Solved;
    }

    // Fill in the puzzle.
    while (!nodes.empty()) {
      sudoku::algorithm::MatrixNode node = nodes.top();
      nodes.pop();

      m_board->put(
        node.column(),
        node.row(),
        node.value(),
        sudoku::DigitKind::Solved
      );
    }
  }

  void
  Game::enable(bool enable) {
    m_state.disabled = !enable;

    if (m_state.disabled) {
      log("Disabled game UI", utils::Level::Verbose);
    }
    else {
      log("Enabled game UI", utils::Level::Verbose);
    }
  }

  void
  Game::updateUI() {
    // Based on the mode, update one or the other menu.
    m_menus.status->setVisible(m_state.mode == Mode::Interactive);
    m_menus.hint->setVisible(m_state.mode == Mode::Interactive);
    m_menus.solve->setVisible(m_state.mode == Mode::Solver);

    switch (m_state.mode) {
      case Mode::Interactive:
        updateUIForInteractive();
        break;
      case Mode::Solver:
        updateUIForSolver();
        break;
      default:
        warn("Invalid game mode " + std::to_string(static_cast<int>(m_state.mode)));
        break;
    }
  }

  void
  Game::updateUIForInteractive() {
    // Update digits count.
    const sudoku::Board& b = (*m_board)();

    unsigned count[9u];
    for (unsigned id = 0u ; id < 9u ; ++id) {
      count[id] = 0u;

      unsigned digit = id + 1u;

      for (unsigned y = 0u ; y < b.h() ; ++y) {
        for (unsigned x = 0u ; x < b.w() ; ++x) {
          if (b.at(x, y) == digit) {
            ++count[id];
          }
        }
      }
    }

    menu::BackgroundDesc all = pge::menu::newColoredBackground(olc::PALE_GREEN);
    menu::BackgroundDesc missing = pge::menu::newColoredBackground(olc::PALE_YELLOW);

    for (unsigned id = 0u ; id < m_menus.digits.size() ; ++id) {
      std::string txt = std::to_string(id + 1u) + "s: " + std::to_string(count[id]);
      m_menus.digits[id]->setText(txt);

      if (count[id] == 9u) {
        m_menus.digits[id]->setBackground(all);
      }
      else {
        m_menus.digits[id]->setBackground(missing);
      }
    }

    for (unsigned id = 0u ; id < m_hint.menus.size() ; ++id) {
      Menu& m = *m_hint.menus[id];

      if (!m_hint.active) {
        m.setVisible(false);
      }
      else {
        m.setVisible(true);

        bool fit = b.canFit(m_hint.x, m_hint.y, id);
        m.setEnabled(fit);
        m.setBackground(menu::newColoredBackground(fit ? BUTTON_BG : DISABLED_BUTTON_BG));
      }
    }
  }

  void
  Game::updateUIForSolver() {
    m_menus.solvedAlert.update(m_state.solverStep == SolverStep::Solved);
    m_menus.unsolvableAlert.update(m_state.solverStep == SolverStep::Unsolvable);
  }

  bool
  Game::TimedMenu::update(bool active) noexcept {
    // In case the menu should be active.
    if (active) {
      if (!wasActive) {
        // Make it active if it's the first time that
        // we detect that it should be active.
        date = utils::now();
        wasActive = true;
        menu->setVisible(true);
      }
      else if (utils::now() > date + utils::toMilliseconds(duration)) {
        // Deactivate the menu in case it's been active
        // for too long.
        menu->setVisible(false);
      }
      else {
        // Update the alpha value in case it's active
        // for not long enough.
        olc::Pixel c = menu->getBackgroundColor();

        float d = utils::diffInMs(date, utils::now()) / duration;
        c.a = static_cast<uint8_t>(
          std::clamp((1.0f - d) * pge::alpha::Opaque, 0.0f, 255.0f)
        );
        menu->setBackground(pge::menu::newColoredBackground(c));
      }
    }
    // Or if the menu shouldn't be active anymore and
    // it's the first time we detect that.
    else if (wasActive) {
      // Deactivate the menu.
      menu->setVisible(false);
      wasActive = false;
    }

    return menu->visible();
  }

}
