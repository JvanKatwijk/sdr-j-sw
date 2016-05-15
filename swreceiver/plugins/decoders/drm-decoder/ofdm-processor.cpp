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
*************************************************************************
 *
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
#include	<stdio.h>
#include	<stdlib.h>
#include	<math.h>
#include	"ofdm-processor.h"
#include	"syncer.h"
#include	"drm-decoder.h"
#include	"basics.h"
#include	"referenceframe.h"

//	The ofdmprocessor will handle segments of a given size,
//	and do all kinds of frequency correction (timecorrection
//	is done in the syncer
//	
//	The caller just calls upon "getWord" to get a new ofdm word
//
//	Included is a function "frequencySync" that will return
//	a. the integer offset of the freqency,
//	b. an estimate of the spectrum

	ofdmProcessor::ofdmProcessor (Syncer	*b,
	                              uint8_t	Mode,
	                              uint8_t	Spectrum,
	                              drmDecoder *mr):
	                                 localOscillator (1200000) {
int16_t	i;
	this	-> buffer	= b;
	this	-> Mode		= Mode;
	this	-> Spectrum	= Spectrum;
	this	-> master	= mr;
	this	-> theAngle	= 0;
	this	-> Tu		= Tu_of (Mode);
	this	-> Ts		= Ts_of (Mode);
	this	-> Tg		= Tg_of (Mode);
	this	-> displayCount	= 0;
	this	-> timedelay	= 0;
	this	-> N_symbols	= symbolsperFrame (Mode);
//	for detecting pilots:
	int16_t k_pilot [3];
	int16_t cnt	= 0;

	for (i = 0; i < Tu; i ++) {
	   if (isFreqCell (Mode, 0, i)) {
	      k_pilot [cnt ++] = i;
	      if (cnt > 3)
	         fprintf (stderr, "Hellup\n");
	   }
	}
	this	-> k_pilot1 = k_pilot [0] + Tu / 2;
	this	-> k_pilot2 = k_pilot [1] + Tu / 2;
	this	-> k_pilot3 = k_pilot [2] + Tu / 2;

	this	-> bufferIndex	= 0;
	this	-> symbolBuffer	= new DSPCOMPLEX *[N_symbols];
	for (i = 0; i < N_symbols; i ++)
	   symbolBuffer [i] = new DSPCOMPLEX [Tu_of (Mode)];
	fft_vector		= (DSPCOMPLEX *)
	                               fftwf_malloc (Tu_of (Mode) *
	                                            sizeof (fftwf_complex));
	hetPlan			= fftwf_plan_dft_1d (Tu_of (Mode),
	                    reinterpret_cast <fftwf_complex *>(fft_vector),
	                    reinterpret_cast <fftwf_complex *>(fft_vector),
	                    FFTW_FORWARD, FFTW_ESTIMATE);
	connect (this, SIGNAL (show_fineOffset (float)),
	         mr, SLOT (show_fineOffset (float)));
	connect (this, SIGNAL (show_coarseOffset (float)),
	         mr, SLOT (show_coarseOffset (float)));
	connect (this, SIGNAL (show_timeDelay	(float)),
	         mr, SLOT (show_timeDelay (float)));
	connect (this, SIGNAL (show_timeOffset	(float)),
	         mr, SLOT (show_timeOffset (float)));
	connect (this, SIGNAL (show_clockOffset (float)),
	         mr, SLOT (show_clockOffset (float)));
	connect (this, SIGNAL (show_angle (float)),
	         mr, SLOT (show_angle (float)));
	kP_freq_controller		= 0.01;
}

		ofdmProcessor::~ofdmProcessor (void) {
int16_t	i;
	fftwf_free (fft_vector);
	fftwf_destroy_plan (hetPlan);
	for (i = 0; i < N_symbols; i ++)
	   delete[]  symbolBuffer [i];
	delete[] symbolBuffer;
}

