#pragma once
#include <string>

#ifndef IN
#	define IN
#endif

#ifndef OUT
#	define OUT
#endif

namespace FS365 {
namespace	Utilities {
namespace		UTF8 {

	/// @param @in nLength - количество байтов в строке (не символов!)
	std::wstring _utf8_to_unicode( IN const char* pszUtf8, IN size_t nLength );
	std::wstring _utf8_to_unicode( IN const char* pszUtf8 );
	std::wstring _utf8_to_unicode( IN const std::string& strUtf8 );

	/// @param @in nLength - количество символов в строке
	std::string _unicode_to_utf8( IN const wchar_t* pszUnicode, IN size_t nLength );
	std::string _unicode_to_utf8( IN const wchar_t* pszUnicode );
	std::string _unicode_to_utf8( IN const std::wstring& strUnicode );

	/// @param @in nLength - количество байтов в строке (не символов!)
	std::string _utf8_to_1251( IN const char* pszUtf8, IN size_t nLength );
	std::string _utf8_to_1251( IN const char* pszUtf8 );
	std::string _utf8_to_1251( IN const std::string& strUtf8 );

	/// @param @in nLength - количество символов в строке
	std::string _1251_to_utf8( IN const char* psz1251, IN size_t nLength );
	std::string _1251_to_utf8( IN const char* psz1251 );
	std::string _1251_to_utf8( IN const std::string& str1251 );

	//! @brief Функция конвертирует строку из ansi в unicode 
	std::wstring _ansi_to_unicode( IN const char* pszAnsi, IN size_t nLength, IN int nCodePage );
	std::wstring _ansi_to_unicode( IN const char* pszAnsi, IN int nCodePage );
	std::wstring _ansi_to_unicode( const std::string& strAnsi, IN int nCodePage );
	
	std::string _ansi_to_utf8( IN const char* pszAnsi, IN size_t nLength, IN int nCodePage );
	std::string _ansi_to_utf8( IN const char* pszAnsi, IN int nCodePage );
	std::string _ansi_to_utf8( const std::string& strAnsi, IN int nCodePage );
	
	//! @brief Функция конвертирует строку из unicode в ansi (нужно указывать целевую кодировку)
	std::string _unicode_to_ansi( IN const wchar_t* pszUnicode, IN size_t nLength, IN int nCodePage );
	std::string _unicode_to_ansi( IN const wchar_t* pszUnicode, IN int nCodePage );
	std::string _unicode_to_ansi( IN const std::wstring& strUnicode, IN int nCodePage);

	/// @param @in nLength - количество байтов в строке (не символов!)
	std::string _utf8_to_ansi( IN const char* pszUtf8, IN size_t nLength, IN int nCodePage );
	std::string _utf8_to_ansi( IN const char* pszUtf8, IN int nCodePage );
	std::string _utf8_to_ansi( IN const std::string& strUtf8, IN int nCodePage );

	namespace UnitTestsImpl {

		bool __do_unit_tests( std::string& strErrMessage );
	}

} } }
