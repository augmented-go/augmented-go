#include "BackendThread.hpp"

#include <iostream>

#include <QDebug>
#include <opencv2/opencv.hpp>

#include "SgPoint.h"
#include "SgSystem.h"

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
    _game_is_initialized(false),
    _scan_timer(new QTimer)
{
    /* define default game rules
     *     handicap: 0
     *     komi:     6.5
     *     scoring:  japanese
     *     game end: after 2 consecutive passes
     */
    _new_game_rules = GoRules(0, GoKomi(6.5), true, true);

    // initialize the scan timer
    connect(_scan_timer.get(), SIGNAL(timeout()), this, SLOT(scan()), Qt::DirectConnection);
    _scan_timer->setInterval(2000);// call the connected slot every 2000 msec
    _scan_timer->start();
}


BackendThread::~BackendThread()
{}

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

void BackendThread::toggleGameMode() {
    if (_scan_timer->isActive()) {
        // go into virtual mode -> no scanning!
        _scan_timer->stop();
    }
    else {
        // go into augmented mode -> do the scanning!
        _scan_timer->start();
    }
}

void BackendThread::playMove(const int x, const int y){
    auto position = SgPointUtil::Pt(x, y);
    _game->playMove(position);
}

void BackendThread::selectBoardManually() {
    _scanner->selectBoardManually();
}

void BackendThread::selectBoardAutomatically() {
    _scanner->selectBoardAutomatically();
}

} // namespace Go_AR