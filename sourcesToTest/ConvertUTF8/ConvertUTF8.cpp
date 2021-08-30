#include "stdafx.h"
#include "ConvertUTF8.h"
#include <string.h>
#ifdef _WIN32
#  include <atlconv.h>
#else
#  include "boost/locale/encoding_utf.hpp"
#  ifndef CP_UTF8
#    define CP_UTF8                   65001       // UTF-8 translation
#  endif
#endif


namespace FS365 {
namespace	Utilities {
namespace	UTF8 {

	std::wstring _utf8_to_unicode( IN const char* pszUtf8, IN size_t nLength )
	{
		return _ansi_to_unicode( pszUtf8, nLength, CP_UTF8 );
	}

	std::wstring _utf8_to_unicode( IN const char* pszUtf8 )
	{
		return _utf8_to_unicode( pszUtf8, strlen( pszUtf8 ) );
	}

	std::wstring _utf8_to_unicode( IN const std::string& strUtf8 )
	{
		return _utf8_to_unicode( strUtf8.c_str(), strUtf8.size() );
	}

	std::string _unicode_to_utf8( IN const wchar_t* pszUnicode, IN size_t nLength )
	{
		return _unicode_to_ansi( pszUnicode, nLength, CP_UTF8 );
	}

	std::string _unicode_to_utf8( IN const wchar_t* pszUnicode )
	{
		return _unicode_to_utf8( pszUnicode, wcslen( pszUnicode ) );
	}

	std::string _unicode_to_utf8( IN const std::wstring& strUnicode )
	{
		return _unicode_to_utf8( strUnicode.c_str(), strUnicode.size() );
	}

	std::wstring _ansi_to_unicode( IN const char* pszAnsi, IN size_t nLength, IN int nCodePage )
	{
#ifdef _WIN32
		std::wstring strwUnicode;

		int nChars = ::MultiByteToWideChar( nCodePage, 0, pszAnsi, (int)nLength,
			0, 0
		);
		if( nChars > 0 ) {

			strwUnicode.resize( nChars );

			::MultiByteToWideChar( nCodePage, 0, pszAnsi, (int)nLength,
				&strwUnicode[ 0 ], nChars
			);
		}

		return strwUnicode;
#else
		return boost::locale::conv::utf_to_utf<wchar_t>(pszAnsi, pszAnsi + nLength);
#endif
	}

	std::wstring _ansi_to_unicode( IN const char* pszAnsi, IN int nCodePage )
	{
		return _ansi_to_unicode( pszAnsi, strlen( pszAnsi ), nCodePage );
	}

	std::wstring _ansi_to_unicode( const std::string& strAnsi, IN int nCodePage )
	{
		return _ansi_to_unicode( strAnsi.c_str(), strAnsi.size(), nCodePage );
	}

	std::string _ansi_to_utf8( IN const char* pszAnsi, IN size_t nLength, IN int nCodePage )
	{
		return _unicode_to_utf8( _ansi_to_unicode( pszAnsi, nLength, nCodePage ) );
	}

	std::string _ansi_to_utf8( IN const char* pszAnsi, IN int nCodePage )
	{
		return _ansi_to_utf8( pszAnsi, strlen( pszAnsi ), nCodePage );
	}

	std::string _ansi_to_utf8( const std::string& strAnsi, IN int nCodePage )
	{
		return _ansi_to_utf8( strAnsi.c_str(), strAnsi.size(), nCodePage );
	}


	std::string _unicode_to_ansi( IN const wchar_t* pszUnicode, IN size_t nLength, IN int nCodePage )
	{
#ifdef _WIN32
		std::string strAnsi;

		int nChars = ::WideCharToMultiByte( nCodePage, 0, pszUnicode, (int)nLength,
			0, 0, 0, 0
		);
		if( nChars > 0 ) {

			strAnsi.resize( nChars );

			::WideCharToMultiByte( nCodePage, 0, pszUnicode, (int)nLength,
				&strAnsi[0], nChars, 0, 0
			);
		}


		return strAnsi;
#else
		return boost::locale::conv::utf_to_utf< char >(pszUnicode, pszUnicode + nLength);
#endif
	}

	std::string _unicode_to_ansi( IN const wchar_t* pszUnicode, IN int nCodePage )
	{
		return _unicode_to_ansi( pszUnicode, wcslen( pszUnicode ), nCodePage );
	}

	std::string _unicode_to_ansi( IN const std::wstring& strUnicode, IN int nCodePage )
	{
		return _unicode_to_ansi( strUnicode.c_str(), strUnicode.size(), nCodePage );
	}
	
	std::string _utf8_to_1251( IN const char* pszUtf8, IN size_t nLength )
	{
		return _unicode_to_ansi( _utf8_to_unicode( pszUtf8, nLength ), 1251 );
	}

	std::string _utf8_to_1251( IN const char* pszUtf8 )
	{
		return _utf8_to_1251( pszUtf8, strlen( pszUtf8 ) );
	}

	std::string _utf8_to_1251( IN const std::string& strUtf8 )
	{
		return _utf8_to_1251( strUtf8.c_str(), strUtf8.size() );
	}


	std::string _1251_to_utf8( IN const char* psz1251, IN size_t nLength )
	{
		return _unicode_to_utf8( _ansi_to_unicode( psz1251, nLength, 1251 ) );
	}

	std::string _1251_to_utf8( IN const char* psz1251 )
	{
		return _1251_to_utf8( psz1251, strlen( psz1251 ) );
	}

	std::string _1251_to_utf8( IN const std::string& str1251 )
	{
		return _1251_to_utf8( str1251.c_str(), str1251.size() );
	}

	std::string _utf8_to_ansi( IN const char* pszUtf8, IN size_t nLength, IN int nCodePage )
	{
		return _unicode_to_ansi( _utf8_to_unicode( pszUtf8, nLength ), nCodePage );
	}

	std::string _utf8_to_ansi( IN const char* pszUtf8, IN int nCodePage )
	{
		return _utf8_to_ansi( pszUtf8, strlen( pszUtf8 ), nCodePage );
	}

	std::string _utf8_to_ansi( IN const std::string& strUtf8, IN int nCodePage )
	{
		return _utf8_to_ansi( strUtf8.c_str(), strUtf8.size(), nCodePage );
	}

	namespace UnitTestsImpl {

		bool __do_unit_tests( std::string& strErrMessage )
		{
			// 1. Кириллица
			{
				std::string str1{ "Здравствуй, Мир!" };

				if( str1 != _unicode_to_ansi( _utf8_to_unicode( _1251_to_utf8( str1 ) ), 1251 ) ) {
					strErrMessage += "test #1 failed\r\n";
				}

				if( str1 !=
					_utf8_to_ansi(
						_unicode_to_ansi(
							_ansi_to_unicode(
								_ansi_to_utf8( str1, 1251 ), CP_UTF8
							), CP_UTF8
						), 1251
					)
				)
				{
					strErrMessage += "test #1.5 failed\r\n";
				}
			}

#ifndef _WIN32
			// 2. Китайский (упрощённый)
			{
				std::wstring str1{ L"你好世界" };

				if( str1 != _utf8_to_unicode( _unicode_to_utf8( str1 ) ) ) {
					strErrMessage += "test #2 failed\r\n";
				}

				std::string str2 = _unicode_to_ansi( str1, 20936 );

				if( str1 != _ansi_to_unicode( str2, 20936 ) ) {
					strErrMessage += "test #3 failed\r\n";
				}
			}
#endif

			return strErrMessage.empty();
		}
	}


} } }
