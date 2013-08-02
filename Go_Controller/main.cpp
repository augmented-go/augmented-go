#include "SgInit.h"
#include "GoInit.h"

#include "BackendWorker.hpp"
#include "GUI.hpp"

int main(int argc, char** argv) {
    QApplication qt_app(argc, argv);
    // changes the current working directory and makes all texture and model paths relative to the executable
    QDir::setCurrent(QCoreApplication::applicationDirPath());

    // init fuego
    SgInit();
    GoInit();

    {
        using Go_AR::BackendWorker;
        using Go_GUI::GUI;

        BackendWorker backend;
        GUI gui;

        qRegisterMetaType<GoRules>("GoRules");

        // connect signal from backend to gui
        QObject::connect(&backend, &BackendWorker::newImage,           &gui, &GUI::slot_newImage,                Qt::QueuedConnection);
        QObject::connect(&backend, &BackendWorker::gameDataChanged,    &gui, &GUI::slot_newGameData,             Qt::QueuedConnection);
        QObject::connect(&backend, &BackendWorker::finishedGameResult, &gui, &GUI::slot_showFinishedGameResults, Qt::QueuedConnection);

        // connect signal from gui to backend
        QObject::connect(&gui, &GUI::stop_backend_thread, &backend, &BackendWorker::stop,      Qt::QueuedConnection);
        QObject::connect(&gui, &GUI::signal_saveGame,     &backend, &BackendWorker::saveSgf,   Qt::QueuedConnection);
        QObject::connect(&gui, &GUI::signal_pass,         &backend, &BackendWorker::pass,      Qt::QueuedConnection);
        QObject::connect(&gui, &GUI::signal_resign,       &backend, &BackendWorker::resign,    Qt::QueuedConnection);
        QObject::connect(&gui, &GUI::signal_newGame,      &backend, &BackendWorker::resetGame, Qt::QueuedConnection);

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