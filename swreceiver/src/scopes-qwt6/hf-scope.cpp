#
/*
 *
 *    Copyright (C)  2014
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
#include	"hf-scope.h"
//
	HFScope::HFScope (QwtPlot	*plotfield,
	                  int16_t	displaySize,
	                  int16_t	rasterSize):Scope (plotfield,
	                                             displaySize, rasterSize) {
int16_t	i;

	this	-> displaySize		= displaySize;
	this	-> rasterSize		= rasterSize;
	this	-> Freq			= 70 * Khz (1);
	this	-> amplification	= 100;
	this	-> displayBuffer	= new double [displaySize];
	this	-> X_axis		= new double [displaySize];
	setXaxis (14000 * Khz (1), 24 * Khz (1), 96000);
}

	HFScope::~HFScope (void) {
	delete[]	this	-> displayBuffer;
	delete[]	this	-> X_axis;
}

void	HFScope::setAmplification (int16_t sliderValue) {
	amplification = (double) sliderValue;
}

void	HFScope::setMarker (int32_t Freq) {
	this -> Freq = Freq;
}

void	HFScope::Display (double *b) {
	   memcpy (displayBuffer, b, displaySize * sizeof (double));
	   Scope::Display (X_axis,
	                   displayBuffer,
	                   amplification,
	                   Freq / Khz (1));
}

void	HFScope::SelectView (int8_t Mode) {
	Scope::SelectView (Mode);
}

void	HFScope::setXaxis (int32_t vfo, int32_t freq, int32_t rate) {
int16_t	i;
int16_t	low	= (vfo - rate / 2) / KHz (1);
int16_t	high	= (vfo + rate / 2) / KHz (1);
double	delta = (high - low) / float (displaySize);

	for (i = 0; i < displaySize; i ++)
	   X_axis [i] = low + i * delta;
	this	-> Freq	= freq;
}

