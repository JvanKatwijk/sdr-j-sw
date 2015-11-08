#
/*
 *    Copyright (C) 2013
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
 *	Channel estimator according to 
 *	" A comparison of Pilot-aided Channel Estimation Methods
 *	  for OFDM systems"
 *	Michele Morelli and Umberto Mengali
 *	IEEE Transactions on Signal Processing Vol 49 No 12 December 2000
 */
#include	<QDebug>
#include	"estimate-1.h"
#include	"basics.h"
#include	"referenceframe.h"

static inline
complex<float> createExp (float s) {
	return complex<float> (cos (s), - sin (s));
}

//
//	We create a single "estimator" for each of the symbols
//	of a frame. We do, however, share he knowledge gained
//	from one symbol to the next one by passing on the channel
//	obtained in the time domain
//
	estimate_1::estimate_1 (DSPCOMPLEX 	**refFrame,
	                        uint8_t	Mode,
	                        uint8_t	Spectrum,
	                        int16_t	refSymbol) {
int16_t	carrier, tap, pilot;
int16_t	next	= 0;

	this ->	refFrame	= refFrame;
	this	-> Mode		= Mode;
	this	-> Spectrum	= Spectrum;
	this	-> refSymbol	= refSymbol;
	numberofCarriers	= Kmax (Mode, Spectrum) -
	                          Kmin (Mode, Spectrum) + 1;
	numberofPilots		= getnrPilots (refSymbol);
	numberofTaps		= Tg_of (Mode) - 1;

	D			= zeros<cx_fmat> (numberofTaps, numberofTaps);
	B			= cx_fmat (numberofPilots, numberofTaps);
	B_H			= cx_fmat (numberofTaps, numberofPilots);
	S_p			= zeros<cx_fmat>(numberofPilots,
	                                                 numberofPilots);
//
	fftSize			= Tu_of (Mode);
	pilotTable		= new int16_t [numberofPilots];

//	we compute the indices of the pilots and at the same time
//	we store the pilot values as the diagonal in the S_p
	next	= 0;
	for (carrier = Kmin (Mode, Spectrum);
	     carrier <= Kmax (Mode, Spectrum); carrier ++)
	   if (isPilotCell (Mode, refSymbol, carrier)) {
	      pilotTable [next] = carrier;
	      S_p (next, next) = getPilotValue (Mode, Spectrum,
	                                          refSymbol, carrier);
	      next ++;
	   }

//	B is computed according to formula 6
	for (pilot = 0; pilot < numberofPilots; pilot ++)
	   for (tap = 0; tap < numberofTaps; tap ++)
	      B (pilot, tap) =
	         cdiv (createExp (2 * M_PI * tap *
	                     (fftSize / 2 + pilotTable [pilot]) / fftSize),
	                       sqrt (fftSize));
//
	B_H	= trans (B);
	D	= B_H * B;	// could have been computed acc to form 15
}

	estimate_1::~estimate_1 (void) {
	delete[]	pilotTable;
}
//
void	estimate_1::estimate (DSPCOMPLEX *testRow, DSPCOMPLEX *resultRow) {
int16_t	pilot;
//	the channels of the pilots in time resp. frequency domain
cx_fvec H_fd (numberofPilots);
cx_fvec h_td (numberofTaps);
//	the input is stored:
cx_fvec Z (numberofPilots);
//
//	We use here pinv (S_p) rather than the trans (S_p) as mentioned
//	in the above paper, since the abs of the pilot values is
//	larger than 1, and the abs values of boost cells differ
//	from that of the other pilots

	for (pilot = 0; pilot < numberofPilots; pilot ++) 
	   Z (pilot) = testRow [indexFor (pilotTable [pilot])];
	Z	= pinv (S_p) * Z;
//
	h_td	= pinv (D) * B_H * Z;	// formula 13
//
//	so, h_td now contains the channel for the pilots in the time domain
//	copy them out, maybe the next symbol may make use of it

//	H_fd are the channels for the pilots in the frequency domain
	H_fd		= B * h_td;
//	The estimated channel values in the freq domain are now
//	in the vector H_fd
	for (pilot = 0; pilot < numberofPilots; pilot ++) 
	   resultRow [indexFor (pilotTable [pilot])] = H_fd (pilot);
}

int16_t	estimate_1::getnrPilots (int16_t symbolno) {
int16_t		carrier;
int16_t		amount = 0;

	for (carrier = Kmin (Mode, Spectrum);
	     carrier <= Kmax (Mode, Spectrum); carrier ++) {
	   if (carrier == 0)
	      continue;
	   if ((Mode == 1) && (carrier == -1 || carrier == 1))
	      continue;
	   if (isPilotCell (Mode, symbolno, carrier)) amount ++;
	}
	return amount;
}

int16_t	estimate_1::indexFor (int16_t carrier) {
	if (carrier < Kmin (Mode, Spectrum) ||
	    carrier > Kmax (Mode, Spectrum))
	   fprintf (stderr, "DDDDDDDDDDDDDDDD %d\n", carrier);
	return carrier - Kmin (Mode, Spectrum);
}
