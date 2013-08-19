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

        GUI gui;

        // The worker can't be on the stack because of Qts signals when object are destroyed,
        // these would trigger assertion errors that say that you can't send events to a different thread
        auto worker = new BackendWorker();

        QThread worker_thread;
        QObject::connect( &worker_thread, SIGNAL(finished()), worker, SLOT(deleteLater()) ); // clean up the worker when the thread is stopped

        // move the worker into the thread
        worker->moveToThread(&worker_thread);

        qRegisterMetaType<GoRules>("GoRules");

        // connect signal from worker to gui
        QObject::connect(worker, &BackendWorker::newImage,           &gui, &GUI::slot_newImage);
        QObject::connect(worker, &BackendWorker::gameDataChanged,    &gui, &GUI::slot_newGameData);
        QObject::connect(worker, &BackendWorker::finishedGameResult, &gui, &GUI::slot_showFinishedGameResults);

        // connect signal from gui to worker
        QObject::connect(&gui, &GUI::signal_saveGame,                    worker, &BackendWorker::saveSgf);
        QObject::connect(&gui, &GUI::signal_pass,                        worker, &BackendWorker::pass);
        QObject::connect(&gui, &GUI::signal_resign,                      worker, &BackendWorker::resign);
        QObject::connect(&gui, &GUI::signal_newGame,                     worker, &BackendWorker::resetGame);
        QObject::connect(&gui, &GUI::signal_boardDetectionAutomatically, worker, &BackendWorker::selectBoardAutomatically);
        QObject::connect(&gui, &GUI::signal_boardDetectionManually,      worker, &BackendWorker::selectBoardManually);
        QObject::connect(&gui, &GUI::signal_setVirtualGameMode,          worker, &BackendWorker::setVirtualGameMode);
        QObject::connect(&gui, &GUI::signal_playMove,                    worker, &BackendWorker::playMove);
        QObject::connect(&gui, &GUI::signal_setScannerDebugImage,        worker, &BackendWorker::setScannerDebugImage);

        worker_thread.start(); // start worker thread

        gui.show();
        qt_app.exec();   // start gui thread (and it's event loop)

        worker_thread.quit();
        worker_thread.wait();
    } 

    // "clean up" fuego
    GoFini();
    SgFini();

    return 0;
}