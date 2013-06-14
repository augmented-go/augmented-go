#pragma once

#include <QtWidgets/qmainwindow>
#include <QtWidgets\qfiledialog>
#include "Game.hpp"

#include "ui_GUI.h"

class VirtualView;
class AugmentedView;

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
    void slot_MenuOpen();
	void slot_MenuInfo();
	void closeEvent(QCloseEvent *event);
	void slot_ViewSwitch();
    void new_image() {
        // TODO: update gui
        printf(">>> New Image arrived! <<<\n\n");
    }

signals:
    void stop_backend_thread();

private:
	Ui::MainWindow ui;
	VirtualView* virtual_view;
	AugmentedView* augmented_view;

};

} // namespace Go_GUI