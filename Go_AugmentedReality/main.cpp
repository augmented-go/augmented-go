#include "SgInit.h"
#include "GoInit.h"

#include "BackendThread.hpp"
#include "GUI.hpp"

int main(int argc, char** argv) {
    QApplication qt_app(argc, argv);
    // changes the current working directory and makes all texture and model paths relative to the executable
    QDir::setCurrent(QCoreApplication::applicationDirPath());

    // init fuego
    SgInit();
    GoInit();

    {
        using Go_AR::BackendThread;
        using Go_GUI::GUI;

        BackendThread backend;
        GUI gui;

        // connect signal from backend to gui
        QObject::connect(&backend, &BackendThread::backend_new_image, &gui, &GUI::slot_newImage, Qt::QueuedConnection);
        QObject::connect(&backend, &BackendThread::game_data_changed, &gui, &GUI::slot_newGameData, Qt::QueuedConnection);
        QObject::connect(&backend, &BackendThread::finished_game_result, &gui, &GUI::slot_showFinishedGameResults, Qt::QueuedConnection);

        // connect signal from gui to backend
        QObject::connect(&gui, &GUI::stop_backend_thread, &backend, &BackendThread::stop, Qt::QueuedConnection);
        QObject::connect(&gui, &GUI::signal_saveGame, &backend, &BackendThread::save_sgf, Qt::QueuedConnection);

        backend.start(); // start backend thread

        gui.show();
        qt_app.exec();   // start gui thread (and it's event loop)
    
        backend.quit();  // failsafe: explicitly tell the backend thread to stop (if the gui hasn't done this)
        backend.wait();  // gui was exited, wait for the backend thread (gui should have sent a signal to the backend thread to quit)
    } 

    // "clean up" fuego
    GoFini();
    SgFini();

    return 0;
}