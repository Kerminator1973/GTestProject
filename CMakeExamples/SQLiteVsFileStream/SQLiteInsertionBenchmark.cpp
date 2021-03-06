#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <chrono>
#include <benchmark/benchmark.h>
#include "sqlite3.h"

// Открываем базу данных
static sqlite3 *db = nullptr;

static bool OpenDatabaseFile(const char* pDatabaseName, bool bUseIndexFor = false);
static void InsertRecord();

// Трассировка, которая выполняется ядром SQLite
static int callback(void *NotUsed, int argc, char **argv, char **azColName);


static void BM_InsertionFunction(benchmark::State& state) 
{
	// Perform setup here
	if( OpenDatabaseFile("test.db") ) {

		for (auto _ : state) {
			// This code gets timed
			InsertRecord();
		}

		// Закрываем базу данных
		sqlite3_close(db);
	}
}

static void BM_InsertionFunctionWithIndexes(benchmark::State& state) 
{
	// Perform setup here
	if( OpenDatabaseFile("test_indexed.db", true ) ) {

		for (auto _ : state) {
			// This code gets timed
			InsertRecord();
		}

		// Закрываем базу данных
		sqlite3_close(db);
	}
}

static void BM_SaveToFile(benchmark::State& state) 
{
	// Запись в файл осуществляется в режиме добавления в конец файла
    std::ofstream out("regular.txt", std::ios::app);

	int count = 1;
	for (auto _ : state) {

		std::stringstream stream;
		stream << "{beginmarker}ВОЛОБУЕВ{separator}" << count << "{anotherseparator}УЛИЦА СВЕТЛОЙ ЯРОВОЙ, " << count << "/4{endmarker}";
		count++;

		out << stream.str();
	}

	// Закрываем файл
	out.flush();
    out.close();
}

// Register the function as a benchmark
BENCHMARK(BM_InsertionFunction);
BENCHMARK(BM_InsertionFunctionWithIndexes);
BENCHMARK(BM_SaveToFile);
// Run the benchmark
BENCHMARK_MAIN();

static bool OpenDatabaseFile(const char* pDatabaseName, bool bUseIndexFor)
{
	// TODO: нужно использовать вызов sqlite3_open_v2()
	int rc = sqlite3_open(pDatabaseName, &db);
	if (rc) {
		std::cerr << "Can't open database: " << sqlite3_errmsg(db) << std::endl;
		return false;
	}
	else {
		std::cout << "Opened database successfully" << std::endl;
	}

	// Формируем структуру базы данных 
	std::stringstream stream;
	stream <<
		"CREATE TABLE IF NOT EXISTS COMPANY("  \
		"ID INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL," \
		"NAME           TEXT    NOT NULL," \
		"AGE            INT     NOT NULL," \
		"ADDRESS        CHAR(50) );";

	if(bUseIndexFor) {
		stream << 
			"CREATE INDEX IF NOT EXISTS idx_company_Address " \
			"ON COMPANY(ADDRESS);";
	}

	// Для целей тестирования производительности движка SQLite, основной ключ
	// не создаётся, т.к. в контексте конкретной операции, он не нужен - поле
	// rowid, выполняет ту же самую задачу, но более эффективно
	//"ID INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL," \

	char *zErrMsg = nullptr;
	rc = sqlite3_exec(db, stream.str().c_str(), callback, 0, &zErrMsg);

	// Выводим информацию с результатами создания таблицы
	if (rc != SQLITE_OK) {
		std::cerr << "SQL error: " << zErrMsg << std::endl;
		sqlite3_free(zErrMsg);

		sqlite3_close(db);
		return false;
	}	

	return true;
}

static void InsertRecord()
{
	static int count = 1;

	// Добавляемые строки являются уникальными
	std::stringstream stream;
	stream << "insert into COMPANY(NAME,AGE,ADDRESS) values(";
	stream << "\"ВОЛОБУЕВ\", " << count << ", \"УЛИЦА СВЕТЛОЙ ЯРОВОЙ, " << count << "/4\");";
	count++;

	sqlite3_stmt *statement;
	const char *query = stream.str().c_str();

	if (sqlite3_prepare(db, query, -1, &statement, 0) == SQLITE_OK)
	{
		// int rc = sqlite3_step(statement);	// Не проверяем код ответа, это не так уж и критично
		sqlite3_step(statement);
		sqlite3_finalize(statement);
	}
	else
	{
		std::cerr << "SQL error";
	}
}

// Трассировка, используемая SQLite Для передачи дополнительной информации о выполнении запроса
static int callback(void *NotUsed, int argc, char **argv, char **azColName) {

	for (int i = 0; i<argc; i++) {
		std::cout << azColName[i] << " = " << (argv[i] ? argv[i] : "NULL") << std::endl;
	}

	std::cout << std::endl;
	return 0;
}
