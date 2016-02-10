#
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//%%                                                                         %
//%%  University of Kaiserslautern, Institute of Communications Engineering  %
//%%  Copyright (C) 2004 Andreas Dittrich                                    %
//%%                                                                         %
//%%  Author(s)    : Andreas Dittrich (dittrich@eit.uni-kl.de)               %
//%%  Project start: 27.05.2004                                              %
//%%  Last change: 02.05.2005, 11:30                                         %
//%%  Changes      : |                                                       %
//%%                                                                         %
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
/*************************************************************************
*
*                           PA0MBO
*
*    COPYRIGHT (C)  2009  M.Bos 
*
*    This file is part of the distribution package RXAMADRM
*
*    This package is free software and you can redistribute is
*    and/or modify it under the terms of the GNU General Public License
*
*    More details can be found in the accompanying file COPYING
*************************************************************************/
/*
 *    Copyright (C) 2015
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair programming
 *
 *    This file is part of the SDR-J (JSDR).
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
 *	The "getMode" function is a reimplementation of the
 *	MMSE algorithm (described in e.q. Tsai), some parts of it
 *	are taken over (or at least inspired by) the RXAMADRM software
 *	and its origin, diorama. 
 */
#include	"syncer.h"
#include	"simplebuf.h"
#include	"drm-decoder.h"
#include	"basics.h"

