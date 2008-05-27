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
// Module: tpl/regex/Basic.h
// Creator: xushiwei
// Email: xushiweizh@gmail.com
// Date: 2006-8-13 9:41:58
// 
// $Id$
// -----------------------------------------------------------------------*/
#ifndef TPL_REGEX_BASIC_H
#define TPL_REGEX_BASIC_H

#ifndef STDEXT_BASIC_H
#include "../../../stdext/include/stdext/Basic.h"
#endif

#ifndef STDEXT_MEMORY_H
#include "../../../stdext/include/stdext/Memory.h"
#endif

#ifndef TPL_UNMANAGED_NEW
#define TPL_UNMANAGED_NEW(alloc, Type)	new((alloc).allocate(sizeof(Type))) Type
#endif

#ifndef TPL_ALLOC_ARRAY
#define TPL_ALLOC_ARRAY(alloc, Type, n)	(Type*)(alloc).allocate((n)*sizeof(Type))
#endif

#ifndef TPL_NEW
#if defined(BOOST_NEW)
#define TPL_NEW(alloc, Type)	BOOST_NEW(alloc, Type)
#else
#define TPL_NEW(alloc, Type)	STD_NEW(alloc, Type)
#endif
#endif

#ifndef TPL_CALL
#if defined(winx_call)
#define TPL_CALL winx_call
#else
#define TPL_CALL
#endif
#endif

#ifndef TPL_ASSERT
#if defined(BOOST_MEMORY_ASSERT)
#define TPL_ASSERT(e)	BOOST_MEMORY_ASSERT(e)
#elif defined(_ASSERTE)
#define TPL_ASSERT(e)	_ASSERTE(e)
#else
#define TPL_ASSERT(e)	0
#endif
#endif

#ifndef NS_TPL_BEGIN
#define NS_TPL_BEGIN			namespace tpl {
#define NS_TPL_END				}
#define NS_TPL					tpl
#endif

#if !defined(__forceinline) && !defined(_MSC_VER)
#define __forceinline inline
#endif

NS_TPL_BEGIN

// =========================================================================
// class Gr: Wrapper a Rule to Grammar

template <class RegExT> class Rule;

template <class RegExT>
class Gr : public RegExT
{
public:
	Gr() {}

	template <class T1>
	Gr(T1& x) : RegExT(x) {}

	template <class T1>
	Gr(const T1& x) : RegExT(x) {}

	template <class T1, class T2>
	Gr(const T1& x, const T2& y) : RegExT(x, y) {}

	template <class T1, class T2>
	Gr(T1& x, const T2& y) : RegExT(x, y) {}

	template <class T1, class T2, class T3>
	Gr(const T1& x, const T2& y, const T3& z) : RegExT(x, y, z) {}

public:
	const Rule<RegExT>& TPL_CALL rule() const {
		return (const Rule<RegExT>&)*this;
	}

	template <class SourceT, class ContextT, class SkipperT>
	bool TPL_CALL match(SourceT& ar, ContextT& context, const Rule<SkipperT>& skipper) const {
		skipper.impl().match(ar, context);
		return RegExT::match(ar, context);
	}
};

// =========================================================================
// enum RuleCharacter

enum RuleCharacter
{
	CHARACTER_MARKED	= 0X01,
	CHARACTER_DEFAULT	= CHARACTER_MARKED,
};

// -------------------------------------------------------------------------
// class Rule

template <class GrammarT> class Grammar;

template <class RegExT>
class Rule : public RegExT
{
public:
	Rule() {}

	template <class T1>
	Rule(T1& x) : RegExT(x) {}

	template <class T1>
	Rule(const T1& x) : RegExT(x) {}

	template <class T1, class T2>
	Rule(const T1& x, const T2& y) : RegExT(x, y) {}

	template <class T1, class T2>
	Rule(T1& x, const T2& y) : RegExT(x, y) {}

	template <class T1, class T2, class T3>
	Rule(const T1& x, const T2& y, const T3& z) : RegExT(x, y, z) {}

public:
	const RegExT& TPL_CALL impl() const {
		return *this;
	}

