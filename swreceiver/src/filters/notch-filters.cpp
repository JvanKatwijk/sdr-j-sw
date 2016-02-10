#
/*
 *    Copyright (C) 2015
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
 *
 */

#include	"notch-filters.h"

		notchFilter::notchFilter (int32_t rate, int32_t freq) {
	this	-> rate	= rate;
	setFrequency	(freq);
//	and the "buffer"
	x_prev	= DSPCOMPLEX (1, 1);
	y_prev	= DSPCOMPLEX (1, 1);
	x_prev_prev	= x_prev;
	y_prev_prev	= y_prev;
}

void	notchFilter::setFrequency	(int32_t freq) {
float	BW	= 0.0066;
float	f	= (float)freq / rate;
float	R	= 1 - 3 * BW;
float	K	= 1 - 2 * R * cos (2 * M_PI * f) + R * R;
	K	= K / (2  -  2 * cos (2 * M_PI * f));

	a0	= K;
	a1	= -2 * K * cos (2 * M_PI * f);
	a2	= K;
	b1	= 2 * R * cos (2 * M_PI * f);
	b2	= -R * R;
}

		notchFilter::~notchFilter (void) {
}

DSPCOMPLEX	notchFilter::Pass	(DSPCOMPLEX x) {
DSPCOMPLEX	y;

	y	= cmul (x, a0) + cmul (x_prev, a1) + cmul (x_prev_prev, a2)
	                       + cmul (y_prev, b1) + cmul (y_prev_prev, b2);

	x_prev_prev	= x_prev;
	x_prev		= x;
	y_prev_prev	= y_prev;
	y_prev		= y;
	return y;
}


