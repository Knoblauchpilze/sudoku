
# include "App.hh"

# include <core_utils/RNG.hh>

namespace pge {

  App::App(const AppDesc& desc):
    PGEApp(desc),

    m_game(nullptr),
    m_state(nullptr),
    m_menus(),

    m_packs(std::make_shared<TexturePack>())
  {}

  bool
  App::onFrame(float fElapsed) {
    // Handle case where no game is defined.
    if (m_game == nullptr) {
      return false;
    }

    if (!m_game->step(fElapsed)) {
      log("This is game over", utils::Level::Info);
    }

    return m_game->terminated();
  }

  void
  App::onInputs(const controls::State& c,
                const CoordinateFrame& cf)
  {
    // Handle case where no game is defined.
    if (m_game == nullptr) {
      return;
    }

    // Handle menus update and process the
    // corresponding actions.
    std::vector<ActionShPtr> actions;
    bool relevant = false;

    for (unsigned id = 0u ; id < m_menus.size() ; ++id) {
      menu::InputHandle ih = m_menus[id]->processUserInput(c, actions);
      relevant = (relevant || ih.relevant);
    }

    if (m_state != nullptr) {
      menu::InputHandle ih = m_state->processUserInput(c, actions);
      relevant = (relevant || ih.relevant);
    }

    for (unsigned id = 0u ; id < actions.size() ; ++id) {
      actions[id]->apply(*m_game);
    }

    bool lClick = (c.buttons[controls::mouse::Left] == controls::ButtonState::Released);
    if (lClick && !relevant) {
      olc::vf2d it;
      olc::vi2d tp = cf.pixelCoordsToTiles(olc::vi2d(c.mPosX, c.mPosY), &it);

      m_game->performAction(tp.x + it.x, tp.y + it.y);
    }

    if (c.keys[controls::keys::P]) {
      m_game->togglePause();
    }
  }

  void
  App::loadData() {
    // Create the game and its state.
    m_game = std::make_shared<Game>();
  }

  void
  App::loadResources() {
    // Assign a specific tint to the regular
    // drawing layer so that we have a built
    // in transparency.
    // We can't do it directly when drawing
    // in the rendering function because as
    // the whole layer will be drawn as one
    // quad in opengl with an opaque alpha,
    // we will lose this info.
    // This means that everything is indeed
    // transparent but that's the only way
    // for now to achieve it.
    setLayerTint(Layer::Draw, olc::Pixel(255, 255, 255, alpha::SemiOpaque));

    log("Load app resources in the 'm_packs' attribute", utils::Level::Info);
  }

  void
  App::loadMenuResources() {
    // Generate the game state.
    m_state = std::make_shared<GameState>(
      olc::vi2d(ScreenWidth(), ScreenHeight()),
      Screen::Game
    );

    m_menus = m_game->generateMenus(ScreenWidth(), ScreenHeight());
  }

  void
  App::cleanResources() {
    if (m_packs != nullptr) {
      m_packs.reset();
    }
  }

  void
  App::cleanMenuResources() {
    m_menus.clear();
  }

  void
  App::drawDecal(const RenderDesc& res) {
    // Clear rendering target.
    SetPixelMode(olc::Pixel::ALPHA);
    Clear(olc::VERY_DARK_GREY);

    // In case we're not in the game screen, do nothing.
    if (m_state->getScreen() != Screen::Game) {
      SetPixelMode(olc::Pixel::NORMAL);
      return;
    }

    // Draw the board
    drawBoard(res);
    drawNumbers(res);

    SetPixelMode(olc::Pixel::NORMAL);
  }

  void
  App::draw(const RenderDesc& /*res*/) {
    // Clear rendering target.
    SetPixelMode(olc::Pixel::ALPHA);
    Clear(olc::Pixel(255, 255, 255, alpha::Transparent));

    // In case we're not in game mode, just render
    // the state.
    if (m_state->getScreen() != Screen::Game) {
      m_state->render(this);
      SetPixelMode(olc::Pixel::NORMAL);
      return;
    }

    SetPixelMode(olc::Pixel::NORMAL);
  }

