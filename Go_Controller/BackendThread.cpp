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

    // "cast" or convert to an IplImage to get easier access to needed infos,
    // no copying involved
    IplImage image = source;

    // create QImage from IplImage
    QImage ret((uchar*) image.imageData, image.width, image.height, QImage::Format_RGB888);

    // swap BGR (opencv format) to RGB
    ret = ret.rgbSwapped();

    return ret;
}

BackendThread::BackendThread()
    : _game(new GoBackend::Game),
    _scanner(new Go_Scanner::Scanner),
    _game_is_initialized(false)
{
    /* define default game rules
     *     handicap: 0
     *     komi:     6.5
     *     scoring:  japanese
     *     game end: after 2 consecutive passes
     */
    _new_game_rules = GoRules(0, GoKomi(6.5), true, true);
}


BackendThread::~BackendThread()
{}

void BackendThread::run()  {
    // use a timer to periodically scan the camera image
    QTimer timer;
    connect(&timer, SIGNAL(timeout()), this, SLOT(scan()), Qt::DirectConnection);
    timer.setInterval(1000); // call the connected slot every 1000 msec
    timer.start();  // put one event in this threads event queue
    exec();         // start this threads event loop
    timer.stop();
}

void BackendThread::stop()  {
    this->quit();
}

void BackendThread::scan() {
    cv::Mat image;
    GoSetup setup;
    int board_size = 19;

    // fetch new camera image
    auto got_new_image = _scanner->scanCamera(setup, board_size, image);

    qDebug() << "\nScan finished!";

    // only process results if a new image was scanned
    if (got_new_image) {
        qDebug() << " New image available!";

        if (_game_is_initialized) {
            // update game state
            _game->update(setup);
        }
        else {
            _game->init(board_size, setup, _new_game_rules);
            _game_is_initialized = true;
        }

        // converting image (OpenCV data type) to QImage (Qt data type)
        const auto scanner_image = mat_to_QImage(image);

        // send signal with new image to gui
        emit newImage(scanner_image);

        // send board data to gui
        // the GUI controls the lifetime of this thread,
        // so passing a pointer to the GoBoard is safe and won't be invalidated
        // as long as the GUI says so
        emit gameDataChanged(_game.get());
    }
}

void BackendThread::saveSgf(QString path, QString blackplayer_name, QString whiteplayer_name, QString game_name) const {
    auto filepath = path.toStdString();

    if (!_game->saveGame(filepath, blackplayer_name.toStdString(), whiteplayer_name.toStdString(), game_name.toStdString()))
        std::cerr << "Error writing game data to file \"" << filepath << "\"!" << std::endl;
}

void BackendThread::pass() {
    _game->pass();
    
    if (_game->hasEnded())
        signalGuiGameHasEnded();
}

void BackendThread::resetGame(GoRules rules) {
    _game_is_initialized = false;
    _new_game_rules      = rules;
}

void BackendThread::finish() {
    _game->finishGame();

    signalGuiGameHasEnded();
}

void BackendThread::resign() {
    _game->resign();

    signalGuiGameHasEnded();
}

void BackendThread::signalGuiGameHasEnded() const {
    auto result = _game->getResult();

    // signal gui that game has ended with this result
    emit finishedGameResult(QString(result.c_str()));
}

} // namespace Go_AR