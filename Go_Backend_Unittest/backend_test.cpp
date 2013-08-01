#include "CppUnitTest.h"

// augmented go
#include "Game.hpp"

// fuego
#include "GoInit.h"
#include "SgInit.h"
#include "GoSetupUtil.h"

// other libraries
#include <string>
#include <fstream>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
namespace GoBackendGameTest
{
    using GoBackend::Game;
    using GoBackend::State;
    using SgPointUtil::Pt;
    using std::string;

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
            auto success = go_game.init(9, setup);

            Assert::AreEqual(false, success);
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
            setup.m_player = SG_WHITE;

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

        TEST_METHOD(cannot_update_board_with_faulty_setup) {
            std::string s(  "....\n"
                            "....\n"
                            "O...\n"
                            ".O..");

            int size;
            auto setup = GoSetupUtil::CreateSetupFromString(s, size);

            Game go_game;
            go_game.init(size, setup);

            s = "....\n"
                "....\n"
                "O...\n"
                ".O..";
            auto new_setup = GoSetupUtil::CreateSetupFromString(s, size);
            new_setup.AddBlack(SgPointUtil::Pt(10, 10)); // this points is outside the 4x4 grid

            go_game.update(new_setup); // silently skips this setup!

            // initial setup should be present
            auto board_setup = GoSetupUtil::CurrentPosSetup(go_game.getBoard());
            Assert::IsTrue(setup == board_setup);
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

            // get a reference to the game board
            auto& board = go_game.getBoard();

            // get information
            {
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

            // play a capturing move
            s = std::string("....\n"
                            "....\n"
                            "X...\n"
                            "OX..");

            setup = GoSetupUtil::CreateSetupFromString(s, size);
            go_game.update(setup);

            // remove the captured stone
            s = std::string("....\n"
                            "....\n"
                            "X...\n"
                            ".X..");

            setup = GoSetupUtil::CreateSetupFromString(s, size);
            go_game.update(setup);

            // get information after the capturing move
            {
                // get current player
                auto current_player = board.ToPlay();
                Assert::AreEqual(SG_WHITE, current_player);

                // get board size
                auto board_size = board.Size();
                Assert::AreEqual(4, board_size);

                // get move number
                auto move_number = board.MoveNumber();
                Assert::AreEqual(1, move_number);

                // get number of captured stones for each player
                auto num_captured_black = board.NumPrisoners(SG_BLACK);
                Assert::AreEqual(0, num_captured_black);

                auto num_captured_white = board.NumPrisoners(SG_WHITE);
                Assert::AreEqual(1, num_captured_white);

                // get all stone positions for each color
                auto black_stones = board.All(SG_BLACK);
            
                auto black_iter = SgSetIterator(black_stones);
                {
                    // first black stone
                    auto point = *black_iter;

                    auto column = SgPointUtil::Col(point);
                    auto row    = SgPointUtil::Row(point);

                    Assert::AreEqual(2, column);
                    Assert::AreEqual(1, row);
                }

                // next black stone
                ++black_iter;

                {
                    // second black stone
                    auto point = *black_iter;

                    auto column = SgPointUtil::Col(point);
                    auto row    = SgPointUtil::Row(point);

                    Assert::AreEqual(1, column);
                    Assert::AreEqual(2, row);
                }

                // no more black stones!
                ++black_iter;
                Assert::IsFalse(black_iter);

                // get whites stones
                auto white_stones = board.All(SG_WHITE);
                auto white_iter = SgSetIterator(white_stones);
                
                // no stones or white! :(
                Assert::IsFalse(white_iter);
            }
        }

        TEST_METHOD(a_player_can_resign) {
            // set up a board
            std::string s(  "....\n"
                            "....\n"
                            "X...\n"
                            "O...");

            int size;
            auto setup = GoSetupUtil::CreateSetupFromString(s, size);

            Game go_game;
            go_game.init(size, setup);

            // black resigns
            go_game.resign();
            Assert::AreEqual(go_game.getResult().c_str(), "W+R");
        }

        TEST_METHOD(can_get_the_result_of_a_finished_game_via_passes) {
            Game go_game;
            GoSetup setup;
            int size;
            std::string s; // setup string

            // no result if the game wasn't finished (e.g. with 2 passes)
            Assert::AreEqual("", go_game.getResult().c_str());

            // O = White
            // X = Black
            //
            // score negative: white wins
            // score positive: black wins
            //----------------------------------------------------------------------------------------
            // white wins
            s = std::string(  "....\n"
                              "OOOO\n"
                              "XXXX\n"
                              ".X..");
            // black = 3
            // white = 4
            // black - white - 6.5 = -7.5f

            setup = GoSetupUtil::CreateSetupFromString(s, size);
            go_game.init(size, setup);

            go_game.pass();
            go_game.pass();

            auto res = go_game.getResult();
            Assert::AreEqual("W+7.5", res.c_str());
        }

        TEST_METHOD(can_get_the_result_of_a_finished_game) {
            Game go_game;
            GoSetup setup;
            int size;
            std::string s; // setup string

            // no result if the game wasn't finished (e.g. with 2 passes)
            Assert::AreEqual("", go_game.getResult().c_str());

            // O = White
            // X = Black
            //
            // score negative: white wins
            // score positive: black wins
            //----------------------------------------------------------------------------------------
            // white wins
            s = std::string(  "....\n"
                              "OOOO\n"
                              "XXXX\n"
                              ".X..");
            // black = 3
            // white = 4
            // black - white - 6.5 = -7.5f

            setup = GoSetupUtil::CreateSetupFromString(s, size);
            go_game.init(size, setup);

            auto res = go_game.finishGame();
            Assert::AreEqual("W+7.5", res.c_str());
        }

        TEST_METHOD(can_check_if_game_has_ended) {
            Game go_game;

            // two passes end a game
            go_game.init(9);
            Assert::AreEqual(false, go_game.hasEnded());

            go_game.pass();
            go_game.pass();
            Assert::AreEqual(true, go_game.hasEnded());

            // a resign ends a game
            go_game.init(9);
            Assert::AreEqual(false, go_game.hasEnded());

            go_game.resign();
            Assert::AreEqual(true, go_game.hasEnded());

            // finishing a game ends a game
            go_game.init(9);
            Assert::AreEqual(false, go_game.hasEnded());

            go_game.finishGame();
            Assert::AreEqual(true, go_game.hasEnded());
        }

        TEST_METHOD(allow_playing_handicap_stones_when_board_is_empty) {
            Game go_game;
            GoSetup setup;

            go_game.init(4, setup);
            Assert::IsTrue(go_game.getState() == State::SettingHandicap);

            //
            // more than one handicap stone placed
            std::string s(  "....\n"
                            "..X.\n"
                            ".X..\n"
                            "....");
            int size;
            GoSetup handicap_setup = GoSetupUtil::CreateSetupFromString(s, size);
            go_game.update(handicap_setup);
            // still able to place handicap
            Assert::IsTrue(go_game.getState() == State::SettingHandicap);

            //
            // white move
            s = "....\n"
                "..X.\n"
                ".XO.\n"
                "....";
            setup = GoSetupUtil::CreateSetupFromString(s, size);
            go_game.update(setup);
            Assert::IsTrue(go_game.getState() == State::Valid);

            auto& board = go_game.getBoard();
            // all stones present on the board
            Assert::AreEqual(1, board.TotalNumStones(SG_WHITE));
            Assert::AreEqual(2, board.TotalNumStones(SG_BLACK));

            // the GoGame initializes the board with the handicap stones
            handicap_setup.m_player = SG_WHITE;
            Assert::IsTrue(board.Setup() == handicap_setup);
        }

        TEST_METHOD(dont_allow_playing_handicap_stones_when_setup_has_white_stones) {
            Game go_game;
            std::string s(  "....\n"
                            "..X.\n"
                            ".O..\n"
                            "....");
            int size;
            GoSetup setup = GoSetupUtil::CreateSetupFromString(s, size);
            go_game.init(size, setup);

            Assert::IsTrue(go_game.getState() == State::Valid);
        }

        TEST_METHOD(allow_playing_handicap_stones_when_setup_contains_only_black_stones) {
            GoSetup setup;
            setup.AddBlack(Pt(1, 1));

            Game go_game;
            go_game.init(19, setup);
            Assert::IsTrue(go_game.getState() == State::SettingHandicap);
        }

        TEST_METHOD(dont_consider_single_black_stone_as_handicap) {
            Game go_game;
            go_game.init(19);

            GoSetup empty_setup;
            empty_setup.m_player = SG_BLACK;

            // first black move
            GoSetup setup;
            setup.AddBlack(Pt(1, 1));
            go_game.update(setup);
            // GoGame initializes the board with the handicap stones if there are any
            Assert::IsTrue(go_game.getBoard().Setup() == empty_setup);

            // first white move
            setup.AddWhite(Pt(1, 2));
            go_game.update(setup);
            Assert::IsTrue(go_game.getBoard().Setup() == empty_setup);
        }

        TEST_METHOD(board_gets_updated_when_handicap_stones_get_added) {
            Game go_game;
            go_game.init(19);

            GoSetup setup;
            setup.AddBlack(Pt(1, 1));
            go_game.update(setup);
            Assert::AreEqual(1, go_game.getBoard().TotalNumStones(SG_BLACK));

            setup.AddBlack(Pt(1, 2));
            go_game.update(setup);
            Assert::AreEqual(2, go_game.getBoard().TotalNumStones(SG_BLACK));


            setup.AddBlack(Pt(1, 3));
            go_game.update(setup);
            Assert::AreEqual(3, go_game.getBoard().TotalNumStones(SG_BLACK));
        }

        TEST_METHOD(black_has_to_play_first) {
            Game go_game;
            go_game.init(19);

            GoSetup setup;
            setup.AddWhite(Pt(1, 2));
            go_game.update(setup);

            Assert::IsTrue(go_game.getState() == State::Invalid);
        }

    };

