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
    _current_state(State::Valid),
    _game_finished(false)
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

    if (setup.IsEmpty()) {
        _current_state = State::SettingHandicap;
    }
    else {
        _current_state = State::Valid;
    }

    _game_finished = false;

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

    // extract new played stones with respect to the current board
    auto added_blacks = new_blacks - current_blacks;
    auto added_whites = new_whites - current_whites;

    auto removed_blacks = current_blacks - new_blacks;
    auto removed_whites = current_whites - new_whites;


    switch (_current_state) {
    case State::Valid:
        updateValid(added_blacks, added_whites, removed_blacks, removed_whites);
        break;

    case State::SettingHandicap:
        updateSettingHandicap(setup);
        break;

    case State::Invalid:
        updateInvalid(setup);
        break;

    case State::WhileCapturing:
        updateInvalid(setup);
        break;

    default:
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

void Game::updateSettingHandicap(GoSetup new_setup) {
    auto blacks = new_setup.m_stones[SG_BLACK];
    auto whites = new_setup.m_stones[SG_WHITE];

    // the handicap is placed when the first white stone gets added
    if (!whites.IsEmpty()) {
        _current_state = State::Valid;

        if (blacks.Size() == 1) {
            // only a single black stone played and is therefore not a handicap stone

            GoSetup black_move_setup;
            black_move_setup.AddBlack(blacks.PointOf());
            // play the black stone just like any other move
            update(black_move_setup);
        }
        else if (blacks.Size() == 0) {
            // no handicap, but white stone played
            // let the update(new_setup) call handle this illegal move
        }
        else {
            SgVector<SgPoint> handicap_stones;
            blacks.ToVector(&handicap_stones);
            _go_game.PlaceHandicap(handicap_stones);
        }

        // play the added white stone just like any other move
        update(new_setup);
    }
}

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

        _game_finished = true;
    }
}

void Game::resign() {
    auto current_player = getBoard().ToPlay();

    // adding a resign comment in the sgf structure
    _go_game.AddResignNode(current_player);

    // sgf: RE: Result: result, usually in the format "B+R" (Black wins by resign)
    auto result = std::string("") + (current_player == SG_BLACK ? "W" : "B") + "+R";   
    _go_game.UpdateResult(result);

    _game_finished = true;
}

std::string Game::getResult() const {
    if (_go_game.GetResult() != "")
        return _go_game.GetResult();
    else
        return "";
}

bool Game::hasEnded() const {
    return _game_finished;
}

} // 
