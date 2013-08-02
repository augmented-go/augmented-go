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

        qRegisterMetaType<GoRules>("GoRules");

        // connect signal from backend to gui
        QObject::connect(&backend, &BackendThread::newImage,           &gui, &GUI::slot_newImage,                Qt::QueuedConnection);
        QObject::connect(&backend, &BackendThread::gameDataChanged,    &gui, &GUI::slot_newGameData,             Qt::QueuedConnection);
        QObject::connect(&backend, &BackendThread::finishedGameResult, &gui, &GUI::slot_showFinishedGameResults, Qt::QueuedConnection);

        // connect signal from gui to backend
        QObject::connect(&gui, &GUI::stop_backend_thread, &backend, &BackendThread::stop,      Qt::QueuedConnection);
        QObject::connect(&gui, &GUI::signal_saveGame,     &backend, &BackendThread::saveSgf,   Qt::QueuedConnection);
        QObject::connect(&gui, &GUI::signal_pass,         &backend, &BackendThread::pass,      Qt::QueuedConnection);
        QObject::connect(&gui, &GUI::signal_resign,       &backend, &BackendThread::resign,    Qt::QueuedConnection);
        QObject::connect(&gui, &GUI::signal_newGame,      &backend, &BackendThread::resetGame, Qt::QueuedConnection);
        QObject::connect(&gui, &GUI::signal_toggleAppMode,      &backend, &BackendThread::toggleAppMode, Qt::QueuedConnection);
        QObject::connect(&gui, &GUI::signal_playMove,     &backend, &BackendThread::playMove,  Qt::QueuedConnection);
        QObject::connect(&gui, &GUI::signal_boardDetectionAutomatically, &backend, &BackendThread::selectBoardAutomatically, Qt::QueuedConnection);
        QObject::connect(&gui, &GUI::signal_boardDetectionManually,      &backend, &BackendThread::selectBoardManually,      Qt::QueuedConnection);

        backend.start(); // start backend thread

        gui.show();
        qt_app.exec();   // start gui thread (and it's event loop)
    
        backend.quit();  // failsafe: explicitly tell the backend thread to stop (if the gui hasn't done this already for whatever reason)
        backend.wait();  // gui was exited, wait for the backend thread
    } 

    // "clean up" fuego
    GoFini();
    SgFini();

    return 0;
}