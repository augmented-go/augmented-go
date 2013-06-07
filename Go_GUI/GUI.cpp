#include <string>

#include "GUI.hpp"
#include "Game.hpp"

#include <Qt3D/qglview.h>
#include "Qt3D/qglbuilder.h"
#include "VirtualView.hpp"
#include "qgraphicsview.h"
#include <QGraphicsPixmapItem>
#include <QMessageBox>
#include <QAction>
#include <Qgridlayout>
#include <QCloseEvent>

namespace Go_GUI {

GUI::GUI(QWidget *parent) : QMainWindow(parent)
{
	ui.setupUi(this);
	this->init();
	QAction* menuitem_open = this->findChild<QAction *>("actionOpen");
	QAction* menuitem_exit = this->findChild<QAction *>("actionExit");
	QAction* menuitem_info = this->findChild<QAction *>("actionInfo");

	connect(menuitem_open, &QAction::triggered, this, &GUI::slot_MenuOpen);
	connect(menuitem_exit, &QAction::triggered, this, &GUI::close);
	connect(menuitem_info, &QAction::triggered, this, &GUI::slot_MenuInfo);
}

void GUI::init(){
	this->setWindowTitle("Augmented Go");
	QWidget* central = this->centralWidget();

	QGridLayout* view_big = central->findChild<QGridLayout *>("view_big");
	QGridLayout* view_small = central->findChild<QGridLayout *>("view_small");
	
	QGraphicsView* graphics_view = central->findChild<QGraphicsView *>("graphicsView");

	QString filename = QString("../Go_GUI/textures/white_stone.png");
	QImage* image = new QImage(filename);
	if (!image->load(filename)){
		QMessageBox m;
		m.setText("File " + filename + " not found!");
		m.exec();
	}
    QGraphicsPixmapItem* item = new QGraphicsPixmapItem( QPixmap::fromImage(*image));
    
	// Adding items
	QGraphicsScene* scene = new QGraphicsScene;
    scene->addItem(item);

	//Setting Scene of View
	//graphics_view->setScene(scene);
	
	QWidget* virtual_view = QWidget::createWindowContainer(new VirtualView());
	view_big->addWidget(virtual_view);
}

void GUI::RenderGame(GoBackend::Game game) {
    // blaa
	//virtual_view->show();
	this->show(); // shows Qt5 Window
}



//////////
//Slots
//////////


void GUI::slot_MenuOpen(const QVariant &){
	QString selfilter = tr("SGF (*.sgf)");
	QString fileName = QFileDialog::getOpenFileName(
        this,
        "open sgf-file",
        NULL,
        tr("SGF (*.sgf)" ),
        &selfilter 
	);
}

void GUI::slot_MenuInfo(const QVariant &){
	// Appliction Info
	std::string output = "Augmented Go - Interactive Game of Go as Augmented Reality Application\n\n\n";

	// Build date and time
	output += "This build of Augmented Go was compiled at " __DATE__ ", " __TIME__ ".\n";

	// Copyright
	std::string year = __DATE__;
	year = year.substr(year.find_last_of(" "));	// deleting day and month
	output += "Copyright " + year + "\n";

	// Licence
	output += "\nThis software is released under the \"MIT License\".\n"
		"See the file LICENSE for full license and copying terms.\n";

	// Final InfoBox
	QMessageBox::about(this, "Info", output.c_str());
}


// override
void GUI::closeEvent(QCloseEvent *event){
	auto reply = QMessageBox::question(this, "Quit?", "You really want to quit?", QMessageBox::StandardButton::Yes, QMessageBox::StandardButton::No);
	if (reply == QMessageBox::StandardButton::Yes) {
        emit stop_backend_thread();
		event->accept();
    }
	else
		event->ignore();
}

} // namespace Go_GUI