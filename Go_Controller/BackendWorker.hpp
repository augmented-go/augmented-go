// Copyright (c) 2013 augmented-go team
// See the file LICENSE for full license and copying terms.
#pragma once

#include <utility>
#include <tuple>
#include <memory>

#include <QThread>
#include <QTimer>
#include <QImage>
#include <QTime>

#include "SgNode.h"

#include "Game.hpp"
#include "Scanner.hpp"

/**
 * Classes for running the camera scanning and game updates in a seperate thread
 */
namespace Go_Controller {
    /**
     * @brief   Class for the scanning- and game-update-loop.\n
     *          Main work is done in the scan() method: scanning the camera and interpreting the result. This is done
     *          periodically by a timer.\n
     *          This object has to run in a seperate thread because it does heavy duty processing and the timer wouldn't
     *          work otherwise.
     *
     * Usage Example with Qt:
       \code{.cpp}
       auto worker = new BackendWorker();

       QThread worker_thread;
       QObject::connect( &worker_thread, SIGNAL(finished()), worker, SLOT(deleteLater()) ); // clean up the worker when the thread is stopped

       // move the worker into the thread
       worker->moveToThread(&worker_thread);

       worker_thread.start(); // start worker thread
       \endcode
     */
    class BackendWorker : public QObject {
        Q_OBJECT

    public:
        BackendWorker();
        ~BackendWorker();
            
    // slots
    public slots:
        /**
         * @brief       Saves the current game as sgf at the specified path with the given player names and game name.
         *              See Go_Backend::Game::saveGame()
         * @param[in]   path                
         * @param[in]   blackplayer_name    
         * @param[in]   whiteplayer_name    
         * @param[in]   game_name           
         */
        void saveSgf(QString path, QString blackplayer_name, QString whiteplayer_name, QString game_name);

        /**
         * @brief       Loads the specified game from a sgf file, discards the current one.
         */
        void loadSgf(QString fileName);


        /**
         * @brief       Plays a pass for the current player.
         *              Also emits finished_game_result if the game has ended because of playing a pass.
         *              See Go_Backend::Game::pass()
         */
        void pass();

        /**
         * @brief       The current player resigns. Also emits finished_game_result.
         *              See Go_Backend::Game::resign()
         */
        void resign();

        /**
         * @brief       Finishes a game. This is a convenience function for playing two passes.
         *              Also emits finished_game_result.
         *              See Go_Backend::Game::finishGame()
         */
        void finish();

        /**
         * @brief       Resets the current game and starts a new one with the given rules and
         *              the current board as the starting setup.
         * @param[in]   rules   Rules for the new game
         */
        void resetGame(GoRules rules);

        /**
         * @brief       Enables or disables the virtual game mode.
         * @param[in]   checked     true: enable virtual game mode and disable the scan-timer
         *                          false: disable virtual game mode and enable the scan-timer
         */
        void setVirtualGameMode(bool checked);

        /**
         * @brief       Plays a move for the current player.
         * @param[in]   x   position on the x-axis of the board
         * @param[in]   y   position on the y-axis of the board
         */
        void playMove(const int x, const int y);

        /**
         * @brief       Triggers the manual board selection in the Scanner.
         *              Blocks this thread and disables the scan-timer until the selection was made.
         *              See Go_Scanner::Scanner::selectBoardManually()
         */
        void selectBoardManually();

        /**
         * @brief       Triggers the autmatic board detection in the Scanner.
         *              Blocks this thread and disables the scan-timer until the selection was made.
         *              See Go_Scanner::Scanner::selectBoardAutomatically()
         */
        void selectBoardAutomatically();

        /**
         * @brief       Sets the option for showing debug image of camera.
         * @param       debug      if true show debug image, false show normal camera image
         */
        void setScannerDebugImage(bool debug);
       
        /**
         * @brief       Changes the scanning interval. Passing 0 will scan as fast as it is possible, depending on the running pc.
         * @param[in]   milliseconds    Minimum time span between two consecutive image scans. The time span may be 
         *                              longer if the cpu isn't the fastest. The scan will then run as soon as possible.
         */
        void changeScanningRate(int milliseconds);

        /**
         * @brief       Navigates the history of the current game.
         *              See Go_Backend::Game::navigateHistory()
         * @param       dir    Direction of the navigation (e.g. NEXT = forward, PREVIOUS = backward)
         */
        void navigateHistory(SgNode::Direction dir);

    private slots:
        void scan(); // our main worker function that is called by the timer
        
    // signals
    signals:
        /**
         * @brief       Signals that a new camera image is available.
         * @param[in]   camera_image    the new camera image
         */
        void newImage(QImage camera_image) const;

        /**
         * @brief       Signals that the game state has changed due to a successful scan or played move.
         * @param[in]   game    Pointer to the current game instance. You can get game information from that object.
         */
        void gameDataChanged(const Go_Backend::Game * game) const;

        /**
         * @brief       Signals that the game has ended with the given result.
         * @param[in]   result  Result in standard format, see Go_Backend::Game::getResult().
         */
        void finishedGameResult(QString result) const;

        /**
         * @brief       Signals an error message.
         * @param[in]   message     Error message
         */
        void displayErrorMessage(QString message) const;

        /**
        * @brief       Signals an error message with a messagebox the user must explicitly close.
        * @param[in]   title     Title of the messagebox
        * @param[in]   message   Error message
        */
        void displayErrorMessagebox(QString title, QString message) const;

        /**
         * @brief       Signals that no camera image could be retrieved.
         */
        void noCameraImage() const;

    private:
        void signalGuiGameHasEnded() const;
        void signalGuiGameDataChanged() const;
        bool virtualModeActive() const;
        bool setupIsStable(const GoSetup& setup);

    // Member vars    
    private:
        Go_Backend::Game    _game;
        Go_Scanner::Scanner _scanner;
        QTimer              _scan_timer;

        GoRules _new_game_rules;
        bool    _game_is_initialized;
        int     _cached_board_size;

        // variables for the check if a setup is stable
        GoSetup _stable_reference_setup;
        QTime   _stable_period_timer;
        int     _stable_setup_period;
    };
}