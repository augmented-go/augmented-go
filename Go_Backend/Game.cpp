#include "Game.hpp"

namespace GoBackend {
Game::Game()
    : _go_game()
{}

void Game::init(int size, GoSetup setup) { 
    // assert valid board size
    assert(size < 20 && size > 1);

    _go_game.Init(size, GoRules());

    // fuego bastards, a SgBWArray<SgPointSet> is essentially the same as a SgBWSet
    // but SetupPosition wants a SbBWArray...
    _go_game.SetupPosition(SgBWArray<SgPointSet>(setup.m_stones[SG_BLACK], setup.m_stones[SG_WHITE]));
}

const GoBoard& Game::getBoard() const {
    return _go_game.Board();
}
}