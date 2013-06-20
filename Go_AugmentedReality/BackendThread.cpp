#include "BackendThread.hpp"

#include <iostream>

#include <QDebug>

namespace Go_AR {

BackendThread::BackendThread()
    : _game(new GoBackend::Game),
    _scanner(new Go_Scanner::Scanner)
{}


BackendThread::~BackendThread()
{}

void BackendThread::run()  {
    // use a timer to periodically scan the camera image
    QTimer timer;
    connect(&timer, SIGNAL(timeout()), this, SLOT(scan()), Qt::DirectConnection);
    timer.setInterval(1000); // every 1000 msec
    timer.start();   // puts one event in the threads event queue
    exec(); // this threads event loop
    timer.stop();
}

void BackendThread::stop()  {
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
    
    // @todo(jschmer): convert image (OpenCV data type) to QImage (Qt data type)
    const auto scanner_image = QImage(200, 400, QImage::Format::Format_RGB32);

    // send signal to gui
    emit backend_new_image(scanner_image);

    // send board data to gui
    // the GUI controls the lifetime of this thread,
    // so passing a pointer to the GoBoard is safe and won't be invalidated
    // as long as the GUI says so
    emit game_data_changed(&(_game->getBoard()));
}

void BackendThread::save_sgf(QString path) const {
    auto filepath = path.toStdString();

    if (!_game->saveGame(filepath))
        std::cerr << "Error writing game data to file \"" << filepath << "\"!" << std::endl;
}

void BackendThread::pass(SgBlackWhite player) {
    assert(!"Passing is not yet implemented");
}

void BackendThread::reset() {
    assert(!"Resetting a game is not yet implemented");
}

void BackendThread::finish() {
    assert(!"Ending a game is not yet implemented");
}

void BackendThread::resign() {
    assert(!"Resigning is not yet implemented");
}

} // namespace Go_AR