#define	BUFFER_SIZE	16384
#define	symbols_to_check(m) (((nSamples + 1) / Ts_of (m)) - 1)

	Syncer::Syncer (RingBuffer<DSPCOMPLEX> *b,
	                drmDecoder *mr, int32_t nSamples):simpleBuf (b, BUFFER_SIZE, mr) {
//
//	for mode determination:
	this	-> nSamples	= nSamples;
//
//	Tu_of (Mode_D) is the smallest of all, so the vectors
//	all have sufficient length
	correlations		= new DSPCOMPLEX [nSamples - Tu_of (Mode_D)];
	summedCorrelations	= new DSPCOMPLEX [nSamples - Tu_of (Mode_D)];
	squares 		= new float [nSamples - Tu_of (Mode_D)];
	summedSquares		= new float [nSamples - Tu_of (Mode_D)];
	EPSILON			= 1.0E-10;
}

	Syncer::~Syncer (void) {
	delete []	correlations;
	delete []	summedCorrelations;
	delete []	squares;
	delete []	summedSquares;
}
//
//	Formula 5.16 from Tsai reads
//	Phi (m) = sum (r=0, R-1, abs (z[m - r] ^ 2)) +
//	          sum (r=0, R-1, abs (z[m - r - L]) ^ 2) -
//	       2 * abs (sum (r=0, R-1, z [m - r] * conj (z [m - r - L])));
//	The start of the symbol is there where
//	Phi (m) is minimal
//
void	Syncer::getMode (smodeInfo	*result) {
DSPCOMPLEX	gamma		[320];
float		squareTerm	[320];
float	list_gammaRelative 	[]	= {0.0, 0.0, 0.0, 0.0};
float	gammaRelative;
float	list_epsilon		[]	= {0.0, 0.0, 0.0, 0.0};
int16_t	list_Offsets 		[]	= {0,   0,   0,   0};
int16_t	Ts, Tu, Tg;
int32_t t_smp;
int16_t mode;
int32_t i, j, theOffset;
//float	a [symbols_to_check (Mode_A)];
int16_t b [symbols_to_check (Mode_A)];
int16_t  averageOffset;
int16_t	theMode;

	needed (nSamples);
//
//	just try all modes
	for (mode = Mode_A; mode <= Mode_D; mode++) {
	   Ts		= Ts_of (mode);
	   Tu		= Tu_of (mode);
	   Tg		= Ts - Tu;
	   memset (gamma, 0, Ts * sizeof (DSPCOMPLEX));
	   memset (squareTerm, 0, Ts * sizeof (float));

//	"correlations" contains the input samples, convolved with the samples
//	Tu locations further
//	In the same run over the input we precompute the 
//	squares s [i] ^ 2 + s [i + Tu] ^ 2
	   for (i = 0; i < nSamples - Tu; i ++) {
	      correlations [i] = data [(i + currentIndex) % bufSize] *
	                     conj (data [(i + Tu + currentIndex) % bufSize]);
	      DSPCOMPLEX f1 = data [(i + currentIndex) % bufSize];
	      DSPCOMPLEX f2 = data [(i + Tu + currentIndex) % bufSize];
	      squares [i] = real (f1 * conj (f1)) + real (f2 * conj (f2));
	   }
//	add up the Tg correlations, starting with i
//	and compute the quadratic term
//	and the quadratic terms
	   for (i = 0; i < nSamples - Tu; i ++) {
	      summedCorrelations [i] = DSPCOMPLEX (0, 0);
	      summedSquares [i] = 0;
	      for (j = 0; j < Tg; j ++) {
	         summedCorrelations [i] += correlations [i + j];
	         summedSquares [i] += squares [i + j];
	      }
	   }
//
//	Now summate over the symbols
	   for (j = 0; j < symbols_to_check (mode); j++) {
	      for (i = 0; i < Ts; i++) {
	         gamma [i]	+= summedCorrelations [j * Ts + Tg + i];
	         squareTerm [i]	+= (float) 0.5 * (EPSILON +
	                               summedSquares [j * Ts + Tg + i]);
	      }
	   }
//
	   theOffset		= 0;
	   float minValue = 10000.0;
	   for (i = 0; i < Ts; i ++) {
	      float mmse = squareTerm [i] - 2 * abs (gamma [i]);
	      if (mmse < minValue) {
	         minValue = mmse;
	         theOffset = i;
	      }
	   }
//
	   gammaRelative = abs (gamma [theOffset]) / squareTerm [theOffset];
	   list_epsilon  [mode - Mode_A] = (float) arg (gamma [theOffset]);
	   list_gammaRelative [mode - Mode_A] = gammaRelative;
	   list_Offsets [mode - Mode_A]	= theOffset;
	}
//
//	when here, we have collected for all modes the gamma and the theta

//	now decide for the mode to be detected */
	theMode	= Mode_B;		// default
	gammaRelative = -1.0E20;
	for (i = Mode_A; i <= Mode_D; i++) {
	   if (list_gammaRelative [i - Mode_A] > gammaRelative) {
	      gammaRelative = list_gammaRelative [i - Mode_A];
	      theMode = i;
	   }
	}
//
//	check if result is reliable */
	bool	maxOK = true;			/* assume reliable */
	if (gammaRelative > 0.7) {
	   for (i = Mode_A; i <= Mode_D; i++) {
	      if ((i != theMode) && (list_gammaRelative [i - Mode_A] > 0.4))
	         maxOK = false;
	   }
	}
	else 
	   maxOK = false;

	if (!maxOK)	{		// no reliable mode found
	   result -> mode_indx		= -1;
	   result -> time_offset	= 0.0;
	   result -> sample_rate_offset	= 0.0;
	   result -> freq_offset_fract	= 0.0;
	   return;
	}

//
//	OK, we seem to have a mode "mode". The offset indicates the
//	offset in the buffer, and therefore the timeoffset.

	Ts = Ts_of (theMode);
	Tu = Tu_of (theMode);
	Tg = Ts - Tu;
	averageOffset = list_Offsets [theMode - 1];
//
//	Now recompute several vars from the selected mode
//
//	Again, first precomputing correlations and squares
	for (i = 0; i < nSamples - Tu; i++) {
	      correlations [i] = data [(i + currentIndex) % bufSize] *
	                     conj (data [(i + Tu + currentIndex) % bufSize]);
	   DSPCOMPLEX f1 = data [(i + currentIndex) % bufSize];
	   DSPCOMPLEX f2 = data [(i + Tu + currentIndex) % bufSize];
	   squares [i] = real (f1 * conj (f1)) + real (f2 * conj (f2));
	}

	for (i = 0; i < nSamples - Tu; i ++) {
	   summedCorrelations [i] = DSPCOMPLEX (0, 0);
	   summedSquares [i] = 0;
	   for (j = 0; j < Tg; j ++) {
	      summedCorrelations [i] += correlations [i + j];
	      summedSquares [i] += squares [i + j];
	   }
	}
//
//	OK, the terms are computed, now find the minimum
	t_smp = Tg + averageOffset + Ts / 2;
	for (j = 0; j < (symbols_to_check (theMode) - 1); j++) {
	   float minValue	= 1000000.0;
	   for (i = 0; i < Ts; i++) {
	      gamma [i] = summedCorrelations [(t_smp + i)];
              squareTerm [i] = (float) (0.5 * (EPSILON +
	                                 summedSquares [t_smp + i]));

	      float mmse = squareTerm [i] - 2 * abs (gamma [i]);
	      if (mmse < minValue) {
	         minValue = mmse;
//	         a [j] = mmse;
	         b [j] = i;
	      }
	   }
	   t_smp += Ts;
	}

//	Now least squares to 0...symbols_to_check and b[0] .. */
	float	sumx	= 0.0;
	float	sumy	= 0.0;
	float	sumxx	= 0.0;
	float	sumxy	= 0.0;
	for (i = 0; i < symbols_to_check (theMode) - 1; i++) {
	   sumx += (float) i;
	   sumy += (float) b [i];
	   sumxx += (float) i * (float) i;
	   sumxy += (float) i * (float) b [i];
	}

	float	slope, boffs;
	slope = (float) (((symbols_to_check (theMode) - 1) * sumxy - sumx * sumy) /
	                 ((symbols_to_check (theMode) - 1) * sumxx - sumx * sumx));
	boffs = (float) ((sumy * sumxx - sumx * sumxy) /
	                 ((symbols_to_check (theMode) - 1) * sumxx - sumx * sumx));

//	OK, all set, we are done

	result -> mode_indx = theMode;
	result -> time_offset = fmodf ((Tg + Ts / 2 + averageOffset  + boffs - 1),
	                               (float) Ts);
	result -> sample_rate_offset = slope / ((float) Ts);
	result -> freq_offset_fract = list_epsilon [theMode - Mode_A];
}

