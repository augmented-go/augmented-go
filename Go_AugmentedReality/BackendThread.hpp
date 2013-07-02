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
        void save_sgf(QString path, QString blackplayer_name, QString whiteplayer_name, QString game_name) const;

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

        void reset();

    private slots:
        void scan(); // our main worker function that is called by the timer
        
    // signals
    signals:
        // signals that a new game image was fetched and processed
        void backend_new_image(QImage camera_image) const;

        // signals that the game board has changed
        void game_data_changed(const GoBoard * game_board) const;

        // signals that the game has ended with the given result
        void finished_game_result(QString result) const;

    // Member vars    
    private:
        std::unique_ptr<GoBackend::Game>     _game;
        std::unique_ptr<Go_Scanner::Scanner> _scanner;
        bool _game_is_initialized;
    };
}