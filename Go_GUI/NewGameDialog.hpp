// Copyright (c) 2013 augmented-go team
// See the file LICENSE for full license and copying terms.
#pragma once

#include <QDialog>

#include "ui_NewGameDialog.h"
#include "GUI.hpp"

/**
 * @class   NewGameDialog
 * @brief   The NewGameDialog is part of Augmented-Go Gui.  Uses the NewGameDialog.ui
 *          The Dialog lets the user choose several settings for a go-game listed below:
 *           - Name of the game
 *           - Name of black and white player
 *           - Rules: Komi, Japanese scoring and if two consecutive passes end the game
 *          When the user presses "OK" the information gets signaled and the dialog closes.
 */
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
     *             If OK is pressed, a signal is emitted to the backend
     *             to start a new game.
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
     * @param   float       komi
     */
    void signal_newgame(QString gamename, QString blackplayername, QString whiteplayername, float komi);

private:
    Ui::Dialog ui_newgame;
};