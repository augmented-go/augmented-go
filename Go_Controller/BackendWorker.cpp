#include "BackendWorker.hpp"

#include <iostream>

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

BackendWorker::BackendWorker()
    : _game(),
    _scanner(),
    _game_is_initialized(false),
    _scan_timer()
{
    /* define default game rules
     *     handicap: 0
     *     komi:     6.5
     *     scoring:  japanese
     *     game end: after 2 consecutive passes
     */
    _new_game_rules = GoRules(0, GoKomi(6.5), true, true);

    connect(&_scan_timer, SIGNAL(timeout()), this, SLOT(scan()), Qt::DirectConnection);
    _scan_timer.setInterval(2000);// call the connected slot every 1000 msec
    _scan_timer.start();  // put one event in this threads event queue
}


BackendWorker::~BackendWorker()
{}

void BackendWorker::scan() {
    cv::Mat image;
    GoSetup setup;
    int board_size = 19;

    // fetch new camera image
    auto scan_result = _scanner->scanCamera(setup, board_size, image);

    using Go_Scanner::ScanResult;

    switch (scan_result) {
    case ScanResult::Success:
        {
            if (_game_is_initialized) {
                // update game state
                _game->update(setup);
            }
            else {
                _game->init(board_size, setup, _new_game_rules);
                _game_is_initialized = true;
            }
            
            // send board data to gui
            // the GUI controls the lifetime of this thread,
            // so passing a pointer to the GoBoard is safe and won't be invalidated
            // as long as the GUI says so
            emit gameDataChanged(_game.get());

            // don't break because Success implies getting an image,
            // so let control flow fall through to ScanResult::Image_Only
        }
    case ScanResult::Image_Only:
        {
            // converting image (OpenCV data type) to QImage (Qt data type)
            const auto scanner_image = mat_to_QImage(image);

            // send signal with new image to gui
            emit newImage(scanner_image);
        }
    case ScanResult::Failed:
        break;
    default:
        assert(!"Unknown ScanResult?!");
    }
}

void BackendWorker::saveSgf(QString path, QString blackplayer_name, QString whiteplayer_name, QString game_name) {
    auto filepath = path.toStdString();

    if (!_game.saveGame(filepath, blackplayer_name.toStdString(), whiteplayer_name.toStdString(), game_name.toStdString()))
        std::cerr << "Error writing game data to file \"" << filepath << "\"!" << std::endl;
}

void BackendWorker::pass() {
    _game.pass();
    
    if (_game.hasEnded())
        signalGuiGameHasEnded();
}

void BackendWorker::resetGame(GoRules rules) {
    _game_is_initialized = false;
    _new_game_rules      = rules;
}

void BackendWorker::finish() {
    _game.finishGame();

    signalGuiGameHasEnded();
}

void BackendWorker::resign() {
    _game.resign();

    signalGuiGameHasEnded();
}

void BackendWorker::signalGuiGameHasEnded() const {
    auto result = _game.getResult();

    // signal gui that game has ended with this result
    emit finishedGameResult(QString(result.c_str()));
}

void BackendWorker::selectBoardManually() {
    _scanner.selectBoardManually();
}

void BackendWorker::selectBoardAutomatically() {
    _scanner.selectBoardAutomatically();
}

} // namespace Go_AR