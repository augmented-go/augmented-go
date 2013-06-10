#include "SgInit.h"
#include "GoInit.h"

#include "BackendThread.hpp"
#include "GUI.hpp"

int main(int argc, char** argv) {
    QApplication qt_app(argc, argv);
    // changes the current working directory and makes all texture and model paths relative to the executable
    QDir::setCurrent(QCoreApplication::applicationDirPath());

    SgInit();
    GoInit();

    {
        using Go_AR::BackendThread;
        using Go_GUI::GUI;

        BackendThread backend;
        GUI gui;

        // connect signal from backend to gui
        QObject::connect(&backend, &BackendThread::backend_new_image, &gui, &GUI::new_image, Qt::QueuedConnection);

        // connect signal from gui to backend
        QObject::connect(&gui, &GUI::stop_backend_thread, &backend, &BackendThread::backend_stop, Qt::QueuedConnection);

        backend.start(); // backend thread

        gui.show();
        qt_app.exec();    // gui thread
    
        backend.wait();
    }

    GoFini();
    SgFini();

    return 0;
}