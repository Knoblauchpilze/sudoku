#ifndef    BOARD_HH
# define   BOARD_HH

# include <vector>
# include <memory>
# include <core_utils/CoreObject.hh>

namespace sudoku {

  class Board: public utils::CoreObject {
    public:

      /**
       * @brief - Create a new 9x9 sudoku board.
       */
      Board() noexcept;

      /**
       * @brief - The width of the board.
       * @return - the width of the board.
       */
      unsigned
      w() const noexcept;

      /**
       * @brief - The height of the board.
       * @return - the height of the board.
       */
      unsigned
      h() const noexcept;

      /**
       * @brief - Whether or not the position at the specified coords
       *          is empty.
       * @param x - the x coordinates.
       * @param y - the y coordinates.
       * @return - `true` if the cell is empty.
       */
      bool
      empty(unsigned x, unsigned y) const;

      /**
       * @brief - Returns the number at the specified position or zero
       *          in case the cell is empty.
       * @param x - the x coordinates.
       * @param y - the y coordinates.
       * @return - the number at this place.
       */
      unsigned
      at(unsigned x, unsigned y) const;

      /**
       * @brief - Allow to determine whether or not the input number
       *          can fit at the specified location.
       * @param x - the input coordinate.
       * @param y - the input coordinate.
       * @param digit - the digit to check.
       * @return - `true` if the input digit can fit.
       */
      bool
      canFit(unsigned x, unsigned y, unsigned digit) const;

      /**
       * @brief - Put a number at a certain spot.
       * @param x - one of the coordinate where to put the digit.
       * @param y - one of the coordinate where to put the digit.
       * @param digit - the digit to put.
       */
      void
      put(unsigned x, unsigned y, unsigned digit);

      /**
       * @brief - Reset all tiles to be empty.
       */
      void
      reset() noexcept;

      /**
       * @brief - Perform the generation of the game with a certain
       *          amount of digits left in the end. In case the number
       *          of digits is not valid (meaning that we can't find
       *          a way to keep only the desired number of digits), the
       *          return value will be false and the board will be left
       *          in its previous state.
       * @param digits - the number of digits to leave on the board.
       * @return - `true` if the game could be generated.
       */
      bool
      generate(unsigned digits) noexcept;

      /**
       * @brief - Used to perform the saving of this board to the
       *          provided file.
       * @param file - the name of the file to save the board to.
       */
      void
      save(const std::string& file) const;

      /**
       * @brief - Loads the content of the board defined in the
       *          input file and use it to replace the content
       *          of this board.
       * @param file - the file defining the board's data.
       */
      void
      load(const std::string& file);

    private:

      unsigned
      linear(unsigned x, unsigned y) const noexcept;

    private:

      /**
       * @brief - The width of the board.
       */
      unsigned m_width;

      /**
       * @brief - The height of the board.
       */
      unsigned m_height;

      /**
       * @brief - The current state of the board.
       */
      mutable std::vector<unsigned> m_board;
  };

  using BoardShPtr = std::shared_ptr<Board>;
}

#endif    /* BOARD_HH */
