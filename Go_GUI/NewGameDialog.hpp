#pragma once

#include <QDialog>

#include "GUI.hpp"

class NewGameDialog : public QDialog
{
    Q_OBJECT
public:
    NewGameDialog(Go_GUI::GUI *parent = 0){
       ui_newgame.setupUi(this);
       connect(this, &NewGameDialog::signal_newgame, parent, &Go_GUI::GUI::slot_setupNewGame);
    };
    ~NewGameDialog(){};

    

    /**
     * @brief      overides the "OK"-Button of the dialog.
     *             If OK is pressed, a new game starts.
     */
    void accept(){
        // empty names are not allowed!
        if (ui_newgame.gamename_lineEdit->text().isEmpty()
            || ui_newgame.blackplayername_lineEdit->text().isEmpty()
            || ui_newgame.whiteplayername_lineEdit->text().isEmpty()){
            QMessageBox::warning(this, "Error", "Empty names are not allowed!");
            return;
        }

        emit signal_newgame(ui_newgame.gamename_lineEdit->text(), 
                            ui_newgame.blackplayername_lineEdit->text(), 
                            ui_newgame.whiteplayername_lineEdit->text(), 
                            static_cast<float>(ui_newgame.komi_spinbox->value()));
        close();
    }

signals:
    /**
     * @brief   emits a signal to the backend that the user wants to start a new game
     * @param   QString     Name of the game
     * @param   QString     Name of black player
     * @param   QString     Name of white player
     */
    void signal_newgame(QString gamename, QString blackplayername, QString whiteplayername, float komi);

private:
    Ui::Dialog ui_newgame;
};