#pragma once

#include "Go_Backend/Backend.h"

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
    void RenderGame(Go_Backend::Backend game);

//slots
public:
	void slot_MenuOpen(const QVariant &);
	void slot_MenuOpen_FileSelected(const QString & file);
	void slot_MenuExit(const QVariant &);

private:
	Ui::MainWindow ui;
};

} // namespace Go_GUI