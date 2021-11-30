#include "gtest/gtest.h"
#include "uniqueRnd.h"
#include "test.h"

TestClass::TestClass() {
}

int TestClass::ReturnZero()
{
	return 1;	// Имитация ошибки
}

int TestClass::ReturnOne()
{
	return 1;	// Имитация успешного выполнения
}

namespace {

	TEST(TestClass, ReturnZero) {
		TestClass tc;
		EXPECT_EQ(0, tc.ReturnZero());
	}

	TEST(TestClass, ReturnOne) {
		TestClass tc;
		EXPECT_EQ(1, tc.ReturnOne());
	}
}

int main(int argc, char** argv) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}