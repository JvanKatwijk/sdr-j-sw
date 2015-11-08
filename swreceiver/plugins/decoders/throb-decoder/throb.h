#
/*
 *
 *    Copyright (C) 2011, 2012, 2013
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Programming
 *
 *    This file is part of the SDR-J
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
#ifndef	__THROB_XXX
#define	__THROB_XXX

#include	<QObject>
#include	"swradio-constants.h"
#include	"fir-filters.h"
#include	"fft-filters.h"
#include	"scope.h"
#include	"fft-scope.h"
#include	"utilities.h"
#include	"oscillator.h"
/*
 *	defines, local for the throbDetector stuff
 */
#define	MODE_THROB1	0100
#define	MODE_THROB2	0101
#define	MODE_THROB4	0102
#define	MODE_THROBX1	0103
#define	MODE_THROBX2	0104
#define	MODE_THROBX4	0105

#define MAX_TONES	15

#define	THROB_IF	800
//
//	for a rate of 8000, the lengths are
#define	SYMLEN_1	8192
#define	SYMLEN_2	4096
#define	SYMLEN_4	2048

#define	THROB_IF	800
#define	FilterFFTLen	8192

class	throb: public QObject {
Q_OBJECT
public:
			throb		(int32_t, uint8_t, QwtPlot *);
			~throb		(void);
	DSPCOMPLEX	doDecode	(DSPCOMPLEX);
	void		setReverse	(bool);
	void		setAfc		(bool);
	void		setThreshold	(int16_t);
private:
	void		decodechar	(int16_t, int16_t);
	void		decodeThrob	(int16_t, int16_t);
	void		decodeThrobx	(int16_t, int16_t);
	int16_t		findtones	(DSPCOMPLEX *word, int16_t *, int16_t *);
	void		throb_sync	(DSPCOMPLEX);
	void		throb_rx	(DSPCOMPLEX);
	DSPFLOAT	signalValuefortone (uint8_t, int16_t);
	bool		afcOn;
	int16_t		symlen_1;
	int16_t		symlen_2;
	int16_t		symlen_4;
	uint8_t		throbMode;
	int16_t		sampleRate;
	int16_t		downRate;
	float		throbIF;
	Oscillator	*localOscillator;
	DSPCOMPLEX	*mk_rxtone	(DSPFLOAT, int16_t, int16_t);
	void		flip_syms	(void);
	void		mk_pulse	(DSPFLOAT *, int16_t len, int16_t);

	int16_t		thresHold;
	bool		throbReverse;
	int16_t		idlesym;
	int16_t		spacesym;
	uint8_t		lastchar;
	DSPFLOAT	metrics;

	fftFilter	*fft_filter;
	Basic_FIR	*syncFilter;

	fft_scope	*ourScope;
	QwtPlot		*throbscope;
	int16_t		symlen;
	DSPFLOAT	*freqs;
//	DSPFLOAT	freqs [55];
	DSPCOMPLEX	*rxtone [55];
	DSPCOMPLEX	symbol [1000];
	DSPFLOAT	syncbuf [1000];

	DSPFLOAT	rxcntr;
	DSPFLOAT	s2n;

	int16_t		rxsymlen;
	int16_t		symptr;
	int16_t		deccntr;
	bool		shift;
	bool		waitsync;
signals:
	void		showChar	(int16_t);
	void		clrText		(void);
	void		setMarker	(float);
	void		showMetrics	(double);
	void		showS2N		(double);
};

#endif

