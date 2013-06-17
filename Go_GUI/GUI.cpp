#include "GUI.hpp"

#include <string>
#include <vector>

#include <Qt3D/qglview.h>
#include <Qt3D/qglbuilder.h>
#include <qgraphicsview>
#include <QGraphicsPixmapItem>
#include <QMessageBox>
#include <QAction>
#include <Qgridlayout>
#include <QCloseEvent>
#include "Game.hpp"

#include "VirtualView.hpp"
#include "AugmentedView.hpp"


namespace Go_GUI {
	
/**
 * @brief	Checks for gui elements and connects signals and slots
 * @param	QWidget/QMainWindow		parent widget that creates this
 */
GUI::GUI(QWidget *parent) : QMainWindow(parent)
{
	ui.setupUi(this);

	// checking for elements
	auto open_menuitem	= this->findChild<QAction *>("open_action");
	auto exit_menuitem	= this->findChild<QAction *>("exit_action");
	auto info_menuitem	= this->findChild<QAction *>("info_action");
	auto big_view		= this->findChild<QGridLayout *>("big_view");
	auto small_view		= this->findChild<QGridLayout *>("small_view");
	auto graphics_view	= this->findChild<QGraphicsView *>("graphics_view");
	auto viewswitch_button = this->findChild<QPushButton *>("viewswitch_button");

	if ( open_menuitem == nullptr || exit_menuitem == nullptr || info_menuitem == nullptr
		|| big_view == nullptr || small_view == nullptr || graphics_view == nullptr)
		QMessageBox::critical(this, "Element not found", 
							QString("An element of GUI could not be found. (Deleted, renamed?)\n\n Element list:\n " 
							 + ((open_menuitem) ? open_menuitem->objectName()	: "<Open> not found!") + "\n"
							 + ((exit_menuitem) ? exit_menuitem->objectName()	: "<Exit> not found!") + "\n"
							 + ((info_menuitem) ? info_menuitem->objectName()	: "<Info> not found!") + "\n"
							 + ((big_view)		? big_view->objectName()		: "<Big view> not found!") + "\n"
							 + ((small_view)	? small_view->objectName()		: "<Small view> not found!") + "\n"
							 + ((graphics_view)	? graphics_view->objectName()	: "<Graphicsview> not found!") + "\n"
							 ));

	// connections
	connect(open_menuitem,		&QAction::triggered,	this, &GUI::slot_MenuOpen);
	connect(exit_menuitem,		&QAction::triggered,	this, &QWidget::close);	
	connect(info_menuitem,		&QAction::triggered,	this, &GUI::slot_MenuInfo);
	connect(viewswitch_button,	&QPushButton::clicked,	this, &GUI::slot_ViewSwitch);

	// setting initial values
	this->init();
}

/**
 * @brief	Sets initial settings, texts and windows
 */
void GUI::init(){
	this->setWindowTitle("Augmented Go");
	QWidget* central = this->centralWidget();

	QGridLayout* big_layout = central->findChild<QGridLayout *>("big_layout");
	QGridLayout* small_layout = central->findChild<QGridLayout *>("small_layout");
	
	//QGraphicsView* graphics_view = central->findChild<QGraphicsView *>("graphics_view");

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
	
	virtual_view = new VirtualView(this);
	augmented_view = new AugmentedView(this);

	QWidget* big_container = QWidget::createWindowContainer(augmented_view, this);
	big_container->setObjectName("big_container");
	big_container->setToolTip("augmented view");
	QWidget* small_container = QWidget::createWindowContainer(virtual_view, this);
	small_container->setObjectName("small_container");
	small_container->setToolTip("virtual view");

	//container->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
	
	big_layout->addWidget(big_container);
	small_layout->addWidget(small_container);
}

void GUI::slot_ViewSwitch(){
	QWidget* big_container = this->findChild<QWidget*>("big_container");
	QWidget* small_container = this->findChild<QWidget*>("small_container");

	QWidget* newbig_container, *newsmall_container;

	if (big_container->toolTip() == "augmented view"){
		newbig_container = QWidget::createWindowContainer(virtual_view, this);
		newbig_container->setObjectName("big_container");
		newbig_container->setToolTip("virtual view");
		newsmall_container = QWidget::createWindowContainer(augmented_view, this);
		newsmall_container->setObjectName("small_container");
		newsmall_container->setToolTip("augmented view");
	}
	else {
		newbig_container = QWidget::createWindowContainer(augmented_view, this);
		newbig_container->setObjectName("big_container");
		newbig_container->setToolTip("augmented view");
		newsmall_container = QWidget::createWindowContainer(virtual_view, this);
		newsmall_container->setObjectName("small_container");
		newsmall_container->setToolTip("virtual view");
	}
	big_container->deleteLater();
	small_container->deleteLater();

	this->findChild<QGridLayout *>("big_layout")->addWidget(newbig_container);
	this->findChild<QGridLayout *>("small_layout")->addWidget(newsmall_container);


	/*for (QWidget* l : big_list){
		small_view->addWidget(l);
		big_view->removeWidget(l);
	}
	for (QWidget* l : small_list){
		big_view->addWidget(l);
		small_view->removeWidget(l);
	}*/
}
/**
 * @brief	shows and renders a GoBackend::Game
 */
void GUI::RenderGame(GoBackend::Game game) {
	//virtual_view->show();
	this->show(); // shows Qt5 Window
}


//////////
//Slots
//////////

/**
 * @brief	SLOT QAction "MenuOpen"
 *			opens a filedialog that lets the user choose an sgf-file.
 * @todo	loading sgf file
 */
void GUI::slot_MenuOpen(){
	QString selfilter = tr("SGF (*.sgf)");
	QString fileName = QFileDialog::getOpenFileName(
        this,
        "open sgf-file",
        NULL,
        tr("SGF (*.sgf)" ),
        &selfilter 
	);
}

/**
 * @brief	SLOT QAction "MenuInfo"
 *			opens a window with information about the application.
 */
void GUI::slot_MenuInfo(){
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

/**
 * @brief	overridden SLOT QCloseEvent "closeEvent"
 *			When trying to close the application a window appears and asks if user is sure.
 *			If answered "yes" the a signal to the backend thread is sent to stop it.
 *			If answered "no" the close event is ignored.
 * @param	QCloseEvent		close event that shall or shall not be executed afterwards.
 */
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