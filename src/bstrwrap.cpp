/*
 * This source file is part of the bstring string library.  This code was
 * written by Paul Hsieh in 2002-2004, and is covered by the BSD open source 
 * license. Refer to the accompanying documentation for details on usage and 
 * license.
 */

/*
 * bstrwrap.c
 *
 * This file is the C++ wrapper for the bstring functions.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <limits.h>
#include "bstrwrap.h"

#if defined(BSTRLIB_MEMORY_DEBUG)
#include "memdbg.h"
#endif

#if defined(BSTRLIB_CAN_USE_IOSTREAM)
#include <iostream>
#endif

namespace Bstrlib {

#ifdef BSTRLIB_THROWS_EXCEPTIONS
struct CBStringException CBStringExceptionStatic;
#endif

// Constructors.

CBString::CBString () {
	slen = 0;
	mlen = 8;
	data = (unsigned char *) malloc (mlen);
	if (!data) {
		mlen = 0;
		bstringThrow ("Failure in default constructor");
	} else {
		data[0] = '\0';
	}
}

CBString::CBString (const void * blk, int len) { 
	data = NULL;
	if (len >= 0) {
		mlen = len + 1;
		slen = len;
		data = (unsigned char *) malloc (mlen);
	}
	if (!data) {
		mlen = slen = 0;
		bstringThrow ("Failure in block constructor");
	} else {
		if (slen > 0) memcpy (data, blk, slen);
		data[slen] = '\0';
	}
}

CBString::CBString (char c, int len) {
	data = NULL;
	if (len >= 0) {
		mlen = len + 1;
		slen = len;
		data = (unsigned char *) malloc (mlen);
	}
	if (!data) {
		mlen = slen = 0;
		bstringThrow ("Failure in repeat(char) constructor");
	} else {
		if (slen > 0) memset (data, c, slen);
		data[slen] = '\0';
	}
}

CBString::CBString (char c) {
	mlen = 2;
	slen = 1;
	if (NULL == (data = (unsigned char *) malloc (mlen))) {
		mlen = slen = 0;
		bstringThrow ("Failure in (char) constructor");
	} else {
		data[0] = (unsigned char) c;
		data[1] = '\0';
	}
}

CBString::CBString (unsigned char c) {
	mlen = 2;
	slen = 1;
	if (NULL == (data = (unsigned char *) malloc (mlen))) {
		mlen = slen = 0;
		bstringThrow ("Failure in (char) constructor");
	} else {
		data[0] = c;
		data[1] = '\0';
	}
}

CBString::CBString (const char *s) {
	if (s) {
		slen = strlen (s);
		mlen = slen + 1;
		if (NULL != (data = (unsigned char *) malloc (mlen))) {
			memcpy (data, s, mlen);
			return;
		}
	}
	data = NULL;
	bstringThrow ("Failure in (char *) constructor");
}

CBString::CBString (int len, const char *s) {
	if (s) {
		slen = strlen (s);
		mlen = slen + 1;
		if (mlen < len) mlen = len;
		if (NULL != (data = (unsigned char *) malloc (mlen))) {
			memcpy (data, s, slen + 1);
			return;
		}
	}
	data = NULL;
	bstringThrow ("Failure in (int len, char *) constructor");
}

CBString::CBString (const CBString& b) {
	slen = b.slen;
	mlen = slen + 1;
	data = NULL;
	if (mlen > 0) data = (unsigned char *) malloc (mlen);
	if (!data) {
		bstringThrow ("Failure in (CBString) constructor");
	} else {
		memcpy (data, b.data, slen);
		data[slen] = '\0';
	}
}

CBString::CBString (const tagbstring& x) {
	slen = x.slen;
	mlen = slen + 1;
	data = NULL;
	if (slen >= 0 && x.data != NULL) data = (unsigned char *) malloc (mlen);
	if (!data) {
		bstringThrow ("Failure in (tagbstring) constructor");
	} else {
		memcpy (data, x.data, slen);
		data[slen] = '\0';
	}
}

// Destructor.

CBString::~CBString () {
	if (data != NULL) {
		free (data);
		data = NULL;
	}
	mlen = 0;
	slen = -__LINE__;
}

// = operator.

const CBString& CBString::operator = (char c) {
	if (mlen <= 0) bstringThrow ("Write protection error");
	if (2 >= mlen) alloc (2);
	if (!data) {
		mlen = slen = 0;
		bstringThrow ("Failure in =(char) operator");
	} else {
		slen = 1;
		data[0] = (unsigned char) c;
		data[1] = '\0';
	}
	return *this;
}

const CBString& CBString::operator = (unsigned char c) {
	if (mlen <= 0) bstringThrow ("Write protection error");
	if (2 >= mlen) alloc (2);
	if (!data) {
		mlen = slen = 0;
		bstringThrow ("Failure in =(char) operator");
	} else {
		slen = 1;
		data[0] = c;
		data[1] = '\0';
	}
	return *this;
}

const CBString& CBString::operator = (const char *s) {
int tmpSlen;
	if (mlen <= 0) bstringThrow ("Write protection error");
	if (NULL == s) s = "";
	if ((tmpSlen = strlen (s)) >= mlen) alloc (tmpSlen);

	if (data) {
		slen = tmpSlen;
		memcpy (data, s, tmpSlen + 1);
	} else {
		mlen = slen = 0;
		bstringThrow ("Failure in =(const char *) operator");
	}
	return *this;
}

const CBString& CBString::operator = (const CBString& b) {
	if (mlen <= 0) bstringThrow ("Write protection error");
	if (b.slen >= mlen) alloc (b.slen);

	slen = b.slen;
	if (!data) {
		mlen = slen = 0;
		bstringThrow ("Failure in =(CBString) operator");
	} else {
		memcpy (data, b.data, slen);
		data[slen] = '\0';
	}
	return *this;
}

const CBString& CBString::operator = (const tagbstring& x) {
	if (mlen <= 0) bstringThrow ("Write protection error");
	if (x.slen < 0) bstringThrow ("Failure in =(tagbstring) operator, badly formed tagbstring");
	if (x.slen >= mlen) alloc (x.slen);

	slen = x.slen;
	if (!data) {
		mlen = slen = 0;
		bstringThrow ("Failure in =(tagbstring) operator");
	} else {
		memcpy (data, x.data, slen);
		data[slen] = '\0';
	}
	return *this;
}

const CBString& CBString::operator += (const CBString& b) {
	if (BSTR_ERR == bconcat (this, (bstring)&b)) {
		bstringThrow ("Failure in concatenate");
	}
	return *this;
}

const CBString& CBString::operator += (const char *s) {
	struct tagbstring x;

	char * d;
	int i, l;

	if (mlen <= 0) bstringThrow ("Write protection error");

	/* Optimistically concatenate directly */
	l = mlen - slen;
	d = (char *) &data[slen];
	for (i=0; i < l; i++) {
		if ((*d++ = *s++) == '\0') {
			slen += i;
			return *this;
		}
	}
	slen += i;

	cstr2tbstr (x, s);
	if (BSTR_ERR == bconcat (this, &x)) {
		bstringThrow ("Failure in concatenate");
	}
	return *this;
}