//	when starting up, we "borrow" the precomputed frequency offset
//	and start building up the spectrumbuffer.
//	
void	ofdmProcessor::getWord (uint8_t		Mode,	// not needed, really
	                        uint8_t		Spectrum,
	                        DSPCOMPLEX	*out,
	                        int32_t		initialFreq,
	                        float		offsetFractional) {
DSPCOMPLEX	temp [Ts];
DSPCOMPLEX	v    [Tu];
int16_t		i;
DSPCOMPLEX	angle	= DSPCOMPLEX (0, 0);
DSPFLOAT	offset	= 0;
int32_t		bufMask		= buffer -> bufSize - 1;
int16_t	d;
float	timeOffsetFractional;

	buffer		-> needed (2 * Ts);
	timedelay	= offsetFractional;	// computed by getMode

	if (timedelay < 0)
	   timedelay = 0;
	d = floor (timedelay + 0.5);
	timeOffsetFractional	= timedelay - d;
	timedelay		-= d;
//	To take into account the fractional timing difference,
//	keep it simple, just linear interpolation
	int f = (int)(floor (buffer -> currentIndex)) & bufMask;
	if (timeOffsetFractional < 0) {
	   timeOffsetFractional = 1 + timeOffsetFractional;
	   f -= 1;
	}

	for (i = 0; i < Ts; i ++) {
//	just linear interpolation
	   DSPCOMPLEX one = buffer -> data [(f + i) & bufMask];
	   DSPCOMPLEX two = buffer -> data [(f + i + 1) & bufMask];
	   temp [i] = cmul (one, 1 - timeOffsetFractional) +
	                       cmul (two, timeOffsetFractional);
	}

//	And we shift the bufferpointer here
	buffer -> currentIndex = (buffer -> currentIndex + Ts) & bufMask;

//	Now: determine the fine grain offset.
	for (i = 0; i < Tg; i ++)
	   angle += conj (temp [Tu + i]) * temp [i];
//	simple averaging
	theAngle	= 0.9 * theAngle + 0.1 * arg (angle);
//
//	offset in Hz / 100
	offset		= theAngle * 1200000 / (2 * M_PI * Tu);
	if (offset == offset)	// precaution to handle undefines
	   for (i = 0; i < Ts; i ++)
	      temp [i] *= localOscillator. nextValue (100 * initialFreq -offset);

	if (++displayCount > 20) {
	   displayCount = 0;
	   show_coarseOffset	(initialFreq);
	   show_fineOffset	(- offset / 100);
	   show_angle		(arg (angle));
	   show_timeDelay	(offsetFractional);
	}

//	and extract the Tu set of samples for fft processsing
	memcpy (fft_vector, &temp [Tg], Tu * sizeof (DSPCOMPLEX));

	fftwf_execute (hetPlan);
	memcpy (v, &fft_vector [Tu / 2], Tu / 2 * sizeof (DSPCOMPLEX));
	memcpy (&v [Tu / 2], fft_vector, Tu / 2 * sizeof (DSPCOMPLEX));
//
//	The output here is the set of "useful" carriers,
//	i.e. the ones [Kmin .. Kmax]
	memcpy (out, &v [Tu_of (Mode) / 2 + Kmin (Mode, Spectrum)],
	               (Kmax (Mode, Spectrum) - Kmin (Mode, Spectrum) + 1) *
	               sizeof (DSPCOMPLEX));
}

