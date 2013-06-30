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
    void slot_ButtonNewGame();
    void slot_setupNewGame(QString game_name, QString blackplayer_name, QString whiteplayer_name);
    void slot_MenuOpen();
    void slot_MenuSave();
    void slot_MenuInfo();
    void slot_ViewSwitch();

public slots:
    void new_image(QImage image) {
        printf(">>> New Image arrived! '%d x %d' -- Format: %d <<<\n", image.width(), image.height(), image.format());

        augmented_view->setImage(image);
        augmented_view->rescaleImage(augmented_view->parentWidget()->size());
    }

    void new_game_data(const GoBoard * game_board) {
        auto current_turn = game_board->MoveNumber();
        auto current_player = game_board->ToPlay();
        switch (current_player) {
            case SG_WHITE:
                ui_main.white_basket->setPixmap(whitebasket_pixmap);
                ui_main.black_basket->setPixmap(closedbasket_pixmap);
                break;
            case SG_BLACK:
                ui_main.white_basket->setPixmap(closedbasket_pixmap);
                ui_main.black_basket->setPixmap(blackbasket_pixmap);
                break;
            default:
                assert(false);
                break;
        }
        
        auto captured_black_stones = game_board->NumPrisoners(SG_BLACK);
        auto captured_white_stones = game_board->NumPrisoners(SG_WHITE);

        ui_main.capturedwhite_label->setText(QString::number(captured_white_stones));
        ui_main.capturedblack_label->setText(QString::number(captured_black_stones));

        printf(">>> New Game data! <<<\n");
    }

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

    void setPlayerLabels(QString blackplayer_name, QString whiteplayer_name);
};

} // namespace Go_GUI