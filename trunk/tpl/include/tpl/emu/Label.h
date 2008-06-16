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
// Module: tpl/emu/Label.h
// Creator: xushiwei
// Email: xushiweizh@gmail.com
// Date: 2006-8-13 9:41:58
// 
// $Id: Label.h 588 2008-05-28 07:22:04Z xushiweizh $
// -----------------------------------------------------------------------*/
#ifndef TPL_EMU_LABEL_H
#define TPL_EMU_LABEL_H

#ifndef STDEXT_ARRAY_H
#include "../../../../stdext/include/stdext/Array.h"
#endif

NS_TPL_EMU_BEGIN

// =========================================================================
// class Label

#define TPL_EMU_DYNAMIC_LABEL	((size_t)-1)
#define TPL_EMU_UNDEFINED_LABEL	((size_t)-1)

template <size_t n = TPL_EMU_DYNAMIC_LABEL>
class Label
{
private:
	typedef size_t* Reference;
	typedef std::Array<Reference, n> References;
	
	size_t m_label;
	References m_refs;

public:
	Label()
		: m_label(TPL_EMU_UNDEFINED_LABEL) {
	}
	
	operator size_t() const {
		TPL_ASSERT(defined());
		return m_label;
	}
	
	bool TPL_CALL defined() const {
		return m_label != TPL_EMU_UNDEFINED_LABEL;
	}
	
	template <class CodeT>
	void TPL_CALL define(const CodeT& code) {
		TPL_ASSERT(!defined());
		m_label = code.size();
		for (size_t i = m_refs.size(); i--; )
			*m_refs[i] = m_label;
	}
	
	template <class CodeT>
	void TPL_CALL refer(CodeT& code) {
		if (m_label == TPL_EMU_UNDEFINED_LABEL) {
			m_refs.push_back(&code.back().para.val);
		}
		else {
			code.back().para.val = m_label;
		}
	}
};

template <>
class Label<0>
{
private:
	size_t m_label;

public:
	Label()
		: m_label(TPL_EMU_UNDEFINED_LABEL) {
	}
	
	operator size_t() const {
		TPL_ASSERT(defined());
		return m_label;
	}
	
	bool TPL_CALL defined() const {
		return m_label != TPL_EMU_UNDEFINED_LABEL;
	}
	
	template <class CodeT>
	void TPL_CALL define(const CodeT& code) {
		TPL_ASSERT(!defined());
		m_label = code.size();
	}

	template <class CodeT>
	void TPL_CALL refer(CodeT& code) {
		TPL_ASSERT(defined());
		code.back().para.val = m_label;
	}
};

template <>
class Label<TPL_EMU_DYNAMIC_LABEL>
{
private:
	struct Reference {
		size_t* offset;
		Reference* prev;
	};

	size_t m_label;
	Reference* m_refs;
	
public:
	Label()
		: m_label(TPL_EMU_UNDEFINED_LABEL), m_refs(NULL) {
	}
	
	operator size_t() const {
		TPL_ASSERT(defined());
		return m_label;
	}
	
	bool TPL_CALL defined() const {
		return m_label != TPL_EMU_UNDEFINED_LABEL;
	}
	
	template <class CodeT>
	void TPL_CALL define(const CodeT& code) {
		TPL_ASSERT(!defined());
		m_label = code.size();
		for (Reference* ref_ = m_refs; ref_; ref_ = ref_->prev)
			*ref_->offset = m_label;
	}
	
	template <class CodeT>
	void TPL_CALL refer(CodeT& code) {
		if (m_label == TPL_EMU_UNDEFINED_LABEL) {
			Reference* ref_ = TPL_ALLOC(code.get_alloc(), Reference);
			ref_->offset = &code.back().para.val;
			ref_->prev = m_refs;
			m_refs = ref_;
		}
		else {
			code.back().para.val = m_label;
		}
	}
};

// =========================================================================
// $Log: $

NS_TPL_EMU_END

#endif /* TPL_EMU_LABEL_H */
