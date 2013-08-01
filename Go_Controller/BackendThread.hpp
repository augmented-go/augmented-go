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
    class BackendThread : public QThread {
        Q_OBJECT

    public:
        BackendThread();
        ~BackendThread();
            
    private:
        // thread function
        void run() override;
        void signalGuiGameHasEnded() const;
        
    // slots
    public slots:
        /**
         * @brief       Stops this thread.
         */
        void stop();

        /**
         * @brief       Saves the current game as sgf at the specified path.
         */
        void saveSgf(QString path, QString blackplayer_name, QString whiteplayer_name, QString game_name) const;

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
         * @brief       Triggers the manual board selection in the Scanner.
         *              Blocks this thread until the selection was made.
         */
        void selectBoardManually();


    private slots:
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
        std::unique_ptr<GoBackend::Game>     _game;
        std::unique_ptr<Go_Scanner::Scanner> _scanner;
        bool _game_is_initialized;
        GoRules _new_game_rules;
    };
}