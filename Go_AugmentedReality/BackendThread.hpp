#pragma once

#include <utility>
#include <tuple>
#include <memory>

#include <QThread>
#include <QTimer>

#include "Game.hpp"

typedef int OpenCVImage;

// using a Scanner dummy until the real implementation is available
namespace Go_Scanner {
    class Scanner {
    public:
        Scanner() {}
    private:
        Scanner(const Scanner&);
        
    public:
        std::pair<GoSetup, OpenCVImage> scanPicture() {
            return std::make_pair(GoSetup(), OpenCVImage());
        }
    };
}


namespace Go_AR {
    class BackendThread : public QThread {
        Q_OBJECT

    public:
        BackendThread();
        ~BackendThread();
            
    private:
        // thread function
        void run() override;
        
    // signals and slots
    public slots:
        void backend_stop();
        void save_sgf(QString path) const;
        void pass(SgBlackWhite player);
        void reset();
        void finish();
        
    private slots:
        void scan();
        
    signals:
        void backend_new_image(const std::shared_ptr<QImage> camera_image);
        void game_data_changed(const GoBoard * game_board);

    // Member vars    
    private:
        std::unique_ptr<GoBackend::Game>    _game;
        std::unique_ptr<Go_Scanner::Scanner> _scanner;
    };
}