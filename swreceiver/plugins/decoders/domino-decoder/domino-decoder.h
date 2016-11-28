#
/*
 *    Copyright (C)  2011, 2012, 2013
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Programming
 *
 *    This file is part of the SDR-J (JSDR).
 *    Many of the ideas as implemented in SDR-J are derived from
 *    other work, made available through the GNU general Public License. 
 *    All copyrights of the original authors are recognized.
 *
 *    The domino software is partly derived from sources in the domino decoder
 *    in fldigi.
 *
 *    Copyright (C) 2001, 2002, 2003
 *    Tomi Manninen (oh2bns@sral.fi)
 *    Copyright (C) 2006
 *    Hamish Moffatt (hamish@debian.org)
 *    Copyright (C) 2006
 *    David Freese (w1hkj@w1hkj.com)
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

#ifndef __DOMINOEX_H
#define __DOMINOEX_H

#include	"swradio-constants.h"
#include	"decoder-interface.h"
#include	"slidingfft.h"
#include	"interleave.h"
#include	"viterbi.h"
#include	"fft.h"
#include	"fft-filters.h"
#include	"utilities.h"
#include	"ui_widget.h"
#include	<QObject>
#include	<QFrame>
#include	<QString>

class		QSettings;

#define NUMTONES 18
#define MAXFFTS  16
#define	MAX_VARICODE_LEN	3

struct CacheLine {
	DSPCOMPLEX vector [MAXFFTS * NUMTONES * 6];
};

class dominoDecoder : public decoderInterface, public Ui_Form {
Q_OBJECT
Q_PLUGIN_METADATA (IID "cwDecoder")
Q_INTERFACES	(decoderInterface)

public:
	QWidget		*createPluginWindow	(int32_t, QSettings *);
			~dominoDecoder		(void);
	bool		initforRate		(int32_t);
	void		doDecode		(DSPCOMPLEX);
	int32_t		rateOut			(void);
	int16_t		detectorOffset		(void);
private:
	enum decoders {
	   MODE_DOMINOEX5 =	0100,
	   MODE_DOMINOEX11 =	0101,
	   MODE_DOMINOEX22 =	0102,
	   MODE_DOMINOEX4 =	0103,
	   MODE_DOMINOEX8 =	0104,
	   MODE_DOMINOEX16 =	0105
	};

private slots:
	void		domino_setMode		(const QString &);
	void		domino_setFec		(const QString &);
	void		domino_setReverse	(const QString &);
	void		domino_setSquelch	(int);

private:
	int32_t		theRate;
	QSettings	*dominoSettings;

	QString		dominoText;
	QString		dominoText2;

	QFrame		*myFrame;
	void		setup_Device		(int32_t);
//
	void		setupInterfaceforMode	(int16_t);
	DSPCOMPLEX	Mixer 			(int16_t, DSPCOMPLEX);
	void		recvchar 		(int16_t c);
	void		decodeDomino		(int16_t c);
	int16_t		findBestTone		(void);
	int16_t		MapTonetoNibble		(int16_t);
	int16_t		adjustSyncCounter	(int16_t, int16_t);
	void		eval_s2n		(int16_t);
	int16_t		get_secondary_char	(void);
	void		put_sec_char		(int16_t);
	void		put_rx_char		(int16_t);
	void		reset_filters		(void);
	int16_t		varidec			(uint16_t);
// MultiPsk FEC scheme
// Rx
	uint16_t	SplittoSecChar		(uint16_t c);
	void		decodeforFec		(int16_t c);

	int32_t		_WorkingRate;
	float		dominoBaudrate;
	int16_t		dominoSamplesperSymbol;
	int8_t		doublespaced;
	int8_t		paths;
	int16_t		numofBins;
	int16_t		basetone;
	int16_t		lotone;
	int16_t		hitone;
	int16_t		extones;
	float		toneSpacing;
	float		bandWidth;
	bool		outofRange;
	bool		dominoSquelch_on;
	bool		dominoFec_on;
	bool		dominoReverse_on;
	double		squelchValue;
	int16_t		dominoModus;
	int16_t		dominoCacheLineP;
	
	int16_t		currTone;
	int16_t		prev1Tone;
	int16_t		prev2Tone;

	double		dominoSignalValue;
	double		dominoNoiseValue;
	double		s2n;
	double		metric;

	int16_t		dominoSyncCounter;

	unsigned char	dominoSymbolbuffer [MAX_VARICODE_LEN];
	int16_t		symcounter;

	bool		staticburst;
	bool		outofrange;

	fftFilter	*bandFilter;
	average		*syncfilter;
	CacheLine	*spectrumCache;
	
	average		*syncFilter;
	double		phase [MAXFFTS];
	slidingFFT	*binsfft [MAXFFTS];
// FEC variables
	viterbi		*dominoViterbi;
	Interleaver	*dominoInterleaver;
	uint8_t		MuSymbolpair[2];
	uint32_t	Datashiftreg;
	int		Fec_symcounter;

	void		domino_clrText		(void);
	void		domino_addText		(char);
	void		domino_addText2		(char);
	void		domino_doshowS2N	(int);
};

#endif
