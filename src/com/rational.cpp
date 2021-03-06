/*@
Copyright (c) 2013-2014, Su Zhenyu steven.known@gmail.com
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of the Su Zhenyu nor the names of its contributors
      may be used to endorse or promote products derived from this software
      without specific prior written permission.

THIS SOFTWARE IS PROVIDED "AS IS" AND ANY
EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
@*/
#include "ltype.h"
#include "comf.h"
#include "rational.h"

#define REDUCE
#define abs(x) ((x) >= 0 ? (x) : -(x))

RATIONAL::RATIONAL()
{
	m_num = 0;
	m_den = 1;
}


RATIONAL::RATIONAL(RATIONAL const& r)
{
	//Sometimes, r need not to initialize always.
	//IS_TRUE(r.m_den != 0, ("denominator is 0!"));
	m_num = r.m_num;
	m_den = r.m_den;
}


RATIONAL::RATIONAL(INT num, INT den)
{
	IS_TRUE(den != 0, ("denominator is 0!"));
	m_num = num,  m_den = den;
}


RATIONAL & RATIONAL::operator = (RATIONAL const& a)
{
	IS_TRUE(a.m_den != 0, ("denominator is 0!"));
	m_num = a.m_num;
	m_den = a.m_den;
	return *this;
}


void RATIONAL::reduce()
{
	if (m_num == 0) {
		m_den = 1;
		return;
	}
	FRAC_TYPE gcd = _gcd(m_num, m_den);
	if (gcd == 1) {
		if (m_den < 0) {
			m_den = -m_den;
			m_num = -m_num;
		}
		return;
	}
	m_num = m_num / gcd;
	m_den = m_den / gcd;
	if (m_den < 0) {
		m_den = -m_den;
		m_num = -m_num;
	}
	return;
}


RATIONAL RATIONAL::rabs()
{
	IS_TRUE(m_den != 0, ("denominator is 0!"));
	RATIONAL b(*this);
	if (b.m_num < 0) {
		b.m_num = -b.m_num;
	}
	if (b.m_den < 0) {
		b.m_den = -b.m_den;
	}
	return b;
}


FRAC_TYPE RATIONAL::_gcd(FRAC_TYPE x, FRAC_TYPE y)
{
	FRAC_TYPE t;
	if (x < 0) { x = -x; }
	if (y < 0) { y = -y; }
	if ( x > y ) {
		t = x, x = y, y = t;
	}
	while (x) {
		t = x;
		x = y % x;
		y = t;
	}
	return y;
}


CHAR * RATIONAL::format(CHAR * buf)
{
	if (m_den == 1) {
		sprintf(buf, "%d", m_num);
	} else {
		sprintf(buf, "%d/%d", m_num, m_den);
	}
	return buf;
}


static inline LONGLONG gcdf(LONGLONG x, LONGLONG y)
{
	LONGLONG t;
	if (x < 0) { x = -x; }
	if (y < 0) { y = -y; }
	if ( x > y ) {
		t = x, x = y, y = t;
	}
	while (x) {
		t = x;
		x = y % x;
		y = t;
	}
	return y;
}


LONGLONG g_red_count = 0;
//Reduction of 64bit longlong integer.
static inline void reduce_ll(LONGLONG & num, LONGLONG & den)
{
	g_red_count++;
	if (num == 0) {
		den = 1;
		return;
	}
	LONGLONG gcd = gcdf(num, den);
	if (gcd == 1) {
		if (den < 0) {
			den = -den;
			num = -num;
		}
		return;
	}
	num = num / gcd;
	den = den / gcd;
	if (den < 0) {
		den = -den;
		num = -num;
	}
}


