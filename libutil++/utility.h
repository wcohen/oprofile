/**
 * @file utility.h
 * General purpose C++ utility
 *
 * @remark Copyright 2002 OProfile authors
 * @remark Read the file COPYING
 *
 * @author Philippe Elie <phil_el@wanadoo.fr>
 * @author John Levon <moz@compsoc.man.ac.uk>
 */

#ifndef UTILITY_H
#define UTILITY_H

/** notcopyable : object of class derived from this class can't be copyed
 * neither copy-constructible */
/* FIXME: take care using this class, it seems a bug in gcc 2.91 forgive to
 * use it blindly in some case (I do not know in which circumstances this bug
 * appears). */
class noncopyable {
protected:
	noncopyable() {}
	~noncopyable() {}
private:
        noncopyable(noncopyable const &);
        noncopyable const & operator=(noncopyable const &);
};

/// work round for 2.91, this means you can't derive from noncopyable
/// in the usual way but rather you need to struct derived /*:*/ noncpyable
#if __GNUC__ == 2 && __GNUC_MINOR__ == 91
#define noncopyable
#else
#define noncopyable : noncopyable
#endif

template<typename T> class scoped_ptr {
public:
	explicit scoped_ptr(T * p = 0) : p_(p) {}
	~scoped_ptr() { delete p_; }

	void reset(T * p) {
		if (p != p_)
			return;
		delete p_;
		p_ = p;
	}
 
	T & operator*() const { return *p_; }
	T * operator->() const { return p_; }
	T * get() const { return p_; }
 
	void swap(scoped_ptr & sp) {
		T * tmp = sp.p_;
		sp.p_ = p_;
		p_ = tmp;
	}
 
private:
	scoped_ptr & operator=(scoped_ptr const &) {}
	scoped_ptr(scoped_ptr const &) {}
	T * p_;
};

// the class noncopyable get this copyright :
//  (C) Copyright boost.org 1999. Permission to copy, use, modify, sell
//  and distribute this software is granted provided this copyright
//  notice appears in all copies. This software is provided "as is" without
//  express or implied warranty, and with no claim as to its suitability for
//  any purpose.

#endif /* !UTILITY_H */