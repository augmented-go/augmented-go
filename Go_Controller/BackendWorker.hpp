// Copyright (c) 2013 augmented-go team
// See the file LICENSE for full license and copying terms.
#pragma once

#include <utility>
#include <tuple>
#include <memory>

#include <QThread>
#include <QTimer>
#include <QImage>

#include "Game.hpp"
#include "Scanner.hpp"

namespace Go_AR {
    class BackendWorker : public QObject {
        Q_OBJECT

    public:
        BackendWorker();
        ~BackendWorker();
            
    private:
        void signalGuiGameHasEnded() const;
        void signalGuiGameDataChanged() const;
        bool virtualModeActive() const;
        
    // slots
    public slots:
        /**
         * @brief       Saves the current game as sgf at the specified path.
         */
        void saveSgf(QString path, QString blackplayer_name, QString whiteplayer_name, QString game_name);

        /**
         * @brief       Plays a pass for the current player.
         *              Also emits finished_game_result if the game has ended because of playing a pass.
         */
        void pass();

        /**
         * @brief       The current player resigns. Also emits finished_game_result.
         */
        void resign();

        /**
         * @brief       Finishes a game. This is a convenience function for playing two passes.
         *              Also emits finished_game_result.
         */
        void finish();

        /**
         * @brief       Resets the current game and starts a new one with the given rules and
         *              the current board as the starting setup.
         */
        void resetGame(GoRules rules);

        /**
         * @brief       Toggles between completely virtual and augmented application mode.
         */
        void setVirtualGameMode(bool checked);

        /**
         * @brief       Sends a move to game
         */
        void playMove(const int x, const int y);

        /**
         * @brief       Triggers the manual board selection in the Scanner.
         *              Blocks this thread until the selection was made.
         */
        void selectBoardManually();

        /**
         * @brief       Triggers the autmatic board detection in the Scanner.
         */
        void selectBoardAutomatically();


    public slots:
        void scan(); // our main worker function that is called by the timer
        
    // signals
    signals:
        // signals that a new game image was fetched and processed
        void newImage(QImage camera_image) const;

        // signals that the game state has changed
        void gameDataChanged(const GoBackend::Game * game) const;

        // signals that the game has ended with the given result
        void finishedGameResult(QString result) const;


    // Member vars    
    private:
        GoBackend::Game     _game;
        Go_Scanner::Scanner _scanner;
        QTimer              _scan_timer;
        GoRules _new_game_rules;
        bool    _game_is_initialized;
    };
}