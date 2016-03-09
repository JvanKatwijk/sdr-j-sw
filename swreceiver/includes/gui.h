#
/*
 *    Copyright (C) 2012, 2013
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
#ifndef __GUI
#define __GUI

#include	"swradio-constants.h"
#include	<QDialog>
#include	<qwt.h>
#include	<QTimer>
#include	<QQueue>
#include	<QWheelEvent>
#include	<QObject>
#include	<sndfile.h>
#include	"ui_sdrgui.h"
#include	"ringbuffer.h"

class	Oscillator;
class	QSettings;
class	QDir;
class	fft_scope;
class	paWriter;
class	decoderInterface;
class	rigInterface;
class	decoderLoader;
class	deviceLoader;
class	fftFilter;
class	agcHandler;
class	notchFilter;
class	resampler;
class	S_Meter;
class	attenuator;
/*
 *	The main gui object. It inherits from
 *	QDialog and the generated form
 */

class RadioInterface: public QDialog,
		      private Ui_SDR {
Q_OBJECT
public:
	enum Decoder {
	   NO_DECODER	= 0,
	   AM_DECODER	= 1,
	   FM_DECODER	= 2,
	   AUDIO_TEST	= 3,
	};

		RadioInterface		(QSettings *,
	                                 int32_t,	// Icardrate
	                                 int32_t,	// Ocardrate
	                                 QWidget *parent = NULL);
		~RadioInterface		(void);

private:
	QSettings	*mainSettings;
	int32_t		inputRate;
	int32_t		audioRate;
	int32_t		workingRate;
	int8_t		runMode;
	int16_t		displaySize;
	int16_t		rasterSize;

	bool		createStreamSelectors	(void);
	paWriter	*mySoundcardOut;
	void		setup_soundcards	(void);
	bool		setupSoundOut		(QComboBox *,
	                                         paWriter	*,
	                                         int32_t, int16_t *);
	int16_t		*outTable;
	int16_t		numberofDevices;
	int16_t		outputDevice;
	void		restoreGUIsettings	(QSettings *);
	void		dumpGUIsettings		(QSettings *);
	void		localConnects		(void);
	void		inc_Tuner		(int32_t);
	void		setup_deviceLoader	(void);
	void		setup_decoderLoader	(void);
	void		ClearPanel		(void);
	void		AddtoPanel		(int);
	int32_t		getPanel		(void);
	void		CorrectPanel		(void);
	void		stop_Dumping		(void);
	int32_t		setFrequency		(int32_t);
	bool		fitsinDisplay		(int32_t);
	void		set_hfGain		(int16_t, int16_t);
	int32_t		currentFrequency;
	decoderInterface	*theDecoder;
	decoderLoader	*my_decoderLoader;
	rigInterface	*theDevice;
	deviceLoader	*my_deviceLoader;
	S_Meter		*theMeter;
	attenuator	*theAttenuator;
	bool		lockLO;

	float		S_attack;
	float		S_decay;
	int32_t		delayCount;

	fft_scope	*hfScope;
	fft_scope	*lfScope;
	uint8_t		HFviewMode;
	uint8_t		LFviewMode;

	uint8_t		inputMode;
	int32_t		NeedlePosition;
	int32_t		DetectorOffset;

	int16_t		mouseIncrement;

	int32_t		Panel;
	QTimer		*lcdTimer;
	QTimer		*displayTimer;
	SNDFILE		*hf_dumpFile;
	SNDFILE		*lf_dumpFile;
	int16_t		bitDepth;
//
	resampler	*ifResampler;
	int32_t		attValueL;
	int32_t		attValueR;
	int32_t		tunedIF;
	fftFilter	*hfFilter;
	fftFilter	*lfFilter;
	Oscillator	*myOscillator;
	agcHandler	*agc;
	bool		notch;
	notchFilter	*my_notchFilter;
	void		set_Filters_hf		(int32_t ifFreq);
	void		set_Filters_if		(int32_t ifFreq);
	int16_t		convertRate		(DSPCOMPLEX,
	                                         DSPCOMPLEX *, int32_t);
	int32_t		getBand_highend		(void);
	int32_t		getBand_lowend		(void);
/*
 *	The UI acts as a kind of server, the public slots are the entries
 *	for the radio elements, signals (below) are the triggers
 *	for the radio elements.
 */
private slots:
	void		set_decoderSelect	(const QString &);
	void		set_mouseIncrement	(int);
	void		clickPause		(void);
	void		set_Start		(void);
	void		adjustFrequencywithClick	(int);

	void		set_notchFilter		(void);
	void		set_HFplotterView	(const QString &);
	void		set_LFplotterView	(const QString &);
	void		set_HFspectrumAmplitude	(int);
	void		set_LFspectrumAmplitude	(int);
	void		lcdTimeout		(void);
	void		updateTimeDisplay	(void);

	void		set_InMiddle		(void);
	void		set_bandWidthSelect	(int);
	void		set_bandWidthOffset	(int);
	void		set_quickBand		(const QString &);
	void		set_InputMode		(const QString &);
	void		set_AGCMode		(const QString &);
	void		addOne			(void);
	void		addTwo			(void);
	void		addThree		(void);
	void		addFour			(void);
	void		addFive			(void);
	void		addSix			(void);
	void		addSeven		(void);
	void		addEight		(void);
	void		addNine			(void);
	void		addZero			(void);
	void		addClear		(void);
	void		AcceptFreqinHz		(void);
	void		AcceptFreqinKhz		(void);
	void		addCorr			(void);
	void		set_Attenuator		(int);
	void		set_BalanceSlider	(int);

	void		decT5			(void);
	void		decT50			(void);
	void		decT500			(void);
	void		decT5000		(void);
	void		incT5			(void);
	void		incT50			(void);
	void		incT500			(void);
	void		incT5000		(void);

	void		set_StreamOutSelector	(int);
	void		abortSystem		(int);
	void		TerminateProcess	();

	void		set_workingRate		(const QString &);
	void		set_deviceSelect	(const QString &);
	void		switchHFDump		(void);
	void		switchLFDump		(void);

	void		set_agcThresholdSlider	(int);
	void		set_symbolLeftshift	(int);
public slots:
	void		wheelEvent		(QWheelEvent *);
	void		adjustFrequency		(int);
	void		showPeak		(int, double);
	void		setDetectorMarker	(int);
	void		showAgcGain		(float);
	void		outputSample		(float, float);
	void		sampleHandler		(int);
//
	void		set_notchFrequency	(int);
//
//	especially for the Extio:
	void		set_ExtFrequency	(int);
	void		set_ExtLO		(int);
	void		set_lockLO		(void);
	void		set_unlockLO		(void);
	void		set_stopHW		(void);
	void		set_startHW		(void);
	void		set_inputRate		(int);
};

#endif
