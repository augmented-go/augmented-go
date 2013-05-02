#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
namespace MyTest
{
  TEST_CLASS(MyTests)
  {
  public:
      TEST_METHOD(MyTestMethod)
      {
          Assert::AreEqual(1, 1);
      }

      TEST_METHOD(MyTestMethod2)
      {
          Assert::AreEqual(1, 1);
      }
  };
}