#
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
 *
 */
#include	"attenuator.h"

	attenuator::attenuator (void) {
	maxOffset		= 8;
	buffer 			= new DSPCOMPLEX [2 * maxOffset];
	symbol_Leftshift	= 0;
	counter			= 0;
	attValueL		= 100;
	attValueR		= 100;
}

	attenuator::~attenuator	(void) {
	delete[] buffer;
}

void	attenuator::set_hfGain	(int16_t balance, int16_t atten) {
int16_t	Bl, Br;
	Bl		= 100 - balance;
	Br		= 100 + balance;
	attValueL	= 10 * atten * (float)Bl / 100;
	attValueR	= 10 * atten * (float)Br / 100;
}
//
//	We allow shifting the left component, i.e. taking the
//	real component from a different sample than the imag component.
DSPCOMPLEX	attenuator::shift_and_attenuate (DSPCOMPLEX s) {
DSPCOMPLEX t;
	buffer [counter] = (s);
	int16_t index	= (counter - maxOffset + 2 * maxOffset) % 2 * maxOffset;
	counter	= (counter + 1) % maxOffset;

	if (symbol_Leftshift != 0) {
	   int16_t indexL = (index + symbol_Leftshift + 2 * maxOffset) %
	                                                 (2 * maxOffset);
	   t = DSPCOMPLEX (real (buffer [indexL]), imag (buffer [index]));
	}
	else
	   t = buffer [index];

	return DSPCOMPLEX (attValueL * real (s),
	                   attValueR * imag (s));
}

void	attenuator::setShift	(int16_t s) {
	if (s < -maxOffset)
	   s = -maxOffset;
	if (s > maxOffset)
	   s = maxOffset;
	symbol_Leftshift	= s;
}

