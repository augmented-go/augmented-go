#include "Scanner.hpp"

using Go_Scanner::Scanner;
using namespace cv;

int main() {
    // try plugging your camera in and out
    // exit by pressing any key

    Scanner scanner;
    GoSetup setup;
    int size;
    Mat image;

    while(true) {
        // note: the standart debug picture won't work, as the working dir is not at the executable (with visual studio)
        // camera works though
        bool success = scanner.scanCamera(setup, size, image);
        if (success)
            imshow("after scanning: ", image);

        if(waitKey(30) >= 0)
            break;
    }
    return 0;
}