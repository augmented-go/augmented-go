#include "Scanner.hpp"
#include <QtCore/QDir>
#include <QtCore/QCoreApplication>

using Go_Scanner::Scanner;
using namespace cv;

int main(int argc, char** argv) {
    QCoreApplication app(argc, argv);
    QDir::setCurrent(QCoreApplication::applicationDirPath());

    // try plugging your camera in and out
    // exit by pressing any key
    Scanner scanner;
    GoSetup setup;
    int size;
    Mat image;

    while(true) {
        bool success = scanner.scanCamera(setup, size, image);
        if (success)
            imshow("after scanning: ", image);

        if(waitKey(30) >= 0)
            break;
    }
    return 0;
}