#include "Game.hpp"

#include <fstream>
#include <iostream>
#include <utility>
#include <sstream>

#include <boost/date_time/gregorian/gregorian.hpp>

#include "GoSetupUtil.h"
#include "SgGameWriter.h"
#include "SgGameReader.h"
#include "SgProp.h"
#include "GoModBoard.h"

namespace GoBackend {
Game::Game()
    : _go_game(),
      _game_finished(false),
      _while_capturing(false)
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

bool Game::init(int size, GoSetup setup, GoRules rules) {
    // assert valid board size
    assert(size < 20 && size > 1);
    assert(rules.Handicap() == 0);

    _go_game.Init(size, rules);

    // check if setup contains only valid stones!
    if (!validSetup(setup)) {
        std::cout << __TIMESTAMP__ << "[" << __FUNCTION__ << "] " << " GoSetup contains invalid stones! Skipping..." << std::endl;
        return false;
    }

    if (setup.m_stones[SG_WHITE].Size() == 0) {
        // no or only black stones, consider them to be handicap stones
        placeHandicap(setup);
    }
    else {
        // a SgBWArray<SgPointSet> is essentially the same as a SgBWSet
        // but SetupPosition wants a SbBWArray...
        _go_game.SetupPosition(SgBWArray<SgPointSet>(setup.m_stones[SG_BLACK], setup.m_stones[SG_WHITE]));
    }

    _game_finished = false;
    _while_capturing = false;

    return true;
}

const GoBoard& Game::getBoard() const {
    return _go_game.Board();
}


UpdateResult Game::update(GoSetup setup) {
    // check if setup contains only valid stones!
    if (!validSetup(setup)) {
        std::cout << __TIMESTAMP__ << " [" << __FUNCTION__ << "] " << " GoSetup contains invalid stones! Skipping..." << std::endl;
        return UpdateResult::Illegal;
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


    // handicap
    if (isPlacingHandicap(current_blacks, current_whites, new_whites)) {
        assert(_while_capturing == false);
        placeHandicap(setup);
        return UpdateResult::Legal;
    }


    if (_while_capturing) {
        assert(getBoard().CapturingMove());
        return updateWhileCapturing(setup);
    }
    else {
        return updateNormal(added_blacks, added_whites, removed_blacks, removed_whites);
    }
}



UpdateResult Game::updateNormal(SgPointSet added_blacks, SgPointSet added_whites, SgPointSet removed_blacks, SgPointSet removed_whites) {
    assert(_while_capturing == false);

    if (added_blacks.IsEmpty() && added_whites.IsEmpty()) {
        if (removed_blacks.IsEmpty() && removed_whites.IsEmpty()) {
            return UpdateResult::Legal;
        }
        else {
            // just removing stones from the board is illegal
            // capturing is not covered here
            return UpdateResult::Illegal;
        }
    }
    else if (added_blacks.Size() == 1 && added_whites.Size() == 0) {
        // black move
        return updateSingleMove(added_blacks.PointOf(), SG_BLACK, removed_blacks, removed_whites);
    }
    else if (added_blacks.Size() == 0 && added_whites.Size() == 1) {
        // white move
        return updateSingleMove(added_whites.PointOf(), SG_WHITE, removed_whites, removed_blacks);
    }
    else {
        // more than a single stone added
        return UpdateResult::Illegal;
    }
}

UpdateResult Game::updateWhileCapturing(GoSetup new_setup) {
    auto board_setup = GoSetupUtil::CurrentPosSetup(getBoard());

    // the player of the setup is ignored
    new_setup.m_player = board_setup.m_player;

    if (new_setup == board_setup) {
        // all stones that are to capture have been removed from the board
        _while_capturing = false;
        return UpdateResult::Legal;
    }
    else {
        // real life board dosn't match internal state
        return UpdateResult::Illegal;
    }
}

UpdateResult Game::updateSingleMove(SgPoint point, SgBlackWhite player, SgPointSet removed_of_player, SgPointSet removed_of_opponent) {
    if (getBoard().MoveNumber() == 0 && getBoard().Rules().Handicap() == 0 && !getBoard().Setup().IsEmpty()) {
        // if no move has been played yet and the setup the board got is not empty, we allow moves from any player
        // the board should also not have a handicap, this is handled elsewhere

        // even when this move turns out to be illegal (removed stones etc), changing the current player won't hurt, 
        // as we still stay in the "any player can play" state
        _go_game.SetToPlay(player);
    }

    if (getBoard().ToPlay() != player || !getBoard().IsLegal(point, player)) {
        // illegal move by game rules
        return UpdateResult::Illegal;
    }
    
    SgPointSet captured_stones = possibleCapturedStones(getBoard(), point);
    if (captured_stones.IsEmpty()) {
        // no capture

        if (removed_of_player.IsEmpty() && removed_of_opponent.IsEmpty()) {
            // completely valid move
            _go_game.AddMove(point, player);
            return UpdateResult::Legal;
        }
        else {
            // played a valid move, but stones have been removed
            return UpdateResult::Illegal;
        }
    }
    else {
        // captured some stones

        if (!removed_of_player.IsEmpty()) {
            // only the enemy's stones can be captured
            return UpdateResult::Illegal;
        }

        if (removed_of_opponent == captured_stones) {
            // all stones that are to capture have already been removed
            _go_game.AddMove(point, player);
            return UpdateResult::Legal;
        }
        else if (removed_of_opponent.IsEmpty() || removed_of_opponent.SubsetOf(captured_stones)) {
            // legal capturing move
            _go_game.AddMove(point, player);

            // some stones may have already been removed after playing the move,
            // but there are still stones left to be removed, tell the user to remove them as well
            _while_capturing = true;
            return UpdateResult::Illegal;
        }
        else {
            // stones that are not beeing captured have been removed
            return UpdateResult::Illegal;
        }
    }
}

SgPointSet Game::possibleCapturedStones(const GoBoard& const_board, SgPoint move) {
    // makes the const_board modifiable, but asserts that the state has been restored when getting deleted
    GoModBoard mod_board(const_board);
    GoBoard& board = mod_board.Board();

    assert(board.IsLegal(move));
    // try playing to see if this move captured any stones
    board.Play(move);
    const GoPointList captured_list = mod_board.Board().CapturedStones();
    board.Undo();
    
    // convert to other datatype
    SgPointSet captured_set;
    for (auto it = GoPointList::Iterator(captured_list); it; ++it) {
        captured_set.Include(*it);
    }
    return captured_set;
}

bool Game::isPlacingHandicap(SgPointSet current_blacks, SgPointSet current_whites, SgPointSet new_whites) {
    // only allow placing handicap when no move has been played
    bool no_moves_played = getBoard().MoveNumber() == 0;

    // this gets the case where we play a move instead of placing the handicap on the very first black stone
    // see placeHandicap()
    bool only_one_black_move = getBoard().MoveNumber() == 1 && current_blacks.Size() == 1;
    
    // only black stones (now whites) and no move played
    return current_whites.IsEmpty() && new_whites.IsEmpty() && (no_moves_played || only_one_black_move);
}

void Game::placeHandicap(GoSetup new_setup) {
    auto previous_blacks = getBoard().All(SG_BLACK);
    auto blacks = new_setup.m_stones[SG_BLACK];
    auto whites = new_setup.m_stones[SG_WHITE];
    assert(whites.IsEmpty());

    if (!blacks.IsEmpty() && blacks.Size() != previous_blacks.Size()) {
        // the GoGame class only allows adding handicap stones all at once
        // when getting a new handicap stone, the GoGame is therefore reset and 
        // and all handicap stones ar added anew

        // the PlaceHandicap() call also increases the handicap inside the rules of the board,
        // these would then be placed automatically by the Init() call, we don't want that
        GoRules rules = getBoard().Rules();
        rules.SetHandicap(0);
        _go_game.Init(getBoard().Size(), rules);

        if (blacks.Size() == 1) {
            // only a single black stone played and is therefore not a handicap stone
            // play the black stone just like a regular move
            assert(getBoard().IsLegal(blacks.PointOf(), SG_BLACK));
            _go_game.AddMove(blacks.PointOf(), SG_BLACK);
        }
        else {
            SgVector<SgPoint> handicap_stones;
            blacks.ToVector(&handicap_stones);
            _go_game.PlaceHandicap(handicap_stones);
        }
    }
}



UpdateResult Game::playMove(SgPoint position) {
    auto current_player = getBoard().ToPlay();

    if (getBoard().IsLegal(position, current_player)) {
        _go_game.AddMove(position, current_player);
        return UpdateResult::Legal;
    }
    else {
        return UpdateResult::Illegal;
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

bool Game::loadGame(string file_path) {
    std::ifstream file(file_path.c_str());
    if (!file.is_open()) {
        return false;
    }

    SgGameReader reader(file);
    auto root_node = reader.ReadGame();

    if (!root_node->HasProp(SG_PROP_SIZE)) {
        // @todo(jschmer): support better diagnostics such as ERROR_NOT_SUPPORTED_BOARD_SIZE
        return false;
    }

    auto size = 0;
    if (!root_node->GetIntProp(SG_PROP_SIZE, &size)) {
        // @todo(jschmer): support better diagnostics such as ERROR_NOT_SUPPORTED_BOARD_SIZE
        return false;
    }

    if (size != 9 && size != 13 && size != 19) {
        // @todo(jschmer): support better diagnostics such as ERROR_NOT_SUPPORTED_BOARD_SIZE
        return false;
    }

    _go_game.Init(root_node);

    // fast forward to latest move
    while (canNavigateHistory(SgNode::Direction::NEXT))
        navigateHistory(SgNode::Direction::NEXT);

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

void Game::navigateHistory(SgNode::Direction dir) {
    _go_game.GoInDirection(dir);
}

bool Game::canNavigateHistory(SgNode::Direction dir) const {
    return _go_game.CanGoInDirection(dir);
}

} // 
