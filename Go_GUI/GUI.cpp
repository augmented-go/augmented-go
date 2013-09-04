#include "GUI.hpp"

#include <string>
#include <vector>

#include <QMessageBox>
#include <QAction>
#include <QCloseEvent>
#include <QFontDatabase>
#include "Game.hpp"

#include "NewGameDialog.hpp"
#include "ChangeScanRateDialog.hpp"
#include "VirtualView.hpp"
#include "AugmentedView.hpp"


namespace Go_GUI {
    

GUI::GUI(QWidget *parent)
    : QMainWindow(parent),
    go_game(nullptr),
    current_scanning_fps(1)
{
    ui_main.setupUi(this);
    
    texture_path = "res/textures/";
    QApplication::setWindowIcon(QIcon(QPixmap(texture_path + "augmented_logo_transparent_icon.png")));
    augmented_logo = QImage(texture_path + "augmented_logo.png");

    virtual_view = new VirtualView(this);
    augmented_view = new AugmentedView(this);

    switchbutton_icon = QIcon(texture_path + "Arrow_SwitchButton.png");
    switchbuttonpressed_icon = QIcon(texture_path + "Arrow_SwitchButton_pressed.png");
    
    whitebasket_pixmap = QPixmap(texture_path + "white_basket.png");
    blackbasket_pixmap = QPixmap(texture_path + "black_basket.png");
    closedbasket_pixmap = QPixmap(texture_path + "Closed_basket.png");
    gotable_pixmap = QPixmap(texture_path + "go_table.png");
    if (blackbasket_pixmap.isNull() || whitebasket_pixmap.isNull() || closedbasket_pixmap.isNull() || gotable_pixmap.isNull())
        QMessageBox::critical(this, "GUI element not found", QString("White and/or black basket textures not found!\n searched relative to exe in: " + texture_path));
    
    // loading font
    QFontDatabase fontDatabase;
    QString font_path = "res/fonts/FromWhereYouAre.ttf";
    if (fontDatabase.addApplicationFont(font_path) == -1)
        QMessageBox::critical(this, "Font not found", QString("FromWhereYouAre font was not found!\n searched relative to exe in: " + font_path));

    // connections
    connect(ui_main.open_action,		&QAction::triggered,	this, &GUI::slot_MenuOpen);
    connect(ui_main.save_action,		&QAction::triggered,	this, &GUI::slot_MenuSave);
    connect(ui_main.exit_action,		&QAction::triggered,	this, &QWidget::close);	
    connect(ui_main.info_action,		&QAction::triggered,	this, &GUI::slot_MenuInfo);
    connect(ui_main.automatic_action,   &QAction::triggered,	this, &GUI::slot_BoardDetectionAutomatically);
    connect(ui_main.manually_action,	&QAction::triggered,	this, &GUI::slot_BoardDetectionManually);
    connect(ui_main.virtual_game_mode_action,	&QAction::triggered, this, &GUI::slot_ToggleVirtualGameMode);
    connect(this,	&GUI::signal_setVirtualGameMode, this->virtual_view, &VirtualView::slot_setVirtualGameMode);
    connect(ui_main.viewswitch_button,	&QPushButton::pressed,	this, &GUI::slot_ViewSwitch);
    connect(ui_main.viewswitch_button,	&QPushButton::released,	this, &GUI::slot_ViewSwitch_released);
    connect(ui_main.newgame_button,	    &QPushButton::clicked,	this, &GUI::slot_ButtonNewGame);
    connect(ui_main.pass_button,	    &QPushButton::clicked,	this, &GUI::slot_ButtonPass);
    connect(ui_main.resign_button,	    &QPushButton::clicked,	this, &GUI::slot_ButtonResign);
    connect(ui_main.backward_button,    &QPushButton::clicked,    this, &GUI::slot_HistoryBackward);
    connect(ui_main.forward_button,     &QPushButton::clicked,    this, &GUI::slot_HistoryForward);
    connect(this->virtual_view,	        &VirtualView::signal_virtualViewplayMove,	this, &GUI::slot_passOnVirtualViewPlayMove);
    connect(ui_main.scannerdebugimage_action,	&QAction::triggered,	this, &GUI::slot_toggleScannerDebugImage);
    
   
    connect(ui_main.scanning_rate_action, &QAction::triggered,	this, &GUI::slot_MenuChangeScanRate);
    // setting initial values
    this->init();
}

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

    ui_main.viewswitch_button->setIcon(switchbutton_icon);

    ui_main.capturedwhite_label->setText(QString());
    ui_main.capturedblack_label->setText(QString());

    emit signal_setVirtualGameMode(ui_main.virtual_game_mode_action->isChecked());

    // initially disable board selection buttons, they get enabled again when the first camera picture arrives
    slot_noCameraImage();
}

void GUI::setPlayerLabels(QString blackplayer_name, QString whiteplayer_name){
    ui_main.blackplayer_label->setText(blackplayer_name);
    ui_main.whiteplayer_label->setText(whiteplayer_name);
}


//////////
//Private Slots
//////////

void GUI::slot_ButtonNewGame(){
    NewGameDialog newgame(this);
    newgame.exec();
}

void GUI::slot_ButtonResign(){
    if (QMessageBox::question(this, "Resign", "Do you really want to admit defeat?") == QMessageBox::Yes)
        emit signal_resign();
}

