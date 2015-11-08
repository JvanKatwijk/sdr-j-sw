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

#ifndef __PSK_DECODER
#define	__PSK_DECODER
#

#include	"swradio-constants.h"
#include	"decoder-interface.h"
#include	"ui_widget.h"

class	LowPassIIR;
class	viterbi;
class	QFrame;
class	QSettings;
class	Oscillator;
/*
 * PSK31  -- receiver classs
 */
class pskDecoder: public decoderInterface, public Ui_Form {
Q_OBJECT
#if QT_VERSION >= 0x050000
Q_PLUGIN_METADATA (IID "pskDecoder")
#endif
Q_INTERFACES (decoderInterface)

public:
	QWidget		*createPluginWindow	(int32_t, QSettings *);
			~pskDecoder		(void);
	void		doDecode		(DSPCOMPLEX);
	bool		initforRate		(int);
	int32_t		rateOut			(void);
	int16_t		detectorOffset		(void);
private slots:
	void	psk_setAfcon		(const QString &);
	void	psk_setReverse		(const QString &);
	void	psk_setSquelchLevel	(int);
	void	psk_setMode		(const QString &);
	void	psk_setFilterDegree	(int);
private:
	enum PskMode {
	   MODE_PSK31 =		0100,
	   MODE_PSK63 =		0101,
	   MODE_PSK125 =	0102,
	   MODE_QPSK31 =	0110,
	   MODE_QPSK63 =	0111,
	   MODE_QPSK125 =	0112
	};

	int32_t		theRate;
	QSettings	*pskSettings;
	QFrame		*myFrame;
	void		setup_pskDecoder	(int32_t);
	void		psk_setup		(void);
	float		speedofPskMode		(void);
	bool		isBinarypskMode		(void);
	bool		isQuadpskMode		(void);	
	uint8_t		decodingIsOn		(uint8_t,
	                                         double,
	                                         uint8_t, DSPCOMPLEX *);
	void		bpsk_bit		(uint16_t);
	void		qpsk_bit		(uint16_t);
	int16_t		pskVaridecode		(uint16_t);
	double		psk_IFadjust		(double, uint16_t);
	uint8_t		getIntPhase		(DSPCOMPLEX);
	int16_t		DecimatingCountforpskMode	(void);

	Oscillator	*LocalOscillator;

	int16_t		pskCycleCount;
	LowPassIIR	*LPM_Filter;
	viterbi		*viterbiDecoder;
	double		psk_IF;
	int16_t		pskDecimatorCount;
	unsigned int 	pskShiftReg;
	int16_t		pskPhaseAcc;
	uint32_t 	pskDecodeShifter;
	DSPCOMPLEX	pskQuality;
	double		pskAfcmetrics;
	bool		pskAfcon;
	bool		pskReverse;
	int16_t		pskSquelchLevel;
	uint8_t		pskMode;
	int16_t		pskFilterDegree;
	DSPCOMPLEX	pskOldz;
	double		psk_phasedifference;

	int16_t		pskPrev1Phase;
	int16_t		pskPrev2Phase;
	int16_t		pskCurrPhase;
	int16_t		pskSyncCounter;
	float		pskBitclk;
	DSPCOMPLEX	*pskBuffer;

	void		mapTable	(int32_t, int32_t);
	bool		resample	(DSPCOMPLEX, DSPCOMPLEX *, int16_t *);
	float		theTable [200];
	DSPCOMPLEX	inTable	 [2400];
	int16_t		tablePointer;
	int16_t		tableSize;
	int16_t		inSize;
//
//	former signals are now handled locally
	QString		pskText;
	void		psk_showDial		(float);
	void		psk_showMetrics		(float);
	void		psk_showIF		(float);
	void		psk_addText		(char);
	void		psk_clrText		();
};

#endif