const CBString& CBString::operator += (char c) {
	if (BSTR_ERR == bconchar (this, c)) {
		bstringThrow ("Failure in concatenate");
	}
	return *this;
}

const CBString& CBString::operator += (unsigned char c) {
	if (BSTR_ERR == bconchar (this, (char) c)) {
		bstringThrow ("Failure in concatenate");
	}
	return *this;
}

const CBString& CBString::operator += (const tagbstring& x) {
	if (mlen <= 0) bstringThrow ("Write protection error");
	if (x.slen < 0) bstringThrow ("Failure in +=(tagbstring) operator, badly formed tagbstring");
	alloc (x.slen + slen + 1);

	if (!data) {
		mlen = slen = 0;
		bstringThrow ("Failure in +=(tagbstring) operator");
	} else {
		memcpy (data + slen, x.data, x.slen);
		slen += x.slen;
		data[slen] = '\0';
	}
	return *this;
}

const CBString CBString::operator + (char c) const {
	CBString retval (*this);
	retval += c;
	return retval;
}

const CBString CBString::operator + (unsigned char c) const {
	CBString retval (*this);
	retval += c;
	return retval;
}

const CBString CBString::operator + (const CBString& b) const {
	CBString retval (*this);
	retval += b;
	return retval;
}

const CBString CBString::operator + (const char *s) const {
	if (s == NULL) bstringThrow ("Failure in + (char *) operator, NULL");
	CBString retval (*this);
	retval += s;
	return retval;
}

const CBString CBString::operator + (const unsigned char *s) const {
	if (s == NULL) bstringThrow ("Failure in + (unsigned char *) operator, NULL");
	CBString retval (*this);
	retval += (char *) s;
	return retval;
}

const CBString CBString::operator + (const tagbstring& x) const {
	if (x.slen < 0) bstringThrow ("Failure in + (tagbstring) operator, badly formed tagbstring");
	CBString retval (*this);
	retval += x;
	return retval;
}

bool CBString::operator == (const CBString& b) const {
	int retval;
	if (BSTR_ERR == (retval = biseq ((bstring)this, (bstring)&b))) {
		bstringThrow ("Failure in compare (==)");
	}
	return retval != 0;
}

bool CBString::operator == (const char * s) const {
	int retval;
	if (NULL == s) {
		bstringThrow ("Failure in compare (== NULL)");
	}
	if (BSTR_ERR == (retval = biseqcstr ((bstring) this, s))) {
		bstringThrow ("Failure in compare (==)");
	}
	return retval != 0;
}

bool CBString::operator == (const unsigned char * s) const {
	int retval;
	if (NULL == s) {
		bstringThrow ("Failure in compare (== NULL)");
	}
	if (BSTR_ERR == (retval = biseqcstr ((bstring) this, (char *) s))) {
		bstringThrow ("Failure in compare (==)");
	}
	return retval != 0;
}

bool CBString::operator != (const CBString& b) const {
	return ! ((*this) == b);
}

bool CBString::operator != (const char * s) const {
	return ! ((*this) == s);
}

bool CBString::operator != (const unsigned char * s) const {
	return ! ((*this) == s);
}

