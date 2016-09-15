#
/*
 *
 *    Copyright (C)  2010, 2011, 2012
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
#include	"fft-scope.h"
#include	"utilities.h"
//
//	In this version of the scope, we introduce two additional parameters
//	the spectrumSize and the repetition frequency.
//	If spectrumSize == -1, we create a scope
//	where all samples are made part of the spectrum: pretty heavy
//	otherwise, we take freq times per second spectrumSize / 2 samples
	fft_scope::fft_scope (QwtPlot	*plotfield,
	                      int16_t	displaySize,
	                      int32_t	scale,
	                      int16_t	rasterSize,
	                      int32_t	spectrumSize,
	                      int32_t	SampleRate,
	                      int16_t	freq):Scope (plotfield,
	                                             displaySize, rasterSize) {
int16_t	i;
double	temp;

	if ((displaySize & (displaySize - 1 )) != 0)
	   displaySize = 512;
	this	-> displaySize		= displaySize;
	this	-> segmentSize		= SampleRate / freq;
	this	-> scale		= scale;
	this	-> rasterSize		= rasterSize;
	this	-> averageCount		= 0;

	if ((spectrumSize & (spectrumSize - 1)) != 0)
	   spectrumSize = 1024;
	if (displaySize > spectrumSize)
	   spectrumSize = displaySize;

	this	-> spectrumSize		= spectrumSize;
	this	-> sampleRate		= SampleRate;
	this	-> MaxFrequency		= SampleRate / 2;
	this	-> freq			= freq;

	this	-> fillPointer		= 0;
	this	-> vfo			= 0;
	this	-> amplification	= 100;
	this	-> left_needle		= 0;
	this	-> right_needle		= 0;
	this	-> displayBuffer	= new double [displaySize];
	this	-> averageBuffer	= new double [displaySize];
	for (i = 0; i < displaySize; i ++)
	   averageBuffer [i] = 0;
	this	-> X_axis		= new double [displaySize];

	this	-> Window		= new DSPFLOAT [spectrumSize];
	this	-> inputBuffer		= new DSPCOMPLEX [spectrumSize];
	this	-> sampleCounter	= 0;
	this	-> spectrum_fft		= new common_fft (spectrumSize);
	this	-> spectrumBuffer	= spectrum_fft	-> getVector ();
	this	-> binWidth		= sampleRate / spectrumSize;

	for (i = 0; i < spectrumSize; i ++) 
	   Window [i] = 0.42 - 0.5 * cos ((2.0 * M_PI * i) / (spectrumSize - 1)) +
	                      0.08 * cos ((4.0 * M_PI * i) / (spectrumSize - 1));

	temp	= (double)MaxFrequency / displaySize;
	for (i = 0; i < displaySize; i ++) {
	   X_axis [i] =
	      ((double)vfo - MaxFrequency
	         + (double)((i * 2.0 * MaxFrequency)) / displaySize) / ((double)scale);
	}
}

	fft_scope::~fft_scope (void) {
	delete[]	this	-> displayBuffer;
	delete[]	this	-> averageBuffer;
	delete[]	this	-> X_axis;
	delete[]	this	-> Window;
	delete		this	-> spectrum_fft;
	delete[]	this	-> inputBuffer;
}

void	fft_scope::setAmplification (int16_t sliderValue) {
	amplification = (double) sliderValue;
}

void	fft_scope::setZero (int64_t vfo) {
int32_t	i;
float	temp	= (double)MaxFrequency / displaySize;
	this	-> vfo = vfo;
	temp	= (double)MaxFrequency / displaySize;
	for (i = 0; i < displaySize; i ++) {
	   X_axis [i] =
	      ((double)vfo - MaxFrequency
	         + (double)((i * 2.0 * MaxFrequency)) / displaySize) / ((double)scale);
	}
}

void	fft_scope::setNeedles (int32_t left_needle,
	                       int32_t right_needle) {
	this	-> left_needle = left_needle;
	this	-> right_needle	= right_needle;
}

void	fft_scope::setNeedles (int32_t left_needle) {
	this	-> left_needle = left_needle;
	this	-> right_needle	= left_needle;
}
//
void	fft_scope::addElements (DSPCOMPLEX *v, int16_t n) {
int16_t	i;

	for (i = 0; i < n; i ++)
	   addElement (v [i]);
}

void	fft_scope::addElement (DSPCOMPLEX x) {
int32_t	i, j;
//DSPFLOAT	multiplier	= (DSPFLOAT)segmentSize / displaySize;
DSPFLOAT	temp [spectrumSize];

	inputBuffer [fillPointer] = x;
	fillPointer	= (fillPointer + 1) % spectrumSize;

	if (++ sampleCounter < segmentSize)
	   return;

	sampleCounter	= 0;
	for (i = 0; i < spectrumSize; i ++) {
	   DSPCOMPLEX tmp = inputBuffer [(fillPointer + i) % spectrumSize];
	   spectrumBuffer [i] = cmul (tmp, Window [i]); 
	}

	spectrum_fft	-> do_FFT ();
	for (i = 0; i < spectrumSize / 2; i ++) {
	   temp [i] = abs (spectrumBuffer [spectrumSize / 2 + i]);
	   temp [spectrumSize / 2 + i] = abs (spectrumBuffer [i]);
	}

	for (i = 0; i < displaySize; i ++) {
	   displayBuffer [i] = 0;
	   for (j = 0; j < spectrumSize / displaySize; j ++)
	      displayBuffer [i] += temp [i * spectrumSize / displaySize + j];
	   displayBuffer [i] /= spectrumSize / displaySize;
	}
	doAverage ();

	showSpectrum ();
}
//
void	fft_scope::showSpectrum (void) {
	Scope::Display (X_axis,
	                displayBuffer,
	                amplification,
	                ((int32_t)vfo + right_needle + 0.5 * scale) / (int32_t)scale);
}

void	fft_scope::SelectView (int8_t Mode) {
	Scope::SelectView (Mode);
}

void	fft_scope::setAverager (bool b) {
	averageCount = b ? 1 : 0;
}
	
void	fft_scope::clearAverage (void) {
int16_t	i;

	if (averageCount > 0) {
	   averageCount = 1;
	   for (i = 0; i < displaySize; i ++)
	      averageBuffer [i] = 0;
	}
}
//
void	fft_scope::doAverage (void) {
int32_t	i;

	if (averageCount > 0) {
	   for (i = 0; i < displaySize; i ++) {
	      if ((displayBuffer [i] != displayBuffer [i]) ||
	           isInfinite (displayBuffer [i]))
	         displayBuffer [i] = 0;
	      averageBuffer [i] =
	                   ((double)(averageCount - 1))/averageCount *
	                                                    averageBuffer [i] +
	                   1.0f / averageCount * displayBuffer [i];
	      displayBuffer [i] = averageBuffer [i];
	   }

	   averageCount ++;
	}
	else {
	   for (i = 0; i < displaySize; i ++) {
	      if ((displayBuffer [i] != displayBuffer [i]) ||
	           isInfinite (displayBuffer [i]))
	         displayBuffer [i] = 0;
	      averageBuffer [i] = 
	               ((double)(freq / 2 - 1)) / (freq / 2) * averageBuffer [i] +
	                1.0f / (freq / 2) * displayBuffer [i];
	      displayBuffer [i] = averageBuffer [i];
	   }
	}
}

