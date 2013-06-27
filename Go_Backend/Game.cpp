#include "Game.hpp"

#include <fstream>
#include <iostream>
#include <utility>
#include <sstream>

#include <boost/date_time/gregorian/gregorian.hpp>

#include "GoSetupUtil.h"
#include "SgGameWriter.h"
#include "SgProp.h"

namespace GoBackend {
Game::Game()
    : _go_game(),
    _current_state(State::Valid)
{}

bool Game::validSetup(const GoSetup& setup) const {
    if (!allValidPoints(setup.m_stones[SG_BLACK])
        || !allValidPoints(setup.m_stones[SG_WHITE]))
        return false;
    return true;
}

bool Game::allValidPoints(const SgPointSet& stones) const {
    auto& board = getBoard();

    // check all stones
    for (auto iter = SgSetIterator(stones); iter; ++iter) {
        auto point = *iter;

        if (!board.IsValidPoint(point))
            return false;
    }

    return true;
}

bool Game::init(int size, GoSetup setup) {
    // assert valid board size
    assert(size < 20 && size > 1);

    auto rules = GoRules(0,             // handicap
                         GoKomi(6.5),   // komi
                         true,          // japanese scoring
                         true);         // two passes end a game

    _go_game.Init(size, rules);

    // check if setup contains only valid stones!
    if (!validSetup(setup)) {
        std::cout << __TIMESTAMP__ << "[" << __FUNCTION__ << "] " << " GoSetup contains invalid stones! Skipping..." << std::endl;
        return false;
    }

    // a SgBWArray<SgPointSet> is essentially the same as a SgBWSet
    // but SetupPosition wants a SbBWArray...
    _go_game.SetupPosition(SgBWArray<SgPointSet>(setup.m_stones[SG_BLACK], setup.m_stones[SG_WHITE]));

    _current_state = State::Valid;
    return true;
}

const GoBoard& Game::getBoard() const {
    return _go_game.Board();
}

State Game::getState() const {
    return _current_state;
}

void Game::update(GoSetup setup) {
    // check if setup contains only valid stones!
    if (!validSetup(setup)) {
        std::cout << __TIMESTAMP__ << " [" << __FUNCTION__ << "] " << " GoSetup contains invalid stones! Skipping..." << std::endl;

        // "silenty" skip this update (besides the debug output)
        return;
    }

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

bool Game::saveGame(string file_path, string name_black, string name_white, string game_name) {
    using boost::gregorian::day_clock;
    using boost::gregorian::to_simple_string;

    std::ofstream file(file_path.c_str());
    if (!file.is_open()) {
        return false;
    }
    
    if (!name_black.empty())
        _go_game.UpdatePlayerName(SG_BLACK, name_black);
    if (!name_white.empty())
        _go_game.UpdatePlayerName(SG_WHITE, name_white);
    if (!game_name.empty())
        _go_game.UpdateGameName(game_name);

    // current date
    string date = to_simple_string(day_clock::local_day());
    _go_game.UpdateDate(date);

    SgGameWriter writer(file);

    bool all_props = true; // all properties like player names and game name
    int file_format = 0; // default file format
    int game_number = SG_PROPPOINTFMT_GO; // the game of go
    int default_size = 19; // default boardsize, never actually relevant as _go_game.Init gets always called
    writer.WriteGame(_go_game.Root(), all_props, file_format, game_number, default_size);

    return true;
}

std::string Game::finishGame() {
    pass();
    pass();

    return getResult();
}

void Game::pass() {
    _go_game.AddMove(SG_PASS, getBoard().ToPlay());

    // update result if the game ended with the second pass
    if (_go_game.EndOfGame()) {
        // get score and update result
        float score = FLT_MIN;
        auto score_successful = GoBoardUtil::ScorePosition(getBoard(), SgPointSet(), score);

        if (score_successful) {
            if (score == 0) {
                // this is a draw
                _go_game.UpdateResult("0");
            }
            else {
                // convert float score to string
                std::ostringstream stream;
                stream.precision(3);
                stream << std::abs(score);

                // a negative score means that black lost
                // sgf: RE: Result: result, usually in the format "B+3.5" (black wins by 3.5 moku). 
                auto result = std::string("") + (score < 0 ? "W" : "B") + "+" + stream.str();
                _go_game.UpdateResult(result);
            }
        }
        else {
            _go_game.UpdateResult("Couldn't score the board.");
        }
    }
}

void Game::resign() {
    auto current_player = getBoard().ToPlay();

    // adding a resign comment in the sgf structure
    _go_game.AddResignNode(current_player);

    // sgf: RE: Result: result, usually in the format "B+R" (Black wins by resign)
    auto result = std::string("") + (current_player == SG_BLACK ? "W" : "B") + "+R";   
    _go_game.UpdateResult(result);
}

std::string Game::getResult() const {
    if (_go_game.GetResult() != "")
        return _go_game.GetResult();
    else
        return "";
}

} // 