//Calculate the approximate rational number.
LONGLONG g_appro_count = 0;
static inline void appro(LONGLONG & num, LONGLONG & den)
{
	g_appro_count++;
	float v = float(num) / float(den);
	if (v < 100.0) {
		v = v * 1000000;
		num = INT(v);
		den = 1000000;
	} else if (v < 1000.0) {
		v = v * 100000;
		num = INT(v);
		den = 100000;
	} else if (v < 100000.0) {
		v = v * 10000;
		num = INT(v);
		den = 10000;
	} else if (v < 1000000.0) {
		v = v * 1000;
		num = INT(v);
		den = 1000;
	} else if (v < 10000000.0) {
		v = v * 100;
		num = INT(v);
		den = 100;
	} else if (v < 100000000.0) {
		v = v * 10;
		num = INT(v);
		den = 10;
	} else if (v < 2147483647.0) {
		num = INT(v);
		den = 1;
	} else {
		IS_TRUE(0, ("overflow the range of integer, 0x7fffFFFF."));
		num = 0;
		den = 1;
	}
	reduce_ll(num, den);
}


bool operator < (RATIONAL const& a, RATIONAL const& b)
{
	IS_TRUE(a.m_den != 0 && b.m_den != 0, ("denominator is 0!"));
	if ((LONGLONG)a.m_num * (LONGLONG)b.m_den <
		(LONGLONG)a.m_den * (LONGLONG)b.m_num) {
		return true;
	}
	return false;
}


bool operator <= (RATIONAL const& a, RATIONAL const& b)
{
	IS_TRUE(a.m_den != 0 && b.m_den != 0, ("denominator is 0!"));
	if (((LONGLONG)(a.m_num) * (LONGLONG)(b.m_den)) <=
		((LONGLONG)(a.m_den) * (LONGLONG)(b.m_num))) {
		return true;
	}
	return false;
}


bool operator > (RATIONAL const& a, RATIONAL const& b)
{
	IS_TRUE(a.m_den != 0 && b.m_den != 0, ("denominator is 0!"));
	if (((LONGLONG)(a.m_num) * (LONGLONG)(b.m_den)) >
		((LONGLONG)(a.m_den) * (LONGLONG)(b.m_num))) {
		return true;
	}
	return false;
}


bool operator >= (RATIONAL const& a, RATIONAL const& b)
{
	IS_TRUE(a.m_den != 0 && b.m_den != 0, ("denominator is 0!"));
	if (((LONGLONG)(a.m_num) * (LONGLONG)(b.m_den)) >=
		((LONGLONG)(a.m_den) * (LONGLONG)(b.m_num))) {
		return true;
	}
	return false;
}


RATIONAL operator * (RATIONAL const& a, RATIONAL const& b)
{
	IS_TRUE(a.m_den != 0 && b.m_den != 0, ("denominator is 0!"));
	LONGLONG rnum = (LONGLONG)(a.m_num) * (LONGLONG)(b.m_num);
	RATIONAL rat;
	if (rnum == 0) {
		rat.m_num = 0;
		rat.m_den = 1;
		return rat;
	}
	LONGLONG rden = (LONGLONG)(a.m_den) * (LONGLONG)(b.m_den);
	IS_TRUE(rden != 0, ("den is zero"));
	if (rnum == rden) { rat.m_num = 1; rat.m_den = 1; return rat; }
	if (rnum == -rden) { rat.m_num = -1; rat.m_den = 1; return rat; }
	if ((rnum < 0 && rden < 0) || rden < 0) { rnum = -rnum; rden = -rden; }
#ifdef REDUCE
	reduce_ll(rnum, rden);
#endif
	IS_TRUE0(rden > 0);
	LONGLONG trnum = abs(rnum);
	if ((trnum >= (LONGLONG)(INT_MAX>>2)) ||
		(rden >= (LONGLONG)(INT_MAX>>2))) {
		reduce_ll(trnum, rden);
		if ((trnum >= (LONGLONG)(INT_MAX)) ||
			(rden >= (LONGLONG)(INT_MAX))) {
			appro(trnum, rden);
			IS_TRUE0((trnum < (LONGLONG)(INT_MAX)) &&
					 (rden < (LONGLONG)(INT_MAX)));
		}
	}

	//Enforce conversion from 'int64' to 'int32',
	//even if it possible loss of data.
	rat.m_num = rnum < 0 ? -trnum : trnum;
	rat.m_den = rden;
	return rat;
}


