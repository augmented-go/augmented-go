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

            setup.AddBlack(SgPointUtil::Pt(2, 2));

            go_game.update(setup);

            // for board info: prisoners/move number/current turn/current stones
            const GoBoard& board = go_game.getBoard();
        }

        TEST_METHOD(can_be_default_constructed) {
            Game go_game;

            Assert::AreEqual(19, go_game.getBoard().Size());
        }

        TEST_METHOD(can_get_underlying_board) {
            Game go_game;
            auto& board = go_game.getBoard();
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

        TEST_METHOD(can_update_board_with_same_board) {
            std::string s(  "....\n"
                            "....\n"
                            "X...\n"
                            ".X..");

            int size;
            auto setup = GoSetupUtil::CreateSetupFromString(s, size);

            Game go_game;
            go_game.init(size, setup);

            auto board_setup = GoSetupUtil::CurrentPosSetup(go_game.getBoard());
            Assert::IsTrue(setup == board_setup);

            go_game.update(setup);

            board_setup = GoSetupUtil::CurrentPosSetup(go_game.getBoard());
            Assert::IsTrue(setup == board_setup);
        }

        TEST_METHOD(can_update_board_with_single_move_played) {
            std::string s(  "....\n"
                            "....\n"
                            "O...\n"
                            ".O..");

            int size;
            auto setup = GoSetupUtil::CreateSetupFromString(s, size);
            setup.m_player = SG_BLACK;

            Game go_game;
            go_game.init(size, setup);

            auto board_setup = GoSetupUtil::CurrentPosSetup(go_game.getBoard());
            Assert::IsTrue(setup == board_setup);

            // updating with a black new move
            s = "....\n"
                "....\n"
                "OX..\n"
                ".O..";
            auto new_setup = GoSetupUtil::CreateSetupFromString(s, size);
            go_game.update(new_setup);
            new_setup.m_player = SG_WHITE;

            board_setup = GoSetupUtil::CurrentPosSetup(go_game.getBoard());
            Assert::IsTrue(new_setup == board_setup);

            // updating with a black new move
            s = "....\n"
                "....\n"
                "OXO.\n"
                ".O..";
            new_setup = GoSetupUtil::CreateSetupFromString(s, size);
            go_game.update(new_setup);
            new_setup.m_player = SG_BLACK;

            board_setup = GoSetupUtil::CurrentPosSetup(go_game.getBoard());
            Assert::IsTrue(new_setup == board_setup);
        }

        TEST_METHOD(cannot_update_board_with_single_move_played_and_its_not_players_turn) {

        }

        TEST_METHOD(can_update_board_and_recognize_captures) {

        }
    };
}