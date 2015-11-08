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

#include	"sstv-buffer.h"
#include	"sdft.h"

		sstvBuffer::sstvBuffer 	(int amountTones, int32_t rate) {
int16_t	i;
	this	-> amountTones = amountTones;
	this	-> theRate	= rate;
//
//	we shift segments of 15 msec each time 10 msec
	bufferLength	= 3 * rate / (2 * 100);
	fftLength	= rate / 100;
	myFFT		= new sliding_fft (bufferLength);
	sampleCounter	= 0;
	toneFirst	= 0;
	toneLast	= 0;
	nrTones		= 0;
	tones		= new float [amountTones];	
	binfor500Hz	= 500 / (rate / bufferLength);
	binfor3300Hz	= 3300 / (rate / bufferLength);
}

		sstvBuffer::~sstvBuffer	(void) {
	delete myFFT;
	delete [] tones;
}

void	sstvBuffer::reset		(void) {
	sampleCounter	= 0;
	toneFirst	= 0;
	toneLast	= 0;
	nrTones		= 0;
}

int16_t	sstvBuffer::tonesAvailable	(void) {
	return nrTones;
}

void	sstvBuffer::addElement	(DSPCOMPLEX s) {
	myFFT -> add_data (s);
	if (++sampleCounter < fftLength)
	   return;
	tones [toneLast] = getBest (myFFT -> getSpectrum ());
	if (nrTones == amountTones) {
	   toneLast = (toneLast + 1) % amountTones;
	   toneFirst = (toneFirst + 1) % amountTones;
	}
	else {
	   nrTones ++;
	   toneLast = (toneLast + 1) % amountTones;
	}
	sampleCounter = 0;
}

float	sstvBuffer::getBest (DSPCOMPLEX *v) {
int16_t	bestBin = 0;
float	Max	= -1;
int16_t	i;
float	Power [bufferLength];
float	res	= 0;

static	int cnt = 0;
	for (i = 0; i < bufferLength / 2; i ++) {
	   Power [i] = abs (v [i]);
	   if (Power [i] > Max) {
	      bestBin	= i;
	      Max	= Power [i];
	   }
	}
	res	= binfor500Hz;	// just a default

	if ((bestBin > binfor500Hz) && (bestBin < binfor3300Hz) &&
	   (Power [bestBin] > 0) && (Power [bestBin + 1] > 0) &&
	                          (Power [bestBin - 1] > 0))
	   res = bestBin +
	           (log (Power [bestBin + 1] / Power[bestBin - 1] )) /
                    (2 * log (pow (Power[bestBin], 2) / (Power[bestBin + 1] * Power[bestBin - 1])));
	
//	In Hertz
	res  = res / bufferLength * theRate;
	return res;
}

void	sstvBuffer::getTones	(float *b) {
int16_t	i;
	for (i = 0; i < nrTones; i ++)
	   b [i] = tones [(toneFirst + i) % nrTones];
}
//
//	shift indicates that the "old" tones are to be deleted
//	and space is made for some new tones to arrive
void	sstvBuffer::shift	(int16_t t) {
	toneFirst	+= t;
	nrTones		-= t;
	if (toneFirst >= 45)
	   toneFirst -= 45;
	if (nrTones < 0) {
	   nrTones = 0;
	   toneLast = toneFirst;
	}
}