void GUI::slot_ButtonPass(){
    if (QMessageBox::question(this, "Pass", "Do you really want to pass?") == QMessageBox::Yes)
        emit signal_pass();
}

void GUI::slot_ViewSwitch(){
    ui_main.viewswitch_button->setIcon(this->switchbuttonpressed_icon);

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

void GUI::slot_ViewSwitch_released(){
    ui_main.viewswitch_button->setIcon(this->switchbutton_icon);
}

void GUI::slot_HistoryBackward(){
    emit signal_navigateHistory(SgNode::Direction::PREVIOUS);
}

void GUI::slot_HistoryForward(){
    emit signal_navigateHistory(SgNode::Direction::NEXT);
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

void GUI::slot_MenuChangeScanRate() {
    ChangeScanRateDialog scan_rate_dialog(this, current_scanning_fps);
    scan_rate_dialog.exec();
}

void GUI::slot_BoardDetectionManually() {
    emit signal_boardDetectionManually();
}

void GUI::slot_BoardDetectionAutomatically() {
    emit signal_boardDetectionAutomatically();
}

void GUI::slot_ToggleVirtualGameMode() {
    // if in augmented mode -> switch to virtual
    if (ui_main.virtual_game_mode_action->isChecked()){
        this->setWindowTitle("Augmented Go - Virtual Mode");

        // change virtual view to big container
        if (ui_main.big_container->toolTip() != "virtual view")
            this->slot_ViewSwitch();
    
        emit signal_setVirtualGameMode(true);
        slot_newImage(augmented_logo);
    }

    // if in virtual mode -> switch to augmented
    else{
        this->setWindowTitle("Augmented Go");

        // change augmented view to big container
        if (ui_main.big_container->toolTip() != "augmented view")
            this->slot_ViewSwitch();
        augmented_view->setStyleSheet("background-color: black");
    
        emit signal_setVirtualGameMode(false);
    }
}

void GUI::slot_passOnVirtualViewPlayMove(const int x, const int y){
    emit this->signal_playMove(x, y);
}

void GUI::slot_toggleScannerDebugImage()
{
    if (ui_main.scannerdebugimage_action->isChecked())
        emit signal_setScannerDebugImage(true);
    else
        emit signal_setScannerDebugImage(false);
}


void GUI::slot_changeScanRate(int fps) {
    current_scanning_fps = fps;

    // convert fps to ms
    auto milliseconds = fps == 0 ? 0 : 1000.f / current_scanning_fps;

    emit signal_new_scanning_rate(milliseconds);
}

//////////
//Public Slots
//////////

void GUI::slot_newImage(QImage image) {
        printf(">>> New Image arrived! '%d x %d' -- Format: %d <<<\n", image.width(), image.height(), image.format());
        augmented_view->setImage(image);
        augmented_view->rescaleImage(augmented_view->parentWidget()->size());

        // we got an image, so board contours can be selected now
        ui_main.automatic_action->setEnabled(true);
        ui_main.manually_action->setEnabled(true);
    }

void GUI::slot_newGameData(const GoBackend::Game* game) {
    // update internal pointer if the board has been changed
    if (go_game != game)
        go_game = game;

    auto& board = go_game->getBoard();

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
    
    // disable navigation button if there is no history in that direction
    ui_main.forward_button->setDisabled(!go_game->canNavigateHistory(SgNode::Direction::NEXT));
    ui_main.backward_button->setDisabled(!go_game->canNavigateHistory(SgNode::Direction::PREVIOUS));

    printf(">>> New Game data! <<<\n");
}    

void GUI::slot_showFinishedGameResults(QString result){
    QMessageBox::information(this, "Game results", result);
    auto answer = QMessageBox::question(this, "New Game?", "Do you want to start a new game?");
    if (answer == QMessageBox::Yes)
        this->slot_ButtonNewGame();
}

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

void GUI::slot_displayErrorMessage(QString message) {
    if (message == "") {
        ui_main.error_label->setText(message);
        ui_main.error_label->lower();
    }
    else {
        ui_main.error_label->raise();
        ui_main.error_label->setText(message);
    }
}

void GUI::slot_noCameraImage() {
    ui_main.automatic_action->setEnabled(false);
    ui_main.manually_action->setEnabled(false);

    // we could also display a placeholder image here
    // currently the last image stays displayed
}

///////////
//Events
///////////

void GUI::closeEvent(QCloseEvent *event){

    // If at least one move was was done
    // TODO If game loaded -> move number greater than start number!
    int answer = 0;

    // if at least one move was made -> ask if user wants to save
    bool saveable = ui_main.movenumber_label->text().toInt() > 0;

    if (saveable)
        answer = QMessageBox::question(this, "Save?", "Do you want to save before quitting?", "Save", "Don't Save", "Cancel");
    else
        answer = QMessageBox::question(this, "Quit?", "Do you really want to quit?", "Quit", "Cancel");
    
    if(answer == 0 && saveable){
        this->slot_MenuSave();
        emit stop_backend_thread();
        event->accept();
    }
    else if((answer == 1 && saveable)
        || (answer == 0 && !saveable)){
        emit stop_backend_thread();
        event->accept();
    }
    else
        event->ignore();
}


} // namespace Go_GUI