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

        QThread worker_thread;
        BackendWorker worker;
        worker.moveToThread(&worker_thread);

        GUI gui;

        qRegisterMetaType<GoRules>("GoRules");

        // connect signal from worker to gui
        QObject::connect(&worker, &BackendWorker::newImage,           &gui, &GUI::slot_newImage,                Qt::QueuedConnection);
        QObject::connect(&worker, &BackendWorker::gameDataChanged,    &gui, &GUI::slot_newGameData,             Qt::QueuedConnection);
        QObject::connect(&worker, &BackendWorker::finishedGameResult, &gui, &GUI::slot_showFinishedGameResults, Qt::QueuedConnection);

        // connect signal from gui to backend
        QObject::connect(&gui, &GUI::stop_backend_thread, &worker_thread, &QThread::quit,      Qt::QueuedConnection);
        QObject::connect(&gui, &GUI::signal_saveGame,     &worker, &BackendWorker::saveSgf,   Qt::QueuedConnection);
        QObject::connect(&gui, &GUI::signal_pass,         &worker, &BackendWorker::pass,      Qt::QueuedConnection);
        QObject::connect(&gui, &GUI::signal_resign,       &worker, &BackendWorker::resign,    Qt::QueuedConnection);
        QObject::connect(&gui, &GUI::signal_newGame,      &worker, &BackendWorker::resetGame, Qt::QueuedConnection);
        QObject::connect(&gui, &GUI::signal_boardDetectionAutomatically, &backend, &BackendThread::selectBoardAutomatically, Qt::QueuedConnection);
        QObject::connect(&gui, &GUI::signal_boardDetectionManually,      &backend, &BackendThread::selectBoardManually,      Qt::QueuedConnection);

        worker_thread.start(); // start worker thread

        gui.show();
        qt_app.exec();   // start gui thread (and it's event loop)
    
        worker_thread.quit();  // failsafe: explicitly tell the worker thread to stop (if the gui hasn't done this already for whatever reason)
        worker_thread.wait();  // gui was exited, wait for the worker thread
    } 

    // "clean up" fuego
    GoFini();
    SgFini();

    return 0;
}