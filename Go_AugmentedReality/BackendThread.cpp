#include "BackendThread.hpp"

#include <iostream>

#include <QDebug>
#include <opencv2/opencv.hpp>

namespace Go_AR {

// converts a cv::Mat to a QImage
// inspired from http://www.qtforum.de/forum/viewtopic.php?t=9721
//
// Has two preconditions:
//   - depth of the cv::Mat has to be CV_8U = 8-bit unsigned integers ( 0..255 )
//   - number of channels has to be 3 (RGB, or BGR in opencv)
QImage mat_to_QImage(cv::Mat source)
{
    assert(source.depth() == CV_8U);
    assert(source.channels() == 3);

    IplImage image = source;

    // create QImage from IplImage
    QImage ret((uchar*) image.imageData, image.width, image.height, QImage::Format_RGB888);

    return ret.convertToFormat(QImage::Format_RGB32).rgbSwapped();
}

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
    int board_size;
    cv::Mat image;

    // analyze new camera image
    auto got_new_image = _scanner->scanCamera(setup, board_size, image);

    qDebug() << "\nScan finished!";

    // only process results if a new image was scanned
    if (got_new_image) {
        qDebug() << " New image available!";

        // update game state
        _game->update(setup);

        // converting image (OpenCV data type) to QImage (Qt data type)
        const auto scanner_image = mat_to_QImage(image);

        // send signal to gui
        emit backend_new_image(scanner_image);

        // send board data to gui
        // the GUI controls the lifetime of this thread,
        // so passing a pointer to the GoBoard is safe and won't be invalidated
        // as long as the GUI says so
        emit game_data_changed(&(_game->getBoard()));
    }
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