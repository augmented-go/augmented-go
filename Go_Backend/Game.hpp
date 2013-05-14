#pragma once

#include "GoGame.h"

namespace GoBackend {

class Game {

public:
    Game();

    // initializes the game board with given size and setup (starting positions)
    void init(int size, GoSetup setup = GoSetup());

    const GoBoard& getBoard() const;

private:
    // Not implemented
    Game(const Game&);
    Game& operator=(const Game&);
    
private:
    GoGame _go_game;
};

}