bool CBString::operator < (const CBString& b) const {
	int retval;
	if (SHRT_MIN == (retval = bstrcmp ((bstring) this, (bstring)&b))) {
		bstringThrow ("Failure in compare (<)");
	}
	return retval < 0;
}

bool CBString::operator < (const char * s) const {
	if (s == NULL) {
		bstringThrow ("Failure in compare (<)");
	}
	return strcmp ((const char *)this->data, s) < 0;
}

bool CBString::operator < (const unsigned char * s) const {
	if (s == NULL) {
		bstringThrow ("Failure in compare (<)");
	}
	return strcmp ((const char *)this->data, (const char *)s) < 0;
}

bool CBString::operator <= (const CBString& b) const {
	int retval;
	if (SHRT_MIN == (retval = bstrcmp ((bstring) this, (bstring)&b))) {
		bstringThrow ("Failure in compare (<=)");
	}
	return retval <= 0;
}

bool CBString::operator <= (const char * s) const {
	if (s == NULL) {
		bstringThrow ("Failure in compare (<=)");
	}
	return strcmp ((const char *)this->data, s) <= 0;
}

bool CBString::operator <= (const unsigned char * s) const {
	if (s == NULL) {
		bstringThrow ("Failure in compare (<=)");
	}
	return strcmp ((const char *)this->data, (const char *)s) <= 0;
}

bool CBString::operator > (const CBString& b) const {
	return ! ((*this) <= b);
}

bool CBString::operator > (const char * s) const {
	return ! ((*this) <= s);
}

bool CBString::operator > (const unsigned char * s) const {
	return ! ((*this) <= s);
}

bool CBString::operator >= (const CBString& b) const {
	return ! ((*this) < b);
}

bool CBString::operator >= (const char * s) const {
	return ! ((*this) < s);
}

bool CBString::operator >= (const unsigned char * s) const {
	return ! ((*this) < s);
}

CBString::operator double () const {
double d = 0;
	if (1 != sscanf ((const char *)this->data, "%lf", &d)) {
		bstringThrow ("Unable to convert to a double");
	}
	return d;
}

CBString::operator float () const {
float d = 0;
	if (1 != sscanf ((const char *)this->data, "%f", &d)) {
		bstringThrow ("Unable to convert to a float");
	}
	return d;
}

CBString::operator int () const {
int d = 0;
	if (1 != sscanf ((const char *)this->data, "%d", &d)) {
		bstringThrow ("Unable to convert to an int");
	}
	return d;
}

CBString::operator unsigned int () const {
unsigned int d = 0;
	if (1 != sscanf ((const char *)this->data, "%u", &d)) {
		bstringThrow ("Unable to convert to an unsigned int");
	}
	return d;
}

#ifdef __TURBOC__
# ifndef BSTRLIB_NOVSNP
#  define BSTRLIB_NOVSNP
# endif
#endif

/* Give WATCOM C/C++, MSVC some latitude for their non-support of vsnprintf */
#if defined(__WATCOMC__) || defined(_MSC_VER)
#define exvsnprintf(r,b,n,f,a) {r = _vsnprintf (b,n,f,a);}
#else
#ifdef BSTRLIB_NOVSNP
/* This is just a hack.  If you are using a system without a vsnprintf, it is 
   not recommended that bformat be used at all. */
#define exvsnprintf(r,b,n,f,a) {vsprintf (b,f,a); r = -1;}
#define START_VSNBUFF (256)
#else

#if defined (__GNUC__) && !defined (__PPC__)
/* Something is making gcc complain about this prototype not being here, so 
   I've just gone ahead and put it in. */
extern "C" {
extern int vsnprintf (char *buf, size_t count, const char *format, va_list arg);
}
#endif

#define exvsnprintf(r,b,n,f,a) {r = vsnprintf (b,n,f,a);}
#endif
#endif

#ifndef START_VSNBUFF
#define START_VSNBUFF (16)
#endif

/*
 * Yeah I'd like to just call a vformat function or something, but because of
 * the ANSI specified brokeness of the va_* macros, it is actually not 
 * possible to do this correctly.
 */

void CBString::format (const char * fmt, ...) {
	va_list arglist;
	va_start (arglist, fmt);
  vformat (fmt, arglist);
}

void CBString::vformat (const char * fmt, va_list arglist) {
	bstring b;
	int r, n;

	if (mlen <= 0) bstringThrow ("Write protection error");
	if (fmt == NULL) {
		*this = "<NULL>";
		bstringThrow ("CBString::format (NULL, ...) is erroneous.");
	} else {

		if ((b = bfromcstr ("")) == NULL) {
#ifdef BSTRLIB_THROWS_EXCEPTIONS
			bstringThrow ("CBString::format out of memory.");
#else
			*this = "<NULL>";
#endif
		} else {
			if ((n = 2*strlen (fmt)) < START_VSNBUFF) n = START_VSNBUFF;
			for (;;) {
				if (BSTR_OK != balloc (b, n + 2)) {
#ifdef BSTRLIB_THROWS_EXCEPTIONS
					bstringThrow ("CBString::format out of memory.");
#else
					b = bformat ("<NULL>");
					break;
#endif
				}

				exvsnprintf (r, (char *) b->data, n + 1, fmt, arglist);
				va_end (arglist);

				b->data[n] = '\0';
				b->slen = (strlen) ((char *) b->data);

				if (b->slen < n) break;
				if (r > n) n = r; else n += n;
			}
			*this = *b;
			bdestroy (b);
		}
	}
}