	const Grammar<Gr<RegExT> >& TPL_CALL grammar() const {
		return (const Grammar<Gr<RegExT> >&)*this;
	}

private:
	// concept:

	enum { character = RegExT::character };

	template <class SourceT, class ContextT>
	bool TPL_CALL match(SourceT& ar, ContextT& context) const;
};

// =========================================================================
// class Grammar

template <class GrammarT>
class Grammar : public GrammarT
{
public:
	Grammar() {}

	template <class T1>
	Grammar(T1& x) : GrammarT(x) {}

	template <class T1>
	Grammar(const T1& x) : GrammarT(x) {}

	template <class T1, class T2>
	Grammar(const T1& x, const T2& y) : GrammarT(x, y) {}

	template <class T1, class T2>
	Grammar(T1& x, const T2& y) : GrammarT(x, y) {}

	template <class T1, class T2, class T3>
	Grammar(const T1& x, const T2& y, const T3& z) : GrammarT(x, y, z) {}

public:
	const GrammarT& TPL_CALL impl() const {
		return *this;
	}

private:
	// concept:

	enum { character = RegExT::character };

	template <class SourceT, class ContextT, class SkipperT>
	bool TPL_CALL match(SourceT& ar, ContextT& context, const Rule<SkipperT>& skipper) const;
};

// =========================================================================
// class SimpleAction

template <class ActionT>
class SimpleAction : public ActionT
{
public:
	SimpleAction() {}

	template <class T1>
	SimpleAction(const T1& x) : ActionT(x) {}

	template <class T1>
	SimpleAction(T1& x) : ActionT(x) {}

	template <class T1, class T2>
	SimpleAction(const T1& x, const T2& y) : ActionT(x, y) {}

	template <class T1, class T2>
	SimpleAction(T1& x, const T2& y) : ActionT(x, y) {}

	template <class T1, class T2>
	SimpleAction(const T1& x, T2& y) : ActionT(x, y) {}

	template <class T1, class T2>
	SimpleAction(T1& x, T2& y) : ActionT(x, y) {}

private:
	// concept:

	void TPL_CALL operator()() const;
};

// =========================================================================
// class Action

template <class ActionT>
class Action : public ActionT
{
public:
	Action() {}

	template <class T1>
	Action(const T1& x) : ActionT(x) {}

	template <class T1>
	Action(T1& x) : ActionT(x) {}

	template <class T1, class T2>
	Action(const T1& x, const T2& y) : ActionT(x, y) {}

	template <class T1, class T2>
	Action(T1& x, const T2& y) : ActionT(x, y) {}

	template <class T1, class T2>
	Action(const T1& x, T2& y) : ActionT(x, y) {}

	template <class T1, class T2>
	Action(T1& x, T2& y) : ActionT(x, y) {}

private:
	// concept:

	template <class Iterator>
	void TPL_CALL operator()(Iterator first, Iterator last) const;
};

// =========================================================================
// class Reference - Reference of a Rule

template <class RefT>
class Reference : public RefT
{
public:
	template <class T1>
	Reference(const T1& x) : RefT(x) {}

private:
	// concept:

	typedef typename RefT::rule_type rule_type;
	typedef typename RefT::dereference_type dereference_type;

	dereference_type TPL_CALL operator()() const;
};

// =========================================================================
// class GReference - Reference of a Grammar

template <class GRefT>
class GReference : public GRefT
{
public:
	template <class T1>
	GReference(const T1& x) : GRefT(x) {}

private:
	// concept:

	typedef typename GRefT::grammar_type grammar_type;
	typedef typename GRefT::dereference_type dereference_type;

	dereference_type TPL_CALL operator()() const;
};

// =========================================================================
// class Transformation

template <class TransformT>
class Transformation : public TransformT
{
public:
	Transformation() {}

	template <class T1>
	Transformation(const T1& x) : TransformT(x) {}

private:
	// concept:

	int TPL_CALL operator()(int ch) const;
};

// =========================================================================
// $Log: $

NS_TPL_END

#endif /* TPL_REGEX_BASIC_H */
