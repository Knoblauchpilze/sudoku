#ifndef    GAME_HH
# define   GAME_HH

# include <vector>
# include <memory>
# include <core_utils/CoreObject.hh>
# include <core_utils/TimeUtils.hh>
# include "Sudoku.hh"

namespace pge {

  // Forward declaration of the `Menu` class to be able
  // to use it right away.
  class Menu;
  using MenuShPtr = std::shared_ptr<Menu>;

  /// @brief - The mode for the game: either solver or a
  /// mode where the user can play.
  enum class Mode {
    Solver,
    Interactive
  };

  class Game: public utils::CoreObject {
    public:

      /**
       * @brief - Create a new game with default parameters.
       */
      Game();

      ~Game();

      /**
       * @brief - Used to perform the creation of the menus
       *          allowing to control the world wrapped by
       *          this game.
       * @param width - the zidth of the window in pixels into
                        which this menu will be inserted.
       * @param height - the height of the window in pixels
       *                 into which this menu will be inserted.
       * @return - the list of menus representing this game.
       */
      virtual std::vector<MenuShPtr>
      generateMenus(float width,
                    float height);

      /**
       * @brief - Used to create a tower with the specified
       *          type (as defined by the `setTowerType`)
       *          method at the specified position. Note
       *          that we will only perform the action if
       *          no elements exist here.
       *          Also note that the coordinates are used as
       *          is and should thus correspond to values that
       *          interpretable by the underlying game data.
       * @param x - the abscissa of the position at which the
       *            action should be taking place.
       * @param y - the ordinate of the position at which the
       *            action will be taking place.
       * @param erase - `true` if the action is an erase action.
       */
      void
      performAction(float x, float y, bool erase);

      /**
       * @brief - Requests the game to be terminated. This is
       *          applied to the next iteration of the game
       *          loop.
       */
      void
      terminate() noexcept;

      /**
       * @brief - Requests the game to be finished. This is applied
       *          to the next iteration of the game loop and can be
       *          used to trigger a 'back-to-main-menu' operation.
       */
      void
      finish() noexcept;

      /**
       * @brief - Returns whether or not the game has been
       *          terminated. The game is terminated when
       *          the user wants to exit the app (usually).
       */
      bool
      terminated() const noexcept;

      /**
       * @brief - Forward the call to step one step ahead
       *          in time to the internal world.
       * @param tDelta - the duration of the last frame in
       *                 seconds.
       * @param bool - `true` in case the game continues,
       *               and `false` otherwise (i.e. if the
       *               game is ended).
       */
      bool
      step(float tDelta);

      /**
       * @brief - Performs the needed operation to handle
       *          the pause and resume operation for this
       *          game. It will automatically disable the
       *          menu if needed or make it visible again.
       */
      void
      togglePause();

      /**
       * @brief - Used to indicate that the world should be
       *          paused. Time based entities and actions
       *          should take actions to correctly resume at
       *          a later time.
       */
      void
      pause();

      /**
       * @brief - Used to indicate that the world should be
       *          resuming its activity. Time based entities
       *          should take actions to be resuming their
       *          pathes, motions, etc.
       */
      void
      resume();

      /**
       * @brief - Set the current mode of the game. Will update
       *          the UI based on the input value.
       * @param mode - the new mode.
       */
      void
      setMode(const Mode& mode) noexcept;

      /**
       * @brief - Reset the game to a new one.
       */
      void
      reset();

      /**
       * @brief - Clear any data in the board.
       */
      void
      clear();

      /**
       * @brief - Returns the board attached to this game.
       * @return - the board attached to this game.
       */
      const sudoku::Board&
      board() const noexcept;

      /**
       * @brief - Loads the board defined in the input file.
       * @param file - the file to use to load a board from.
       */
      void
      load(const std::string& file);

      /**
       * @brief - Save the current state of the board to a default
       *          file with the name provided in input.
       * @param file - the file to save the board into.
       */
      void
      save(const std::string& file) const;

      /**
       * @brief - Called to notify the current highlighted cell.
       */
      void
      setActiveCell(float x, float y);

      /**
       * @brief - Reset the active cell.
       */
      void
      resetActiveCell();

      /**
       * @brief - Used to notify that a digit has been pressed
       *          by the user.
       * @param digit - the value of the pressed digit.
       */
      void
      onDigitPressed(unsigned digit);

