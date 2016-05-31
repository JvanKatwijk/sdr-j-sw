#
/*
 *    Copyright (C) 2010, 2011
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
#ifndef	__PLL_FILTER
#define	__PLL_FILTER
/*
 */
#include	<math.h>

class	Lowpass_biquad {
private:
	float	freq;
	int	rate;
	float	Q;

	double	omega;
	double	sn;
	double	cs;
	double	Alpha;

	double	b0, b1, b2;
	double	a0, a1, a2;

	double	x1, x2;
	double	y1, y2;
public:
		Lowpass_biquad	(int rate,
	                         float freq,
	                         float Q) {
	this	-> rate		= rate;
	this	-> freq		= freq;
	this	-> Q		= Q;

	omega	= 2 * M_PI * freq / rate;
	sn	= sin (omega);
	cs	= cos (omega);
	Alpha	= sn / (2 * Q);

	b0	= (1 - cs) / 2;
	b1	= 1 - cs;
	b2	= (1 - cs) / 2;
	a0	= 1 + Alpha;
	a1	= -2 * cs;
	a2	= 1 - Alpha;
//	and scale
	b0	/= a0;
	b1	/= a0;
	b2	/= a0;
	a1	/= a0;
	a2	/= a0;

	x1	= 0;
	x2	= 0;
	y1	= 0;
	y2	= 0;
}

float	Pass	(float x) {
float	y;
static int cnt	= 0;
	y	= b0 * x + b1 * x1 + b2 * x2 - a1 * y1 - a2 * y2;
	x2	= x1;
	x1	= x;
	y2	= y1;
	y1	= y;
	return y;
}
};

#endif

