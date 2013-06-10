#include "BackendThread.hpp"

#include <QDebug>
#include <QImage>

namespace Go_AR {

BackendThread::BackendThread()
    : _game(new GoBackend::Game),
    _scanner(new Go_Scanner::Scanner)
{}


BackendThread::~BackendThread()
{}

void BackendThread::run()  {
    qDebug() << "Backend Thread: " << thread()->currentThreadId();

    // use a timer to periodically scan the camera image
    QTimer timer;
    connect(&timer, SIGNAL(timeout()), this, SLOT(scan()), Qt::DirectConnection);
    timer.setInterval(1000); // msec
    timer.start();   // puts one event in the threads event queue
    exec();
    timer.stop();
}

void BackendThread::backend_stop()  {
    qDebug() << "Stop signal arrived, stopping backend thread...";

    this->quit();
}

void BackendThread::scan() {
    GoSetup setup;
    OpenCVImage image;

    // analyze new camera picture
    std::tie(setup, image) = _scanner->scanPicture();

    // update game state
    _game->update(setup);

    qDebug() << "\nScan finished! new image available!";
    
    const auto testparam = std::make_shared<QImage>(200, 400, QImage::Format::Format_RGB32);

    // send signal to gui
    emit backend_new_image(testparam);

    // send board data to gui
    // the GUI controls the lifetime of this thread,
    // so passing a pointer to the GoBoard is safe and won't be invalidated
    // as long as the GUI says so
    emit game_data_changed(&(_game->getBoard()));
}

} // namespace Go_AR