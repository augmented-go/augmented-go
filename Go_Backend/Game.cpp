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

void Game::update(GoSetup setup) {
    // get new and current stones
    auto new_blacks = setup.m_stones[SG_BLACK];
    auto new_whites = setup.m_stones[SG_WHITE];

    auto current_blacks = getBoard().All(SG_BLACK);
    auto current_whites = getBoard().All(SG_WHITE);

    // check if the setup is the same as the current board
    if (new_blacks == current_blacks
        && new_whites == current_whites)
        // no update needed
        return;

    // extract new played stones with respect to the current board
    auto added_blacks = new_blacks - current_blacks;
    auto added_whites = new_whites - current_whites;

    auto removed_blacks = current_blacks - new_blacks;
    auto removed_whites = current_whites - new_whites;

    if (added_blacks.Size() == 1 && added_whites.Size() == 0) {
        // blacks move
        auto point = added_blacks.PointOf();

        _go_game.AddMove(point, SG_BLACK);
    }
    else if (added_blacks.Size() == 0 && added_whites.Size() == 1) {
        // whites move
        auto point = added_whites.PointOf();

        _go_game.AddMove(point, SG_WHITE);
    }
    else {
        assert(!"Neither black nor white move?!");
    }
}
}