//
//	The getWord as below is used in the main loop, to obtain
//	a next ofdm word
//	With this "getWord" we  also correct the frequency and
//	timing offsets based on tracking values, computed
//	elsewhere.
void	ofdmProcessor::getWord (uint8_t		Mode,	// not needed, really
	                        uint8_t		Spectrum,
	                        DSPCOMPLEX	*out,
	                        int32_t		initialFreq,
	                        bool		firstTime,
	                        float		offsetFractional,
	                        float		angle,
	                        float		clockOffset) {
DSPCOMPLEX	temp [Ts];
DSPCOMPLEX	v    [Tu];
int16_t		i;
DSPFLOAT	offset	= 0;
int32_t		bufMask		= buffer -> bufSize - 1;

int16_t	timeOffsetInteger;
float	timeOffsetFractional;
//	If we start in the main cycle, we "compute" the
//	time offset, once in the main cycle, we "track"
//	based on the measured difference
	buffer		-> needed (2 * Ts);

	if (firstTime) {		// set the values
	   theAngle		= 0;
	   sampleclockOffset	= 0;
	}
	else { 			// apparently tracking mode
//	Note that the constants are chosen pretty arbitrarily
//	as long as the tracking values are not too large
	   sampleclockOffset	= 0.9 * sampleclockOffset + 0.1 * clockOffset;
	   timedelay		-= 0.5 * offsetFractional;
	}
	
	timeOffsetInteger	= floor (timedelay + 0.5);
	timeOffsetFractional	= timedelay - timeOffsetInteger;
	timedelay		-= timeOffsetInteger;
//	To take into account the fractional timing difference,
//	we do some interpolation. We assume that -0.5 <= c && c <= 0.5
	int f	= (int)(floor (buffer -> currentIndex + 
	                              timeOffsetInteger)) & bufMask;
//	just linear interpolation
	for (i = 0; i < Ts; i ++) {
	   if (timeOffsetFractional < 0) {
	      timeOffsetFractional = 1 + timeOffsetFractional;
	      f -= 1;
	   }
	   else 
	   if (timeOffsetFractional > 1) {
	      timeOffsetFractional -= 1;
	      f += 1;
	   }
	   DSPCOMPLEX one = buffer -> data [(f + i) & bufMask];
	   DSPCOMPLEX two = buffer -> data [(f + i + 1) & bufMask];
	   temp [i] = cmul (one, 1 - timeOffsetFractional) +
	              cmul (two, timeOffsetFractional);
//	   timeOffsetFractional -= sampleclockOffset / 2;
	}

//	And we adjust the bufferpointer here
	buffer -> currentIndex = (buffer -> currentIndex + 
	                                timeOffsetInteger + Ts) & bufMask;
//	correct the phase
//
//	If we are here for the first time, we compute an initial offset.
	if (firstTime) {	// compute a vfo offset
	   DSPCOMPLEX c = DSPCOMPLEX (0, 0);
	   for (i = 0; i < Tg; i ++)
	      c += conj (temp [Tu + i]) * temp [i];
	   theAngle = arg (c);
	}
	else
	   theAngle	= theAngle - 0.2 * angle;
//	offset in 0.01 * Hz
	offset		= theAngle * 1200000 / (2 * M_PI * Tu);
	if (offset == offset)	// precaution to handle undefines
	   for (i = 0; i < Ts; i ++) 
	      temp [i] *= localOscillator. nextValue (100 * initialFreq
	                                                        -offset);
	
	if (++displayCount > 20) {
	   displayCount = 0;
	   show_coarseOffset	(initialFreq);
	   show_fineOffset	(- offset / 100);
	   show_angle		(angle);
	   show_timeOffset	(offsetFractional);
	   show_timeDelay	(timedelay);
	   show_clockOffset	(Ts * clockOffset);
	}

//	and extract the Tu set of samples for fft processsing
	memcpy (fft_vector, &temp [Tg], Tu * sizeof (DSPCOMPLEX));

	fftwf_execute (hetPlan);
	memcpy (v, &fft_vector [Tu / 2], Tu / 2 * sizeof (DSPCOMPLEX));
	memcpy (&v [Tu / 2], fft_vector, Tu / 2 * sizeof (DSPCOMPLEX));
//
//	The output here is the set of "useful" carriers,
//	i.e. the ones [Kmin .. Kmax] that we extract from the vector v
	memcpy (out, &v [Tu_of (Mode) / 2 + Kmin (Mode, Spectrum)],
	               (Kmax (Mode, Spectrum) - Kmin (Mode, Spectrum) + 1) *
	               sizeof (DSPCOMPLEX));
}
//
//	get_zeroBin cooperates with getWord. It uses the
//	two dimensional array "symbolBuffer" with in the rows the
//	precomputed spectra of the last N_symbols  ofdm words,
//	starting at start (i.e. circular)
//
//	Note that the symbolbuffer, as well as the N_symbols
//	belong to the class
int32_t	ofdmProcessor::get_zeroBin (int16_t start) {
int16_t i, j;
DSPCOMPLEX correlationSum [Tu];
DSPCOMPLEX tmp1;
float abs_sum [Tu];
float	highest = -1.0E20;
int dcOffset;

//	accumulate phase diffs of all carriers 
//	we know that the correlation for the (frequency) pilot values
//	in subsequent symbols should be high, while
//	random correlations have low correlation.
//	Alternatively, we could add up the  abs vales of the "bin"s
//	and look for the highest. Still not sure what is the best
	memset (correlationSum, 0, Tu * sizeof (DSPCOMPLEX));
	memset (abs_sum, 0, Tu * sizeof (float));

//	accumulate phase diffs of all carriers in subsequent symbols
	for (j = start + 1; j < start + N_symbols; j++) {
	   int16_t jmin1 	= (j - 1) % N_symbols;
	   int16_t jj		= j % N_symbols;
	   for (i = 0; i < Tu; i++) {
	      tmp1 = symbolBuffer [jmin1][i] * conj (symbolBuffer [jj][i]);
	      correlationSum [i] += tmp1;
	   }
	}
//
//	switch the fft output and compute the abs values
	for (i = 0; i < Tu / 2; i++) { 
	   abs_sum [i] = abs (correlationSum [Tu / 2 + i]);
	   abs_sum [Tu / 2 + i] = abs (correlationSum [i]);
	}

	highest		= 0;
	dcOffset	= 0;
	for (i = Tu / 2 - Tu / 10; i < Tu / 2 + Tu / 10; i ++) {
	   float sum = abs_sum [i + k_pilot1 - Tu / 2] +
	               abs_sum [i + k_pilot2 - Tu / 2] +
	               abs_sum [i + k_pilot3 - Tu / 2];
	   if (sum > highest) {
	      dcOffset	= i - Tu / 2;
	      highest	= sum;
	   }
	}

	return dcOffset;
}
//
//
bool ofdmProcessor::frequencySync (float	offset_fractional,
	                           uint8_t	*theSpectrum,
	                           int32_t	*freq_offset_integer) {
int16_t	i;
int32_t	localIndex	= 0;
float	occupancyIndicator [6];
uint8_t	spectrum;

	buffer	-> needed (N_symbols * Ts + Ts);
//
//	first, load spectra for the first N_symbol symbols
//	into the (circular) buffer "symbolBuffer",
//	However: do not move the "currentIndex" in the simplebuf,
//	i.e. do not consume the words from the input
	for (i = 0; i < N_symbols; i ++) {
	 int16_t  time_offset_integer =
	          getWord (buffer -> data,
	                   buffer -> bufSize,
	                   buffer -> currentIndex + localIndex,
	                   i,
	                   offset_fractional);
	   localIndex += Ts + time_offset_integer;
	}
//
//	our version of get_zeroBin returns the "bin" number
//	of the strongest freqency
	int32_t	binNumber;
	binNumber = get_zeroBin (0);
	*freq_offset_integer	= binNumber * 12000 / Tu;

	binNumber = binNumber < 0 ? binNumber + Tu : binNumber;
	for (i = 0; i < 4; i ++)
	   occupancyIndicator [i] = 0;

// 4,5 not supported yet -> if so, we have to use doubled sampling rate
	for (spectrum = 0; spectrum <= 3; spectrum ++) {
	   int16_t K_min_ = Kmin (Mode, spectrum);
	   int16_t K_max_ = Kmax (Mode, spectrum);

	   if (K_min_ == K_max_) {	// should not happen
	      occupancyIndicator [spectrum] = 0;
	      continue;
	   }
//	now we go

	   int K_dc_indx	= binNumber;
	   int K_dc_plus2_indx	= (K_dc_indx + 2 + Tu) % Tu;
	   int K_min_indx	= (K_dc_indx + K_min_ + Tu) % Tu;
//	for spectr.-occu. 0/1 K_min_minus1 equals the dc-carrier
	   int K_min_minus4_indx = (K_min_indx - 4 + Tu) % Tu;
	   int K_max_indx	= (K_dc_indx + K_max_ + Tu) % Tu;
	   int K_max_plus1_indx = (K_max_indx + 1 + Tu) % Tu;

	   float tmp1	= 0;
	   float tmp2	= 0;
	   float tmp3	= 0;
	   float tmp4	= 0;
	   float tmp5	= 0;
	   float tmp6	= 0;

	   for (i = 0; i < N_symbols; i ++) {
	      tmp1 += real (symbolBuffer [i][K_dc_plus2_indx] *
	                    conj (symbolBuffer [i][K_dc_plus2_indx]));
	      tmp2 += real (symbolBuffer [i][K_dc_indx] *
	                    conj (symbolBuffer [i][K_dc_indx]));
	      tmp3 += real (symbolBuffer [i][K_max_indx] *
	                    conj (symbolBuffer [i][K_max_indx]));
	      tmp4 += real (symbolBuffer [i][K_max_plus1_indx] *
	                    conj (symbolBuffer [i][K_max_plus1_indx]));
	      tmp5 += real (symbolBuffer [i][K_min_indx] *
	                    conj (symbolBuffer [i][K_min_indx]));
	      tmp6 += real (symbolBuffer [i][K_min_minus4_indx] *
	                    conj (symbolBuffer [i][K_min_minus4_indx]));
	   }

	   float energy_ratio_K_max_to_K_max_p1 = tmp3 / tmp4;
	   float energy_ratio_K_min_to_K_min_m4 = tmp5 / tmp6;
	   occupancyIndicator [spectrum] =
	         energy_ratio_K_min_to_K_min_m4 +
	         energy_ratio_K_max_to_K_max_p1;
	}
	        
	float tmp1	= 0.0;
	for (spectrum = 0; spectrum < 4; spectrum ++) {	
	   if (occupancyIndicator [spectrum] >= tmp1) {
	      tmp1 = occupancyIndicator [spectrum];
	      *theSpectrum = spectrum;
	   }
	}
	return true;
}

