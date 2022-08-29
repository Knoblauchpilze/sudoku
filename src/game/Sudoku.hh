#ifndef    SUDOKU_HH
# define   SUDOKU_HH

# include <vector>
# include <unordered_set>
# include <memory>
# include <core_utils/CoreObject.hh>
# include "Board.hh"

namespace sudoku {

  /// @brief - The complexity of the game we are generating.
  enum class Level {
    Easy,
    Medium,
    Hard
  };

  class Game: public utils::CoreObject {
    public:

      /**
       * @brief - Create a new sudoku game with the specified
       *          difficulty level.
       * @param level - the difficulty level.
       */
      Game(const Level& level) noexcept;

      /**
       * @brief - The width of the board attached to this game.
       * @return - the width in cells.
       */
      unsigned
      w() const noexcept;

      /**
       * @brief - The height of the board attached to this game.
       * @return - the height in cells.
       */
      unsigned
      h() const noexcept;

      /**
       * @brief - Decay the game into its board component.
       * @return - the board used by this sudoku game.
       */
      const Board&
      operator()() const noexcept;

      /**
       * @brief - Clear any data contained in the board.
       */
      void
      clear() noexcept;

      /**
       * @brief - Initialize the board with a new game.
       */
      void
      initialize() noexcept;

      /**
       * @brief - Loads the content of the board defined in the
       *          input file and use it to replace the content
       *          of this board.
       * @param file - the file defining the board's data.
       */
      void
      load(const std::string& file);

      /**
       * @brief - Used to perform the saving of this board to the
       *          provided file.
       * @param file - the name of the file to save the board to.
       */
      void
      save(const std::string& file) const;

      /**
       * @brief - Attempt to put a number at a certain spot.
       * @param x - one of the coordinate where to put the digit.
       * @param y - one of the coordinate where to put the digit.
       * @param digit - the digit to put.
       * @param kind - the kind of digit to put.
       * @return - `true` if the digit could be put.
       */
      bool
      put(unsigned x,
          unsigned y,
          unsigned digit,
          const DigitKind& kind);

    private:

      /**
       * @brief - The current state of the board.
       */
      mutable Board m_board;

      /**
       * @brief - The difficulty level.
       */
      Level m_level;
  };

  using GameShPtr = std::shared_ptr<Game>;
}

#endif    /* SUDOKU_HH */
