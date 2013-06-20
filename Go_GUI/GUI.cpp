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
#include <QFile>
#include <QFontDatabase>
#include <qscreen.h>
#include <QStackedWidget>
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

	// Pixmaps are saved relative to ui-file. 
	// Here we have to set them again manually to get paths right!
	QPixmap whitebasket_pixmap = QPixmap("../Go_GUI/textures/white_basket.png");
	QPixmap blackbasket_pixmap = QPixmap("../Go_GUI/textures/black_basket.png");
	if (blackbasket_pixmap.isNull() || whitebasket_pixmap.isNull())
		QMessageBox::critical(this, "GUI element not found", QString("White and/or black basket textures not found!"));
	else{
		this->findChild<QLabel* >("white_basket")->setPixmap(whitebasket_pixmap);
		this->findChild<QLabel* >("black_basket")->setPixmap(blackbasket_pixmap);
	}
	
	// loading in font
	QFontDatabase fontDatabase;
	if (fontDatabase.addApplicationFont("../Go_GUI/fonts/SHOJUMARU-REGULAR.TTF") == -1)
		QMessageBox::critical(this, "Font not found", QString("Shojumaru font was not found!"));

	// checking for elements
	auto open_menuitem	= this->findChild<QAction *>("open_action");
	auto exit_menuitem	= this->findChild<QAction *>("exit_action");
	auto info_menuitem	= this->findChild<QAction *>("info_action");
	auto big_layout		= this->findChild<QGridLayout *>("big_layout");
	auto small_layout	= this->findChild<QGridLayout *>("small_layout");
	auto viewswitch_button = this->findChild<QPushButton *>("viewswitch_button");

	if ( open_menuitem == nullptr || exit_menuitem == nullptr || info_menuitem == nullptr
		|| big_layout == nullptr || small_layout == nullptr)
		QMessageBox::critical(this, "GUI element not found", 
							QString("An element of GUI could not be found. (Deleted, renamed?)\n\n Element list:\n " 
							 + ((open_menuitem) ? open_menuitem->objectName()	: "<Open> not found!") + "\n"
							 + ((exit_menuitem) ? exit_menuitem->objectName()	: "<Exit> not found!") + "\n"
							 + ((info_menuitem) ? info_menuitem->objectName()	: "<Info> not found!") + "\n"
							 + ((big_layout)	? big_layout->objectName()		: "<Big view> not found!") + "\n"
							 + ((small_layout)	? small_layout->objectName()	: "<Small view> not found!") + "\n"
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
	this->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	
	virtual_view = new VirtualView(this);
	augmented_view = new AugmentedView(this);

	/** working windows with QGridLayout
	QWidget* big_container = new QWidget();
	augmented_view->setParent(big_container);
	augmented_view->rescaleImage(getBigWindowSize());
	big_container->setObjectName("big_container");
	big_container->setToolTip("augmented view");
	this->findChild<QGridLayout *>("big_layout")->addWidget(big_container);
	QWidget* small_container = QWidget::createWindowContainer(virtual_view, this);
	small_container->setObjectName("small_container");
	small_container->setToolTip("virtual view");
	this->findChild<QGridLayout *>("small_layout")->addWidget(small_container);
	*/

	// working windows with QWidgets (simpler)
	QWidget* big_container = this->findChild<QWidget *>("big_container");
	augmented_view->setParent(big_container);
	augmented_view->rescaleImage(big_container->size());
	big_container->setToolTip("augmented view");

	QWidget* small_container = this->findChild<QWidget *>("small_container");
	QSize small_container_size = small_container->size();	// saving size
	small_container = QWidget::createWindowContainer(virtual_view, small_container, Qt::Widget);
	small_container->resize(small_container_size);
	virtual_view->resize(small_container_size);
	small_container->setToolTip("virtual view");
	

	/** not working windows with QStackedWidget (ownership is taken away!)
	QStackedWidget* big_container = this->findChild<QStackedWidget *>("big_container");
	QStackedWidget* small_container = this->findChild<QStackedWidget *>("small_container");

	big_container->addWidget(augmented_view);
	small_container->addWidget(augmented_view);

	virtual_container = QWidget::createWindowContainer(virtual_view);

	big_container->addWidget(virtual_container);
	small_container->addWidget(virtual_container);

	big_container->setCurrentWidget(augmented_view);
	small_container->setCurrentWidget(virtual_container);
	*/
}

/**
 * @brief	shows and renders a GoBackend::Game
 */
void GUI::RenderGame(GoBackend::Game game) {
	this->show(); // shows Qt5 Window
}


//////////
//Slots
//////////

/**
 * @brief	SLOT "ViewSwitch"
 *			Switches big view with small view.
 *			To assign a view to something a QWidget has to be created.
 */
void GUI::slot_ViewSwitch(){
	QWidget* big_container = this->findChild<QWidget *>("big_container");
	QWidget* small_container = this->findChild<QWidget *>("small_container");

	//QWidget* newbig_container, *newsmall_container;

	/* working windows with QGridLayout
	if (big_container->toolTip() == "augmented view"){
		newbig_container = QWidget::createWindowContainer(virtual_view, this->findChild<QGridLayout *>("big_layout")->widget());
		newbig_container->setObjectName("big_container");
		newbig_container->setToolTip("virtual view");

		newsmall_container = new QWidget(this);
		augmented_view->setParent(newsmall_container);
		augmented_view->rescaleImage(getSmallWindowSize());

		newsmall_container->setObjectName("small_container");
		newsmall_container->setToolTip("augmented view");
	}
	else {
		newbig_container = new QWidget(this);
		augmented_view->setParent(newbig_container);
		augmented_view->rescaleImage(getBigWindowSize());
		newbig_container->setObjectName("big_container");
		newbig_container->setToolTip("augmented view");
		
		newsmall_container = QWidget::createWindowContainer(virtual_view, this->findChild<QGridLayout *>("big_layout")->widget());
		newsmall_container->setObjectName("small_container");
		newsmall_container->setToolTip("virtual view");
	}
	big_container->deleteLater();
	small_container->deleteLater();

	this->findChild<QGridLayout *>("big_layout")->addWidget(newbig_container);
	this->findChild<QGridLayout *>("small_layout")->addWidget(newsmall_container);
	*/

	
	if (big_container->toolTip() == "virtual view"){
		augmented_view->setParent(big_container);
		augmented_view->rescaleImage(big_container->size());
		big_container->setToolTip("augmented view");

		QWidget* small_view = QWidget::createWindowContainer(virtual_view, small_container, Qt::Widget);
		small_view->resize(small_container->size());
		virtual_view->resize(small_container->size());
		small_container->setToolTip("virtual view");

		small_view->show();	// when changing parent, it gets invisible -> show again! -.- !!
		augmented_view->show();		// when changing parent, it gets invisible -> show again! -.- !!
	}
	else if (big_container->toolTip() == "augmented view"){
		augmented_view->setParent(small_container);
		augmented_view->rescaleImage(small_container->size());
		small_container->setToolTip("augmented view");

		QWidget* big_view = QWidget::createWindowContainer(virtual_view, big_container, Qt::Widget);
		big_view->resize(big_container->size());
		virtual_view->resize(big_container->size());
		big_container->setToolTip("virtual view");

		big_view->show();		// when changing parent, it gets invisible -> show again! -.- !!
		augmented_view->show();		// when changing parent, it gets invisible -> show again! -.- !!
	}

	
	//augmented_view->setFixedSize(this->findChild<QGridLayout *>("big_layout")->totalSizeHint());
}

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