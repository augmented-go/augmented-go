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
    void slot_HistoryBackward();
    void slot_HistoryForward();
    void closeEvent(QCloseEvent *event);

signals:
    void signal_saveGame(QString fileName, QString blackplayer_label, QString whiteplayer_label, QString game_name);
    void signal_openGame(QString fileName);
    void signal_pass();
    void signal_resign();
    void stop_backend_thread();
    void signal_newGame(GoRules rules);

private:
    Ui::MainWindow ui_main;
    VirtualView* virtual_view;
    AugmentedView* augmented_view;
    QPixmap whitebasket_pixmap, blackbasket_pixmap, closedbasket_pixmap, gotable_pixmap;
    QString game_name;

    // Pointer to the game board, will be set & cached in the slot "slot_newGameData".
    // This pointer will be valid until the GUI exits the application or the backend sends a new one.
    const GoBackend::Game* go_game;

    void setPlayerLabels(QString blackplayer_name, QString whiteplayer_name);
};

} // namespace Go_GUI