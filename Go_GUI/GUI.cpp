#include "GUI.hpp"

#include <string>
#include <vector>

#include <QMessageBox>
#include <QAction>
#include <QCloseEvent>
#include <QFontDatabase>
#include "Game.hpp"

#include "NewGameDialog.hpp"
#include "VirtualView.hpp"
#include "AugmentedView.hpp"


namespace Go_GUI {
    
/**
 * @brief	Checks for gui elements and fonts and connects signals and slots
 * @param	QWidget/QMainWindow		parent widget that creates this
 */
GUI::GUI(QWidget *parent) : QMainWindow(parent), go_game(nullptr)
{
    ui_main.setupUi(this);

    virtual_view = new VirtualView(this);
    augmented_view = new AugmentedView(this);

    QString texture_path = "res/textures/";
    whitebasket_pixmap = QPixmap(texture_path + "white_basket.png");
    blackbasket_pixmap = QPixmap(texture_path + "black_basket.png");
    closedbasket_pixmap = QPixmap(texture_path + "Closed_basket.png");
    gotable_pixmap = QPixmap(texture_path + "go_table.png");
    if (blackbasket_pixmap.isNull() || whitebasket_pixmap.isNull() || closedbasket_pixmap.isNull() || gotable_pixmap.isNull())
        QMessageBox::critical(this, "GUI element not found", QString("White and/or black basket textures not found!\n searched relative to exe in: " + texture_path));
    
    // loading font
    QFontDatabase fontDatabase;
    QString font_path = "res/fonts/SHOJUMARU-REGULAR.TTF";
    if (fontDatabase.addApplicationFont(font_path) == -1)
        QMessageBox::critical(this, "Font not found", QString("Shojumaru font was not found!\n searched relative to exe in: " + font_path));

    // connections
    connect(ui_main.open_action,		&QAction::triggered,	this, &GUI::slot_MenuOpen);
    connect(ui_main.save_action,		&QAction::triggered,	this, &GUI::slot_MenuSave);
    connect(ui_main.exit_action,		&QAction::triggered,	this, &QWidget::close);	
    connect(ui_main.info_action,		&QAction::triggered,	this, &GUI::slot_MenuInfo);
    connect(ui_main.automatic_action,   &QAction::triggered,	this, &GUI::slot_BoardDetectionAutomatically);
    connect(ui_main.manually_action,	&QAction::triggered,	this, &GUI::slot_BoardDetectionManually);
    connect(ui_main.toggle_app_mode_action,	&QAction::triggered, this, &GUI::slot_ToggleAppMode);
    connect(ui_main.viewswitch_button,	&QPushButton::clicked,	this, &GUI::slot_ViewSwitch);
    connect(ui_main.newgame_button,	    &QPushButton::clicked,	this, &GUI::slot_ButtonNewGame);
    connect(ui_main.pass_button,	    &QPushButton::clicked,	this, &GUI::slot_ButtonPass);
    connect(ui_main.resign_button,	    &QPushButton::clicked,	this, &GUI::slot_ButtonResign);
    connect(this->virtual_view,	        &VirtualView::signal_virtualViewplayMove,	this, &GUI::slot_passOnVirtualViewPlayMove);

    // setting initial values
    this->init();
}

/**
 * @brief	Sets initial settings, texts and windows
 */
void GUI::init(){
    this->setWindowTitle("Augmented Go");
    this->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    
    // Attaching augmented view to big container
    augmented_view->setParent(ui_main.big_container);
    augmented_view->rescaleImage(ui_main.big_container->size());
    ui_main.big_container->setToolTip("augmented view");
    augmented_view->show();

    // Attaching virtual view to small container
    virtual_view->setParent(ui_main.small_container);
    ui_main.small_container->setToolTip("virtual view");
    virtual_view->show();

    ui_main.white_basket->setPixmap(closedbasket_pixmap);
    ui_main.black_basket->setPixmap(closedbasket_pixmap);
    ui_main.go_table_label->setPixmap(gotable_pixmap);

    ui_main.capturedwhite_label->setText(QString());
    ui_main.capturedblack_label->setText(QString());
}

/**
 * @brief	sets labels and variables for player names.
 * @param	QString		name of black player (default: "Black"
 * @param	QString		name of white player (default: "White"
 */
void GUI::setPlayerLabels(QString blackplayer_name, QString whiteplayer_name){
    ui_main.blackplayer_label->setText(blackplayer_name);
    ui_main.whiteplayer_label->setText(whiteplayer_name);
}

//////////
//Public Slots
//////////

/**
 * @brief   SLOT "new image"
 *          If a new image is sent to GUI, refresh and rescale picture.
 * @param   QImage  new image from scanner
 */
void GUI::slot_newImage(QImage image) {
        printf(">>> New Image arrived! '%d x %d' -- Format: %d <<<\n", image.width(), image.height(), image.format());

        augmented_view->setImage(image);
        augmented_view->rescaleImage(augmented_view->parentWidget()->size());
    }

/**
 * @brief   SLOT "new game data"
 *          If new game data is sent to GUI, refresh display of current player and captured stones.
 * @param   game     new game representation
 */
void GUI::slot_newGameData(const GoBackend::Game* game) {
    // update internal pointer if the board has been changed
    if (game != game)
        game = game;

    auto& board = game->getBoard();

    auto current_player = board.ToPlay();

    // Updating basket pictures
    switch (current_player) {
    case SG_WHITE:
        ui_main.white_basket->setPixmap(whitebasket_pixmap);
        ui_main.black_basket->setPixmap(closedbasket_pixmap);
        break;
    case SG_BLACK:
        ui_main.white_basket->setPixmap(closedbasket_pixmap);
        ui_main.black_basket->setPixmap(blackbasket_pixmap);
        break;
    default:
        assert(false);
        break;
    }

    // Updating Game-Settings
    ui_main.movenumber_label->setText(QString::number(board.MoveNumber()));
    ui_main.kominumber_label->setText(QString::number(board.Rules().Komi().ToFloat()));
    ui_main.handicapnumber_label->setText(QString::number(board.Rules().Handicap()));
    ui_main.capturedwhite_label->setText(QString::number(board.NumPrisoners(SG_WHITE)));
    ui_main.capturedblack_label->setText(QString::number(board.NumPrisoners(SG_BLACK)));

    // refresh virtual view
    if (ui_main.big_container->toolTip() == "virtual view")
        virtual_view->createAndSetScene(ui_main.big_container->size(), &board);
    
    else if (ui_main.big_container->toolTip() == "augmented view")
        virtual_view->createAndSetScene(ui_main.small_container->size(), &board);
    
    printf(">>> New Game data! <<<\n");
}    

/**
 * @brief   SLOT "Show finished game results"
 *          If a game ended, the BackendThread sends a signal with the results.
 *          Here the results are shown to the user.
 */
void GUI::slot_showFinishedGameResults(QString result){
    QMessageBox::information(this, "Game results", result);
    auto answer = QMessageBox::question(this, "New Game?", "Do you want to start a new game?");
    if (answer == QMessageBox::Yes)
        this->slot_ButtonNewGame();
}

/**
 * @brief   SLOT "setup new game"
 *          When a new game has been started, setup game name and player names on gui.
 * @param   QString    game name
 * @param   QString    black player name
 * @param   QString    white player name
 */
void GUI::slot_setupNewGame(QString game_name, QString blackplayer_name, QString whiteplayer_name, float komi){

    // emit to backend that gui wants to set up a new game!
    auto rules = GoRules(0, GoKomi(komi), true, true);
    emit signal_newGame(rules);

    // Setting up new names for players
    ui_main.gamename_label->setText(game_name);
    ui_main.blackplayer_label->setText(blackplayer_name);
    ui_main.whiteplayer_label->setText(whiteplayer_name);
    ui_main.kominumber_label->setText(QString::number(komi));
    ui_main.handicapnumber_label->setText(QString::number(0));
}

//////////
//Private Slots
//////////

/**
 * @brief   SLOT "NewGame/Reset"
 *          Opens a Dialog that asks for game rules and names.
 */
void GUI::slot_ButtonNewGame(){
    NewGameDialog* newgame = new NewGameDialog(this);
    newgame->exec();
}

/**
 * @brief   SLOT "Resign"
 *          Opens a Dialog that asks for confirmation.
 *			If answered yes, a signal is sent to backend that the current player surrenders.
 */
void GUI::slot_ButtonResign(){
    if (QMessageBox::question(this, "Resign", "Do you really want to admit defeat?") == QMessageBox::Yes)
        emit signal_resign();
}

/**
 * @brief   SLOT "Pass"
 *          Opens a Dialog that asks for confirmation.
 *			If answered yes, a signal is sent to backend that the current player passes.
 */
void GUI::slot_ButtonPass(){
    if (QMessageBox::question(this, "Pass", "Do you really want to pass?") == QMessageBox::Yes)
        emit signal_pass();
}

/**
 * @brief	SLOT "ViewSwitch"
 *			Switches big view with small view.
 *			To assign a view to something a QWidget has to be created.
 */
void GUI::slot_ViewSwitch(){
    if (ui_main.big_container->toolTip() == "virtual view"){

        // switching augmented view to big container
        augmented_view->setParent(ui_main.big_container);
        augmented_view->rescaleImage(ui_main.big_container->size());
        ui_main.big_container->setToolTip("augmented view");
        augmented_view->show();		// when changing parent, it gets invisible -> show again! -.- !!

        // new style
        virtual_view->setParent(ui_main.small_container);
        virtual_view->createAndSetScene(ui_main.small_container->size(), &(go_game->getBoard()));
        ui_main.small_container->setToolTip("virtual view");
        virtual_view->show();
        
    }
    else if (ui_main.big_container->toolTip() == "augmented view"){
        // switching augmented view to small container
        augmented_view->setParent(ui_main.small_container);
        augmented_view->rescaleImage(ui_main.small_container->size());
        ui_main.small_container->setToolTip("augmented view");
        augmented_view->show();		// when changing parent, it gets invisible -> show again! -.- !!

        virtual_view->setParent(ui_main.big_container);
        virtual_view->createAndSetScene(ui_main.big_container->size(), &(go_game->getBoard()));
        ui_main.big_container->setToolTip("virtual view");
        virtual_view->show(); 
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

    if (!fileName.isNull()){
        // TODO ask if user wants to save the current game!
        emit signal_openGame(fileName);
    }
}

/**
 * @brief	SLOT QAction "MenuSave"
 *			opens a filedialog that lets the user choose an sgf-file.
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
        emit signal_saveGame(fileName, ui_main.blackplayer_label->text(), ui_main.whiteplayer_label->text(), this->game_name);
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

void GUI::slot_BoardDetectionManually() {
    emit signal_boardDetectionManually();
}

void GUI::slot_BoardDetectionAutomatically() {
    emit signal_boardDetectionAutomatically();
}

void GUI::slot_ToggleAppMode() {
    emit signal_toggleAppMode();
}

/**
 * @brief	overridden SLOT QCloseEvent "closeEvent"
 *			When trying to close the application a window appears and asks if user is sure.
 *			If answered "yes" the a signal to the backend thread is sent to stop it.
 *			If answered "no" the close event is ignored.
 * @param	QCloseEvent		close event that shall or shall not be executed afterwards.
 */
void GUI::closeEvent(QCloseEvent *event){

    // If at least one move was was done
    // TODO If game loaded -> move number greater than start number!
    if (ui_main.movenumber_label->text().toInt() > 0){
        if (QMessageBox::question(this, "Save?", "Save current game?") == QMessageBox::Yes)
            this->slot_MenuSave();
    }

    // Ask if the user wants to quit
    if (QMessageBox::question(this, "Quit?", "Do you really want to quit?") == QMessageBox::Yes){
        emit stop_backend_thread();
        event->accept();
    }
    else
        event->ignore();
    
}

void GUI::slot_passOnVirtualViewPlayMove(const int x, const int y){
    emit this->signal_playMove(x, y);
}

} // namespace Go_GUI