#include "gtest/gtest.h"
#include "ConvertUTF8.h"
#include "test.h"

TestClass::TestClass() {
}

bool TestClass::Str1251()															// Конвертация в 1251 кириллицой
{
	std::string str1{ "Здравствуй, Мир!" };

	if (str1 != _unicode_to_ansi(_utf8_to_unicode(_1251_to_utf8(str1)), 1251)) {
		return false;
	}
	return true;
}

bool TestClass::Str1251Many()														// Множдественная конвертация кириллицой
{
	std::string str2{ "Здравствуй, Мир!" };
	if (str2 != _utf8_to_ansi(_unicode_to_ansi(_ansi_to_unicode(_ansi_to_utf8(str2, 1251), CP_UTF8), CP_UTF8), 1251))
	{
		return false;
	}
	return true;
}

namespace {

	TEST(TestClass, Str1251) {
		TestClass tc;
		EXPECT_EQ(1, tc.Str1251());
	}

	TEST(TestClass, Str1251Many) {
		TestClass tc;
		EXPECT_EQ(1, tc.Str1251Many());
	}
}

int main(int argc, char** argv) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}