#
/*
 *    Copyright (C) 2013, 2014, 2016
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Programming
 *
 *    This file is part of the SDR-J (JSDR).
 *    SDR-J is free software; you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation; either version 2 of the License, or
 *    (at your option) any later version.
 *
 *    SDR-J is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with SDR-J; if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
#
#ifndef	_THE_DECODERBASE
#define	_THE_DECODERBASE
//
//	This file is to be included in the analogDecoder file

#define	DC_ALPHA	0.98

class	amDecoder;
class	decoderBase {
private:
	int32_t		rate;
public:
		decoderBase	(int32_t rate) {
	this	-> rate	= rate;
}

virtual		~decoderBase	(void) {
}

virtual
DSPCOMPLEX	decode		(DSPCOMPLEX z, float V) {
	(void)V;
	return z;
}
};

class	amDecoder: public decoderBase {
private:
float m_w1;
public:
		amDecoder	(int32_t rate):decoderBase (rate) {
	m_w1	= 0;
	fprintf (stderr, "new am decoder\n");
}
		~amDecoder	(void) {
}

DSPCOMPLEX	decode		(DSPCOMPLEX z, float Vol) {
float	res;
float	mag		= abs (z);
//
//	HF filtering according to CuteSDR
	float	w0	= mag + (DC_ALPHA * m_w1);
	res		= w0 - m_w1;
	m_w1		= w0;
	return DSPCOMPLEX (Vol * res, Vol * res);
}
};

class	lsbDecoder:public decoderBase {
private:
HilbertFilter SSB_Filter;
public:
		lsbDecoder	(int32_t rate):
	                                decoderBase (rate),
	                                SSB_Filter (31, 0.25, rate){
}

		~lsbDecoder	(void) {
}

DSPCOMPLEX	decode		(DSPCOMPLEX z, float Vol) {
	z	= SSB_Filter. Pass (z);
	return	 DSPCOMPLEX (Vol * (real (z) + imag (z)),
	                     Vol * (real (z) + imag (z)));
}
};

class	usbDecoder:public decoderBase {
private:
HilbertFilter SSB_Filter;
public:
		usbDecoder	(int32_t rate):
	                                decoderBase (rate), 
	                                SSB_Filter (31, 0.25, rate) {
}

		~usbDecoder	(void) {
}

DSPCOMPLEX	decode (DSPCOMPLEX z, double Vol) {
	z	= SSB_Filter. Pass (z);
	return  DSPCOMPLEX (Vol * (real (z) - imag (z)),
			    Vol * (real (z) - imag (z)));
}
};

class	isbDecoder:public decoderBase {
private:
HilbertFilter SSB_Filter;
public:
		isbDecoder	(int32_t rate): 
	                                decoderBase (rate),
		                        SSB_Filter (31, 0.25, rate) {
}

		~isbDecoder	(void) {
}

DSPCOMPLEX	decode (DSPCOMPLEX z, double Vol) {
	z	= SSB_Filter. Pass (z);
	return DSPCOMPLEX (Vol * (real (z) - imag (z)),
	                   Vol * (real (z) + imag (z)));
}
};


class	fmDecoder:public decoderBase {
private:
pllC	the_pll;
squelch	*mySquelch;
public:
		fmDecoder (int32_t rate, squelch *s):
	                                  decoderBase (rate),
	                                  the_pll (rate,
	                                           0,
	                                           - 0.95 * rate / 2,
	                                           + 0.95 * rate / 2,
	                                           0.85 * rate) {
	mySquelch	= s;
}

		~fmDecoder (void) {
}

DSPCOMPLEX	decoder	(DSPCOMPLEX z, float Vol) {
float	res;
float	I, Q;

	if (abs (z) <= 0.001)
	   I = Q = 0.001;
	else {
	   I = real (z) / abs (z);
	   Q = imag (z) / abs (z);
	}

	the_pll. do_pll (DSPCOMPLEX (I, Q), 1);
	res	= the_pll. getPhaseIncr ();
	res	= mySquelch -> do_squelch (res);
	return 	DSPCOMPLEX (Vol * res, Vol * res);
}
};

class	syncDecoder:public decoderBase {
private:
float	m_w1;
pllC	the_pll;
float	currLock;
public:
		syncDecoder (int32_t rate):
	                           decoderBase (rate),
	                           the_pll (rate,
	                                    0,
	                                    - 0.95 * rate / 2,
	                                    + 0.95 * rate / 2,
	                                    500) {
	m_w1	= 0;
	currLock	= 0;
}

		~syncDecoder (void) {
}

DSPCOMPLEX	decode	(DSPCOMPLEX z, float Vol) {
DSPFLOAT	res;

	the_pll. do_pll (z, -1);
	float	mag	= real (the_pll. getDelay ());
//	HF filtering according to CuteSDR
	float	w0	= mag + (DC_ALPHA * m_w1);
	res		= w0 - m_w1;
	m_w1		= w0;
	currLock	= 0.9999 * currLock +
	                  0.0001 * the_pll. getNco ();
	return DSPCOMPLEX (Vol * res, Vol * res);
}
};

#endif