//
//	In this version of getWord, the input buffer is only looked
//	at, not read!! It is called by the frequency synchronizer
//	No reduction of the output to the Kmin .. Kmax useful
//	carriers is made and the order of the low-high freqencies
//	is not changed
//	It is a private version, for use with the frequencySync function
//	only
int16_t	ofdmProcessor::getWord (DSPCOMPLEX	*buffer,
	                        int32_t		bufSize,
	                        int32_t		theIndex,
	                        int16_t		wordNumber,
	                        float		offsetFractional) {
DSPCOMPLEX	temp [Ts];
int16_t		i;
int16_t		bufMask	= bufSize - 1;
DSPCOMPLEX	angle	= DSPCOMPLEX (0, 0);
DSPFLOAT	offset	= 0;
//

//	To take into account the fractional timing difference,
//	we do some interpolation between samples in the time domain
	int f	= (int)(floor (theIndex)) & bufMask;
	if (offsetFractional < 0) {
	   offsetFractional = 1 + offsetFractional;
	   f -= 1;
	}
	for (i = 0; i < Ts; i ++) {
	   DSPCOMPLEX een = buffer [(f + i) & bufMask];
	   DSPCOMPLEX twee = buffer [(f + i + 1) & bufMask];
	   temp [i] = cmul (een, 1 - offsetFractional) +
	              cmul (twee, offsetFractional);
	}

//	Now: estimate the fine grain offset.
	for (i = 0; i < Tg; i ++)
	   angle += conj (temp [Tu + i]) * temp [i];
//	simple averaging:
	theAngle	= 0.9 * theAngle + 0.1 * arg (angle);
//
//	offset in Hz / 100
	offset		= theAngle * 1200000 / (2 * M_PI * Tu);
	if (++displayCount > 20) {
	   displayCount = 0;
	   show_coarseOffset	(0);
	   show_fineOffset	(- offset / 100);
	   show_angle		(arg (angle));
	   show_timeDelay	(offsetFractional);
	}

	if (offset == offset)	// precaution to handle undefines
	   for (i = 0; i < Ts; i ++)
	      temp [i] *= localOscillator. nextValue (-offset);

//	and extract the Tu set of samples for fft processsing
	memcpy (fft_vector, &temp [Tg], Tu * sizeof (DSPCOMPLEX));

	fftwf_execute (hetPlan);
	memcpy (symbolBuffer [wordNumber],
	        fft_vector , Tu * sizeof (DSPCOMPLEX));

	return 0;
}

