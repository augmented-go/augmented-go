#pragma once

#include "../Go_Backend/Game.hpp"

#include <QtWidgets/qmainwindow>
#include "ui_GUI.h"			// is build while compiling GUI.ui

class VirtualView;

namespace Go_GUI {

class GUI : public QMainWindow
{
	Q_OBJECT

public:
	GUI(QWidget *parent = 0)
		: QMainWindow(parent)
	{
		ui.setupUi(this);
		this->init();
	}

	~GUI(){};

	void init();
    void RenderGame(Go_Backend::Backend game);

private:
	Ui::MainWindow ui;
};

} // namespace Go_GUI