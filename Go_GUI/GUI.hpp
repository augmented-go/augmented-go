#pragma once

#include <QMainWindow>
#include <QFileDialog>
#include <QDebug>

#include "Game.hpp"

#include "ui_GUI.h"
#include "ui_NewGameDialog.h"
#include "AugmentedView.hpp"

class VirtualView;

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
    void slot_newGameData(const GoBoard* board);
    void slot_setupNewGame(QString game_name, QString blackplayer_name, QString whiteplayer_name);

private slots:
    void slot_ButtonNewGame();
    void slot_MenuOpen();
    void slot_MenuSave();
    void slot_MenuInfo();
    void slot_ViewSwitch();
    void closeEvent(QCloseEvent *event);

signals:
    void signal_saveGame(QString fileName, QString blackplayer_label, QString whiteplayer_label, QString game_name);
    void stop_backend_thread();

private:
    Ui::MainWindow ui_main;
    VirtualView* virtual_view;
    AugmentedView* augmented_view;
    QPixmap whitebasket_pixmap, blackbasket_pixmap, closedbasket_pixmap, gotable_pixmap;
    QString game_name;

    // Pointer to the game board, will be set exactly once ("slot_board_init") as soon as the backend
    // send it here. This pointer will be valid until the GUI exits the application.
    const GoBoard* game_board;

    void setPlayerLabels(QString blackplayer_name, QString whiteplayer_name);
};

} // namespace Go_GUI