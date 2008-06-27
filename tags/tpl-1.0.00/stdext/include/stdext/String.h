/* -------------------------------------------------------------------------
// WINX: a C++ template GUI library - MOST SIMPLE BUT EFFECTIVE
// 
// This file is a part of the WINX Library.
// The use and distribution terms for this software are covered by the
// Common Public License 1.0 (http://opensource.org/licenses/cpl.php)
// which can be found in the file CPL.txt at this distribution. By using
// this software in any fashion, you are agreeing to be bound by the terms
// of this license. You must not remove this notice, or any other, from
// this software.
// 
// Module: stdext/String.h
// Creator: xushiwei
// Email: xushiweizh@gmail.com
// Date: 2006-8-18 18:48:49
// 
// $Id: String.h,v 1.3 2007/01/10 09:35:16 xushiwei Exp $
// -----------------------------------------------------------------------*/
#ifndef STDEXT_STRING_H
#define STDEXT_STRING_H

#ifndef STDEXT_BASIC_H
#include "Basic.h"
#endif

#ifndef STDEXT_TEXT_BASICALGO_H
#include "text/BasicAlgo.h"
#endif

#ifndef STDEXT_TEXT_BASICSTRING_H
#include "text/BasicString.h"
#endif

#ifndef STDEXT_TEXT_BASICSTRINGBUILDER_H
#include "text/BasicStringBuilder.h"
#endif

#ifndef STDEXT_TEXT_STRINGALGO_H
#include "text/StringAlgo.h"
#endif

NS_STD_BEGIN

// -------------------------------------------------------------------------
// class tstring

#if defined(UNICODE)
typedef std::basic_string<WCHAR> tstring;
#else
typedef std::basic_string<char> tstring;
#endif

// -------------------------------------------------------------------------
// trim

template <class _E, class _Tr, class _Alloc>
inline void winx_call trim(std::basic_string<_E, _Tr, _Alloc>& str, const _E* blanks)
{
	typedef std::basic_string<_E, _Tr, _Alloc> StringT;
	typedef typename StringT::size_type size_type;

	size_type pos1 = str.find_first_not_of(blanks);
	if (pos1 == StringT::npos)
	{
		str.erase();
		return;
	}

	size_type count = str.find_last_not_of(blanks) - pos1 + 1;
	if (str.length() != count)
		str = str.substr(pos1, count);
}

template <class _Tr, class _Alloc>
inline void winx_call trim(std::basic_string<char, _Tr, _Alloc>& str)
{
	const char blanks[] = { ' ', '\t', '\r', '\n', '\0' };
	trim(str, blanks);
}

template <class _Tr, class _Alloc>
inline void winx_call trim(std::basic_string<WCHAR, _Tr, _Alloc>& str)
{
	const WCHAR blanks[] =
	{
		/* ֧�����Ŀո� */
		' ', 12288, '\t', '\r', '\n', '\0'
	};
	trim(str, blanks);
}

// -------------------------------------------------------------------------
// strupper, strlower

template <class StringT>
inline void winx_call strupper(StringT& str)
{
    typename StringT::iterator itEnd = str.end();
    for (typename StringT::iterator it = str.begin(); it != itEnd; ++it)
    {
		*it = ::toupper(*it);
    }
}

template <class StringT>
inline void winx_call strlower(StringT& str)
{
    typename StringT::iterator itEnd = str.end();
    for (typename StringT::iterator it = str.begin(); it != itEnd; ++it)
    {
		*it = ::tolower(*it);
    }
}

// -------------------------------------------------------------------------
// replaceText

template <class StringT>
inline void winx_call replaceText(
	StringT& szText, const StringT& szFindText, const StringT& szReplaceText)
{
	typedef typename StringT::size_type size_type;
	size_type pos = 0;
	while (
		(pos = szText.find(szFindText, pos)) != StringT::npos
		)
	{
		szText.replace(pos, szFindText.size(), szReplaceText);
		pos += szReplaceText.size();
	}
}

// -------------------------------------------------------------------------
// class TestString

#if defined(STD_UNITTEST)

template <class LogT>
class TestString : public TestCase
{
public:
	WINX_TEST_SUITE(TestString);
		WINX_TEST(testTrim);
		WINX_TEST(testUpperLower);
	WINX_TEST_SUITE_END();

public:
	void setUp() {}
	void tearDown() {}

public:
	void testTrim(LogT& log)
	{
		std::string s = " \t  abc 123  \r\n";
		std::trim(s);
		AssertExp(s == "abc 123");
	}

	void testUpperLower(LogT& log)
	{
		std::string s = "abc123";
		std::strupper(s);
		AssertExp(s == "ABC123");
		std::strlower(s);
		AssertExp(s == "abc123");
	}
};

#endif // defined(STD_UNITTEST)

// -------------------------------------------------------------------------
// $Log: String.h,v $

NS_STD_END

#endif /* STDEXT_STRING_H */