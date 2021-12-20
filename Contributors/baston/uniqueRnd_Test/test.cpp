#include "gtest/gtest.h"
#include "uniqueRnd.h"
#include <boost/filesystem.hpp>
#include "test.h"

TestClass::TestClass() {
}

int TestClass::TestInit() {
	const bool bInit = (argc == 2) && (std::string("init") == argv[1]);				// ����� �������������
	if (bInit) boost::filesystem::remove(filename);									// �������� ����� � ���������������� �������
	path p;
	auto filename = ((p = argv[0]).remove_filename() / "emv.random.history");
	return bInit;
}

bool TestClass::CheckFirstElem() {
	UniqueRandomNumbersChecker checker(filename.string());							// �������������� ������ ��������� ��������� �����
	
	uint32_t valueToRegister = 8500;

	if (bInit) {																	// ���������� ���������
		for (int i = 1; i < 13600; i++) {											 
			if (!checker.RegisterIfUnique(i)) return false;
		}
		valueToRegister++;
	}
	if (!checker.RegisterIfUnique(valueToRegister)) return false;					// ���������, ��� ����� ������ ����� � ������� ���
	return true;
}

bool TestClass::CheckElem5000() {													// ���������, ��� � ������ ���� �����, ������� ���� ����������� �� ����� ��������� 5000 �������
	if (checker.RegisterIfUnique(12404)) return false;
	return true;
}



namespace {

	TEST(TestClass, TestInit) {
		TestClass tc;
		EXPECT_NE(0, tc.TestInit());
	}

	TEST(TestClass, CheckFirstElem) {
		TestClass tc;
		EXPECT_EQ(true, tc.CheckFirstElem());
	}

	TEST(TestClass, CheckElem5000) {
		TestClass tc;
		EXPECT_EQ(true, tc.CheckElem5000());
	}
}

int main(int argc, char** argv) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}