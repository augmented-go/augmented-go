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
public:
    GUI(QWidget *parent = 0);
    ~GUI(){};

    void init();
    void RenderGame(GoBackend::Game game);

public slots:
    void slot_showFinishedGameResults(QString result);
    void slot_newImage(QImage image);
    void slot_newGameData(const GoBackend::Game* game);
    void slot_setupNewGame(QString game_name, QString blackplayer_name, QString whiteplayer_name, float komi);

private slots:
    void slot_ButtonNewGame();
    void slot_ButtonPass();
    void slot_ButtonResign();
    void slot_MenuOpen();
    void slot_MenuSave();
    void slot_MenuInfo();
    void slot_ViewSwitch();
    void slot_BoardDetectionManually();
    void slot_BoardDetectionAutomatically();
    void slot_ToggleVirtualGameMode();
    void closeEvent(QCloseEvent *event);
    void slot_passOnVirtualViewPlayMove(const int x, const int y);

signals:
    void signal_saveGame(QString fileName, QString blackplayer_label, QString whiteplayer_label, QString game_name);
    void signal_openGame(QString fileName);
    void signal_pass();
    void signal_resign();
    void signal_boardDetectionManually();
    void signal_boardDetectionAutomatically();
    void signal_setVirtualGameMode(bool checked);
    void stop_backend_thread();
    void signal_newGame(GoRules rules);
    void signal_playMove(const int x, const int y);

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

    void setPlayerLabels(QString blackplayer_name, QString whiteplayer_name);

    void resizeEvent(QResizeEvent* event){
        QMainWindow::resizeEvent(event);
        virtual_view->resizeEvent(event);
    }
};

} // namespace Go_GUI