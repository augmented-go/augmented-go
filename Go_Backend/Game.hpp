#pragma once

#include "GoGame.h"

namespace GoBackend {

// possible states of the Game
enum class State {
    Valid,
    // in this state, the user has to restore the last valid state of the board
    // getBoard() will contain the last valid state
    Invalid,

    // basically the same as Invalid, but can only be reached through a capturing move
    WhileCapturing
};

class Game {

public:
    Game();

    // initializes the game board with given size and setup (starting positions)
    void init(int size, GoSetup setup = GoSetup());

    // tries to extract a valid move from the setup, ignores current player information in setup
    void update(GoSetup setup);

    const GoBoard& getBoard() const;
    State getState() const;

private:
    // Not implemented
    Game(const Game&);
    Game& operator=(const Game&);

private:
    // these functions are called on update with the current state matching
    // they may change the state
    void updateValid(SgPointSet added_blacks, SgPointSet added_whites, 
                     SgPointSet removed_blacks, SgPointSet removed_whites);
    //void updateWhileCapturing(SgPointSet added_blacks, SgPointSet added_whites, 
    //                          SgPointSet removed_blacks, SgPointSet removed_whites);
    void updateInvalid(GoSetup new_setup);

private:
    GoGame _go_game;
    State _current_state;
};

}