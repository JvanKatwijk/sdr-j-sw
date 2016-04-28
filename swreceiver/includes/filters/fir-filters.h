#
/*
 *    Copyright (C) 2010, 2011, 2012
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Programming
 *
 *    This file is part of the SDR-J.
 *    Many of the ideas as implemented in SDR-J are derived from
 *    other work, made available through the GNU general Public License. 
 *    All copyrights of the original authors are recognized.
 *
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

#ifndef __FIR_FILTERS
#define __FIR_FILTERS

#include	"swradio-constants.h"

class	HilbertFilter;

class	Basic_FIR	{
	public:
	int16_t		filterSize;
	DSPCOMPLEX	*filterKernel;
	DSPCOMPLEX	*Buffer;
	int16_t		ip;
	int32_t		sampleRate;

			Basic_FIR (int16_t size) {
	int16_t		i;
	filterSize	= size;
	filterKernel	= new DSPCOMPLEX [filterSize];
	Buffer		= new DSPCOMPLEX [filterSize];
	ip		= 0;

	for (i = 0; i < filterSize; i ++) {
	   filterKernel [i]	= 0;
	   Buffer [i]		= 0;
	}
}

			~Basic_FIR (void) {
	delete[]	filterKernel;
	delete[]	Buffer;
}
//
//	we process the samples backwards rather than reversing
//	the kernel
	DSPCOMPLEX		Pass (DSPCOMPLEX z) {
int16_t	i;
DSPCOMPLEX	tmp	= 0;

	Buffer [ip]	= z;
	for (i = 0; i < filterSize; i ++) {
	   int16_t index = ip - i;
	   if (index < 0)
	      index += filterSize;
	   tmp		+= Buffer [index] * filterKernel [i];
	}

	ip = (ip + 1) % filterSize;
	return tmp;
}

	DSPFLOAT	Pass (DSPFLOAT v) {
int16_t		i;
DSPFLOAT	tmp	= 0;

	Buffer [ip] = DSPCOMPLEX (v, 0);
	for (i = 0; i < filterSize; i ++) {
	   int16_t index = ip - i;
	   if (index < 0)
	      index += filterSize;
	   tmp += real (Buffer [index]) * real (filterKernel [i]);
	}

	ip = (ip + 1) % filterSize;
	return tmp;
}

	void		setTaps (int16_t len,
	                         DSPFLOAT *itaps, DSPFLOAT *qtaps) {
int16_t	i;

	if (len != filterSize)
	   fprintf (stderr, "H E L P\n");

	for (i = 0; i < len; i ++) {
	   DSPFLOAT re = 0;
	   DSPFLOAT im = 0;

	   if (itaps != NULL)
	      re = itaps [i];
	   if (qtaps != NULL)
	      im = qtaps [i];
	   filterKernel [i] = DSPCOMPLEX (re, im);
	}
}
	      
	
};

class	LowPassFIR : public Basic_FIR {
public:
			LowPassFIR (int16_t,	// order
	                            int32_t, 	// cutoff frequency
	                            int32_t	// samplerate
	                           );
			~LowPassFIR (void);
	DSPCOMPLEX	*getKernel	(void);
	void		newKernel	(int32_t);	// cutoff
};
//
//	Both for lowpass band bandpass, we provide:
class	DecimatingFIR: public Basic_FIR {
public:
		         DecimatingFIR	(int16_t, int32_t, int32_t, int16_t);
	                 DecimatingFIR	(int16_t, int32_t, int32_t,
	                                                   int32_t, int16_t);
			~DecimatingFIR	(void);
	void		newKernel	(int32_t);
	void		newKernel	(int32_t, int32_t);
	bool		Pass	(DSPCOMPLEX, DSPCOMPLEX *);
	bool		Pass	(DSPFLOAT, DSPFLOAT *);
private:
	int16_t	decimationFactor;
	int16_t	decimationCounter;
};

class	HighPassFIR: public Basic_FIR {
public:
			HighPassFIR	(int16_t, int32_t, int32_t);
			~HighPassFIR	(void);
	void		newKernel	(int32_t);
};

class	bandpassFIR: public Basic_FIR {
public:
			bandpassFIR	(int16_t, int32_t, int32_t, int32_t);
			~bandpassFIR	(void);
	DSPCOMPLEX	*getKernel	(void);
	void		newKernel	(int32_t, int32_t);
private:
};

class	BasicBandPass: public Basic_FIR {
public:
			BasicBandPass	(int16_t, int32_t, int32_t, int32_t);
			~BasicBandPass	(void);
	DSPCOMPLEX	*getKernel	(void);
private:
};

class	adaptiveFilter {
public:
		adaptiveFilter	(int16_t, float);
		~adaptiveFilter	();
	DSPCOMPLEX	Pass		(DSPCOMPLEX);

private:
	int16_t		ip;
	float		err;
	float		mu;
	int16_t		firsize;
	DSPFLOAT	*Kernel;
	DSPCOMPLEX	*Buffer;
};

class HilbertFilter {
public:
		HilbertFilter	(int16_t, DSPFLOAT, int32_t);
		~HilbertFilter	();
	DSPCOMPLEX	Pass		(DSPCOMPLEX);
	DSPCOMPLEX	Pass		(DSPFLOAT, DSPFLOAT);
private:
	int16_t		ip;
	int16_t		firsize;
	int32_t		rate;
	DSPFLOAT	*cosKernel;
	DSPFLOAT	*sinKernel;
	DSPCOMPLEX	*Buffer;
	void		adjustFilter	(DSPFLOAT);
};

#endif

