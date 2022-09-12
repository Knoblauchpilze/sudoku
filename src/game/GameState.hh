#ifndef    GAME_STATE_HH
# define   GAME_STATE_HH

# include <memory>
# include <core_utils/CoreObject.hh>
# include "olcEngine.hh"
# include "Menu.hh"
# include "SavedGames.hh"

namespace pge {

  /// @brief - Convenience enumeration defining the current state
  /// of the application: each value roughly corresponds
  /// to a distinct menu in the application.
  enum class Screen {
    Home,
    ModeSelector,
    DifficultySelector,
    LoadGameSelector,
    LoadGame,
    Game,
    GameOver,
    Exit
  };

  class GameState: public utils::CoreObject {
    public:

      /**
       * @brief - Creates a new game state with the specified
       *          state.
       * @param dims - the dimensions of the desired UI.
       * @param screen - the current screen.
       * @param game - the game attached to this state.
       */
      GameState(const olc::vi2d& dims,
                const Screen& screen,
                Game& game);

      /**
       * @brief - Destructor to disconnect the signal from the
       *          saved games object.
       */
      ~GameState();

      /**
       * @brief - Retrieves the currently selected screen.
       * @return - the currently selected screen.
       */
      Screen
      getScreen() const noexcept;

      /**
       * @brief - Define a new active screen for this game.
       * @param screen - the new screen to apply.
       */
      void
      setScreen(const Screen& screen);

      /**
       * @brief - Used to render the screen corresponding to
       *          the current state using the provided game
       *          renderer.
       * @param pge - the engine to use to render the game
       *              state.
       */
      void
      render(olc::PixelGameEngine* pge) const;

      /**
       * @brief - Performs the interpretation of the controls
       *          provided as input to update the selected
       *          screen. Actions may be generated through
       *          this mechanism.
       * @param c - the controls and user input for this
       *            frame.
       * @param actions - the list of actions produced by the
       *                  menu while processing the events.
       * @return - the description of what happened when the
       *           inputs has been processed.
       */
      menu::InputHandle
      processUserInput(const controls::State& c,
                       std::vector<ActionShPtr>& actions);

      /**
       * @brief - Save the state of this game to a file named
       *          based on the existing files in the directory
       *          where saved games exist.
       */
      void
      save() const;

    private:

      /**
       * @brief - A slot used to receive notifications of a user
       *          picking a new saved game.
       * @param game - the path to the saved game that was picked.
       */
      void
      onSavedGamePicked(const std::string& game);

      void
      generateHomeScreen(const olc::vi2d& dims);

      void
      generateModeSelectorScreen(const olc::vi2d& dims);

      void
      generateDifficultySelectorScreen(const olc::vi2d& dims);

      void
      generateLoadGameScreen(const olc::vi2d& dims);

      void
      generateLoadGameSelectorScreen(const olc::vi2d& dims);

      void
      generateGameOverScreen(const olc::vi2d& dims);

    private:

      /**
       * @brief - Defines the current screen selected in this
       *          game. Updated whenever the user takes action
       *          to change it.
       */
      Screen m_screen;

      /**
       * @brief - Defines the screen to display when the game is
       *          on the home screen.
       */
      MenuShPtr m_home;

      /**
       * @brief - Defines the screen to display when the game is
       *          on the mode selector screen.
       */
      MenuShPtr m_modeSelector;

      /**
       * @brief - Defines the screen to display when the game is
       *          on the difficulty selector screen.
       */
      MenuShPtr m_difficultySelector;

      /**
       * @brief - Defines the screen to display when the game is
       *          on the loading game screen.
       */
      MenuShPtr m_loadGame;

      /**
       * @brief - If not empty the string corresponding to the game
       *          to load.
       */
      std::string m_gameToLoad;

      /**
       * @brief - Defines the screen to display when the game is
       *          on picking a mode for the loading of a game.
       */
      MenuShPtr m_loadGameModeSelector;

      /**
       * @brief - The data needed to represent the list of games
       *          available for loading.
       */
      SavedGames m_savedGames;

      /**
       * @brief - Defines the menu to display in case
       *          the game is over.
       */
      MenuShPtr m_gameOver;

      /**
       * @brief - The game attached to this state.
       */
      Game& m_game;
  };

  using GameStateShPtr = std::shared_ptr<GameState>;
}

#endif    /* GAME_STATE_HH */
