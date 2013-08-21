// Copyright (c) 2013 augmented-go team
// See the file LICENSE for full license and copying terms.
#pragma once

#include <QMainWindow>
#include <QFileDialog>
#include <QDebug>
#include <QTimer>

#include "Game.hpp"

#include "ui_GUI.h"
#include "ui_NewGameDialog.h"
#include "AugmentedView.hpp"
#include "VirtualView.hpp"

namespace Go_GUI {

class GUI : public QMainWindow
{
    Q_OBJECT

private:
    Ui::MainWindow ui_main;
    VirtualView* virtual_view;
    AugmentedView* augmented_view;

    QPixmap whitebasket_pixmap, blackbasket_pixmap, closedbasket_pixmap, gotable_pixmap;
    QImage augmented_logo;
    QString game_name, texture_path;

    // Pointer to the game board, will be set & cached in the slot "slot_newGameData".
    // This pointer will be valid until the GUI exits the application or the backend sends a new one.
    const GoBackend::Game* go_game;

    /**
     * @brief	Sets initial settings like the content of views, texts and windows
     */
    void init();

    /**
     * @brief	sets labels and variables for player names.
     * @param	QString		name of black player (default: "Black"
     * @param	QString		name of white player (default: "White"
     */
    void setPlayerLabels(QString blackplayer_name, QString whiteplayer_name);

    /**
     * @brief	overridden SLOT QCloseEvent "resizeEvent"
     * @param	QResizeEvent*	resize event
     */
    void resizeEvent(QResizeEvent* event){
        QMainWindow::resizeEvent(event);
        virtual_view->resizeEvent(event);
    }

    /**
     * @brief	overridden SLOT QCloseEvent "closeEvent"
     *			When trying to close the application a window appears and asks if user is sure.
     *			If answered "yes" the a signal to the backend thread is sent to stop it.
     *			If answered "no" the close event is ignored.
     * @param	QCloseEvent		close event that shall or shall not be executed afterwards.
     */
    void closeEvent(QCloseEvent *event);

public:

    /**
     * @brief	Checks for gui elements and fonts.
     *          Connects signals and slots.
     * @param	QWidget/QMainWindow		parent widget that creates this
     */
    GUI(QWidget *parent = 0);
    ~GUI(){};

signals:
    /**
     * @brief signals that the user wants to save a game.
     * @param   QString filename            name of file
     * @param   QString blackplayer_label   name of black player
     * @param   QString whiteplayer_label   name of white player
     * @param   QString game_name           name of game
     */
    void signal_saveGame(QString fileName, QString blackplayer_label, QString whiteplayer_label, QString game_name);

    /**
     * @brief signals that the user wants to open a game.
     * @param   QString filename     name of file
     */
    void signal_openGame(QString fileName);

    /** @brief signals that the user wants to pass. */
    void signal_pass();

    /** @brief signals that the user wants to resign. */
    void signal_resign();

    /** @brief signals that the user wants to use manual board detection. */
    void signal_boardDetectionManually();

    /** @brief signals that the user wants to use automatic board detection. */
    void signal_boardDetectionAutomatically();

    /** @brief signals that the user wants to use virtual game mode. */
    void signal_setVirtualGameMode(bool checked);

    /** @brief signals that backend thread should stop. */
    void stop_backend_thread();

    /** 
     * @brief signals that the user wants to start a new game. 
     * @param   GoRules rules   rules of the new game
     */
    void signal_newGame(GoRules rules);

    /** 
     * @brief   signals that the user wants to play a move.
     *          Coordinates on board start with 1,1 !
     * @param   int x   x-Coordinate on board
     * @param   int y   y-Coordinate on board
     */
    void signal_playMove(const int x, const int y);

    void signal_setScannerDebugImage(bool debug);

private slots:
    /**
     * @brief   SLOT "NewGame/Reset"
     *          Opens a Dialog that asks for game rules and names.
     */
    void slot_ButtonNewGame();

    /**
     * @brief   SLOT "Resign"
     *          Opens a Dialog that asks for confirmation.
     *			If answered yes, a signal is sent to backend that the current player surrenders.
     */
    void slot_ButtonResign();

    /**
     * @brief   SLOT "Pass"
     *          Opens a Dialog that asks for confirmation.
     *			If answered yes, a signal is sent to backend that the current player passes.
     */
    void slot_ButtonPass();

    /**
     * @brief	SLOT QAction "MenuOpen"
     *			opens a filedialog that lets the user choose an sgf-file.
     *          sends via "signal_openGame" filename to backend
     */
    void slot_MenuOpen();
 
    /**
     * @brief	SLOT QAction "MenuSave"
     *			opens a filedialog that lets the user choose an sgf-file.
     *          sends via "signal_saveGame" filename, gamename and playernames to backend
     */
    void slot_MenuSave();

    /**
     * @brief	SLOT QAction "MenuInfo"
     *			opens a window with information about the application.
     */
    void slot_MenuInfo();
    /**
     * @brief	SLOT "ViewSwitch"
     *			Switches big view with small view.
     *			To assign a view to something a QWidget has to be created.
     */
    void slot_ViewSwitch();

    /**
     * @brief   SLOT BoardDetectionManually
     *          Repeates a signal from the QAction "manually_action"
     *          to notify to manually detect gameboard
     */
    void slot_BoardDetectionManually();

    /**
     * @brief   SLOT BoardDetectionManually
     *          Repeates a signal from the QAction "manually_action"
     *          to notify to automatically detect gameboard
     */
    void slot_BoardDetectionAutomatically();

    /**
     * @brief   SLOT ToggleVirtualGameMode
     *          Sends a signal which determines the game mode the user chose
     */
    void slot_ToggleVirtualGameMode();

    /**
     * @brief   SLOT passOnVirtualViewPlayMove
     *          Repeates a signal from Virtual View to backend.
     *          The signal has the position of the board the player wants to
     *          place a stone to.
     */
    void slot_passOnVirtualViewPlayMove(const int x, const int y);

    /**
     * @brief   SLOT toggleScannerDebugImage
     *          Sends a signal which determines the scanner image mode the user chose
     */
    void slot_toggleScannerDebugImage();


public slots:
    
    /**
     * @brief   SLOT "new image"
     *          If a new image is sent to GUI, refresh and rescale picture.
     * @param   QImage  new image from scanner
     */
    void slot_newImage(QImage image);
    
    /**
     * @brief   SLOT "new game data"
     *          If new game data is sent to GUI, refresh display of current player and captured stones.
     * @param   game     new game representation
     */
    void slot_newGameData(const GoBackend::Game* game);

    /**
     * @brief   SLOT "Show finished game results"
     *          If a game ended, the BackendThread sends a signal with the results.
     *          Here the results are shown to the user.
     */
    void slot_showFinishedGameResults(QString result);

    /**
     * @brief   SLOT "setup new game"
     *          When a new game has been started, setup game name and player names on gui.
     * @param   QString    game name
     * @param   QString    black player name
     * @param   QString    white player name
     */
    void slot_setupNewGame(QString game_name, QString blackplayer_name, QString whiteplayer_name, float komi);

    /**
     * @brief   SLOT "display error message"
     *          Displays the error message on the gui. Message gets overwritten by subsequent calls.
     */
    void slot_displayErrorMessage(QString message);
};

} // namespace Go_GUI