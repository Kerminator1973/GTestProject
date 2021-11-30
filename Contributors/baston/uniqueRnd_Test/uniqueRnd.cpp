#include "uniqueRnd.h"

uint32_t UniqueRandomNumbersChecker::UNIndex = 0;
uint32_t UniqueRandomNumbersChecker::UnpredictableNumbers[UNDPREDICTABLE_NUMBERS_STORAGE_SIZE];

UniqueRandomNumbersChecker::UniqueRandomNumbersChecker(const std::string& filename)
	: m_Filename(filename)
{
	UNIndex = 0;
	memset(UnpredictableNumbers, 0, sizeof(uint32_t) * UNDPREDICTABLE_NUMBERS_STORAGE_SIZE);

	// Считываем данные из постоянного хранилища - файла
	loadNumbersFromFile();
}

bool UniqueRandomNumbersChecker::RegisterIfUnique(const uint32_t nValue)
{
	for (int i = 0; i < UNDPREDICTABLE_NUMBERS_STORAGE_SIZE; i++) {
		if (nValue == UnpredictableNumbers[i])
			return false;
	}

	UnpredictableNumbers[UNIndex++] = nValue;
	if (UNDPREDICTABLE_NUMBERS_STORAGE_SIZE == UNIndex)
		UNIndex = 0;

	// Сохраняем изменение в постоянном хранилище
	saveNumbersToFile();

	return true;
}

// TODO: Переделать по фэншуй! В частности, сбрасывать исключения в случае критичных ошибок
void UniqueRandomNumbersChecker::loadNumbersFromFile()
{
	FILE* file = nullptr;
	fopen_s(&file, m_Filename.c_str(), "rb");
	if (file) {

		fseek(file, 0L, SEEK_END);
		auto actualSize = ftell(file);
		rewind(file);

		if (actualSize != (UNDPREDICTABLE_NUMBERS_STORAGE_SIZE + 1) * sizeof(uint32_t)) {
			fclose(file);

			// TODO: Вот здесь нужно сбросить исключение
			return;
		}

		fread(&UNIndex, sizeof(uint32_t), 1, file);
		fread(UnpredictableNumbers, sizeof(uint32_t), UNDPREDICTABLE_NUMBERS_STORAGE_SIZE, file);
		fclose(file);
	}
}

void UniqueRandomNumbersChecker::saveNumbersToFile()
{
	FILE* file = nullptr;
	fopen_s(&file, m_Filename.c_str(), "wb");
	if (file) {
		fwrite(&UNIndex, sizeof(uint32_t), 1, file);
		fwrite(UnpredictableNumbers, sizeof(uint32_t), UNDPREDICTABLE_NUMBERS_STORAGE_SIZE, file);
		fclose(file);
	}
}