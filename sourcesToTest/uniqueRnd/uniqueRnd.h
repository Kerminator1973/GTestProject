#include <string>
#include <stdint.h>		// Импортируется определение uint32_t

#define UNDPREDICTABLE_NUMBERS_STORAGE_SIZE	5000

class UniqueRandomNumbersChecker {
public:
	UniqueRandomNumbersChecker(const std::string& filename);
	bool RegisterIfUnique(const uint32_t nValue);

private:
	void loadNumbersFromFile();
	void saveNumbersToFile();

private:
	static uint32_t UNIndex;
	static uint32_t UnpredictableNumbers[UNDPREDICTABLE_NUMBERS_STORAGE_SIZE];
	
	std::string m_Filename;
};