    TEST_CLASS(GoRulesTest)
    {
    public:
        TEST_METHOD(can_set_default_rules) {
            Game go_game;
            go_game.init(9);

            auto& board_rules = go_game.getBoard().Rules();
            Assert::AreEqual(0,    board_rules.Handicap());
            Assert::AreEqual(6.5f, board_rules.Komi().ToFloat());
            Assert::AreEqual(true, board_rules.JapaneseScoring());
            Assert::AreEqual(true, board_rules.TwoPassesEndGame());
        }

        TEST_METHOD(can_set_a_custom_komi) {
            Game go_game;
            GoSetup setup;
            GoRules rules;
            int size;
            std::string s; // setup string

            // O = White
            // X = Black
            //
            // score negative: white wins
            // score positive: black wins
            //----------------------------------------------------------------------------------------
            // white wins
            s = std::string(  "....\n"
                              "OOOO\n"
                              "XXXX\n"
                              ".X..");
            // black = 8
            // white = 8
            // black - white - 4.5 = -4.5f

            setup = GoSetupUtil::CreateSetupFromString(s, size);
            rules.SetKomi(GoKomi(4.5));
            go_game.init(size, setup, rules);

            // check rules
            auto& board_rules = go_game.getBoard().Rules();
            Assert::AreEqual(0,     board_rules.Handicap());
            Assert::AreEqual(4.5f,  board_rules.Komi().ToFloat());
            Assert::AreEqual(false, board_rules.JapaneseScoring());
            Assert::AreEqual(true,  board_rules.TwoPassesEndGame());

            // check if rules are applied properly
            auto res = go_game.finishGame();
            Assert::AreEqual("W+4.5", res.c_str());
        }

        TEST_METHOD(can_set_a_custom_scoring_type) {
            Game go_game;
            GoSetup setup;
            GoRules rules;
            int size;
            std::string s; // setup string

            // O = White
            // X = Black
            //
            // score negative: white wins
            // score positive: black wins
            //----------------------------------------------------------------------------------------
            // white wins
            s = std::string(  "....\n"
                              "OOOO\n"
                              "XXXX\n"
                              ".X..");
            // black = 4
            // white = 3
            // black - white - 6.5 = -7.5f

            setup = GoSetupUtil::CreateSetupFromString(s, size);
            rules.SetJapaneseScoring(true);
            go_game.init(size, setup, rules);

            // check rules
            auto& board_rules = go_game.getBoard().Rules();
            Assert::AreEqual(0,     board_rules.Handicap());
            Assert::AreEqual(6.5f,  board_rules.Komi().ToFloat());
            Assert::AreEqual(true, board_rules.JapaneseScoring());
            Assert::AreEqual(true,  board_rules.TwoPassesEndGame());

            // check if rules are applied properly
            auto res = go_game.finishGame();
            Assert::AreEqual("W+7.5", res.c_str());
        }
    };

