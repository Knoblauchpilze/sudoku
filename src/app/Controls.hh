#ifndef    CONTROLS_HH
# define   CONTROLS_HH

# include <vector>

namespace pge {
  namespace controls {

    namespace keys {

      /// @brief - The list of keys that are trackes by the
      /// controls structure.
      enum Keys {
        Right,
        Up,
        Left,
        Down,

        Space,

        P,
        S,

        Zero,
        One,
        Two,
        Three,
        Four,
        Five,
        Six,
        Seven,
        Eight,
        Nine,

        Del,

        KeysCount
      };

    }

    namespace mouse {

      /// @brief - The possible mouse buttons recognized for
      /// controls.
      enum MouseButton {
        Left,
        Middle,
        Right,

        ButtonsCount
      };

    }

    /// @brief - The possible states for a button. Note
    /// that the `Pressed` state state is only active
    /// once when the button is first pressed. Similarly
    /// the `Released` state is only active once when
    /// the button is first released.
    enum class ButtonState {
      Free,
      Released,
      Pressed,
      Held
    };

    /// @brief - Describe a structure holding the controls
    /// that are relevant extracted from the keys pressed
    /// by the user and the mouse info.
    struct State {
      // The position of the mouse along the x coordinates.
      int mPosX;

      // The position of the mouse along the y coordinates.
      int mPosY;

      // The current state of the keys.
      std::vector<bool> keys;

      // The current state of the mouse buttons.
      std::vector<ButtonState> buttons;

      // Whether the tab key is pressed.
      bool tab;
    };

    /**
     * @brief - Create a new controls structure.
     * @return - the created structure.
     */
    State
    newState() noexcept;

  }
}

# include "Controls.hxx"

#endif    /* CONTROLS_HH */
