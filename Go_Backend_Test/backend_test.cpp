#include "CppUnitTest.h"

// augmented go
#include "Game.hpp"

// fuego
#include "GoInit.cpp"
#include "SgInit.h"
#include "GoSetupUtil.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
namespace GoBackendGameTest
{
    using GoBackend::Game;
    using GoBackend::State;
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

            Assert::AreEqual(19, board.Size());

            auto whites = board.TotalNumStones(SG_WHITE);
            auto blacks = board.TotalNumStones(SG_BLACK);

            Assert::AreEqual(0, whites);
            Assert::AreEqual(0, blacks);
        }

        TEST_METHOD(cannot_initialize_board_with_faulty_setup) {
            GoSetup setup;
            setup.AddWhite(Pt(12, 1));
            setup.AddWhite(Pt(5, 5));

            Game go_game;
            go_game.init(9, setup); // WARNING: aborts execution for now (because of invalid stones in setup)
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

        TEST_METHOD(overriding_board_resets_state) {
            Game go_game;
            go_game.init(6);

            GoSetup setup;
            setup.AddWhite(Pt(1, 1));
            setup.m_player = SG_BLACK;

            go_game.update(setup);
            Assert::IsTrue(go_game.getState() == State::Invalid);

            go_game.init(6, setup);
            Assert::IsTrue(go_game.getState() == State::Valid);
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

            // updating with a white new move
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
            std::string s(  "....\n"
                            "....\n"
                            "O...\n"
                            ".O..");

            int size;
            auto setup = GoSetupUtil::CreateSetupFromString(s, size);

            Game go_game;
            go_game.init(size, setup);

            // white moved while blacks turn
            s = "....\n"
                "..O.\n"
                "O...\n"
                ".O..";
            auto new_setup = GoSetupUtil::CreateSetupFromString(s, size);
            go_game.update(new_setup);

            Assert::IsTrue(go_game.getState() == State::Invalid);


            // black moved while whites turn
            go_game.init(size, setup);
            // first a black move so white gets the turn
            s = "....\n"
                "....\n"
                "OX..\n"
                ".O..";
            new_setup = GoSetupUtil::CreateSetupFromString(s, size);
            go_game.update(new_setup);

            Assert::IsTrue(go_game.getState() == State::Valid);

            // ...then the illegal black move
            s = "....\n"
                ".X..\n"
                "OX..\n"
                ".O..";
            new_setup = GoSetupUtil::CreateSetupFromString(s, size);
            go_game.update(new_setup);

            Assert::IsTrue(go_game.getState() == State::Invalid);
        }

        TEST_METHOD(can_update_board_and_detect_illegal_move) {
            std::string s(  "....\n"
                            "....\n"
                            "O...\n"
                            ".O..");
            int size;
            auto setup = GoSetupUtil::CreateSetupFromString(s, size);
            setup.m_player = SG_BLACK;

            Game go_game;
            go_game.init(size, setup);

            // suicide
            s = "....\n"
                "....\n"
                "O...\n"
                "XO..";
            auto new_setup = GoSetupUtil::CreateSetupFromString(s, size);
            go_game.update(new_setup);
            Assert::IsTrue(go_game.getState() == State::Invalid);

            // valid state again after removing illegal move
            s = "....\n"
                "....\n"
                "O...\n"
                ".O..";
            new_setup = GoSetupUtil::CreateSetupFromString(s, size);
            go_game.update(new_setup);
            Assert::IsTrue(go_game.getState() == State::Valid);
        }

        TEST_METHOD(detect_invalid_updates) {
            std::string s(  "....\n"
                            "....\n"
                            "O...\n"
                            ".O..");
            int size;
            auto setup = GoSetupUtil::CreateSetupFromString(s, size);

            Game go_game;
            go_game.init(size, setup);

            // removed stone
            s = "....\n"
                "....\n"
                "....\n"
                ".O..";
            auto new_setup = GoSetupUtil::CreateSetupFromString(s, size);
            go_game.update(new_setup);
            Assert::IsTrue(go_game.getState() == State::Invalid);

            go_game.init(size, setup);
            // added two stones
            s = "....\n"
                ".XX.\n"
                "O...\n"
                ".O..";
            new_setup = GoSetupUtil::CreateSetupFromString(s, size);
            go_game.update(new_setup);
            Assert::IsTrue(go_game.getState() == State::Invalid);

            go_game.init(size, setup);
            // added stones of both colors
            s = "....\n"
                ".X..\n"
                "OO..\n"
                ".O..";
            new_setup = GoSetupUtil::CreateSetupFromString(s, size);
            go_game.update(new_setup);
            Assert::IsTrue(go_game.getState() == State::Invalid);

            go_game.init(size, setup);
            // played black stone and removed white stone
            s = "....\n"
                ".X..\n"
                "....\n"
                ".O..";
            new_setup = GoSetupUtil::CreateSetupFromString(s, size);
            go_game.update(new_setup);

            Assert::IsTrue(go_game.getState() == State::Invalid);
        }

        TEST_METHOD(recognize_captures) {
            std::string s(  "..OX\n"
                            "X...\n"
                            "OX..\n"
                            "O...");
            int size;
            auto setup = GoSetupUtil::CreateSetupFromString(s, size);

            Game go_game;
            go_game.init(size, setup);

            // updating with black move that captures
            s = "..OX\n"
                "X...\n"
                "OX..\n"
                "OX..";
            size;
            auto new_setup = GoSetupUtil::CreateSetupFromString(s, size);

            go_game.update(new_setup);
            Assert::IsTrue(go_game.getState() == GoBackend::State::WhileCapturing);

            // internal board state should automatically remove stones
            s = "..OX\n"
                "X...\n"
                ".X..\n"
                ".X..";
            new_setup = GoSetupUtil::CreateSetupFromString(s, size);
            new_setup.m_player = SG_WHITE;
            auto current_board_setup = GoSetupUtil::CurrentPosSetup(go_game.getBoard());
            Assert::IsTrue(new_setup == current_board_setup);

            // back to valid state after the stones have been removed by the user
            go_game.update(new_setup);
            Assert::IsTrue(go_game.getState() == GoBackend::State::Valid);

            // whites capture
            s = "..OX\n"
                "X..O\n"
                ".X..\n"
                ".X..";
            new_setup = GoSetupUtil::CreateSetupFromString(s, size);
            go_game.update(new_setup);
            Assert::IsTrue(go_game.getState() == GoBackend::State::WhileCapturing);
        }

        TEST_METHOD(capture_move_with_removal) {
            Assert::IsTrue(false);
        }

        TEST_METHOD(can_get_board_information) {
            // set up a board
            std::string s(  "....\n"
                            "....\n"
                            "X...\n"
                            "O...");

            int size;
            auto setup = GoSetupUtil::CreateSetupFromString(s, size);

            Game go_game;
            go_game.init(size, setup);

            // get all the board info we need
            auto& board = go_game.getBoard();

            // get current player
            auto current_player = board.ToPlay();
            Assert::AreEqual(SG_BLACK, current_player);

            // get board size
            auto board_size = board.Size();
            Assert::AreEqual(4, board_size);

            // get move number
            auto move_number = board.MoveNumber();
            Assert::AreEqual(0, move_number);

            // get number of captured stones for each player
            auto num_captured_black = board.NumPrisoners(SG_BLACK);
            Assert::AreEqual(0, num_captured_black);

            auto num_captured_white = board.NumPrisoners(SG_WHITE);
            Assert::AreEqual(0, num_captured_white);

            // get all stone positions for each color
            auto black_stones = board.All(SG_BLACK);
            for (auto iter = SgSetIterator(black_stones); iter; ++iter) {
                auto point = *iter;

                auto column = SgPointUtil::Col(point);
                auto row    = SgPointUtil::Row(point);

                Assert::AreEqual(1, column);
                Assert::AreEqual(2, row);
            }

            auto white_stones = board.All(SG_WHITE);
            for (auto iter = SgSetIterator(white_stones); iter; ++iter) {
                auto point = *iter;

                auto column = SgPointUtil::Col(point);
                auto row    = SgPointUtil::Row(point);

                Assert::AreEqual(1, column);
                Assert::AreEqual(1, row);
            }
        }
    };
}