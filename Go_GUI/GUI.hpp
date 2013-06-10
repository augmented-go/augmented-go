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
    void new_image(const std::shared_ptr<QImage> param) {
        // TODO: update gui
        printf(">>> New Image arrived! '%d x %d' <<<\n", param->width(), param->height());
    }

    void new_game_data(const GoBoard * game_board) {
        printf(">>> New Game data! 'Movenum: %d' <<<\n", game_board->MoveNumber());
    }

signals:
    void stop_backend_thread();

private:
	Ui::MainWindow ui;
};

} // namespace Go_GUI