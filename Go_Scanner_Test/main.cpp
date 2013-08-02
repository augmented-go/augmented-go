#include "Scanner.hpp"
#include <QtCore/QDir>
#include <QtCore/QCoreApplication>

using Go_Scanner::Scanner;
using namespace cv;

int main(int argc, char** argv) {
    QCoreApplication app(argc, argv);
    QDir::setCurrent(QCoreApplication::applicationDirPath());

    Go_Scanner::Scanner scanner;
    GoSetup setup;
    int size;
    Mat image;

    // read camera, else debug picture and call scanner_main
    bool success = scanner.scanCamera(setup, size, image);

    cvWaitKey(0);
    return 0;
}