void CBString::formata (const char * fmt, ...) {
	va_list arglist;
	va_start (arglist, fmt);
  vformata (fmt, arglist);
}

void CBString::vformata (const char * fmt, va_list arglist) {
	bstring b;
	int r, n;

	if (mlen <= 0) bstringThrow ("Write protection error");
	if (fmt == NULL) {
		*this += "<NULL>";
		bstringThrow ("CBString::formata (NULL, ...) is erroneous.");
	} else {

		if ((b = bfromcstr ("")) == NULL) {
#ifdef BSTRLIB_THROWS_EXCEPTIONS
			bstringThrow ("CBString::format out of memory.");
#else
			*this += "<NULL>";
#endif
		} else {
			if ((n = 2*strlen (fmt)) < START_VSNBUFF) n = START_VSNBUFF;
			for (;;) {
				if (BSTR_OK != balloc (b, n + 2)) {
#ifdef BSTRLIB_THROWS_EXCEPTIONS
					bstringThrow ("CBString::format out of memory.");
#else
					b = bformat ("<NULL>");
					break;
#endif
				}

				exvsnprintf (r, (char *) b->data, n + 1, fmt, arglist);
				va_end (arglist);

				b->data[n] = '\0';
				b->slen = (strlen) ((char *) b->data);

				if (b->slen < n) break;
				if (r > n) n = r; else n += n;
			}
			*this += *b;
			bdestroy (b);
		}
	}
}

int CBString::caselessEqual (const CBString& b) const {
int ret;
	if (BSTR_ERR == (ret = biseqcaseless ((bstring) this, (bstring) &b))) {
		bstringThrow ("CBString::caselessEqual Unable to compare");
	}
	return ret;
}

int CBString::caselessCmp (const CBString& b) const {
int ret;
	if (SHRT_MIN == (ret = bstricmp ((bstring) this, (bstring) &b))) {
		bstringThrow ("CBString::caselessCmp Unable to compare");
	}
	return ret;
}

int CBString::find (const CBString& b, int pos) const {
	return binstr ((bstring) this, pos, (bstring) &b);
}

int CBString::find (const char * b, int pos) const {
int i, j;

	if (NULL == b) {
#ifdef BSTRLIB_THROWS_EXCEPTIONS
		bstringThrow ("CBString::find NULL.");
#else
		return BSTR_ERR;
#endif
	}

	if ((unsigned int) pos > (unsigned int) slen) return BSTR_ERR;
	if ('\0' == b[0]) return pos;
	if (pos == slen) return BSTR_ERR;

	i = pos;
	j = 0;

	for (;i + j < slen;) {
		if ((unsigned char) b[j] == data[i + j]) {
			j++;
			if ('\0' == b[j]) return i;
			continue;
		}
		i++;
		j=0;
	}

	return BSTR_ERR;
}

int CBString::find (char c, int pos) const {
	if (pos < 0) return BSTR_ERR;
	for (;pos < slen; pos++) {
		if (data[pos] == (unsigned char) c) return pos;
	}
	return BSTR_ERR;
}

int CBString::reversefind (const CBString& b, int pos) const {
	return binstrr ((bstring) this, pos, (bstring) &b);
}

int CBString::reversefind (const char * b, int pos) const {
struct tagbstring t;
	if (NULL == b) {
#ifdef BSTRLIB_THROWS_EXCEPTIONS
		bstringThrow ("CBString::find NULL.");
#else
		return BSTR_ERR;
#endif
	}
	cstr2tbstr (t, b);
	return binstrr ((bstring) this, pos, &t);
}

int CBString::reversefind (char c, int pos) const {
	if (pos > slen) return BSTR_ERR;
	if (pos == slen) pos--;
	for (;pos >= 0; pos--) {
		if (data[pos] == (unsigned char) c) return pos;
	}
	return BSTR_ERR;
}

int CBString::findchr (const CBString& b, int pos) const {
	return binchr ((bstring) this, pos, (bstring) &b);
}

int CBString::findchr (const char * s, int pos) const {
struct tagbstring t;
	if (NULL == s) {
#ifdef BSTRLIB_THROWS_EXCEPTIONS
		bstringThrow ("CBString::findchr NULL.");
#else
		return BSTR_ERR;
#endif
	}
	cstr2tbstr (t, s);
	return binchr ((bstring) this, pos, (bstring) &t);
}

int CBString::nfindchr (const CBString& b, int pos) const {
	return bninchr ((bstring) this, pos, (bstring) &b);
}

int CBString::nfindchr (const char * s, int pos) const {
struct tagbstring t;
	if (NULL == s) {
#ifdef BSTRLIB_THROWS_EXCEPTIONS
		bstringThrow ("CBString::nfindchr NULL.");
#else
		return BSTR_ERR;
#endif
	}
	cstr2tbstr (t, s);
	return bninchr ((bstring) this, pos, &t);
}

int CBString::reversefindchr (const CBString& b, int pos) const {
	return binchrr ((bstring) this, pos, (bstring) &b);
}

