#include "CppUnitTest.h"

// augmented go
#include "Game.hpp"

// fuego
#include "GoInit.h"
#include "SgInit.h"
#include "GoSetupUtil.h"
#include "GoBoardUtil.h"

// other libraries


using namespace Microsoft::VisualStudio::CppUnitTestFramework;
namespace ScoringTest
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

    TEST_CLASS(ScoringTest)
    {
    public:
        TEST_METHOD(can_score_simple_board) {
            Game go_game;
            GoSetup setup;
            int size;
            std::string s; // setup string
            float score;
            bool score_successful;


            auto rules = go_game.getBoard().Rules().Komi();

            // O = White
            // X = Black
            //
            // score negative: white wins
            // score positive: black wins
            //----------------------------------------------------------------------------------------
            // black loses by amount of komi
            s = std::string(  "....\n"
                              "....\n"
                              "....\n"
                              "....");
            // black = 0
            // white = 0
            // black - white - 6.5 = -6.5f

            setup = GoSetupUtil::CreateSetupFromString(s, size);
            go_game.init(size, setup);

            score = FLT_MIN;
            score_successful = GoBoardUtil::ScorePosition(go_game.getBoard(), SgPointSet(), score);

            Assert::AreEqual(true, score_successful);
            Assert::AreEqual(-6.5f, score);

            //----------------------------------------------------------------------------------------
            // black loses by amount of komi
            s = std::string(  ".XO.\n"
                              ".XO.\n"
                              ".XO.\n"
                              ".XO.");
            // black = 4
            // white = 4
            // black - white - 6.5 = -6.5f

            setup = GoSetupUtil::CreateSetupFromString(s, size);
            go_game.init(size, setup);

            score = FLT_MIN;
            score_successful = GoBoardUtil::ScorePosition(go_game.getBoard(), SgPointSet(), score);

            Assert::AreEqual(true, score_successful);
            Assert::AreEqual(-6.5f, score);

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

            score = FLT_MIN;
            score_successful = GoBoardUtil::ScorePosition(go_game.getBoard(), SgPointSet(), score);

            Assert::AreEqual(true, score_successful);
            Assert::AreEqual(-7.5f, score);
            
            //----------------------------------------------------------------------------------------
            // middle of the game - white wins
            s = std::string(  "....\n"
                              "..O.\n"
                              "XXOX\n"
                              ".XO.");
            // black = 1
            // white = 0
            // black - white - 6.5 = -5.5f

            setup = GoSetupUtil::CreateSetupFromString(s, size);
            go_game.init(size, setup);

            score = FLT_MIN;
            score_successful = GoBoardUtil::ScorePosition(go_game.getBoard(), SgPointSet(), score);

            Assert::AreEqual(true, score_successful);
            Assert::AreEqual(-5.5f, score);
        }
    };
}