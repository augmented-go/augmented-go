#include "BackendWorker.hpp"

#include <iostream>

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

BackendWorker::BackendWorker()
    : _game(),
    _scanner(),
    _game_is_initialized(false),
    _cached_board_size(0),
    _scan_timer(this) // this makes sure that the timer has the same thread affinity as its parent (this)
{
    /* define default game rules
     *     handicap: 0
     *     komi:     6.5
     *     scoring:  japanese
     *     game end: after 2 consecutive passes
     */
    _new_game_rules = GoRules(0, GoKomi(6.5), true, true);

    connect(&_scan_timer, SIGNAL(timeout()), this, SLOT(scan()));
    _scan_timer.setInterval(2000);// call the connected slot every 1000 msec
    _scan_timer.start();  // put one event in this threads event queue
}


BackendWorker::~BackendWorker()
{}

void BackendWorker::scan() {
    cv::Mat image;
    GoSetup setup;

    // fetch new camera image
    auto scan_result = _scanner.scanCamera(setup, _cached_board_size, image);

    using Go_Scanner::ScanResult;
    using GoBackend::UpdateResult;

    switch (scan_result) {
    case ScanResult::Success:
        {
            if (_game_is_initialized) {
                // update game state
                UpdateResult result = _game.update(setup);
                if (result == UpdateResult::Illegal)
                    emit displayErrorMessage("Your board differs from virtual board!");
                else {
                    emit displayErrorMessage(""); // no error
                }
            }
            else {
                // the gui doesn't support other sizes
                if (_cached_board_size == 9 || _cached_board_size == 13 || _cached_board_size == 19 ) {
                    _game.init(_cached_board_size, setup, _new_game_rules);
                    _game_is_initialized = true;
                    emit displayErrorMessage("");
                }
                else {
                    emit displayErrorMessage(QString("Not supported board size of %1x%1 detected!").arg(_cached_board_size));
                }
            }

            signalGuiGameDataChanged();

            // converting image (OpenCV data type) to QImage (Qt data type)
            const auto scanner_image = mat_to_QImage(image);
            // and send signal with new image to gui
            emit newImage(scanner_image);

            break;
        }
    case ScanResult::Failed:
        {
            // we still have a camera image to display, even when the scanning failed
            // converting image (OpenCV data type) to QImage (Qt data type)
            const auto scanner_image = mat_to_QImage(image);
            // send signal with new image to gui
            emit newImage(scanner_image);

            emit displayErrorMessage("Board could not be detected correctly!\nBoard selection still accurate?");
            break;
        }
    case ScanResult::NoCamera:
        // disables board detection menu items
        // the menu items will be reactivated when the next newImage signal gets handled
        emit noCameraImage();
        emit displayErrorMessage("No camera image could be retrieved!");
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

    signalGuiGameDataChanged();
}

void BackendWorker::resetGame(GoRules rules) {
    _game_is_initialized = false;
    _new_game_rules      = rules;

    if (virtualModeActive()) {
        _game.init(19, GoSetup(), _new_game_rules);

        signalGuiGameDataChanged();
    }
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

void BackendWorker::setVirtualGameMode(bool checked) {
    if (virtualModeActive()) {
        // go into augmented mode -> do the scanning!
        _scan_timer.start();
    }
    else {
        // go into virtual mode -> no scanning!
        _scan_timer.stop();
        // also hide any scanning related error messages
        emit displayErrorMessage("");

        // initialize a game
        if (!_game_is_initialized)
            _game.init(19, GoSetup(), _new_game_rules);

        signalGuiGameDataChanged();
    }
}

void BackendWorker::playMove(const int x, const int y){
    auto position = SgPointUtil::Pt(x, y);
    _game.playMove(position);

    signalGuiGameDataChanged();
}

void BackendWorker::selectBoardManually() {
    _scan_timer.stop();
    _cached_board_size = 0;
    _scanner.selectBoardManually();
    _scan_timer.start();
}

void BackendWorker::selectBoardAutomatically() {
    _scan_timer.stop();
    _cached_board_size = 0;
    _scanner.selectBoardAutomatically();
    _scan_timer.start();
}

void BackendWorker::setScannerDebugImage(bool debug) {
    if (debug)
        _scanner.setDebugImage();
    else
        _scanner.setNormalImage();
}


bool BackendWorker::virtualModeActive() const {
    return !_scan_timer.isActive();
}

void BackendWorker::signalGuiGameDataChanged() const {
    // send board data to gui
    // the GUI controls the lifetime of this thread,
    // so passing a pointer to the GoBoard is safe and won't be invalidated
    // as long as the GUI says so
    emit gameDataChanged(&_game);
}

} // namespace Go_AR