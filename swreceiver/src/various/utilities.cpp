#
/*
 *    Copyright (C) 2014
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair programming
 *
 *    This file is part of the SDR-J (JSDR).
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

#include	"utilities.h"

DSPFLOAT	decayingAverage (DSPFLOAT old,
	                         DSPFLOAT input, DSPFLOAT weight) {
	if (weight <= 1)
	   return input;
	return input * (1.0 / weight) + old * (1.0 - (1.0 / weight));
}

DSPFLOAT	clamp (DSPFLOAT X, DSPFLOAT Min, DSPFLOAT Max) {
	if (X > Max)
	   return Max;
	if (X < Min)
	   return Min;
	return X;
}

	average::average (int16_t s) {
int16_t	i;

	size	= s;
	vec	= new DSPFLOAT [s];
	filp	= 0;

	for (i = 0; i < s; i ++)
	   vec [i] = 0;
}

	average::~average () {
	delete[]	vec;
}

DSPFLOAT	average::filter (DSPFLOAT e) {
int16_t	i;
DSPFLOAT	out	= 0.0;

	vec [filp] = e;
	filp = (filp + 1) % size;

	for (i = 0; i < size; i ++)
	   out += vec [i];

	return out / size;
}

void	average::clear (float c ) {
int16_t	i;

	for (i = 0; i < size; i ++)
	   vec [i] = c;
	filp = 0;
}


int32_t	nearestTwoPower (int16_t n) {
int32_t	res	= 1;

	if (n < 100)
	   return 128;
	while (n != 0) {
	   n >>= 1;
	   res <<= 1;
	}

	return res;
}

