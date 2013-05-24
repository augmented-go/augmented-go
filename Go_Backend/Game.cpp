#include "Game.hpp"

#include "GoSetupUtil.h"

namespace GoBackend {
Game::Game()
    : _go_game(),
    _current_state(State::Valid)
{}

void Game::init(int size, GoSetup setup) {
    // assert valid board size
    assert(size < 20 && size > 1);

    _go_game.Init(size, GoRules());

    // @Todo: check if setup contains only valid stones!

    // a SgBWArray<SgPointSet> is essentially the same as a SgBWSet
    // but SetupPosition wants a SbBWArray...
    _go_game.SetupPosition(SgBWArray<SgPointSet>(setup.m_stones[SG_BLACK], setup.m_stones[SG_WHITE]));

    _current_state = State::Valid;
}

const GoBoard& Game::getBoard() const {
    return _go_game.Board();
}

State Game::getState() const {
    return _current_state;
}

void Game::update(GoSetup setup) {
    // @Todo: check if setup contains only valid stones!

    // get new and current stones
    auto new_blacks = setup.m_stones[SG_BLACK];
    auto new_whites = setup.m_stones[SG_WHITE];

    auto current_blacks = getBoard().All(SG_BLACK);
    auto current_whites = getBoard().All(SG_WHITE);

    if (_current_state == State::Valid) {
        // extract new played stones with respect to the current board
        auto added_blacks = new_blacks - current_blacks;
        auto added_whites = new_whites - current_whites;

        auto removed_blacks = current_blacks - new_blacks;
        auto removed_whites = current_whites - new_whites;
        updateValid(added_blacks, added_whites, removed_blacks, removed_whites);
    }
    else if (_current_state == State::WhileCapturing) {
        updateInvalid(setup);
    }
    else if (_current_state == State::Invalid) {
        updateInvalid(setup);
    }
    else {
        assert(false);
    }
}

void Game::updateValid(SgPointSet added_blacks, SgPointSet added_whites, SgPointSet removed_blacks, SgPointSet removed_whites) {
    SgPoint point;
    SgBlackWhite player;

    // check if nothing has changed
    if (added_blacks.IsEmpty() && added_whites.IsEmpty() && 
        removed_blacks.IsEmpty() && removed_whites.IsEmpty())
        return;

    if (!removed_blacks.IsEmpty() || !removed_whites.IsEmpty()) {
         // removing stones is invalid in this state
        _current_state = State::Invalid;
        return;
    }

    if (added_blacks.Size() == 1 && added_whites.Size() == 0) {
        // blacks move
        point = added_blacks.PointOf();
        player = SG_BLACK;
    }
    else if (added_blacks.Size() == 0 && added_whites.Size() == 1) {
        // whites move
        point = added_whites.PointOf();
        player = SG_WHITE;
    }
    else {
        // not a single stone difference
        _current_state = State::Invalid;
        return;
    }

    if (getBoard().ToPlay() == player && getBoard().IsLegal(point, player)) {
        _go_game.AddMove(point, player);
        // handle capturing moves
        const GoPointList& captured = getBoard().CapturedStones();
        if (getBoard().CapturingMove()) {
            _current_state = State::WhileCapturing;
        }
    }
    else {
        // illegal move or not players turn
        _current_state = State::Invalid;
    }
}

//void Game::updateWhileCapturing(SgPointSet added_blacks, SgPointSet added_whites, SgPointSet removed_blacks, SgPointSet removed_whites) {
//    // the pointsets are calculated with respect to the board where the capturing move has already been played,
//    // the board would be in a invalid state otherwise
//    // this means, added_black or added_whites are the stones that are still to capture
//
//    // example:
//    // internal state after capturingmove at B1:
//    // ....
//    // X...
//    // .X..
//    // .X..
//    // received through update:
//    // ....
//    // X...
//    // .X..
//    // OX..
//    // this means added_whites contains A1 and is still to capture
//
//    const GoPointList& captured_list = getBoard().CapturedStones();
//    assert(!captured_list.IsEmpty());
//    SgPointSet to_capture;
//    for(GoPointList::Iterator it(captured_list); it; ++it)
//        to_capture.Include(*it);
//
//    if (!removed_blacks.IsEmpty() && !removed_whites.IsEmpty()) {
//        assert(!"not impl");
//        _current_state = State::Invalid;
//    }
//
//    // the player who got his stones captured has the current turn
//    SgBlackWhite to_play = getBoard().ToPlay();
//
//    if (added_blacks.Size() == 0 && added_whites.Size() != 0) {
//        // black stones are still to capture
//    }
//    else if (added_blacks.Size() != 0 && added_whites.Size() == 0) {
//        // white stones are still to capture
//    }
//    else { // both empty
//
//    }
//
//}

void Game::updateInvalid(GoSetup new_setup) {
    auto board_setup = GoSetupUtil::CurrentPosSetup(getBoard());

    // the player of the setup is ignored
    new_setup.m_player = board_setup.m_player;

    if (new_setup == board_setup) {
        // the last valid state has been restored
        _current_state = State::Valid;
    }
}

}
