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

#ifndef	__SNR_CALC
#define	__SNR_CALC

#include	<math.h>
#include	"swradio-constants.h"

class	common_fft;

class	SNRcalc {
public:
		SNRcalc		(int32_t);
		~SNRcalc	(void);
	bool	addElement	(DSPCOMPLEX, float *);
private:
	int32_t		theRate;
	common_fft	*myfft;
	DSPCOMPLEX	*fftbuffer;
	int32_t		count;
	int32_t		bufferp;

	int32_t		getBin	(int32_t);
};
#endif


