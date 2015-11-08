#
/*
 *    Copyright (C) 2014
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

#include	<math.h>
#include	"fft.h"
#include	"snr-calc.h"

#define	FFT_SIZE	1024

		SNRcalc::SNRcalc (int32_t rate) {
	theRate		= rate;
	myfft		= new common_fft (rate / 4);
	fftbuffer	= myfft	-> getVector ();
	count		= 0;
	bufferp		= 0;
}

		SNRcalc::~SNRcalc	(void) {
	delete myfft;
}

bool	SNRcalc::addElement	(DSPCOMPLEX s, float *out) {
double	video_plus_noise	= 0;
double	noise_only		= 0;
int32_t	i;

	fftbuffer [bufferp] = s;
	if (++bufferp < theRate / 4)
	   return false;

	bufferp	= 0;
	myfft	-> do_FFT ();

	for (i = getBin (1500); i < getBin (2300); i ++)
	   video_plus_noise	+= abs (fftbuffer [i]);
	
//	Calculate noise-only power (400-800 Hz + 2700-3400 Hz)
	for (i = getBin (400); i < getBin (800); i ++)
	   noise_only		+= abs (fftbuffer [i]);

	for (i = getBin (2700); i < getBin (3400); i ++)
	   noise_only		+= abs (fftbuffer [i]);
//	Bandwidths
int32_t	video_plus_noise_bins	= getBin (2300) - getBin (1500) + 1;
int32_t	noise_only_bins		= getBin (800)  - getBin (400)  + 1 +
	                          getBin (3400) - getBin (2700) + 1;

int32_t	receiverBins		= getBin (3400) - getBin (400);

//	Eq 15
float	Pnoise  = noise_only * (1.0 * receiverBins / noise_only_bins);
float	Psignal = video_plus_noise -
	          noise_only * (1.0 * video_plus_noise_bins / noise_only_bins);

// Lower bound to -20 dB
	*out = ((Psignal / Pnoise < .01) ? -20 :
	                         10 * log10(Psignal / Pnoise));
	return true;
}
//
//	calculate the bin for the frequency f
int32_t	SNRcalc::getBin (int32_t f) {
	return  f / 4;
}

