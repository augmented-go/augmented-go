#include "CppUnitTest.h"

#include "Game.hpp"
#include "GoInit.cpp"
#include "SgInit.h"
#include "GoSetupUtil.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
namespace GoBackendGameTest
{
    using GoBackend::Game;
    using SgPointUtil::Pt;

    // fuego needs those to work
    TEST_MODULE_INITIALIZE(Fugeo_Init) {
        SgInit();
        GoInit();
    }
    TEST_MODULE_CLEANUP(Fugeo_Fini) {
        GoFini();
        SgFini();
    }

    TEST_CLASS(GameTest)
    {
    public:
        TEST_METHOD(interface_spec)
        {
            GoSetup setup;
            setup.AddWhite(SgPointUtil::Pt(1, 1));

            int size = 9;

            Game go_game;
            go_game.init(size, setup);

            //go_game.update(setup);

            //// for board info: prisoners/move number/current turn/current stones
            //const GoBoard& board = go_game.getBoard();
        }

        TEST_METHOD(can_be_constructed) {
            Game go_game;
        }

        // standard size: 9, 13, 19
        TEST_METHOD(can_initialize_board_with_default_setup) {
            Game go_game;
            go_game.init(19);

            auto& board = go_game.getBoard();

            auto whites = board.TotalNumStones(SG_WHITE);
            auto blacks = board.TotalNumStones(SG_BLACK);

            Assert::AreEqual(0, whites);
            Assert::AreEqual(0, blacks);
        }

        TEST_METHOD(can_override_game_with_new_board) {
            Game go_game;
            go_game.init(6);

            Assert::AreEqual(6, go_game.getBoard().Size());

            // initializing with new board
            GoSetup setup;
            setup.AddWhite(Pt(1, 1));
            setup.AddWhite(Pt(5, 5));
            setup.AddBlack(Pt(9, 9));

            go_game.init(9, setup);

            auto& board = go_game.getBoard();

            Assert::AreEqual(9, board.Size());

            auto whites = board.TotalNumStones(SG_WHITE);
            auto blacks = board.TotalNumStones(SG_BLACK);
            Assert::AreEqual(2, whites);
            Assert::AreEqual(1, blacks);
        }

        TEST_METHOD(can_initialize_with_uncaptured_stone) {
            // O = White
            // X = Black
            std::string s(  "....\n"
                            "....\n"
                            "X...\n"
                            "OX..");

            int size;
            auto setup = GoSetupUtil::CreateSetupFromString(s, size);

            Game go_game;
            go_game.init(size, setup);

            auto& board = go_game.getBoard();

            Assert::IsTrue(board.IsColor(Pt(1, 1), SG_WHITE));
        }

        //TEST_METHOD(can_update_board) {
        //    Game go_game;
        //    go_game.init(9, GoSetup());

        //    GoSetup new_board;
        //    new_board.AddWhite(SgPointUtil::Pt(1, 1));
        //    //go_game.update(new_board);

        //    // can_extract_move_from_subsequent_board?
        //}

        //TEST_METHOD(constructing_SgPoints) {
        //    // constructing SgPoints
        //    SgPoint pt = SgPointUtil::Pt(1, 1);
        //}

        //TEST_METHOD(update_from_scanner) {
        //    Game go_game;

        //    
        //    GoSetup setup;
        //    // inital setup
        //    // add every detected stone
        //    // bottom left intersection has coords (1, 1)
        //    setup.AddWhite(SgPointUtil::Pt(1, 1));
        //    // 19 is the detected size of the board
        //    go_game.init(setup, 19);

        //    // a new stone has been placed/scanned
        //    GoSetup setup2;
        //    // again, add every detected stone
        //    setup2.AddWhite(SgPointUtil::Pt(1, 1));
        //    setup2.AddBlack(SgPointUtil::Pt(1, 2));
        //    go_game.update(setup2);

        //    // for board info: prisoners/move number/current turn/current stones
        //    const GoBoard& board = go_game.getBoard();
        //}
    };
}