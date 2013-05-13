#include "CppUnitTest.h"
#include "Backend.hpp"
#include "GoInit.cpp"
#include "SgInit.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
namespace BackendTest
{
    // fuego needs those to work
    TEST_MODULE_INITIALIZE(Fugeo_Init) {
        SgInit();
        GoInit();
    }
    TEST_MODULE_CLEANUP(Fugeo_Fini) {
        GoFini();
        SgFini();
    }

    TEST_CLASS(BackendTest)
    {
    public:
        TEST_METHOD(backend_interface_test)
        {
            GoSetup setup;
            setup.AddWhite(SgPointUtil::Pt(1, 1));

            GoBackend::Backend backend;
            //backend.init(setup, size);

            //backend.update(setup);

            // for board info: prisoners/move number/current turn/current stones
            //const GoBoard& board = backend.getBoard();
        }

        //TEST_METHOD(constructing_SgPoints) {
        //    // constructing SgPoints
        //    SgPoint pt = SgPointUtil::Pt(1, 1);
        //}

        //TEST_METHOD(update_from_scanner) {
        //    Backend backend;

        //    
        //    GoSetup setup;
        //    // inital setup
        //    // add every detected stone
        //    // bottom left intersection has coords (1, 1)
        //    setup.AddWhite(SgPointUtil::Pt(1, 1));
        //    // 19 is the detected size of the board
        //    backend.init(setup, 19);

        //    // a new stone has been placed/scanned
        //    GoSetup setup2;
        //    // again, add every detected stone
        //    setup2.AddWhite(SgPointUtil::Pt(1, 1));
        //    setup2.AddBlack(SgPointUtil::Pt(1, 2));
        //    backend.update(setup2);

        //    // for board info: prisoners/move number/current turn/current stones
        //    const GoBoard& board = backend.getBoard();
        //}
    };
}