RATIONAL operator / (RATIONAL const& a, RATIONAL const& b)
{
	FRAC_TYPE anum = a.m_num;
	FRAC_TYPE aden = a.m_den;
	FRAC_TYPE bnum = b.m_num;
	FRAC_TYPE bden = b.m_den;

	IS_TRUE(aden != 0 && bden != 0, ("denominator is 0"));
	IS_TRUE(bnum != 0, ("'a' divided by 0"));

	RATIONAL rat;
	if (anum == 0) { rat.m_num = 0; rat.m_den = 1; return rat; }
	if (anum == aden) {
		if (bnum < 0) {
			rat.m_num = -bden;
			rat.m_den = -bnum;
		} else {
			rat.m_num = bden;
			rat.m_den = bnum;
		}
		return rat;
	}

	LONGLONG ratnum = (LONGLONG)(anum) * (LONGLONG)(bden);
	LONGLONG ratden = (LONGLONG)(aden) * (LONGLONG)(bnum);
	if (ratnum == ratden) { rat.m_num = 1; rat.m_den = 1; return rat; }
	if (ratnum == -ratden) { rat.m_num = -1; rat.m_den = 1; return rat; }
	if ((ratnum < 0 && ratden < 0) || ratden < 0) {
		ratnum = -ratnum; ratden = -ratden;
	}
#ifdef REDUCE
	reduce_ll(ratnum, ratden);
#endif
	IS_TRUE0(ratden > 0);
	LONGLONG trnum = abs(ratnum);
	if ((trnum >= (LONGLONG)(INT_MAX >> 2)) ||
		(ratden >= (LONGLONG)(INT_MAX >> 2))) {
		reduce_ll(trnum, ratden);
		if ((trnum >= (LONGLONG)(INT_MAX)) ||
			(ratden >= (LONGLONG)(INT_MAX))) {
			appro(trnum, ratden);
			IS_TRUE0((trnum < (LONGLONG)(INT_MAX)) &&
					(ratden < (LONGLONG)(INT_MAX)));
		}
	}
	rat.m_num = ratnum < 0 ? -trnum : trnum;
	rat.m_den = ratden;
	return rat;
}


RATIONAL operator + (RATIONAL const& a, RATIONAL const& b)
{
	IS_TRUE(a.m_den != 0 && b.m_den != 0, ("denominator is 0!"));
	RATIONAL rat;
	LONGLONG rnum = (LONGLONG)(a.m_num) * (LONGLONG)(b.m_den) +
					(LONGLONG)(a.m_den) * (LONGLONG)(b.m_num);
	if (rnum == 0) {
		rat.m_num = 0;
		rat.m_den = 1;
		return rat;
	}
	LONGLONG rden = (LONGLONG)(a.m_den) * (LONGLONG)(b.m_den);
	IS_TRUE(rden != 0, ("den is 0"));
	if (rnum == rden) { rat.m_num = 1; rat.m_den = 1; return rat; }
	if (rnum == -rden) { rat.m_num = -1; rat.m_den = 1; return rat; }
	if ((rnum < 0 && rden < 0) || rden < 0) { rnum = -rnum; rden = -rden; }
#ifdef REDUCE
	reduce_ll(rnum, rden);
#endif
	IS_TRUE0(rden > 0);
	LONGLONG trnum = abs(rnum);
	if ((trnum >= (LONGLONG)(INT_MAX>>2)) ||
		(rden >= (LONGLONG)(INT_MAX>>2))) {
		reduce_ll(trnum, rden);
		if ((trnum >= (LONGLONG)(INT_MAX)) ||
			(rden >= (LONGLONG)(INT_MAX))) {
			appro(trnum, rden);
			IS_TRUE0((trnum < (LONGLONG)(INT_MAX)) &&
					(rden < (LONGLONG)(INT_MAX)));
		}
	}
	rat.m_num = rnum < 0 ? -trnum : trnum;
	rat.m_den = rden;
	return rat;
}