#include "SgInit.h"
#include "GoInit.h"

#include "BackendThread.hpp"
#include "GUI.hpp"

int main(int argc, char** argv) {
    QApplication qt_app(argc, argv);

    SgInit();
    GoInit();

    {
        using Go_AR::BackendThread;
        using Go_GUI::GUI;

        BackendThread backend;
        GUI gui;

        // register std::shared_ptr as meta type to allow this type as a parameter in a signal-slot connection
        qRegisterMetaType<std::shared_ptr<QImage>>("std::shared_ptr<QImage>");

        // connect signal from backend to gui
        QObject::connect(&backend, &BackendThread::backend_new_image, &gui, &GUI::new_image, Qt::QueuedConnection);
        QObject::connect(&backend, &BackendThread::game_data_changed, &gui, &GUI::new_game_data, Qt::QueuedConnection);

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