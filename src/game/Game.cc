
# include "Game.hh"
# include <cxxabi.h>
# include "Menu.hh"

/// @brief - The height of the main menu.
# define STATUS_MENU_HEIGHT 50

/// @brief - The delay in milliseconds to display a hint.
# define HINT_DISPLAY_DELAY_MS 500

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

  // pge::MenuShPtr
  // generateMessageBoxMenu(const olc::vi2d& pos,
  //                        const olc::vi2d& size,
  //                        const std::string& text,
  //                        const std::string& name,
  //                        bool alert)
  // {
  //   pge::menu::MenuContentDesc fd = pge::menu::newMenuContent(text, "", size);
  //   fd.color = (alert ? olc::RED : olc::GREEN);
  //   fd.align = pge::menu::Alignment::Center;

  //   return std::make_shared<pge::Menu>(
  //     pos,
  //     size,
  //     name,
  //     pge::menu::newColoredBackground(alert ? olc::VERY_DARK_RED : olc::VERY_DARK_GREEN),
  //     fd,
  //     pge::menu::Layout::Horizontal,
  //     false,
  //     false
  //   );
  // }

}

namespace pge {

  Game::Game():
    utils::CoreObject("game"),

    m_state(
      State{
        true,  // paused
        true,  // disabled
        false, // terminated
      }
    ),

    m_menus(),

    /// TODO: Handle game difficulty.
    m_board(std::make_shared<sudoku::Game>(sudoku::Level::Medium)),
    m_hint({-1, -1, utils::TimeStamp(), false, std::vector<MenuShPtr>()})
  {
    setService("game");
  }

  Game::~Game() {}

  std::vector<MenuShPtr>
  Game::generateMenus(float width,
                      float height)
  {
    olc::Pixel bg(250, 248, 239);
    olc::Pixel buttonBG(185, 172, 159);

    // Generate the status menu.
    MenuShPtr status = generateMenu(olc::vi2d(), olc::vi2d(width, STATUS_MENU_HEIGHT), "", "status", bg);

    olc::vi2d pos;
    olc::vi2d dims(50, STATUS_MENU_HEIGHT);
    MenuShPtr mLabel = generateMenu(pos, dims, "Cell(s):", "cells_label", buttonBG);
    // m_menus.moves = generateMenu(pos, dims, "0", "moves", buttonBG);

    // The list of remaining numbers
    m_menus.digits.resize(9u);

    m_menus.digits[0u] = generateMenu(pos, dims, "1s: 9", "ones", buttonBG);
    status->addMenu(m_menus.digits[0u]);
    m_menus.digits[1u] = generateMenu(pos, dims, "2s: 9", "twos", buttonBG);
    status->addMenu(m_menus.digits[1u]);
    m_menus.digits[2u] = generateMenu(pos, dims, "3s: 9", "threes", buttonBG);
    status->addMenu(m_menus.digits[2u]);
    m_menus.digits[3u] = generateMenu(pos, dims, "4s: 9", "fours", buttonBG);
    status->addMenu(m_menus.digits[3u]);
    m_menus.digits[4u] = generateMenu(pos, dims, "5s: 9", "fives", buttonBG);
    status->addMenu(m_menus.digits[4u]);
    m_menus.digits[5u] = generateMenu(pos, dims, "6s: 9", "sixes", buttonBG);
    status->addMenu(m_menus.digits[5u]);
    m_menus.digits[6u] = generateMenu(pos, dims, "7s: 9", "sevens", buttonBG);
    status->addMenu(m_menus.digits[6u]);
    m_menus.digits[7u] = generateMenu(pos, dims, "8s: 9", "eights", buttonBG);
    status->addMenu(m_menus.digits[7u]);
    m_menus.digits[8u] = generateMenu(pos, dims, "9s: 9", "nines", buttonBG);
    status->addMenu(m_menus.digits[8u]);

    MenuShPtr reset = generateMenu(pos, dims, "Reset", "reset", buttonBG, true);
    reset->setSimpleAction(
      [](Game& g) {
        g.reset();
      }
    );
    status->addMenu(reset);

    MenuShPtr hint = generateMenu(olc::vi2d(0, height - STATUS_MENU_HEIGHT), olc::vi2d(width, STATUS_MENU_HEIGHT), "", "hint", bg);

    dims = olc::vi2d(50, STATUS_MENU_HEIGHT);
    for (unsigned id = 0u ; id < 9u ; ++id) {
      std::string str = std::to_string(id);
      MenuShPtr d = generateMenu(pos, dims, str, "digit" + str, buttonBG);
      d->setEnabled(false);
      m_hint.menus.push_back(d);
      hint->addMenu(d);
    }

    // Package menus for output.
    std::vector<MenuShPtr> menus;

    menus.push_back(status);
    menus.push_back(hint);

    return menus;
  }

  void
  Game::performAction(float /*x*/, float /*y*/) {
    // Only handle actions when the game is not disabled.
    if (m_state.disabled) {
      log("Ignoring action while menu is disabled");
      return;
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
  Game::reset() {
    // Reset the sudoku game.
    m_board->initialize();
  }

  const sudoku::Board&
  Game::board() const noexcept {
    return (*m_board)();
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
    warn("Should handle digit " + std::to_string(digit));
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
        m.setEnabled(b.canFit(m_hint.x, m_hint.y, id));
      }
    }
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