    TEST_CLASS(SgfTest)
    {
    public:
        TEST_METHOD(save_current_game_state_as_sgf_file) {
            string filename = "save_current_game_state_as_sgf_file.sgf";
            std::string s(  "....\n"
                            "....\n"
                            "X...\n"
                            "O...");
            int size;
            auto setup = GoSetupUtil::CreateSetupFromString(s, size);

            Game go_game;
            go_game.init(size, setup);

            s = "....\n"
                "....\n"
                "X...\n"
                "OX..";
            setup = GoSetupUtil::CreateSetupFromString(s, size);
            go_game.update(setup);

            go_game.saveGame(filename);

            std::ifstream file(filename);
            std::string contents((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
     
            // probably nice to load the game back in, and check the GoGame instance..
            Assert::IsTrue(contents.find("(;SZ[4]KM[6.5]") != string::npos);
            Assert::IsTrue(contents.find("AB[ac]\nAW[ad];B[bd])") != string::npos);
        }

        TEST_METHOD(save_empty_game_whith_names_as_sgf_file) {
            string filename = "save_empty_game_whith_names_as_sgf_file.sgf";
            Game go_game;
            go_game.init(19);
            // always nice to not just support ascii ;)
            go_game.saveGame(filename, "進藤ヒカル", "塔矢アキラ", "第一局");
            
            std::ifstream file(filename);
            std::string contents((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
            Assert::IsTrue(contents.find("(;SZ[19]KM[6.5]\nPB[進藤ヒカル]\nPW[塔矢アキラ]\nGN[第一局]") != string::npos);
        }

        TEST_METHOD(a_resign_is_commented_in_sgf_file) {
            string filename = "resignation.sgf";
            Game go_game;
            go_game.init(19);

            // black resigns
            go_game.resign();

            go_game.saveGame(filename);
            
            std::ifstream file(filename);
            std::string contents((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
            Assert::IsTrue(contents.find("C[Black resigned]") != string::npos);
            file.close();

            
            go_game.init(19);
            go_game.pass();

            // white resigns
            go_game.resign();

            // always nice to not just support ascii ;)
            go_game.saveGame(filename);
            
            file.open(filename);
            contents = std::string((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
            Assert::IsTrue(contents.find("C[White resigned]") != string::npos);
            file.close();
        }

        TEST_METHOD(result_of_a_finished_game_is_saved_in_sgf_file) {
            string filename = "two_passes.sgf";
            Game go_game;
            go_game.init(19);

            go_game.pass();
            go_game.pass();

            // white wins by 6.5 moku
            go_game.saveGame(filename);
            
            std::ifstream file(filename);
            std::string contents((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
            Assert::IsTrue(contents.find("RE[W+6.5]") != string::npos);
            file.close();
            
            // white = 0, black = 8,
            // 8 - 0 - 6.5 = 1.5, black wins with 1.5 moku
            std::string s = ".O..\n"
                            "XXXX\n"
                            "....\n"
                            "....";
            int size;
            auto setup = GoSetupUtil::CreateSetupFromString(s, size);
            go_game.init(size, setup);

            go_game.pass();
            go_game.pass();

            go_game.saveGame(filename);
            
            file.open(filename);
            contents = std::string((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
            Assert::IsTrue(contents.find("RE[B+1.5]") != string::npos);
        }
    };

    TEST_CLASS(HistoryNavigationTest) {
        TEST_METHOD(board_updates_after_history_movement) {
            Game go_game;
            go_game.init(19);

            // two moves
            GoSetup setup;
            setup.AddBlack(Pt(1, 2));
            setup.m_player = SG_WHITE;
            go_game.update(setup);

            GoSetup setup2 = setup;
            setup2.AddWhite(Pt(1, 3));
            setup2.m_player = SG_BLACK;
            go_game.update(setup2);

            // can't navigate into "future"
            Assert::IsFalse(go_game.canNavigateHistory(SgNode::Direction::NEXT));

            // back on step
            go_game.navigateHistory(SgNode::Direction::PREVIOUS);
            Assert::IsTrue(GoSetupUtil::CurrentPosSetup(go_game.getBoard()) == setup);
            
            // back to the last move
            go_game.navigateHistory(SgNode::Direction::NEXT);
            Assert::IsTrue(GoSetupUtil::CurrentPosSetup(go_game.getBoard()) == setup2);
        }

        TEST_METHOD(history_allows_variations) {
            // to make the variations properly work with handicap, some backend refactoring is probably needed, if we even want that
            // (navigating while still being in the SettingHandicap sate doesn't preserve the history)
            Game go_game;
            go_game.init(19);

            GoSetup first_move;
            first_move.AddBlack(Pt(1, 1));
            first_move.m_player = SG_WHITE;
            go_game.update(first_move);

            // fist white move
            GoSetup first_variation = first_move;
            first_variation.AddWhite(Pt(1, 2));
            first_variation.m_player = SG_BLACK;
            go_game.update(first_variation);

            go_game.navigateHistory(SgNode::Direction::PREVIOUS);
            // one black stone
            Assert::IsTrue(GoSetupUtil::CurrentPosSetup(go_game.getBoard()) == first_move);

            // white variation
            GoSetup second_variation = first_move;
            second_variation.AddWhite(Pt(1, 3));
            go_game.update(second_variation);

            // back to original variation
            bool left = go_game.canNavigateHistory(SgNode::Direction::LEFT_BROTHER);
            bool right = go_game.canNavigateHistory(SgNode::Direction::RIGHT_BROTHER);
            bool branch = go_game.canNavigateHistory(SgNode::Direction::PREV_BRANCH);

            go_game.navigateHistory(SgNode::Direction::LEFT_BROTHER);
            Assert::IsTrue(GoSetupUtil::CurrentPosSetup(go_game.getBoard()) == first_variation);
        }
    };
}