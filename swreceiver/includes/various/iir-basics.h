#
/*
 *    Copyright (C) 2008, 2009, 2010
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

#ifndef	__IIR_BASICS
#define	__IIR_BASICS

#include	"swradio-constants.h"

struct biquad {
	DSPFLOAT	A0, A1, A2;
	DSPFLOAT	B0, B1, B2;
};

typedef	struct biquad element;

#define	MAXORDER	0176

	DSPFLOAT	Epsf		(int);
int	guessOrder		(int, int, int, int);
int	guessOrderB		(int, int, int, int);
int	guessOrderC		(int, int, int, int);
int	guessOrderIC		(int, int, int, int);

DSPFLOAT	warpDtoA		(int, int);
int	warpAtoD		(DSPFLOAT, int);
DSPFLOAT	newButterworth		(element *, int, int, DSPFLOAT);
DSPFLOAT	newChebyshev		(element *, int, int, DSPFLOAT);
DSPFLOAT	newChebyshevI		(element *, int, int, DSPFLOAT);

DSPFLOAT	newNormalized		(element *, int, int, int, int);
DSPFLOAT	unnormalizeLP		(element *, DSPFLOAT, int);
DSPFLOAT	unnormalizeHP		(element *, DSPFLOAT, int);
DSPFLOAT	unnormalizeBP		(element *, element *, DSPFLOAT, DSPFLOAT, int);

DSPFLOAT	Bilineair		(element *, int, int);
void	cQuadratic			(DSPCOMPLEX, DSPCOMPLEX,
	                         	 DSPCOMPLEX,
	                                 DSPCOMPLEX *, DSPCOMPLEX *);
DSPFLOAT	sinhm1			(DSPFLOAT);
DSPFLOAT	coshm1			(DSPFLOAT);

#endif

