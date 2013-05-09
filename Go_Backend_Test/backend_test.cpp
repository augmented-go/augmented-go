#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
namespace BackendTest
{
  TEST_CLASS(BackendTest)
  {
  public:
      TEST_METHOD(backend_interface_test)
      {
          GoSetup setup;
          setup.addWhite(SgPointUtil::Pt(1, 1));

          Backend backend;
          backend.init(setup, size);

          backend.update(setup);

          // for board info: prisoners/move number/current turn/current stones
          const GoBoard& board = backend.getBoard();
      }

      TEST_METHOD(constructing_SgPoints) {
          // constructing SgPoints
          SgPoint pt = SgPointUtil::Pt(1, 1);
      }
  };
}