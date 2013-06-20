#pragma once

#include "Game.hpp"

#include <QtWidgets/qmainwindow>
#include <QtWidgets\qfiledialog>
#include "ui_GUI.h"

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
	void closeEvent(QCloseEvent *event);

//slots
public:
	void slot_MenuOpen(const QVariant &);
	void slot_MenuOpen_FileSelected(const QString & file);
	void slot_MenuExit(const QVariant &);

public slots:
    // note: just a sample slot demonstrating a signal from the backend thread
    void new_image(const QImage image) {
        // TODO: update gui
        printf(">>> New Image arrived! '%d x %d' <<<\n", image.width(), image.height());
    }

    void new_game_data(const GoBoard * game_board) {
        auto current_turn = game_board->MoveNumber();
        auto current_player = game_board->ToPlay();
        switch (current_player) {
        case SG_WHITE:
            break;
        case SG_BLACK:
            break;
        default:
            assert(false);
        }
        
        auto captured_black_stones = game_board->NumPrisoners(SG_BLACK);
        auto captured_white_stones = game_board->NumPrisoners(SG_WHITE);

        printf(">>> New Game data! <<<\n");
    }

signals:
    void stop_backend_thread();

private:
	Ui::MainWindow ui;
};

} // namespace Go_GUI