#
/*
 *    Copyright (C) 2010, 2011, 2012, 2013
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Programming
 *
 *    This file is part of the SDR-J (JSDR).
 *    Many of the ideas as implemented in ESDR are derived from
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

#ifndef __MFSK_DECODER
#define	__MFSK_DECODER
#
/*		
 *	Mfsk  -- receiver classs
 */
#include	<QObject>
#include	<QFrame>
#include	"ui_widget.h"
#include	<QString>
#include	"swradio-constants.h"
#include	"decoder-interface.h"
#include	"slidingfft.h"
#include	"viterbi.h"
#include	"interleave.h"
#include	"fft.h"
#include	"fft-filters.h"
#include	"utilities.h"
#include	"oscillator.h"

class	QSettings;
class	Cache;

class mfskDecoder: public decoderInterface, public Ui_Form {
Q_OBJECT
Q_PLUGIN_METADATA (IID "mfskDecoder")
Q_INTERFACES (decoderInterface)

public:
	QWidget		*createPluginWindow	(int32_t, QSettings *);
			~mfskDecoder		(void);
	void		doDecode		(DSPCOMPLEX);
	bool		initforRate		(int32_t);
	int32_t		rateOut			(void);
	int16_t		detectorOffset		(void);
private:
	int32_t		theRate;
	QSettings	*mfskSettings;

	QFrame		*myFrame;
	void		AdjustParameters	(void);

	QString		mfskText;
	enum decoders {
	   MODE_MFSK_4 =	0100,
	   MODE_MFSK_8 =	0101,
	   MODE_MFSK_11	=	0102,
	   MODE_MFSK_16 =	0103,
	   MODE_MFSK_22	=	0104,
	   MODE_MFSK_31	=	0105,
	   MODE_MFSK_32	=	0106,
	   MODE_MFSK_64 =	0107
	};
	void		MapTonetoBits 	(DSPCOMPLEX *, int16_t, unsigned char *);
	void		setupInterfaceforMode	(uint8_t);
	int16_t		IndextoFullestBin	(DSPCOMPLEX *);
	void		handle_mfsk_bits	(uint8_t);
	void		shiftBit		(uint8_t);
	int16_t		vari_Decode		(uint16_t symbol);

	int32_t		_WorkingRate;
	Oscillator	*LocalOscillator;

	double		mfskIF;
	bool		mfsk_afcon;
	uint8_t		mfsk_mode;
	bool		mfsk_reverse;
	int16_t		mfsk_squelch;
	int16_t		mfskSamplesperSymbol;
	int16_t		mfskNumBits;
	int16_t		mfskBaseTone;	/* 1500 Hz		*/
	int16_t		mfskNumTones;
	int16_t		mfskBorder;
	double		mfskBaudRate;
	double		mfskToneSpacing;

	float		mfskBw;

	unsigned char	mfskDibitpair [2];
	fftFilter	*mfskbandFilter;

	viterbi		*ViterbiDecodera;
	viterbi		*ViterbiDecoderb;
	Interleaver	*mfskInterleaver;
	Cache		*mfskSpectrumCache;
	uint16_t	mfskCacheLineP;

	slidingFFT	*mfskSlidingfft;
	uint16_t	mfskDatashreg;

	DSPCOMPLEX	mfskPrev1Vector;
	DSPCOMPLEX	mfskPrev2Vector;

	uint16_t	mfskPrev1Index;
	uint16_t	mfskPrev2Index;
	int16_t		showCount;

	float		mfskMet1;
	float		mfskMet2;
	float		mfskSignalMetric;
	uint8_t		mfskbitCounter;
	int		mfskSyncCounter;

	float		mfskbaudrateEstimate;


private slots:
	void		mfsk_setAfcon		(const QString &);
	void		mfsk_setReverse		(const QString &);
	void		mfsk_setMode		(const QString &);
	void		mfsk_setSquelch		(int);
private:
	void		mfsk_clrText	();
	void		mfsk_addText	(char);
	void		mfsk_showLocalScope	(int);
	void		mfsk_addLocalScope	(int, double);
	void		mfsk_showCF	(float);
	void		mfsk_showS2N	(int);
	void		mfsk_showTuning	(int);
	void		mfsk_showEstimate	(float);
};

class Cache {
public:
		Cache		(int16_t, int16_t);
		~Cache		(void);
DSPCOMPLEX	*CacheLine	(int16_t a);
private:
	int16_t		rowBound;
	int16_t		columnBound;
	DSPCOMPLEX	**data;
};

#endif
