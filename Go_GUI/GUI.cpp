#include "GUI.hpp"

#include <Qt3D/qglview.h>
#include <Qt3D/qglbuilder.h>
#include <QGraphicsView>
#include <QGraphicsPixmapItem>
#include <QMessageBox>
#include <QAction>
#include <Qgridlayout>
#include <QCloseEvent>

#include "VirtualView.hpp"
#include "Game.hpp"


namespace Go_GUI {

GUI::GUI(QWidget *parent) : QMainWindow(parent)
{
	ui.setupUi(this);
	this->init();
	QAction* menuitem_open = this->findChild<QAction *>("actionOpen");
	QAction* menuitem_exit = this->findChild<QAction *>("actionExit");

	connect(menuitem_open, &QAction::triggered, this, &GUI::slot_MenuOpen);
	connect(menuitem_exit, &QAction::triggered, this, &GUI::slot_MenuExit);
}

void GUI::init(){
	this->setWindowTitle("Augmented Go");
	QWidget* central = this->centralWidget();

	QGridLayout* view_big = central->findChild<QGridLayout *>("view_big");
	QGridLayout* view_small = central->findChild<QGridLayout *>("view_small");
	
	QGraphicsView* graphics_view = central->findChild<QGraphicsView *>("graphicsView");

	QString filename = "res/textures/white_stone.png";
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
	QFileDialog* open_file_dialog = new QFileDialog();
	// first connect FileDialog to event_OpenFileSelected function THEN execute FileDialog!
	connect(open_file_dialog, &QFileDialog::fileSelected, this, &GUI::slot_MenuOpen_FileSelected);
	open_file_dialog->exec();	// executed FileDialog	
}

void GUI::slot_MenuOpen_FileSelected(const QString & file){
	QMessageBox m;
	m.setText(file + " selected !");
	m.exec();
}

void GUI::slot_MenuExit(const QVariant &){
	auto reply = QMessageBox::question(this, "Quit?", "You really want to quit?", QMessageBox::StandardButton::Yes, QMessageBox::StandardButton::No);
	if (reply == QMessageBox::StandardButton::Yes) {
        emit stop_backend_thread();
		this->close();
    }
	else
		return; // do nothing
}

// override
void GUI::closeEvent(QCloseEvent *event){
	emit stop_backend_thread();
	event->accept();
}

} // namespace Go_GUI