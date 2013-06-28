#include "GUI.hpp"

#include <string>
#include <vector>

#include <QMessageBox>
#include <QAction>
#include <QCloseEvent>
#include <QFontDatabase>
#include "Game.hpp"

#include "VirtualView.hpp"
#include "AugmentedView.hpp"


namespace Go_GUI {
    
/**
 * @brief	Checks for gui elements and fonts and connects signals and slots
 * @param	QWidget/QMainWindow		parent widget that creates this
 */
GUI::GUI(QWidget *parent) : QMainWindow(parent)
{
    ui.setupUi(this);

    QString texture_path = "res/textures/";
    whitebasket_pixmap = QPixmap(texture_path + "white_basket.png");
    blackbasket_pixmap = QPixmap(texture_path + "black_basket.png");
    closedbasket_pixmap = QPixmap(texture_path + "Closed_basket.png");
	gotable_pixmap = QPixmap(texture_path + "go_table.png");
    if (blackbasket_pixmap.isNull() || whitebasket_pixmap.isNull() 
		|| closedbasket_pixmap.isNull() || gotable_pixmap.isNull())
        QMessageBox::critical(this, "GUI element not found", QString("A texture was not found!\n searched relative to exe in: " + texture_path));
    
    // loading font
    QFontDatabase fontDatabase;
    QString font_path = "res/fonts/SHOJUMARU-REGULAR.TTF";
    if (fontDatabase.addApplicationFont(font_path) == -1)
        QMessageBox::critical(this, "Font not found", QString("Shojumaru font was not found!\n searched relative to exe in: " + font_path));

    // checking for elements
    auto open_menuitem	= this->findChild<QAction *>("open_action");
    auto save_menuitem	= this->findChild<QAction *>("save_action");
    auto exit_menuitem	= this->findChild<QAction *>("exit_action");
    auto info_menuitem	= this->findChild<QAction *>("info_action");
    auto big_container	= this->findChild<QWidget *>("big_container");
    auto small_container= this->findChild<QWidget *>("small_container");
    auto viewswitch_button = this->findChild<QPushButton *>("viewswitch_button");
    auto capturedwhite_label = this->findChild<QLabel *>("capturedwhite_label");
    auto capturedblack_label = this->findChild<QLabel *>("capturedblack_label");
	auto whiteplayer_label = this->findChild<QLabel *>("whiteplayer_label");
    auto blackplayer_label = this->findChild<QLabel *>("blackplayer_label");
	auto go_table_label = this->findChild<QLabel *>("go_table_label");

    // throwing an error message of elements that were not found
    if ( open_menuitem == nullptr || exit_menuitem == nullptr || info_menuitem == nullptr
        || save_menuitem == nullptr	|| big_container == nullptr || small_container == nullptr
        || capturedwhite_label == nullptr || capturedblack_label == nullptr 
		|| whiteplayer_label == nullptr || blackplayer_label == nullptr || go_table_label == nullptr)
        QMessageBox::critical(this, "GUI element not found", 
                            QString("An element of GUI could not be found. (Deleted, renamed?)\n\n Element list:\n " 
                             + ((open_menuitem) ? open_menuitem->objectName()	: "<Open> not found!") + "\n"
                             + ((save_menuitem) ? save_menuitem->objectName()	: "<Save> not found!") + "\n"
                             + ((exit_menuitem) ? exit_menuitem->objectName()	: "<Exit> not found!") + "\n"
                             + ((info_menuitem) ? info_menuitem->objectName()	: "<Info> not found!") + "\n"
                             + ((big_container)	? big_container->objectName()	: "<Big container> not found!") + "\n"
                             + ((small_container) ? small_container->objectName()	: "<Small container> not found!") + "\n"
                             + ((capturedwhite_label) ? capturedwhite_label->objectName()	: "<Captured white label> not found!") + "\n"
                             + ((capturedblack_label) ? capturedblack_label->objectName()	: "<Captured black label> not found!") + "\n"
							 + ((whiteplayer_label) ? whiteplayer_label->objectName()	: "<player1 label> not found!") + "\n"
							 + ((blackplayer_label) ? blackplayer_label->objectName()	: "<player2 label> not found!") + "\n"
							 + ((go_table_label) ? go_table_label->objectName()	: "<Go table label> not found!") + "\n"
                             ));

    // connections
    connect(open_menuitem,		&QAction::triggered,	this, &GUI::slot_MenuOpen);
    connect(save_menuitem,		&QAction::triggered,	this, &GUI::slot_MenuSave);
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

    // Attaching augmented view to big container
    QWidget* big_container = this->findChild<QWidget *>("big_container");
    augmented_view->setParent(big_container);
    augmented_view->rescaleImage(big_container->size());
    big_container->setToolTip("augmented view");
	
    // Attaching virtual view to small container
    QWidget* small_container = this->findChild<QWidget *>("small_container");
    QSize small_container_size = small_container->size();	// saving size
    small_container = QWidget::createWindowContainer(virtual_view, small_container, Qt::Widget);
    small_container->resize(small_container_size);
    virtual_view->resize(small_container_size);
    small_container->setToolTip("virtual view");
	
    this->findChild<QLabel* >("white_basket")->setPixmap(closedbasket_pixmap);
    this->findChild<QLabel* >("black_basket")->setPixmap(closedbasket_pixmap);
	this->findChild<QLabel* >("go_table_label")->setPixmap(gotable_pixmap);

    this->findChild<QLabel* >("capturedwhite_label")->setText(QString());
    this->findChild<QLabel* >("capturedblack_label")->setText(QString());

	setPlayerLabels("Player 1", "Player 2");
}

/**
 * @brief	sets labels and variables for player names.
 * @param	QString		name of white player (default: "Player 1"
 * @param	QString		name of black player (default: "Player 2"
 */
void GUI::setPlayerLabels(QString whiteplayer_name, QString blackplayer_name){
	this->findChild<QLabel* >("whiteplayer_label")->setText(whiteplayer_name);
	this->findChild<QLabel* >("blackplayer_label")->setText(blackplayer_name);
	this->whiteplayer_name = whiteplayer_name;
	this->blackplayer_name = blackplayer_name;
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
    
    if (big_container->toolTip() == "virtual view"){
        // switching augmented view to big container
        augmented_view->setParent(big_container);
        augmented_view->rescaleImage(big_container->size());
        big_container->setToolTip("augmented view");

        // switching virtual view to small container
        QWidget* small_view = QWidget::createWindowContainer(virtual_view, small_container, Qt::Widget);
        small_view->resize(small_container->size());
        virtual_view->resize(small_container->size());
        small_container->setToolTip("virtual view");

        small_view->show();			// when changing parent, it gets invisible -> show again! -.- !!
        augmented_view->show();		// when changing parent, it gets invisible -> show again! -.- !!
    }
    else if (big_container->toolTip() == "augmented view"){
        // switching augmented view to small container
        augmented_view->setParent(small_container);
        augmented_view->rescaleImage(small_container->size());
        small_container->setToolTip("augmented view");

        // switching virtual view to big container
        QWidget* big_view = QWidget::createWindowContainer(virtual_view, big_container, Qt::Widget);
        big_view->resize(big_container->size());
        virtual_view->resize(big_container->size());
        big_container->setToolTip("virtual view");

        big_view->show();			// when changing parent, it gets invisible -> show again! -.- !!
        augmented_view->show();		// when changing parent, it gets invisible -> show again! -.- !!
    }
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
 * @brief	SLOT QAction "MenuOpen"
 *			opens a filedialog that lets the user choose an sgf-file.
 *			The file name can be null if the user cancels the file dialog.
 */
void GUI::slot_MenuSave(){
    QString selfilter = tr("SGF (*.sgf)");
    QString fileName = QFileDialog::getSaveFileName(
        this,
        "save sgf-file",
        NULL,
        tr("SGF (*.sgf)" ),
        &selfilter 
    );
	
	if (!fileName.isNull())
		emit signal_saveGame(this->whiteplayer_name, this->blackplayer_name, fileName);
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