      /**
       * @brief - Defines a new difficulty level for the game.
       *          This will reset the current grid.
       * @param level - the new difficulty level.
       */
      void
      setDifficultyLevel(const sudoku::Level& level);

      /**
       * @brief - Attempts to solve the sudoku in its current
       *          state.
       */
      void
      solve();

    private:

      /**
       * @brief - Used to enable or disable the menus that
       *          compose the game. This allows to easily
       *          hide any game related component.
       * @param enable - `true` if the menus are enabled.
       */
      void
      enable(bool enable);

      /**
       * @brief - Used during the step function and by any process
       *          that needs to update the UI and the text content
       *          of menus.
       */
      virtual void
      updateUI();

      /**
       * @brief - Used to update the UI in case of the interactive
       *          mode. We assume that the visibility status is
       *          already set.
       */
      void
      updateUIForInteractive();

      /**
       * @brief - Used to update the UI in case of the solver mode.
       *          We assume that the visibility status is already
       *          set.
       */
      void
      updateUIForSolver();

    private:

      /// @brief - Convenience structure allowing to group information
      /// about a timed menu.
      struct TimedMenu {
        // Information about when the menu started appearing.
        utils::TimeStamp date;

        // Keep track of whether the menu was already active.
        bool wasActive;

        // The alert menu indicating controlled by this object.
        MenuShPtr menu;

        // The duration of the alert.
        int duration;

        /**
         * @brief - Used to update the internal attribute with
         *          the current value of whether the menu should
         *          be active or not.
         * @param active - `true` if the menu should still be
         *                 active.
         * @return - `true` if the menu is still visible.
         */
        bool
        update(bool active) noexcept;
      };

      /// @brief - An enumeration allowing to determine whether the
      /// player already requested the sudoku to be solved or not.
      enum class SolverStep {
        None,
        Preparing,
        Solving,
        Solved,
        Unsolvable,
      };

      /// @brief - Convenience information defining the state of the
      /// game. It includes information about whether the menus should
      /// be displayed and if the user actions should be interpreted
      /// or not.
      struct State {
        // Defines whether this world is paused (i.e.
        // internal attributes of the mobs/blocks/etc
        // have already been updated to reflect the
        // pause status) or not. This allows to react
        // to consecutive pause requests and prevent
        // weird behaviors to occur.
        bool paused;

        // Whether or not the UI is disabled.
        bool disabled;

        // Used to hold whether or not the game has been shut
        // down. It usually indicates that no simulation will
        // be performed anymore and usually indicates that a
        // termination request has been received.
        bool terminated;

        // The current mode attached to the game.
        Mode mode;

        // The current solver step. Only relevant in case the
        // general mode of the game is set to `Solver`.
        SolverStep solverStep;

        // Whether or not the game has been finished.
        bool done;
      };

      /// @brief - Convenience structure allowing to regroup
      /// all info about the menu in a single struct.
      struct Menus {
        // The menus holding the remaining digits count to find.
        std::vector<MenuShPtr> digits;

        // The status menu for the digits to find.
        MenuShPtr status;

        // The hint menu for the hint available for a slot.
        MenuShPtr hint;

        // The solve button for the solver mode.
        MenuShPtr solve;

        // The back button to return to the mode selection from
        // the solver mode.
        MenuShPtr back;

        // The alert menu indicating that the sudoku was solved.
        TimedMenu solvedAlert;

        // The alert menu indicating the the sudoku couldn't be
        // solved.
        TimedMenu unsolvableAlert;
      };

      /// @brief - Convenience structure registering the properties
      /// used for the display of hints.
      struct HintData {
        // The abscissa of the active cell.
        int x;

        // The ordinate of the active cell.
        int y;

        // The digit to put on a click: cycles from 1 to 9.
        unsigned digit;

        // Since when the cell is active.
        utils::TimeStamp since;

        // Whether or not the menu is allowed or not.
        bool active;

        // The menus representing the hints.
        std::vector<MenuShPtr> menus;
      };

      /**
       * @brief - The definition of the game state.
       */
      State m_state;

      /**
       * @brief - The menus displaying information about the
       *          current state of the simulation.
       */
      Menus m_menus;

      /**
       * @brief - The board managed by this game.
       */
      sudoku::GameShPtr m_board;

      /**
       * @brief - The required data to maintain the active cell
       *          and the hints.
       */
      HintData m_hint;
  };

  using GameShPtr = std::shared_ptr<Game>;
}

# include "Game.hxx"

#endif    /* GAME_HH */
