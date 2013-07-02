#include "CppUnitTest.h"
#include "Scanner.hpp"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
namespace MyTest
{
    using Go_Scanner::Scanner;
    using namespace cv;

    TEST_CLASS(MyTests)
    {
    public:
        TEST_METHOD(CameraReadingWorks)
        {
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
        }
    };
}