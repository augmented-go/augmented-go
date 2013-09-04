// Copyright (c) 2013 augmented-go team
// See the file LICENSE for full license and copying terms.
#pragma once

#include "GoGame.h"
#include <string>


namespace GoBackend {
using std::string;

enum class UpdateResult {
    /**
     * The internal board state after the update() call does not match the real life board.
     * This may happen due to an invalid move according to the rules or captured stone have not yet been removed completely.
     * This means, the user has to make the real life board match the internal board again.
     */
    Illegal,
    // could be split into:
    // Illegal due to rules (ko, turn)
    // Illegal due to stones not removed after capture
    // Illegal due to removing or adding multiple stones.

    /**
     * Internal board matches real life board.
     */
    Legal
};

/**
 * Basic game class to play and keep track of a game of go. One instance handles one
 * game of go. 
 * The board locations begin at (1,1) and go up to max. (19,19), depending on the
 * board size. These points are represented by the fuego type 'SgPoint'.
 * The fuego type 'GoSetup' is used for updating the board. GoSetup uses
 * 'SgPoints' to add stones to such a setup. The game then automatically tries to read
 * a move out of the current and updated board.
 *
 * The rules are:
 *   Handicap: 0
 *   Scoring: Japanese
 *   Komi: 6.5
 *   2 Passes end a game
 *   
 * Usage Example:
 *   Game game;
 *   GoSetup setup;
 *   setup.AddWhite(SgPointUtil::Pt(1, 1));
 *
 *   game.init(9, setup);
 *
 *   setup.AddBlack(SgPointUtil::Pt(2, 2));
 *   game.update(setup);
 */
class Game {

public:
    Game();

    /**
     * @brief       Initializes the game board with given size, setup (starting positions) and rules.
     *              The setup defaults to an empty board.
     *              The rules default to
     *                  handicap: 0
     *                  komi:     6.5
     *                  scoring:  japanese
     *                  game end: after 2 consecutive passes
     *
     * @param[in]   size    board size (size x size)
     * @param[in]   setup   initial board setup
     * @param[in]   rules   game rules for this setup/game, 
                            only allows a handicap of 0, the placement of handicap stones is handled in a differnt way
     * @returns     false - the setup contains invalid stones\n
     *              true  - init successful
     */
    bool init(int size, GoSetup setup = GoSetup(), GoRules rules = GoRules(0, GoKomi(6.5), true, true));

    /**
     * @brief       Updates the game with the given setup. Tries to extract a valid move from the setup.
     *              Note: Ignores current player information in setup. 
     * @param[in]   setup   new board setup
     * @returns     See UpdateResult class. Also returns Illegal when the setup includes invalid stones.
     */
    UpdateResult update(GoSetup setup);

    /**
     * @brief       Plays a move at given position for the current player.
     * @returns     Whether the move was legal or not.
     */
    UpdateResult playMove(SgPoint position);

    /**
     * @brief       Get current board information
     * @returns     reference to current board instance
     */
    const GoBoard& getBoard() const;

    /**
     * @brief        Writes the current game state to a sgf file.
     *               Names for players and game are only added to the file if not empty.
     *               Also includes the current date.
     * @returns      false if the file could not be opened
     */
    bool saveGame(string file_path, string name_black = "", string name_white = "", string game_name = "");

    /**
     * @brief        Convenience function to quickly finish a game. Plays a pass for each player and
     *               returns the result (see Game::getResult)
     */
    std::string finishGame();

    /**
     * @brief        Plays a 'pass' for the current player.
     */
    void pass();

    /**
     * @brief        Current player resigns and the game ends.
     */
    void resign();

    /**
     * @brief        Returns the result of the game in a standard way,
     *               for example W+1.5 (White wins by 1.5 moku)
     *               or B+R (Black wins by resign)
     *               or 0 for a drawn game
     *               If the game was not ended properly, this returns an empty string.
     *               Also doesn't consider any dead stones!
     */
    std::string getResult() const;

    /**
     * @brief       Checks if the game has ended. A game ends if a player resigns or if both player
     *              have played a pass.
     */
    bool hasEnded() const;


    /** 
     * @brief        Navigates the history in the given direction and updates the board accordingly.
                     Make sure that going in that direction is even possible, by calling canNavigateHistory.
     */
    void navigateHistory(SgNode::Direction dir);

    /** 
     * @brief        Returns whether there is actually any history in the given direction.
     */
    bool canNavigateHistory(SgNode::Direction dir) const;

private:
    // Not implemented
    Game(const Game&);
    Game& operator=(const Game&);

private:
    UpdateResult updateNormal(SgPointSet added_blacks, SgPointSet added_whites, SgPointSet removed_blacks, SgPointSet removed_whites);
    UpdateResult updateWhileCapturing(GoSetup new_setup);

    /**
     * @brief       Checks whether handicap stones are being placed or not.
     */
    bool isPlacingHandicap(SgPointSet current_blacks, SgPointSet current_whites, SgPointSet new_whites);
    void placeHandicap(GoSetup new_setup);

    /**
     * @brief       Tries to play a move at point for player. 
     *              Also checks if removed_of_player and removed_of_opponent are captured stones.
     *              Gets called inside updateNormal.
     * @returns     Whether the move was legal or illegal.
     */
    UpdateResult updateSingleMove(SgPoint point, SgBlackWhite player, SgPointSet removed_of_player, SgPointSet removed_of_opponent);

    /**
     * @brief       Returns the possibly captured stones when current player of cons_board would play at point.
     */
    SgPointSet possibleCapturedStones(const GoBoard& const_board, SgPoint point);

    bool validSetup(const GoSetup& setup) const;
    bool allValidPoints(const SgPointSet& stones) const;

private:
    GoGame _go_game;
    bool _game_finished; // we need this variable because fuego doesn't tag a game
                         // finished if a player resigns

    bool _while_capturing;
};

}