int CBString::reversefindchr (const char * s, int pos) const {
struct tagbstring t;
	if (NULL == s) {
#ifdef BSTRLIB_THROWS_EXCEPTIONS
		bstringThrow ("CBString::nfindchr NULL.");
#else
		return BSTR_ERR;
#endif
	}
	cstr2tbstr (t, s);
	return binchrr ((bstring) this, pos, &t);
}

int CBString::nreversefindchr (const CBString& b, int pos) const {
	return bninchrr ((bstring) this, pos, (bstring) &b);
}

int CBString::nreversefindchr (const char * s, int pos) const {
struct tagbstring t;
	if (NULL == s) {
#ifdef BSTRLIB_THROWS_EXCEPTIONS
		bstringThrow ("CBString::nfindchr NULL.");
#else
		return BSTR_ERR;
#endif
	}
	cstr2tbstr (t, s);
	return bninchrr ((bstring) this, pos, &t);
}

const CBString CBString::midstr (int left, int len) const {
struct tagbstring t;
	if (left < 0) {
		len += left;
		left = 0;
	}
	if (len > slen - left) len = slen - left;
	if (len <= 0) return CBString ("");
	blk2tbstr (t, data + left, len);
	return CBString (t);
}

void CBString::alloc (int length) {
	if (BSTR_ERR == balloc ((bstring)this, length)) {
		bstringThrow ("Failure in alloc");
	}
}

void CBString::fill (int length, unsigned char fill) {
	slen = 0;
	if (BSTR_ERR == bsetstr (this, length, NULL, fill)) {
		bstringThrow ("Failure in fill");
	}
}

void CBString::setsubstr (int pos, const CBString& b, unsigned char fill) {
	if (BSTR_ERR == bsetstr (this, pos, (bstring) &b, fill)) {
		bstringThrow ("Failure in setstr");
	}
}

void CBString::setsubstr (int pos, const char * s, unsigned char fill) {
struct tagbstring t;
	if (NULL == s) {
#ifdef BSTRLIB_THROWS_EXCEPTIONS
		bstringThrow ("setsubstr NULL.");
#else
		return;
#endif
	}
	cstr2tbstr (t, s);
	if (BSTR_ERR == bsetstr (this, pos, &t, fill)) {
		bstringThrow ("Failure in setstr");
	}
}

void CBString::insert (int pos, const CBString& b, unsigned char fill) {
	if (BSTR_ERR == binsert (this, pos, (bstring) &b, fill)) {
		bstringThrow ("Failure in insert");
	}
}

void CBString::insert (int pos, const char * s, unsigned char fill) {
struct tagbstring t;
	if (NULL == s) {
#ifdef BSTRLIB_THROWS_EXCEPTIONS
		bstringThrow ("insert NULL.");
#else
		return;
#endif
	}
	cstr2tbstr (t, s);
	if (BSTR_ERR == binsert (this, pos, &t, fill)) {
		bstringThrow ("Failure in insert");
	}
}

void CBString::insertchrs (int pos, int len, unsigned char fill) {
	if (BSTR_ERR == binsertch (this, pos, len, fill)) {
		bstringThrow ("Failure in insertchrs");
	}
}

void CBString::replace (int pos, int len, const CBString& b, unsigned char fill) {
	if (BSTR_ERR == breplace (this, pos, len, (bstring) &b, fill)) {
		bstringThrow ("Failure in replace");
	}
}

void CBString::replace (int pos, int len, const char * s, unsigned char fill) {
struct tagbstring t;
int q;

	if (mlen <= 0) bstringThrow ("Write protection error");
	if (NULL == s || (pos|len) < 0) {
		bstringThrow ("Failure in replace");
	} else {
		if (pos + len >= slen) {
			cstr2tbstr (t, s);
			if (BSTR_ERR == bsetstr (this, pos, &t, fill)) {
				bstringThrow ("Failure in replace");
			} else if (pos + t.slen < slen) {
				slen = pos + t.slen;
				data[slen] = '\0';
			}
		} else {

			/* Aliasing case */
			if ((unsigned int) (data - (unsigned char *) s) < (unsigned int) slen) {
				replace (pos, len, CBString(s), fill);
				return;
			}

			if ((q = strlen (s)) > len) {
				alloc (slen + q - len);
				if (NULL == data) return;
			}
			if (q != len) memmove (data + pos + q, data + pos + len, slen - (pos + len));
			memcpy (data + pos, s, q);
			slen += q - len;
			data[slen] = '\0';
		}
	}
}

void CBString::findreplace (const CBString& find, const CBString& repl, int pos) {
	if (BSTR_ERR == bfindreplace (this, (bstring) &find, (bstring) &repl, pos)) {
		bstringThrow ("Failure in findreplace");
	}
}

void CBString::findreplace (const CBString& find, const char * repl, int pos) {
struct tagbstring t;
	if (NULL == repl) {
#ifdef BSTRLIB_THROWS_EXCEPTIONS
		bstringThrow ("findreplace NULL.");
#else
		return;
#endif
	}
	cstr2tbstr (t, repl);
	if (BSTR_ERR == bfindreplace (this, (bstring) &find, (bstring) &t, pos)) {
		bstringThrow ("Failure in findreplace");
	}
}

