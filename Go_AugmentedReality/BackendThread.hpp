#pragma once

#include <utility>
#include <tuple>
#include <memory>

#include <QThread>
#include <QTimer>
#include <QImage>

#include "Game.hpp"
#include "Scanner.hpp"

typedef int OpenCVImage;

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
        void stop();
        void save_sgf(QString path) const;
        void pass(SgBlackWhite player);
        void reset();
        void finish();
        void resign();

    private slots:
        void scan(); // our main worker function
        
    signals:
        void backend_new_image(QImage camera_image);
        void game_data_changed(const GoBoard * game_board);

    // Member vars    
    private:
        std::unique_ptr<GoBackend::Game>     _game;
        std::unique_ptr<Go_Scanner::Scanner> _scanner;
    };
}