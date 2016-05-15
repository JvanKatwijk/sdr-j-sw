#
/*
 *    Copyright (C) 2014
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Computing
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
 */

#ifndef	__OFDM_PROCESSOR
#define	__OFDM_PROCESSOR

#include	<QObject>
#include	"basics.h"
#include	"swradio-constants.h"
#include	<stdio.h>
#include	<stdlib.h>
#include	<cstring>
#include	<math.h>
#include	<fftw3.h>
#include	"oscillator.h"

class	Syncer;
class	simpleBuf;
class	drmDecoder;

class	ofdmProcessor:public QObject {
Q_OBJECT
public:
			ofdmProcessor 		(Syncer *,
	                                         uint8_t,
	                                         uint8_t,
	                                         drmDecoder *);
			~ofdmProcessor 		(void);
	void		getWord			(uint8_t,
	                                         uint8_t,
	                                         DSPCOMPLEX *,
	                                         int32_t,
	                                         float);
	void		getWord			(uint8_t,
	                                         uint8_t,
	                                         DSPCOMPLEX *,
	                                         int32_t,
	                                         bool,
	                                         float,
	                                         float,
	                                         float);
	bool		frequencySync 		(float,
	                                         uint8_t *,
	                                         int32_t *);
private:
	int16_t		getWord			(DSPCOMPLEX *,
	                                         int32_t,
	                                         int32_t,
	                                         int16_t,
	                                         float);
	int32_t		get_zeroBin 		(int16_t);
	Syncer		* buffer;
	uint8_t		Mode;
	uint8_t		Spectrum;
	int16_t		k_pilot1;
	int16_t		k_pilot2;
	int16_t		k_pilot3;
	drmDecoder	*master;
	float		theAngle;
	float		sampleclockOffset;
	int16_t		Tu;
	int16_t		Ts;
	int16_t		Tg;
	int16_t		displayCount;
	float		timedelay;
	float		averageTimeDelay;
	int16_t		N_symbols;
	int16_t		bufferIndex;
	DSPCOMPLEX	**symbolBuffer;
	DSPCOMPLEX	*fft_vector;
	fftwf_plan	hetPlan;
	Oscillator	localOscillator;
//
//
	float		kP_freq_controller;
signals:
	void		show_coarseOffset	(float);
	void		show_fineOffset		(float);
	void		show_timeOffset		(float);
	void		show_timeDelay		(float);
	void		show_clockOffset	(float);
	void		show_angle		(float);
};

#endif