void CBString::findreplace (const char * find, const CBString& repl, int pos) {
struct tagbstring t;
	if (NULL == find) {
#ifdef BSTRLIB_THROWS_EXCEPTIONS
		bstringThrow ("findreplace NULL.");
#else
		return;
#endif
	}
	cstr2tbstr (t, find);
	if (BSTR_ERR == bfindreplace (this, (bstring) &t, (bstring) &repl, pos)) {
		bstringThrow ("Failure in findreplace");
	}
}

void CBString::findreplace (const char * find, const char * repl, int pos) {
struct tagbstring t, u;
	if (NULL == repl || NULL == find) {
#ifdef BSTRLIB_THROWS_EXCEPTIONS
		bstringThrow ("findreplace NULL.");
#else
		return;
#endif
	}
	cstr2tbstr (t, find);
	cstr2tbstr (u, repl);
	if (BSTR_ERR == bfindreplace (this, (bstring) &t, (bstring) &u, pos)) {
		bstringThrow ("Failure in findreplace");
	}
}

void CBString::remove (int pos, int len) {
	if (BSTR_ERR == bdelete (this, pos, len)) {
		bstringThrow ("Failure in remove");
	}
}

void CBString::trunc (int len) {
	if (len < 0) {
		bstringThrow ("Failure in trunc");
	}
	if (len < slen) {
		slen = len;
		data[len] = '\0';
	}
}

void CBString::ltrim (const CBString& b) {
	int l = nfindchr (b, 0);
	if (l == BSTR_ERR) l = slen;
	remove (0, l);
}

void CBString::rtrim (const CBString& b) {
	int l = nreversefindchr (b, slen - 1);
	if (l == BSTR_ERR) l = slen - 1;
	slen = l + 1;
	if (mlen > slen) data[slen] = '\0';
}

void CBString::toupper () {
	if (BSTR_ERR == btoupper ((bstring) this)) {
		bstringThrow ("Failure in toupper");
	}
}

void CBString::tolower () {
	if (BSTR_ERR == btolower ((bstring) this)) {
		bstringThrow ("Failure in tolower");
	}
}

void CBString::repeat (int count) {
	count *= slen;
	if (count == 0) {
		trunc (0);
		return;
	}
	if (count < 0 || BSTR_ERR == bpattern (this, count)) {
		bstringThrow ("Failure in repeat");
	}
}

int CBString::gets (bNgetc getcPtr, void * parm, char terminator) {
	if (mlen <= 0) bstringThrow ("Write protection error");
	bstring b = bgets (getcPtr, parm, terminator);
	if (b == NULL) {
		slen = 0;
		return -1;
	}
	*this = *b;
	bdestroy (b);
	return 0;
}

int CBString::read (bNread readPtr, void * parm) {
	if (mlen <= 0) bstringThrow ("Write protection error");
	bstring b = bread (readPtr, parm);
	if (b == NULL) {
		slen = 0;
		return -1;
	}
	*this = *b;
	bdestroy (b);
	return 0;
}

const CBString operator + (const char *a, const CBString& b) {
	return CBString(a) + b;
}

const CBString operator + (const unsigned char *a, const CBString& b) {
	return CBString((const char *)a) + b;
}

const CBString operator + (char c, const CBString& b) {
	return CBString(c) + b;
}

const CBString operator + (unsigned char c, const CBString& b) {
	return CBString(c) + b;
}

const CBString operator + (const tagbstring& x, const CBString& b) {
	return CBString(x) + b;
}

void CBString::writeprotect () {
	if (mlen >= 0) mlen = -1;
}

void CBString::writeallow () {
	if (mlen == -1) mlen = slen + (slen == 0);
	else if (mlen < 0) {
		bstringThrow ("Cannot unprotect a constant");
	}
}

#if defined(BSTRLIB_CAN_USE_STL)

// Constructors.

CBString::CBString (const CBStringList& l) {
int c;
size_t i;

	for (c=1, i=0; i < l.size(); i++) {
		c += l.at(i).slen;
	}

	mlen = c;
	slen = 0;
	data = (unsigned char *) malloc (c);
	if (!data) {
		mlen = slen = 0;
		bstringThrow ("Failure in (CBStringList) constructor");
	} else {
		for (i=0; i < l.size(); i++) {
			*this += l.at(i);
		}
	}
}

CBString::CBString (const struct CBStringList& l, const CBString& sep) {
int c;
size_t i;

	for (c=1, i=0; i < l.size(); i++) {
		c += l.at(i).slen;
	}

	mlen = c + l.size () * sep.length ();
	slen = 0;
	data = (unsigned char *) malloc (mlen);
	if (!data) {
		mlen = slen = 0;
		bstringThrow ("Failure in (CBStringList) constructor");
	} else {
		for (i=0; i < l.size(); i++) {
			if (i > 0) *this += sep;
			*this += l.at(i);
		}
	}
}

CBString::CBString (const struct CBStringList& l, char sep) {
int c;
size_t i;

	for (c=1, i=0; i < l.size(); i++) {
		c += l.at(i).slen;
	}

	mlen = c + l.size ();
	slen = 0;
	data = (unsigned char *) malloc (mlen);
	if (!data) {
		mlen = slen = 0;
		bstringThrow ("Failure in (CBStringList) constructor");
	} else {
		for (i=0; i < l.size(); i++) {
			if (i > 0) *this += sep;
			*this += l.at(i);
		}
	}
}

