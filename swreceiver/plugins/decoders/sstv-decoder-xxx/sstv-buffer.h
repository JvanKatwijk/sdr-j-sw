#
/*
 *    Copyright (C) 2011, 2012, 2013
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Computing
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

#ifndef	__SSTV_BUFFER
#define	__SSTV_BUFFER

#include	"math.h"
#include	"swradio-constants.h"
class	sliding_fft;


class	sstvBuffer {
public:
		sstvBuffer 	(int amountTones, int32_t rate);
		~sstvBuffer	(void);
int16_t		tonesAvailable	(void);
void		addElement	(DSPCOMPLEX s);
float		getBest		(DSPCOMPLEX *v);
void		getTones	(float *b);
void		shift		(int16_t t);
void		reset		(void);
private:

	int32_t		theRate;
	int16_t		bufferLength;
	sliding_fft	*myFFT;
	int16_t		amountTones;
	int16_t		sampleCounter;
	int16_t		fftLength;
	int16_t		toneFirst;
	int16_t		toneLast;
	int16_t		nrTones;
	float		*tones;
	int16_t		binfor500Hz;
	int16_t		binfor3300Hz;
};
#endif

