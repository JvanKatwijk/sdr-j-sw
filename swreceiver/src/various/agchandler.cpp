#
/*
 *    Copyright (C) 2014
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
#include	"agchandler.h"
#include	"utilities.h"

#define	AGCSLOW		(cardrate / 3)
#define	AGCFAST		(cardrate / 8);

#define	RISE_ATTACKWEIGHT	(cardrate / 500)
#define	FALL_ATTACKWEIGHT	(cardrate / 100)
#define	RISE_DECAYWEIGHT	(cardrate / 200)
#define	FALL_DECAYWEIGHT	(cardrate / 100)
/*
 *	Handling agc, still rudimentary.
 *	The approach taken is derived from cuteSDR
 */
	agcHandler::agcHandler	(int32_t cardrate) {
	this	-> cardrate	= cardrate;
	agcMode			= AGC_OFF;
	currentPeak		= 0;
	attackAverage		= 1;
	decayAverage		= 1;
	bufferSize		= cardrate / 50;	// 20 msec buffer
	buffer			= new DSPFLOAT [bufferSize];
	bufferP			= 0;
	ThresholdValue		= -115 / (DSPFLOAT)16;		// default
}

	agcHandler::~agcHandler	() {
	delete	buffer;
}

void	agcHandler::setMode	(uint8_t M) {
	agcMode		= M;
	agcHangtime	= 0;
	agcHangInterval	= AGC_SLOW ? AGCSLOW : AGCFAST;
}

void	agcHandler::setLevel	(int16_t L) {
	(void)L;
}

void	agcHandler::setSamplerate (int32_t L) {
	(void)L;
}
//
//	Note: in this revised version, the handler
//	returns the amplification factor rather than the amplified value
DSPFLOAT	agcHandler::doAgc	(DSPCOMPLEX z) {
DSPFLOAT	currentMag;
DSPFLOAT	oldest_Mag;
DSPFLOAT	gain;
int16_t	i;

	oldest_Mag		= buffer [bufferP];
	currentMag		= abs (z);
	buffer [bufferP]	= currentMag;
	bufferP			= (bufferP + 1) % bufferSize;

	if (agcMode == AGC_OFF) 
	   return 1.0;

	if (currentMag >= currentPeak)
	   currentPeak = currentMag;
	else
	if (oldest_Mag >= currentPeak) { // largest one shifted out
	   currentPeak = currentMag;
	   for (i = 0; i < bufferSize; i ++) 
	      if (buffer [i] >= currentPeak)
	         currentPeak = buffer [i];
	}
//
//	now we have a valid currentPeak
//
	if (currentPeak > attackAverage)
	   attackAverage = decayingAverage (attackAverage,
	                                    currentPeak, RISE_ATTACKWEIGHT);
	else
	   attackAverage = decayingAverage (attackAverage,
	                                    currentPeak, FALL_ATTACKWEIGHT);

	if (currentPeak > decayAverage) {
	   decayAverage = decayingAverage (decayAverage,
	                                   currentPeak, RISE_DECAYWEIGHT);
	   agcHangtime = 0;
	}
	else {
	   if (agcHangtime < agcHangInterval)
	      agcHangtime ++;	// keep current decayAverage
	   else
	      decayAverage = decayingAverage (decayAverage,
	                                      currentPeak, FALL_DECAYWEIGHT);
	}

	if (attackAverage > decayAverage)
	   gain = gain_for (attackAverage);
	else
	   gain = gain_for (decayAverage);

	return gain;
}
//
//	set the threshold value for the agc in db
void	agcHandler::setThreshold (int16_t s) {
	if (s < get_db (0))
	   s = get_db (0);
	if (s > 0)
	   s = 0;

	ThresholdValue	= (DSPFLOAT)s / 16;
}

#define	MAX_AMPLITUDE	(256 * 32767)

DSPFLOAT	agcHandler::gain_for (DSPFLOAT v) {
DSPFLOAT mag	= log10 (v / MAX_AMPLITUDE + 1e-8);

	if (mag < ThresholdValue)	// knee value
	   return 1.0;
	
	return  pow (10.0, - (mag - ThresholdValue));
}