CBString::CBString (const struct CBStringList& l, unsigned char sep) {
int c;
size_t i;

	for (c=1, i=0; i < l.size(); i++) {
		c += l.at(i).slen;
	}

	mlen = c + l.size ();
	slen = 0;
	data = (unsigned char *) malloc (mlen);
	if (!data) {
		mlen = slen = 0;
		bstringThrow ("Failure in (CBStringList) constructor");
	} else {
		for (i=0; i < l.size(); i++) {
			if (i > 0) *this += sep;
			*this += l.at(i);
		}
	}
}

void CBString::join (const struct CBStringList& l) {
int c;
size_t i;

	if (mlen <= 0) {
		bstringThrow ("Write protection error");
	}

	for (c=1, i=0; i < l.size(); i++) {
		c += l.at(i).slen;
	}

	alloc (c);
	slen = 0;
	if (!data) {
		mlen = slen = 0;
		bstringThrow ("Failure in (CBStringList) constructor");
	} else {
		for (i=0; i < l.size(); i++) {
			*this += l.at(i);
		}
	}
}

void CBString::join (const struct CBStringList& l, const CBString& sep) {
int c;
size_t i;

	if (mlen <= 0) {
		bstringThrow ("Write protection error");
	}

	for (c=1, i=0; i < l.size(); i++) {
		c += l.at(i).slen;
	}

	alloc (c + l.size () * sep.length ());
	slen = 0;
	if (!data) {
		mlen = slen = 0;
		bstringThrow ("Failure in (CBStringList) constructor");
	} else {
		for (i=0; i < l.size(); i++) {
			if (i > 0) *this += sep;
			*this += l.at(i);
		}
	}
}


void CBString::join (const struct CBStringList& l, char sep) {
int c;
size_t i;

	if (mlen <= 0) {
		bstringThrow ("Write protection error");
	}

	for (c=1, i=0; i < l.size(); i++) {
		c += l.at(i).slen;
	}

	alloc (c + l.size ());
	slen = 0;
	if (!data) {
		mlen = slen = 0;
		bstringThrow ("Failure in (CBStringList) constructor");
	} else {
		for (i=0; i < l.size(); i++) {
			if (i > 0) *this += sep;
			*this += l.at(i);
		}
	}
}

void CBString::join (const struct CBStringList& l, unsigned char sep) {
int c;
size_t i;

	if (mlen <= 0) {
		bstringThrow ("Write protection error");
	}

	for (c=1, i=0; i < l.size(); i++) {
		c += l.at(i).slen;
	}

	alloc (c + l.size ());
	slen = 0;
	if (!data) {
		mlen = slen = 0;
		bstringThrow ("Failure in (CBStringList) constructor");
	} else {
		for (i=0; i < l.size(); i++) {
			if (i > 0) *this += sep;
			*this += l.at(i);
		}
	}
}

// Split functions.

void CBStringList::split (const CBString& b, unsigned char splitChar) {
int p, i;

	p = 0;
	do {
		for (i = p; i < b.length (); i++) {
			if (b.character (i) == splitChar) break;
		}
		if (i > p) this->push_back (CBString (&(b.data[p]), i - p));
		p = i + 1;
	} while (p < b.length ());
}

void CBStringList::split (const CBString& b, const CBString& s) {
struct { unsigned long content[(1 << CHAR_BIT) / 32]; } chrs;
unsigned char c;
int p, i;

	if (s.length() == 0) bstringThrow ("Null splitstring failure");
	if (s.length() == 1) {
		this->split (b, s.character (0));
	} else {

		for (i=0; i < ((1 << CHAR_BIT) / 32); i++) chrs.content[i] = 0x0;
		for (i=0; i < s.length(); i++) {
			c = s.character (i);
			chrs.content[c >> 5] |= ((long)1) << (c & 31);
		}

		p = 0;
		do {
			for (i = p; i < b.length (); i++) {
				c = b.character (i);
				if (chrs.content[c >> 5] & ((long)1) << (c & 31)) break;
			}
			if (i > p) this->push_back (CBString (&(b.data[p]), i - p));
			p = i + 1;
		} while (p < b.length ());
	}
}

static int streamSplitCb (void * parm, int ofs, const bstring entry) {
CBStringList * r = (CBStringList *) parm;

	ofs = ofs;
	r->push_back (CBString (*entry));
	return 0;
}

void CBStringList::split (const CBStream& b, const CBString& s) {
	if (0 > bssplitscb (b.m_s, (bstring) &s, streamSplitCb, 
	                    (void *) this)) {
		bstringThrow ("Split bstream failure");
	}
}
 
void CBStringList::split (const CBStream& b, unsigned char splitChar) {
CBString sc (splitChar);
	if (0 > bssplitscb (b.m_s, (bstring) &sc,
	                    streamSplitCb, (void *) this)) {
		bstringThrow ("Split bstream failure");
	}
}

#endif

#if defined(BSTRLIB_CAN_USE_IOSTREAM)

std::ostream& operator << (std::ostream& sout, CBString b) {
	return sout.write ((const char *)b, b.length());
}

#include <ctype.h>