  void
  App::drawUI(const RenderDesc& /*res*/) {
    // Clear rendering target.
    SetPixelMode(olc::Pixel::ALPHA);
    Clear(olc::Pixel(255, 255, 255, alpha::Transparent));

    // In case we're not in game mode, just render
    // the state.
    if (m_state->getScreen() != Screen::Game) {
      m_state->render(this);
      SetPixelMode(olc::Pixel::NORMAL);
      return;
    }

    // Render the game menus.
    for (unsigned id = 0u ; id < m_menus.size() ; ++id) {
      m_menus[id]->render(this);
    }

    SetPixelMode(olc::Pixel::NORMAL);
  }

  void
  App::drawDebug(const RenderDesc& res) {
    // Clear rendering target.
    SetPixelMode(olc::Pixel::ALPHA);
    Clear(olc::Pixel(255, 255, 255, alpha::Transparent));

    // In case we're not in game mode, just render
    // the state.
    if (m_state->getScreen() != Screen::Game) {
      m_state->render(this);
      SetPixelMode(olc::Pixel::NORMAL);
      return;
    }

    // Draw cursor's position.
    olc::vi2d mp = GetMousePos();
    olc::vf2d it;
    olc::vi2d mtp = res.cf.pixelCoordsToTiles(mp, &it);

    int h = GetDrawTargetHeight();
    int dOffset = 15;
    DrawString(olc::vi2d(0, h / 2), "Mouse coords      : " + toString(mp), olc::CYAN);
    DrawString(olc::vi2d(0, h / 2 + 1 * dOffset), "World cell coords : " + toString(mtp), olc::CYAN);
    DrawString(olc::vi2d(0, h / 2 + 2 * dOffset), "Intra cell        : " + toString(it), olc::CYAN);

    SetPixelMode(olc::Pixel::NORMAL);
  }

  void
  App::drawBoard(const RenderDesc& res) noexcept {
    // Draw the outer border.
    SpriteDesc sd = {};
    sd.loc = pge::RelativePosition::Center;
    sd.radius = 9.1f;

    sd.x = 4.5f;
    sd.y = 4.5f;

    sd.sprite.tint = olc::BLACK;

    drawRect(sd, res.cf);

    // Draw the cells.
    sd = {};
    sd.loc = pge::RelativePosition::Center;
    sd.radius = 8.9f;

    sd.x = 4.5f;
    sd.y = 4.5f;

    sd.sprite.tint = olc::WHITE;

    drawRect(sd, res.cf);

    // Draw the horizontal borders.
    olc::vf2d bigSz = olc::vf2d(9.1f, 0.1f) * res.cf.tileSize();
    olc::vf2d smallSz = olc::vf2d(9.1f, 0.05f) * res.cf.tileSize();
    for (unsigned y = 1u ; y < 9u ; ++y) {
      olc::vf2d sz = (y % 3u != 0u ? smallSz : bigSz);

      olc::vf2d p = res.cf.tileCoordsToPixels(0.45f, y + 0.5f, pge::RelativePosition::Center, 1.0f);

      FillRectDecal(p, sz, olc::BLACK);
    }

    // Draw the vertical borders.
    bigSz = olc::vf2d(0.1f, 9.1f) * res.cf.tileSize();
    smallSz = olc::vf2d(0.05f, 9.1f) * res.cf.tileSize();
    for (unsigned x = 1u ; x < 9u ; ++x) {
      olc::vf2d sz = (x % 3u != 0u ? smallSz : bigSz);

      olc::vf2d p = res.cf.tileCoordsToPixels(x + 0.5f, 0.45f, pge::RelativePosition::Center, 1.0f);

      FillRectDecal(p, sz, olc::BLACK);
    }
  }

  void
  App::drawNumbers(const RenderDesc& /*res*/) noexcept {

  }

}
