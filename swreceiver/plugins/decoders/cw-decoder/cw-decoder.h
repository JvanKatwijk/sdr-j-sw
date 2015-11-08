#
/*
 *    Copyright (C) 2011, 2012, 2013
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Programming
 *
 *    This file is part of the SDR-J (JSDR).
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

#ifndef __CW_DECODER
#define	__CW_DECODER
#
#include	<QWidget>
#include	<QFrame>
#include	"swradio-constants.h"
#include	"decoder-interface.h"
#include	"ui_widget.h"

class	LowPassIIR;
class	Oscillator;
class	average;
class	QSettings;

#define	CWError		1000
#define	CWNewLetter	1001
#define	CWCarrierUp	1002
#define	CWCarrierDown	1005
#define	CWStroke	1003
#define	CWDot		1004
#define	CWendofword	1006
#define	CWFailing	1007

#define	CW_RECEIVE_CAPACITY	040

class cwDecoder: public decoderInterface, public Ui_Form {
Q_OBJECT
#if QT_VERSION >= 0x050000
Q_PLUGIN_METADATA (IID "cwDecoder")
#endif
Q_INTERFACES (decoderInterface)

public:
	QWidget	*createPluginWindow	(int32_t,
	                                 QSettings *);
		~cwDecoder		(void);
	void	doDecode		(DSPCOMPLEX);
	bool	initforRate		(int32_t);
	int32_t	rateOut			(void);
	int16_t	detectorOffset		(void);
private:
	int32_t		theRate;
	QSettings	*cwSettings;
	QFrame		*myFrame;

private slots:
	void		cw_setWordsperMinute	(int);
	void		cw_setTracking		(void);
	void		cw_setFilterDegree	(int);
	void		cw_setSquelchValue	(int);

private:
	void		setup_cwDecoder		(int32_t);
	void		cw_clrText		(void);
	void		cw_addText		(char);
	QString		cwText;
	void		cw_showdotLength	(int);
	void		cw_showspaceLength	(int);
	void		cw_showspeed		(int);
	void		cw_showagcpeak		(int);
	void		cw_shownoiseLevel	(int);
//
	void		speedAdjust		(void);
	void		printChar		(char, char);
	void		addtoPlotter		(DSPCOMPLEX);
	void		addtoStream		(double);
	void		add_cwTokens		(char *);
	int32_t		newThreshold		(int32_t, int32_t);
	int32_t		getMeanofDotDash	(int32_t, int32_t);
	void		lookupToken		(char *, char *);

	Oscillator	*LocalOscillator;
	int32_t		cwPhaseAcc;

	int32_t		CycleCount;
	int16_t		cwFilterDegree;
	average		*SmoothenSamples;
	average		*thresholdFilter;
	average		*spaceFilter;

	float		agc_peak;
	float		noiseLevel;
	float		value;
	float		metric;
	double		cw_IF;
	LowPassIIR	*cw_LowPassFilter;
	int32_t		cwDotLength;
	int32_t		cwSpaceLength;
	int32_t		cwDashLength;
	int16_t		SquelchValue;
	int16_t		cwState;
	int16_t		cwPreviousState;
	int32_t		cw_adaptive_threshold;
	int32_t		currentTime;
	int32_t		cwCurrent;
	int16_t		cwSpeed;
	int16_t		cwDefaultSpeed;
	char		dataBuffer [CW_RECEIVE_CAPACITY];
	int32_t		CWrange;
	int32_t		cwDefaultDotLength;
	int32_t		lowerBoundWPM;
	int32_t		upperBoundWPM;
	int32_t		cwDotLengthMin;
	int32_t		cwDotLengthMax;
	int32_t		cwDashLengthMin;
	int32_t		cwDashLengthMax;
	int32_t		lengthofPreviousTone;
	int32_t		cwNoiseSpike;
	int32_t		cwStartTimestamp;
	int32_t		cwEndTimestamp;
	bool		cwTracking;

signals:
};
#endif