static int istreamGets (void * parm) {
	char c = '\n';
	((std::istream *)parm)->get(c);
	if (isspace (c)) c = '\n';
	return c;
}

std::istream& operator >> (std::istream& sin, CBString& b) {
	do {
		b.gets ((bNgetc) istreamGets, &sin, '\n');
		if (b.slen > 0 && b.data[b.slen-1] == '\n') b.slen--;
	} while (b.slen == 0 && !sin.eof ());
 	return sin;
}

struct sgetc {
	std::istream * sin;
	char terminator;
};

static int istreamGetc (void * parm) {
	char c = ((struct sgetc *)parm)->terminator;
	((struct sgetc *)parm)->sin->get(c);
	return c;
}

std::istream& getline (std::istream& sin, CBString& b, char terminator) {
struct sgetc parm;
	parm.sin = &sin;
	parm.terminator = terminator;
	b.gets ((bNgetc) istreamGetc, &parm, terminator);
	if (b.slen > 0 && b.data[b.slen-1] == terminator) b.slen--;
 	return sin;
}

#endif

CBStream::CBStream (bNread readPtr, void * parm) {
	m_s = bsopen (readPtr, parm);
}

CBStream::~CBStream () {
	bsclose (m_s);
}

int CBStream::buffLengthSet (int sz) {
	if (sz <= 0) {
		bstringThrow ("buffLengthSet parameter failure");
	}
	return bsbufflength (m_s, sz);
}

int CBStream::buffLengthGet () {
	return bsbufflength (m_s, 0);
}

CBString CBStream::readLine (char terminator) {
	CBString ret("");
	if (0 > bsreadln ((bstring) &ret, m_s, terminator) && eof () < 0) {
		bstringThrow ("Failed readLine");
	}
	return ret;
}

CBString CBStream::readLine (const CBString& terminator) {
	CBString ret("");
	if (0 > bsreadlns ((bstring) &ret, m_s, (bstring) &terminator) && eof () < 0) {
		bstringThrow ("Failed readLine");
	}
	return ret;
}

void CBStream::readLine (CBString& s, char terminator) {
	if (0 > bsreadln ((bstring) &s, m_s, terminator) && eof () < 0) {
		bstringThrow ("Failed readLine");
	}
}

void CBStream::readLine (CBString& s, const CBString& terminator) {
	if (0 > bsreadlns ((bstring) &s, m_s, (bstring) &terminator) && eof () < 0) {
		bstringThrow ("Failed readLine");
	}
}

void CBStream::readLineAppend (CBString& s, char terminator) {
	if (0 > bsreadlna ((bstring) &s, m_s, terminator) && eof () < 0) {
		bstringThrow ("Failed readLineAppend");
	}
}

void CBStream::readLineAppend (CBString& s, const CBString& terminator) {
	if (0 > bsreadlnsa ((bstring) &s, m_s, (bstring) &terminator) && eof () < 0) {
		bstringThrow ("Failed readLineAppend");
	}
}

#define BS_BUFF_SZ (1024)

CBString CBStream::read () {
	CBString ret("");
	while (!bseof (m_s)) {
		if (0 > bsreada ((bstring) &ret, m_s, BS_BUFF_SZ) && eof () < 0) {
			bstringThrow ("Failed read");
		}
	}
	return ret;
}

CBString CBStream::read (int n) {
	CBString ret("");
	if (0 > bsread ((bstring) &ret, m_s, n) && eof () < 0) {
		bstringThrow ("Failed read");
	}
	return ret;
}

void CBStream::read (CBString& s) {
	s.slen = 0;
	while (!bseof (m_s)) {
		if (0 > bsreada ((bstring) &s, m_s, BS_BUFF_SZ)) {
			bstringThrow ("Failed read");
		}
	}
}

void CBStream::read (CBString& s, int n) {
	if (0 > bsread ((bstring) &s, m_s, n)) {
		bstringThrow ("Failed read");
	}
}

void CBStream::readAppend (CBString& s) {
	while (!bseof (m_s)) {
		if (0 > bsreada ((bstring) &s, m_s, BS_BUFF_SZ)) {
			bstringThrow ("Failed readAppend");
		}
	}
}

void CBStream::readAppend (CBString& s, int n) {
	if (0 > bsreada ((bstring) &s, m_s, n)) {
		bstringThrow ("Failed readAppend");
	}
}

void CBStream::unread (const CBString& s) {
	if (0 > bsunread (m_s, (bstring) &s)) {
		bstringThrow ("Failed unread");
	}
}

CBString CBStream::peek () const {
	CBString ret ("");
	if (0 > bspeek ((bstring) &ret, m_s)) {
		bstringThrow ("Failed peek");
	}
	return ret;
}

void CBStream::peek (CBString& s) const {
	s.slen = 0;
	if (0 > bspeek ((bstring) &s, m_s)) {
		bstringThrow ("Failed peek");
	}
}

void CBStream::peekAppend (CBString& s) const {
	if (0 > bspeek ((bstring) &s, m_s)) {
		bstringThrow ("Failed peekAppend");
	}
}

int CBStream::eof () const {
	int ret = bseof (m_s);
	if (0 > ret) {
		bstringThrow ("Failed eof");
	}
	return ret;
}

} // namespace Bstrlib
