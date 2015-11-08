#
/*
 *    Copyright (C) 2010, 2011, 2012
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
#ifndef	__HELL_DECODER
#define	__HELL_DECODER

#include	<QFrame>
#include	"ui_widget.h"
#include	<QImage>
#include	<QLabel>
#include	"swradio-constants.h"
#include	"decoder-interface.h"
#include	"fft-filters.h"
#include	"utilities.h"

class	Oscillator;
class	QSettings;

#define	HELL_BW0	245.0		// 
#define	HELL_BW_FH	245.0		// FH width
#define	HELL_BW_SH	30.0		// Slow Hell bandwidth
#define	HELL_BW_X5	1225.0		// X5 Hell bandwidth
#define	HELL_BW_X9	2205.0		// X9 Hell bandwidth
#define	HELL_BW_FSK	180.0		// FSK Hell bandwidth
#define	HELL_BW_FSK105	100.0		// FSK105 width
#define	HELL_BW_HELL80	450.0		// HELL80 width
#define	HELL_BW_FSK245	450.0

#define	MODE_FELDHELL	0100
#define	MODE_SLOWHELL	0101
#define	MODE_HELLX5	0102
#define	MODE_HELLX9	0103

#define	MODE_FSKHELL	0110
#define	MODE_FSKH105	0111
#define	MODE_HELL80	0112
#define	MODE_FSKH245	0113
#define	RxColumnLen	30

class hellDecoder: public decoderInterface, public Ui_Form {
Q_OBJECT
#if QT_VERSION >= 0x050000
Q_PLUGIN_METADATA (IID "hellDecoder")
#endif
Q_INTERFACES (decoderInterface)

public:
	QWidget		*createPluginWindow	(int32_t, QSettings *);
			~hellDecoder		(void);
	void		doDecode		(DSPCOMPLEX);
	bool		initforRate		(int32_t);
	int32_t		rateOut			(void);
	int16_t		detectorOffset		(void);
private:
	int32_t		theRate;
	QSettings	*hellSettings;
	void		decodeAM	(DSPCOMPLEX);
	void		decodeFSK	(DSPCOMPLEX);
	void		selectMode	(int16_t);
	void		addColumn	(int16_t *);
	void		storeDataPoint	(int16_t);
	void		setup_hellDecoder	(int32_t);
	void		displayMetrics	(double);

	QFrame		*myFrame;
	QLabel		*imageLabel;
	QImage		image;
	int16_t		columnBuffer	[2 * RxColumnLen];
	int16_t		columnBufferp;
	int16_t		displayColumn;
	uint8_t		hellMode;
	int16_t		hellIF;
	DSPCOMPLEX	prevSample;
	double		sampleCounter;
	double		agc;
	double 		currentMax;
	double 		maxValue;
	bool		reverse;
	bool		blackBackground;
	bool		fullWidth;

	double		filter_bandwidth;
	double		hell_bandwidth;
	double 		PixelsperSample;
	int16_t		rateCorrector;
	double		metrics;

	Oscillator	*localOscillator;

	fftFilter	*bandpass;
	average		*smoothener;
	average		*averager;
private slots:
	void		selectMode	(QString);
//	void		switchReverse	(void);
	void		switchBlackback	(void);
	void		switchWidth	(void);
	void		set_rateCorrector	(int);
signals:
};

#endif

