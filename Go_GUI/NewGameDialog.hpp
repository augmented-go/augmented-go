#pragma once

#include <QDialog>

class NewGameDialog : public QDialog
{
    Q_OBJECT
public:
    NewGameDialog(QWidget *parent = 0){
       ui_newgame.setupUi(this);
       // TODO connect signal to gui slot
    };
    ~NewGameDialog(){};

    

    /**
     * @brief      overides the "OK"-Button of the dialog.
     *             If OK is pressed, a new game starts.
     */
    void accept(){
        // TODO emit changes made inside dialog
        emit signal_newgame("game", "black", "white");
        close();
    }

signals:
    /**
     * @brief   emits a signal to the backend that the user wants to start a new game
     * @param   QString     Name of the game
     * @param   QString     Name of black player
     * @param   QString     Name of white player
     */
    void signal_newgame(QString gamename, QString blackplayername, QString whiteplayername);

private:
    Ui::Dialog ui_newgame;
};