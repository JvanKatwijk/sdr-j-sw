#
/*
 *    Copyright (C)  2011, 2012, 2013
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Programming
 *
 *    This file is part of SDR-J (JSDR).
 *    Many of the ideas as implemented in SDR-J are derived from
 *    other work, made available through the GNU general Public License. 
 *    All copyrights of the original authors are acknowledged and recognized.
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
#include	"gui.h"
#include	<unistd.h>
#include	<QInputDialog>
#include	<QSettings>
#include	<QMessageBox>
#include	<QFileDialog>
#include	<QDebug>
#include	<QDateTime>
#include	<QFile>
#include	<QTextStream>
#include	<QObject>
#include	<QWidget>
#include	<QPushButton>
#include	<QDir>
#include	"no-rig.h"
#include	"fft-scope.h"
#include	"decoder-interface.h"
#include	"decoderloader.h"
#include	"rig-interface.h"
#include	"deviceloader.h"
#include	"agchandler.h"
#include	"fft-filters.h"
#include	"fir-filters.h"
#include	"pa-writer.h"
#include	"utilities.h"
#include	"oscillator.h"
#include	"resampler.h"
#include	"sndfile.h"
#include	"s-meter.h"
#include	"attenuator.h"
//
//	runmodes
#define	IDLE		0100
#define	PAUSED		0101
#define	RUNNING		0102
#define	BUFFERSIZE	1024
/*
 *	We use the creation function merely to set up the
 *	user interface and make the connections between the
 *	gui elements and the handling agents. All real action
 *	is embedded in actions, initiated by gui buttons
 *	and by the "sampleHandler" slot that is "called"
 *	from the external device
 */
	RadioInterface::RadioInterface (QSettings	*sI,
	                                int32_t		rateInx,
	                                int32_t		rateOutx,
	                                QWidget		*parent): QDialog (parent) {
bool	soundSeemsOk;
QString h;
int32_t	k;

// 	the setup for the generated part of the ui
	fprintf (stderr, "going to setup\n");
	setupUi (this);
	this	-> mainSettings	= sI;
	if (mainSettings == NULL)
	   fprintf (stderr, "foutje\n");
	this	-> runMode	= IDLE;

	this	-> displaySize =
	           mainSettings -> value ("displaywidth", 1024). toInt ();

	this -> inputRate =
	              mainSettings -> value ("inputRate", 96000). toInt ();

	this -> audioRate =
	              mainSettings -> value ("audioRate", "48000"). toInt ();

	if (audioRate < 48000) {
	   qDebug () << "Version 6 requires output to be a multiple of 48000\n";
	   audioRate = 48000;
	}
//
//	defaults
	this	-> rasterSize	= 50;
	this	-> bitDepth	= 24;	// effective from the EMU 202
//
//	workingrate - if set in the ini file - should match a value
//	on the combobox
	workingRate		= 12000;	// default
	h		= mainSettings -> value ("workingRate", "12000").
	                                                          toString ();
	k		= workingrateSelect	-> findText (h);
	if (k != -1) {
	   workingrateSelect -> setCurrentIndex (k);
	   workingRate = mainSettings -> value ("workingRate", "12000"). toInt ();
	}
//
//	for this setting
	ifResampler	= new resampler (inputRate, workingRate, BUFFERSIZE);

	setup_soundcards	();
//	... and restore restore GUI settings 
	restoreGUIsettings	(mainSettings);

//	lfScope setup
	lfScope			= new fft_scope (lfscope,
	                                         displaySize,
	                                         1,
	                                         rasterSize,
	                                         1024,
	                                         workingRate,
	                                         10);
	LFviewMode		= SPECTRUM_MODE;
	lfScope			-> SelectView (SPECTRUM_MODE);
	lfScope			-> setNeedle (0);
	lfScope			-> setBitDepth	(bitDepth - 1);
	connect (lfScope, SIGNAL (clickedwithLeft (int)),
	         this, SLOT (AdjustFrequency (int)));
//
//	hfScope setup
	hfScope		= new fft_scope (hfscope,
	                                 2 * displaySize,
	                                 KHz (1),
	                                 rasterSize,
	                                 4 * displaySize,	// spectrumSize
	                                 inputRate,
	                                 10);
	HFviewMode		= SPECTRUM_MODE;
	hfScope			-> SelectView (SPECTRUM_MODE);
	hfScope			-> setNeedle (this -> inputRate / 4);
	hfScope			-> setBitDepth	(bitDepth - 1);

	connect (hfScope,
	         SIGNAL (clickedwithLeft (int)),
	         this,
	         SLOT (AdjustFrequencywithKHz (int)));
//
	QString Version		= QString ("sdrJ-SW-V");
	Version. append (QString(CURRENT_VERSION));
	systemindicator		-> setText (Version);
	lcd_SampleRate		-> display (inputRate);
	ClearPanel		();
/*
 * 	Timers:
 * 	a. displayTimer deals with showing actual time
 * 	b. lcdTimer deals with frequency display lcd screen
 *	Create a timer for dealing with a non acting (or reacting) user
 *	on the keyboard
 */
	displayTimer		= new QTimer ();
	displayTimer		-> setInterval (1000);
	connect (displayTimer, SIGNAL (timeout ()),
	         this, SLOT (updateTimeDisplay ()));

	lcdTimer		= new QTimer ();
	lcdTimer		-> setSingleShot (true);	// single shot
	lcdTimer		-> setInterval (5000);
	connect (lcdTimer, SIGNAL (timeout ()),
	         this, SLOT (lcdTimeout ()));
//
//	Device handling is delegated to a separate deviceLoader and
//	decoder handling is delegated to a separate decoderloader
	setup_deviceLoader	();
	setup_decoderLoader	();
//
//	just defaulting
	theDevice		= my_deviceLoader -> loadDevice ("no rig",
	                                                         inputRate);

	this	-> lockLO	= false;	// only to be changed by Extio

	tunedIF			= inputRate / 4;
	bandLow			= -workingRate / 2;
	bandHigh		= workingRate / 2;
//
//	Initially, the filter is set over the whole input range
	hfFilter		= new fftFilter (2048, 377);
	hfFilter		-> setBand (bandLow, bandHigh, inputRate);
	myOscillator		= new Oscillator (inputRate);
//
//	The second one is less demanding
	lfFilter		= new fftFilter (1024, 127);
	lfFilter		-> setBand (bandLow, bandHigh, workingRate);
//
//
	agc			= new agcHandler (workingRate);
	agc			-> setMode (agcHandler::AGC_OFF);
//	for the S meter
	theMeter		= new S_Meter (inputRate);
	theAttenuator		= new attenuator;
	delayCount		= 0;
//
	hf_dumpFile		= NULL;
	if_dumpFile		= NULL;
	lf_dumpFile		= NULL;
//
//	set the "state" values to match the GUI settings
	set_Attenuator		(attenuationSlider	-> value ());
	set_BalanceSlider	(balanceSlider		-> value ());
	set_InputMode		(inputModeSelect	-> currentText ());
	set_AGCMode		(AGC_select		-> currentText ());
	set_quickBand		(quickbandSelect	-> currentText ());
//	set_bandWidthSelect	(bandwidthSlider	-> value ());
//	set_bandWidthOffset	(bandoffsetSlider 	-> value ());
	set_LFspectrumAmplitude (LFspectrumamplitudeSlider -> value ());
	set_HFspectrumAmplitude (HFspectrumamplitudeSlider -> value ());
	set_symbolLeftshift	(symbol_shifter		-> value ());
	set_mouseIncrement	(mouseIncrementSlider	-> value ());
	set_StreamOutSelector 	(streamOutSelector	-> currentIndex ());
//	The connects from buttons and sliders from the GUI to the
//	different handlers is done in:
	localConnects		();
//
//	Note that setFrequency has as side effect that the bandfilters
//	are set/changed.
	tunedIF			= setFrequency (Khz (14070));
	connect	(theDevice, SIGNAL (samplesAvailable (int)),
	         this, SLOT (sampleHandler (int)));
	qDebug ("GUI is set up and working\n"); 
	displayTimer		-> start (1000);
}

	RadioInterface::~RadioInterface () {
	delete		hfScope;
	delete		lfScope;
	delete		lcdTimer;
	delete		displayTimer;
	delete		ifResampler;
	delete		agc;
	delete		theMeter;
	delete		theAttenuator;
}
//
//	We distinguish between restoring the settings of labels, sliders
//	etc on the GUI and setting the control state, based on these
//	settings. Here, we merely set the sliders etc. Since no
//	connects are made yet, nothing further will happen
void	RadioInterface::restoreGUIsettings	(QSettings *s) {
int	k;
QString	h;
	pauseButton		-> setText (QString ("Pause"));
	HFdumpButton		-> setText ("hf dump");
	IFdumpButton		-> setText ("if dump");
	LFdumpButton		-> setText ("lf dump");

	k			= s -> value ("balanceSlider", 0). toInt ();
	balanceSlider		-> setValue (k);

	k			= s -> value ("attenuationSlider", 50).
	                                                      toInt ();
	attenuationSlider	-> setValue (k);

	h		= s -> value ("inputModeSelect", "IandQ"). toString ();
	k		= inputModeSelect	-> findText (h);
	if (k != -1) 
	   inputModeSelect -> setCurrentIndex (k);

	h		= s -> value ("AGC_select", "AGC off"). toString ();
	k		= AGC_select		-> findText (h);
	if (k != -1) 
	   AGC_select	-> setCurrentIndex (k);
	k	= s -> value ("agcThresholdSlider", -115). toInt();
	agc_ThresholdSlider -> setValue (k);

	h		= s -> value ("quickbandSelect", "usb"). toString ();
	k		= quickbandSelect 	-> findText (h);
	if (k != -1) 
	   quickbandSelect -> setCurrentIndex (k);

	k		= s -> value ("LFspectrumamplitudeSlider", 50).
	                                                           toInt ();
	LFspectrumamplitudeSlider	-> setValue (k);

	k		= s -> value ("HFspectrumamplitudeSlider", 50).
	                                                           toInt ();
	HFspectrumamplitudeSlider	-> setValue (k);

	k		= s -> value ("symbol_shifter", 0). toInt ();
	symbol_shifter		-> setValue (k);
//
	h	= s -> value ("streamOutSelector", "default").
	                                              toString ();
	k	= streamOutSelector -> findText (h);
	if (k != -1) 
	   streamOutSelector -> setCurrentIndex (k);
	fprintf (stderr, "streamOutSelector %s (%d)\n", h. toLatin1 (). data (),
	                                                    k);
	k	= s -> value ("mouseIncrement", 20). toInt ();
	mouseIncrementSlider	-> setValue (k);
}
//
//	dumping the GUI settings should mirror the restauration
//
void	RadioInterface::dumpGUIsettings (QSettings *s) {
	if (s == NULL)
	   return;

	s	-> setValue ("balanceSlider",
	                          balanceSlider -> value ());
	s	-> setValue ("attenuationSlider",
	                          attenuationSlider -> value ());
	s	-> setValue ("displaywidth", displaySize);
	s	-> setValue ("HFplotterView",
	                          HFplotterView	-> currentText ());
	s	-> setValue ("LFplotterView",
	                          LFplotterView	-> currentText ());
	s	-> setValue ("inputModeSelect",
	                          inputModeSelect -> currentText ());
	s	-> setValue ("AGC_select",
	                          AGC_select	-> currentText ());
	s	-> setValue ("quickbandSelect",
	                          quickbandSelect -> currentText ());

//	set_bandWidthSelect	(bandwidthSlider	-> value ());
//	set_bandWidthOffset	(bandoffsetSlider 	-> value ());

	s	-> setValue ("LFspectrumamplitudeSlider",
	                          LFspectrumamplitudeSlider -> value ());
	s	-> setValue ("HFspectrumamplitudeSlider",
	                          HFspectrumamplitudeSlider -> value ());
	s	-> setValue ("workingRate",
	                          workingrateSelect -> currentText ());
	s	-> setValue ("symbol_shifter",
	                          symbol_shifter -> value ());

	s	-> setValue ("agcThresholdSlider",
	                          agc_ThresholdSlider -> value ());
	s	-> setValue ("streamOutSelector",
	                          streamOutSelector -> currentText ());
	s	-> setValue ("mouseIncrement",
	                      mouseIncrementSlider -> value ());
}
//
//	clicking on the pause button
void	RadioInterface::clickPause (void) {
	if ((runMode == IDLE) || (theDevice == NULL))
	   return;

	if (runMode == RUNNING) {		// setting to Pause
	   theDevice		-> stopReader ();
	   mySoundcardOut	-> stopWriter ();
	   pauseButton -> setText (QString ("Continue"));
	   runMode = PAUSED;
	   return;
	}

	if (runMode == PAUSED) {
	   theDevice		-> restartReader ();
	   mySoundcardOut	-> restartWriter ();
	   pauseButton -> setText (QString ("Pause"));
	   runMode = RUNNING;
	}
}
/*
 *	In setStart we trigger the hf processor
 */
void	RadioInterface::set_Start	(void) {

	if ((runMode == RUNNING) || (theDevice == NULL))
	   return;

	setFrequency (theDevice -> defaultFrequency () + inputRate);
	theDevice	-> restartReader ();
	mySoundcardOut	-> restartWriter ();
	pauseButton -> setText (QString ("Pause"));
	runMode = RUNNING;
}
//
//
void	RadioInterface::TerminateProcess (void) {
	stop_Dumping	();
	runMode		= IDLE;		// this will ensure that
	                                // signals on incoming data
	                                // will be ignored
//	ensure that the device processing software is closed
	if (theDevice != NULL) 
	   theDevice		-> exit ();
	usleep (1000);
	delete theDevice;

//	and ensure proper termination of decoders
	if (theDecoder != NULL) {
	   delete theDecoder;
	   theDecoder = NULL;
	}

	mySoundcardOut	-> stopWriter ();
//	Everything now has come to a halt,
//	save some slider values
	if (mainSettings != NULL)
	   dumpGUIsettings (mainSettings);

	accept ();
	qDebug () <<  "End of termination procedure";
}

void	RadioInterface::abortSystem (int d) {
	qDebug ("aborting for reason %d\n", d);
	accept ();
}
///////////////////////////////////////////////////////////////////
//	changing the inputrate only will be called from
//	within the rigs.
//	Quite a few of the instantiated classes have to
//	be renewed.
void	RadioInterface::set_inputRate (int32_t newInputRate) {

	if (inputRate == newInputRate)
	   return;

	if (hf_dumpFile != NULL) {
	   sf_close (hf_dumpFile);
	   hf_dumpFile	= NULL;
	   HFdumpButton	-> setText ("hf dump");
	}

	runMode			= IDLE;
	theDevice		-> stopReader ();
	inputRate		= newInputRate;
	lcd_SampleRate		-> display (inputRate);

	delete ifResampler;
	ifResampler		= new resampler (inputRate,
	                                         workingRate, BUFFERSIZE);
	delete hfFilter;
	hfFilter		= new fftFilter (2048, 511);
	hfFilter		-> setBand (getbandLow (),
	                                    getbandHigh (), inputRate);

	delete myOscillator;
	myOscillator		= new Oscillator (inputRate);

//	we create a new scope
	delete	hfScope;
	hfScope			= new fft_scope (hfscope,
	                                         displaySize,
	                                         Khz (1),
	                                         rasterSize,
	                                         4096,
	                                         theDevice	-> getRate (),
	                                         10);
	connect (hfScope,
	         SIGNAL (clickedwithLeft (int)),
	         this,
	         SLOT (AdjustFrequencywithKHz (int)));
	hfScope		-> setNeedle (inputRate / 4);
	hfScope			-> setBitDepth	(bitDepth - 1);
	set_HFplotterView	(HFplotterView	-> currentText ());

//	and we restore some settings, directly from the GUI settings
	set_Attenuator		(attenuationSlider	-> value ());
	set_BalanceSlider	(balanceSlider		-> value ());
	set_InputMode		(inputModeSelect	-> currentText ());
	set_AGCMode		(AGC_select		-> currentText ());
	set_agcThresholdSlider	(agc_ThresholdSlider	-> value ());
	set_symbolLeftshift	(symbol_shifter		-> value ());
	set_bandWidthOffset	(bandoffsetSlider 	-> value ());
	set_bandWidthSelect	(bandwidthSlider	-> value ());
	lcd_SampleRate		-> display (inputRate);
	tunedIF		= setFrequency (theDevice -> defaultFrequency ());
}
//
//	Changing the workingrate might be called from the GUI,
void	RadioInterface::set_workingRate (const QString &s) {
int32_t	newWorkingRate	= (int32_t)(s. toInt ());

	if (newWorkingRate == workingRate)
	   return;

	if (if_dumpFile != NULL) {
	   sf_close (if_dumpFile);
	   if_dumpFile = NULL;
	   IFdumpButton	-> setText ("if dump");
	}

	workingRate		= newWorkingRate;

	delete ifResampler;
	ifResampler		= new resampler (inputRate,
	                                         workingRate, BUFFERSIZE);
//	we also need a new LF scope
	delete	lfScope;
	lfScope			= new fft_scope (lfscope,
	                                         displaySize,
	                                         1,
	                                         rasterSize,
	                                         1024,
	                                         workingRate,
	                                         10);
	connect (lfScope,
	         SIGNAL (clickedwithLeft (int)),
	         this,
	         SLOT (AdjustFrequency (int)));
	lfScope			-> setNeedle (0);
	lfScope			-> setBitDepth	(bitDepth - 1);
	set_LFplotterView	(LFplotterView	-> currentText ());
}
////////////////////////////////////////////////////////////////
//	lots of local settings
//
void	RadioInterface::set_symbolLeftshift	(int s) {
	theAttenuator -> setShift (s);
}
//
//	will be called from the detectors
void	RadioInterface::setDetectorMarker (int foi) {
	lfScope	-> setNeedle (foi);
}

void	RadioInterface::set_InputMode (const QString &s) {
	if (s == "I and Q")
	   inputMode	= IandQ;
	else
	if (s == "Q and I")
	   inputMode	= QandI;
	else
	if (s == "I Only")
	   inputMode	= I_Only;
	else
	if (s == "Q Only")
	   inputMode	= Q_Only;
	else
	   inputMode	= IandQ;
}

void	RadioInterface::set_AGCMode (const QString &s) {
uint8_t	gainControl;

	if (s == "off AGC")
	   gainControl	= agcHandler::AGC_OFF;
	else
	if (s == "slow")
	   gainControl	= agcHandler::AGC_SLOW;
	else
	   gainControl	= agcHandler::AGC_FAST;
	agc	-> setMode	(gainControl);
}

void	RadioInterface::set_agcThresholdSlider (int v) {
	agc	-> setThreshold (v);
}
/*
 * 	the balance slider runs between 0 .. 30
 */
void	RadioInterface::set_BalanceSlider (int n) {
	IQBalanceDisplay -> display (n);
	theAttenuator -> set_hfGain (n, attenuationSlider -> value ());
}

void	RadioInterface::set_Attenuator (int n) {
	attenuationLevelDisplay	-> display (n);
	theAttenuator -> set_hfGain (balanceSlider -> value (), n);
}

void	RadioInterface::set_HFspectrumAmplitude (int n) {
	hfScope	-> setAmplification (n);
}

void	RadioInterface::set_LFspectrumAmplitude (int n) {
	lfScope	-> setAmplification (n);
}
/*
 * 	Handling the numeric keypad is boring, but needed
 */
void RadioInterface::addOne() {
	AddtoPanel (1);
}

void RadioInterface::addTwo() {
	AddtoPanel (2);
}

void RadioInterface::addThree() {
	AddtoPanel (3);
}

void RadioInterface::addFour() {
	AddtoPanel (4);
}

void RadioInterface::addFive() {
	AddtoPanel (5);
}

void RadioInterface::addSix() {
	AddtoPanel (6);
}

void RadioInterface::addSeven() {
	AddtoPanel (7);
}

void RadioInterface::addEight() {
	AddtoPanel (8);
}

void RadioInterface::addNine() {
	AddtoPanel (9);
}

void RadioInterface::addZero() {
	AddtoPanel (0);
}

void RadioInterface::addClear() {
	ClearPanel ();
	if (lcdTimer	-> isActive ())
	   lcdTimer -> stop ();
	lcdFrequency -> display (currentFrequency);
}

void RadioInterface::AcceptFreqinHz (void) {
int32_t	p;

	if (!lcdTimer -> isActive ())
	   return;
	lcdTimer	-> stop ();
	p 		= getPanel ();
	ClearPanel	();
	if (!lockLO) 
	   tunedIF	= setFrequency (p);
}

void RadioInterface::AcceptFreqinKhz (void) {
int32_t	p;

	if (!lcdTimer -> isActive ())
	   return;
	lcdTimer -> stop ();
	p = KHz (getPanel ());
	ClearPanel ();
	if (!lockLO) 
	   tunedIF	= setFrequency (p);
}

void RadioInterface::addCorr (void) {
	if (lcdTimer -> isActive ()) {
	   lcdTimer	-> stop ();
	   Panel = (Panel - (Panel % 10)) / 10;
	   lcdFrequency -> display ((int)Panel);
	   lcdTimer	-> start (5000);		// restart timer
	}
}

void	RadioInterface::AddtoPanel (int n) {
	if (lcdTimer -> isActive ())
	   lcdTimer -> stop ();
	   
	Panel = 10 * Panel + n;
	lcdFrequency	-> display ((int)Panel);
	lcdTimer	-> start (5000);		// restart timer
}

void	RadioInterface::ClearPanel (void) {
	Panel = 0;
}

int32_t	RadioInterface::getPanel (void) {
	return Panel;
}

void RadioInterface::decT5 (void) {
	AdjustFrequency (-1);
}

void RadioInterface::decT50 (void) {
	AdjustFrequency (-10);
}

void RadioInterface::decT500 (void) {
	AdjustFrequency (-100);
}

void RadioInterface::decT5000 (void) {
	AdjustFrequency (-1000);
}

void RadioInterface::incT5 (void) {
	AdjustFrequency (1);
}

void RadioInterface::incT50 (void) {
	AdjustFrequency (10);
}

void RadioInterface::incT500 (void) {
	AdjustFrequency (100);
}

void RadioInterface::incT5000 (void) {
	AdjustFrequency (1000);
}

void	RadioInterface::AdjustFrequency (int32_t n) {
	if (lcdTimer	-> isActive ()) {
	   ClearPanel ();
	   lcdTimer -> stop ();
	}

	inc_Tuner (n);
}

void	RadioInterface::AdjustFrequencywithKHz (int n) {
	AdjustFrequency (Khz (n));
}
//
//	If/when we want the needle in the middle, we just
//	set the frequency
void	RadioInterface::set_InMiddle (void) {
int32_t	vfo	= currentFrequency - inputRate / 4;
	theDevice	-> setVFOFrequency (vfo);
	hfScope	-> setZero	(vfo);
	hfScope	-> setNeedle    (inputRate / 4);
	set_Filters_hf		(inputRate / 4);
	lcdFrequency	-> display (currentFrequency);
}

void	RadioInterface::inc_Tuner (int32_t amount) {
	if (!lockLO) {
	   tunedIF	= setFrequency (currentFrequency + amount);
	} 
}

//
void	RadioInterface::set_HFplotterView (const QString &s) {
	hfScope -> SelectView (s == "waterfallView" ?
	                               WATERFALL_MODE : SPECTRUM_MODE);
}

void	RadioInterface::set_LFplotterView (const QString &s) {
	lfScope -> SelectView (s == "waterfallView" ?
	                               WATERFALL_MODE : SPECTRUM_MODE);
}
/*
 *	... and now we manage the passband here.
 *	n is an integer range 0 .. 1000
 */
void	RadioInterface::set_bandWidthSelect (int n) {
int32_t	offset	= bandoffsetSlider -> value () * workingRate / 1000;
int32_t	low	= offset - n * workingRate / 1000 / 2;
int32_t	high	= offset + n * workingRate / 1000 / 2;

	bandwidthDisplay	-> display (high - low);
	bandoffsetDisplay	-> display (offset);
	bandLow		= low;
	bandHigh	= high;
	set_Filters_hf (tunedIF);
	set_Filters_if (tunedIF);
}
/*
 * 	n is an integer range -500 .. 500
 */
void	RadioInterface::set_bandWidthOffset (int n) {
int32_t	offset	= n * (int32_t)workingRate / 1000;
int32_t	k	= bandwidthSlider	-> value ();
int32_t	low	= offset - k * (int32_t)workingRate / 1000 / 2;
int32_t	high	= offset + k * (int32_t)workingRate / 1000 / 2;
	bandwidthDisplay	-> display (high - low);
	bandoffsetDisplay	-> display (offset);
	bandLow		= low;
	bandHigh	= high;
	set_Filters_hf (tunedIF);
	set_Filters_if (tunedIF);
}
//
//	presets for the band selection
void	RadioInterface::set_quickBand (const QString &s) {
	if (s == "am") {
	   int32_t n		= 9000 * 1000 / workingRate;
	   bandwidthSlider		-> setValue (n);
	   set_bandWidthSelect		(n);
	   bandoffsetSlider		-> setValue (0);
	   set_bandWidthOffset		(0);
	}
	else 
	if (s == "usb") {
	   int32_t n		= 2500 * 1000 / workingRate;
	   bandwidthSlider		-> setValue (n);
	   set_bandWidthSelect		(n);
	   bandoffsetSlider		-> setValue (1250 * 1000 / workingRate);
	   set_bandWidthOffset		(1250 * 1000 / workingRate);
	}
	else
	if (s == "lsb") {		/* mode = lsb	*/
	   int32_t	bw;
	   bw			= 2500 * 1000 / workingRate;
	   bandwidthSlider		-> setValue (bw);
	   set_bandWidthSelect		(bw);
	   bandoffsetSlider		-> setValue (- 1250 * 1000/ workingRate);
	   set_bandWidthOffset		(- 1250 * 1000 / workingRate);
	}
	else
	if (s == "drm") {		/* special for drm	*/
	   bandwidthSlider		-> setValue (1000 * 12000 / workingRate);
	   bandoffsetSlider		-> setValue (0 * 1000 / workingRate);
	   set_bandWidthOffset		(0 * 1000 / workingRate);
	}
	else
	if (s == "fax") {
	   bandwidthSlider		-> setValue (1100 * 1000 / workingRate);
	   set_bandWidthSelect		(1100 * 1000 / workingRate);
	   bandoffsetSlider		-> setValue (1900 * 1000 / workingRate);
	   set_bandWidthOffset		(1900 * 1000 / workingRate);
	}
	else
	if (s == "psk") {
	   bandwidthSlider		-> setValue (1000 * 1000 / workingRate);
	   set_bandWidthSelect		(1000 * 1000 / workingRate);
	   bandoffsetSlider		-> setValue (800 * 1000 / workingRate);
	   set_bandWidthOffset		(800 * 1000 / workingRate);
	}
	else
	if (s == "rtty") {
	   bandwidthSlider		-> setValue (1200 * 1000 / workingRate);
	   set_bandWidthSelect		(1200 * 1000 / workingRate);
	   bandoffsetSlider		-> setValue (800 * 1000 / workingRate);
	   set_bandWidthOffset		(800 * 1000 / workingRate);
	}
	else
	if (s == "sstv") {
	   bandwidthSlider		-> setValue (3000 * 1000 / workingRate);
	   set_bandWidthSelect		(3000 * 1000 / workingRate);
	   bandoffsetSlider		-> setValue (1500 * 1000 / workingRate);
	   set_bandWidthOffset		(1500 * 1000 / workingRate);
	}
	else {				/* mode = small	*/
	   bandwidthSlider		-> setValue (500 * 1000 / workingRate);
	   set_bandWidthSelect		(500 * 1000 / workingRate);
	   bandoffsetSlider		-> setValue (0 * 1000 / workingRate);
	   set_bandWidthOffset		(0 * 1000 / workingRate);
	}
}

void	RadioInterface::updateTimeDisplay () {
QDateTime	currentTime = QDateTime::currentDateTime ();
	timeDisplay	-> setText (currentTime.
	                            toString (QString ("dd.MM.yy:hh:mm:ss")));
}

void	RadioInterface::lcdTimeout () {
	Panel		= 0;			// throw away anything
	lcdFrequency	-> display (currentFrequency);
}
//
//
void	RadioInterface::set_StreamOutSelector (int idx) {
	if (idx == 0)
	   return;

	outputDevice = outTable [idx];
	if (!mySoundcardOut -> isValidDevice (outputDevice)) 
	   return;

	mySoundcardOut	-> stopWriter	();
	if (!mySoundcardOut -> selectDevice (outputDevice)) {
	   QMessageBox::warning (this, tr ("sdr"),
	                               tr ("Selecting  input stream failed\n"));
	   return;
	}

	qWarning () << "selected output device " << idx << outputDevice;
}
//
//	do not forget that ocnt starts with 1, due
//	to Qt list conventions
bool	RadioInterface::setupSoundOut (QComboBox	*streamOutSelector,
	                               paWriter		*mySoundcardOut,
	                               int32_t		cardRate,
	                               int16_t		*table) {
uint16_t	ocnt	= 1;
uint16_t	i;

	for (i = 0; i < mySoundcardOut -> numberofDevices (); i ++) {
	   qDebug ("Investigating Device %d\n", i);
	   const char *so =
	             mySoundcardOut -> outputChannelwithRate (i, cardRate);

	   if (so != NULL) {
	      streamOutSelector -> insertItem (ocnt, so, QVariant (i));
	      table [ocnt] = i;
	      qDebug (" (output):item %d wordt stream %d\n", ocnt , i);
	      ocnt ++;
	   }
	}

	qDebug () << "added items to combobox";
	return ocnt > 1;
}
//
//	Pressing the various "dump" buttons
//	toggle the HFdump button
void RadioInterface::switchHFDump (void) {
SF_INFO *sf_info = (SF_INFO *)alloca (sizeof (SF_INFO));
	if (theDevice == NULL)	// just ignore, bur should not happen
	   return;

	if (hf_dumpFile != NULL) {
	   sf_close (hf_dumpFile);
	   hf_dumpFile	= NULL;
	   HFdumpButton	-> setText ("hf dump");
	   return;
	}

	QString fileName = QFileDialog::getSaveFileName (this,
	                                        tr ("open file .."),
	                                        QDir::homePath (),
	                                        tr ("Sound (*.wav)"));
	fileName	= QDir::toNativeSeparators (fileName);
	sf_info		-> samplerate	= inputRate;
	sf_info		-> channels	= 2;
	sf_info		-> format	= SF_FORMAT_WAV | SF_FORMAT_PCM_24;

	hf_dumpFile	= sf_open (fileName. toLatin1 (). data (),
	                                   SFM_WRITE, sf_info);
	if (hf_dumpFile == NULL) {
	   qDebug () << "Cannot open " << fileName. toLatin1 (). data ();
	   return;
	}
	HFdumpButton	-> setText ("WRITING");
}
//
//
void	RadioInterface::switchIFDump (void) {
SF_INFO *sf_info = (SF_INFO *)alloca (sizeof (SF_INFO));

	if (if_dumpFile != NULL) {
	   if_dumpFile	= NULL;
	   IFdumpButton	-> setText ("if dump");
	   return;
	}

	QString fileName = QFileDialog::getSaveFileName (this,
	                                        tr ("open file .."),
	                                        QDir::homePath (),
	                                        tr ("Sound (*.wav)"));
	fileName	= QDir::toNativeSeparators (fileName);
	sf_info		-> samplerate	= workingRate;
	sf_info		-> channels	= 2;
	sf_info		-> format	= SF_FORMAT_WAV | SF_FORMAT_PCM_24;

	if_dumpFile	= sf_open (fileName. toLatin1 (). data (),
	                                   SFM_WRITE, sf_info);
	if (if_dumpFile == NULL) {
	   qDebug () << "Cannot open " << fileName. toLatin1 (). data ();
	   return;
	}

	IFdumpButton	-> setText ("WRITING");
}
//
//
void RadioInterface::switchLFDump (void) {
SF_INFO *sf_info = (SF_INFO *)alloca (sizeof (SF_INFO));

	if (lf_dumpFile != NULL) {
	   sf_close (lf_dumpFile);
	   lf_dumpFile	= NULL;
	   LFdumpButton	-> setText ("lf dump");
	   return;
	}

	QString fileName = QFileDialog::getSaveFileName (this,
	                                        tr ("open file .."),
	                                        QDir::homePath (),
	                                        tr ("Sound (*.wav)"));
	fileName	= QDir::toNativeSeparators (fileName);
	sf_info		-> samplerate	= audioRate;
	sf_info		-> channels	= 2;
	sf_info		-> format	= SF_FORMAT_WAV | SF_FORMAT_PCM_24;

	lf_dumpFile	= sf_open (fileName. toLatin1 (). data (),
	                                   SFM_WRITE, sf_info);
	if (lf_dumpFile == NULL) {
	   qDebug () << "Cannot open " << fileName. toLatin1 (). data ();
	   return;
	}
	LFdumpButton	-> setText ("WRITING");
}
//
//	Handling device switching is delegated to the deviceLoader
//	Changing a "device" is so fundamental for the operation
//	of the front end, that we just delete lots of instances and
//	create new ones.
//	Note that all interface functions are handled by
//	the GUI thread, so while we are busy here, no other
void	RadioInterface::set_deviceSelect (const QString &s) {

	theDevice	-> exit ();
	mySoundcardOut	-> stopWriter	();
	runMode	= IDLE;		// will ensure that we have to push "start"
//
//	We started with a dummy device, so there is always
//	a device to unload
	my_deviceLoader		-> unloadDevice ();
	theDevice		= my_deviceLoader -> loadDevice (s, inputRate);

//	We do not know anything about the rig, other than that
//	it implements the abstract interface.
//	The cardreader based interfaces might use "set_changeRate",
//	Extio interfaces, i.e. Winrad compatible dll's, may
//	give more signals
	bitDepth	= theDevice	-> bitDepth ();
	connect (theDevice, SIGNAL (set_ExtFrequency (int)),
	         this, SLOT (set_ExtFrequency (int)));
	connect (theDevice, SIGNAL (set_ExtLO (int)),
	         this, SLOT (set_ExtLO (int)));
	connect (theDevice, SIGNAL (set_lockLO (void)),
	         this, SLOT (set_lockLO (void)));
	connect (theDevice, SIGNAL (set_unlockLO (void)),
	         this, SLOT (set_unlockLO (void)));
	connect (theDevice, SIGNAL (set_stopHW (void)),
	         this, SLOT (set_stopHW (void)));
	connect (theDevice, SIGNAL (set_startHW (void)),
	         this, SLOT (set_startHW (void)));
	connect (theDevice, SIGNAL (set_changeRate (int)),
	         this, SLOT (set_inputRate (int)));
	connect (theDevice, SIGNAL (samplesAvailable (int)),
	         this, SLOT (sampleHandler (int)));
//
//	we create a new scope since the new device probably supports
//	a different X-axis
	delete	hfScope;
	hfScope			= new fft_scope (hfscope,
	                                         displaySize,
	                                         Khz (1),
	                                         rasterSize,
	                                         4096,
	                                         theDevice	-> getRate (),
	                                         10);
	connect (hfScope,
	         SIGNAL (clickedwithLeft (int)),
	         this,
	         SLOT (AdjustFrequencywithKHz (int)));
	hfScope		-> setNeedle (inputRate / 4);
	set_HFplotterView	(HFplotterView	-> currentText ());
//	and we restore some settings, directly from the GUI settings
	set_Attenuator		(attenuationSlider	-> value ());
	set_BalanceSlider	(balanceSlider		-> value ());
	set_InputMode		(inputModeSelect	-> currentText ());
	set_AGCMode		(AGC_select		-> currentText ());
	set_agcThresholdSlider	(agc_ThresholdSlider	-> value ());
	set_symbolLeftshift	(symbol_shifter		-> value ());
	set_bandWidthOffset	(bandoffsetSlider 	-> value ());
	set_bandWidthSelect	(bandwidthSlider	-> value ());
	tunedIF		= setFrequency (theDevice -> defaultFrequency ());
	hfScope		-> setBitDepth	(bitDepth - 1);
	lfScope		-> setBitDepth	(bitDepth - 1);
//	qDebug() << "new processor for " << s << endl;
	inputRate		= theDevice -> getRate ();
	lcd_SampleRate		-> display (inputRate);
}
//
//	we delegate selecting and loading a decoder
//	to the decoderLoader. Pattern is similar to that of the
//	rigLoader
void	RadioInterface::set_decoderSelect (const QString &s) {
	my_decoderLoader	-> unloadDecoder ();
	theDecoder	= my_decoderLoader -> loadDecoder (s, workingRate);
	if (theDecoder == NULL) 	// should not happen
	   return;
	setDetectorMarker (theDecoder -> detectorOffset ());
	connect (theDecoder, SIGNAL (setDetectorMarker (int)),
	         this, SLOT (setDetectorMarker (int)));
	connect (theDecoder, SIGNAL (outputSample (float, float)),
	         this, SLOT (outputSample (float, float)));
}

void	RadioInterface::set_mouseIncrement (int inc) {
	mouseIncrement = inc;
}

void	RadioInterface::wheelEvent (QWheelEvent *e) {
	inc_Tuner ((e -> delta () > 0) ?  mouseIncrement : -mouseIncrement);
}

//	... and when there are hf samples available for	display 
/*
 *	there is a tremendous amount of signal/slot connections
 *	The local connects, knobs, sliders and displays,
 *	are connected here.
 */
void	RadioInterface::localConnects (void) {
	connect (pauseButton, SIGNAL (clicked ()),
	              this, SLOT (clickPause ()));
	connect (startButton, SIGNAL (clicked ()),
	              this, SLOT (set_Start ()));
	connect (streamOutSelector, SIGNAL (activated (int)),
	              this, SLOT (set_StreamOutSelector (int)));
	connect (QuitButton, SIGNAL (clicked ()),
	              this, SLOT (TerminateProcess ()));
	connect (symbol_shifter, SIGNAL (valueChanged (int)),
	              this, SLOT (set_symbolLeftshift (int)));
/*
 */
	connect (workingrateSelect, SIGNAL (activated(const QString&) ),
	              this, SLOT (set_workingRate (const QString&) ) );

	connect (rigSelectButton, SIGNAL (activated (const QString &)),
	              this, SLOT (set_deviceSelect (const QString &)));
	connect (decoderSelectButton, SIGNAL (activated (const QString &)),
	              this, SLOT (set_decoderSelect (const QString &)));
//
	connect (HFdumpButton, SIGNAL (clicked ()),
	              this, SLOT (switchHFDump (void)));
	connect (IFdumpButton, SIGNAL (clicked ()),
	              this, SLOT (switchIFDump (void)));
	connect (LFdumpButton, SIGNAL (clicked (void)),
	              this, SLOT (switchLFDump (void)));
	connect (HFplotterView, SIGNAL (activated (const QString &)),
	              this, SLOT (set_HFplotterView (const QString &)));
	connect (LFplotterView, SIGNAL (activated (const QString &)),
	              this, SLOT (set_LFplotterView (const QString &)));

	connect (HFspectrumamplitudeSlider, SIGNAL (valueChanged (int)),
	              this, SLOT (set_HFspectrumAmplitude (int)));
	connect (LFspectrumamplitudeSlider, SIGNAL (valueChanged (int)),
	              this, SLOT (set_LFspectrumAmplitude (int)));
	connect (attenuationSlider, SIGNAL (valueChanged (int) ),
		      this, SLOT (set_Attenuator (int) ) );
	connect (balanceSlider, SIGNAL (valueChanged (int) ),
		      this, SLOT (set_BalanceSlider (int) ) );
	connect (bandwidthSlider, SIGNAL (valueChanged(int) ),
	              this, SLOT (set_bandWidthSelect(int) ) );
	connect (bandoffsetSlider, SIGNAL (valueChanged (int)),
	              this, SLOT (set_bandWidthOffset (int)));
/*
 *	Mode setters
 */
	connect (AGC_select,      SIGNAL (activated(const QString&) ),
	              this, SLOT (set_AGCMode (const QString&) ) );
	connect (agc_ThresholdSlider, SIGNAL (valueChanged (int)),
	              this, SLOT (set_agcThresholdSlider (int)));
	connect (inputModeSelect, SIGNAL (activated(const QString&) ),
	              this, SLOT (set_InputMode (const QString&) ) );
	connect (quickbandSelect, SIGNAL (activated(const QString&) ),
	              this, SLOT (set_quickBand(const QString&) ) );
	connect (mouseIncrementSlider, SIGNAL (valueChanged (int)),
	              this, SLOT (set_mouseIncrement (int)));
/*
 *	connections for the console, first the digits
 */
	connect (add_one,   SIGNAL (clicked() ), this, SLOT (addOne() ) );
	connect (add_two,   SIGNAL (clicked() ), this, SLOT (addTwo() ) );
	connect (add_three, SIGNAL (clicked() ), this, SLOT (addThree() ) );
	connect (add_four,  SIGNAL (clicked() ), this, SLOT (addFour() ) );
	connect (add_five,  SIGNAL (clicked() ), this, SLOT (addFive() ) );
	connect (add_six,   SIGNAL (clicked() ), this, SLOT (addSix() ) );
	connect (add_seven, SIGNAL (clicked() ), this, SLOT (addSeven() ) );
	connect (add_eight, SIGNAL (clicked() ), this, SLOT (addEight() ) );
	connect (add_nine,  SIGNAL (clicked() ), this, SLOT (addNine() ) );
	connect (add_zero,  SIGNAL (clicked() ), this, SLOT (addZero() ) );
/*
 *	function buttons
 */
	connect (setFrequencyinMiddle, SIGNAL (clicked (void)),
	                      this, SLOT (set_InMiddle (void)));
	connect (HzSelector,  SIGNAL (clicked (void) ),
	                      this, SLOT (AcceptFreqinHz (void) ) );
	connect (khzSelector, SIGNAL (clicked (void)),
	                      this, SLOT (AcceptFreqinKhz (void)));
	connect (add_correct, SIGNAL (clicked (void)),
	                      this, SLOT (addCorr (void) ) );
	connect (add_clear,   SIGNAL (clicked (void)),
	                      this, SLOT (addClear (void) ) );
/*
 *	The increment and decrement buttons
 */
	connect (dec_5,	   SIGNAL (clicked() ), this, SLOT (decT5() ) );
	connect (dec_50,   SIGNAL (clicked() ), this, SLOT (decT50() ) );
	connect (dec_500,  SIGNAL (clicked() ), this, SLOT (decT500() ) );
	connect (inc_5,	   SIGNAL (clicked() ), this, SLOT (incT5() ) );
	connect (inc_50,   SIGNAL (clicked() ), this, SLOT (incT50() ) );
	connect (inc_500,  SIGNAL (clicked() ), this, SLOT (incT500() ) );
}

//
void	RadioInterface::setup_deviceLoader (void) {
QString	deviceBase;

//	Device handling is delegated to a separate Loader.
//	default is a local directory
	deviceBase	= QDir::currentPath ();
	QString defaultBase
	                = QDir::currentPath (). append ("/input-plugins-sw");
	defaultBase	= QDir::toNativeSeparators (defaultBase);
	if (!QDir (defaultBase). exists ())
	   defaultBase = QDir::currentPath ();
	deviceBase	= mainSettings	-> value ("deviceBase", 
	                                defaultBase). toString ();
	deviceBase	= QDir::toNativeSeparators (deviceBase);
	fprintf (stderr, "devicebase = %s\n",
	                    deviceBase. toLatin1 (). data ());
	my_deviceLoader	= new deviceLoader (rigSelectButton,
	                                    deviceBase,
	                                    rigFrame,
	                                    mainSettings);
	QDir devicePlugdir (deviceBase);
	my_deviceLoader	-> setup_deviceTable (devicePlugdir);
}
//
//	The setup for the decoderloader is similar:
void	RadioInterface::setup_decoderLoader	(void) {
QString	decoderBase;
QString	defaultBase	= QDir::currentPath (). append ("/decoder-plugins");
	defaultBase	= QDir::toNativeSeparators (defaultBase);
	if (!QDir (defaultBase). exists ())
	   defaultBase = QDir::currentPath ();
	decoderBase	= mainSettings	-> value ("decoderBase",
	                                           defaultBase). toString ();
	decoderBase	= QDir::toNativeSeparators (decoderBase);
	theDecoder		= NULL;
	my_decoderLoader	= new decoderLoader (decoderSelectButton,
	                                             decoderBase,
	                                             decoderFrame,
	                                             mainSettings);
	QDir decoderPlugdir (decoderBase);
	my_decoderLoader -> setup_decoderTable (decoderPlugdir);
}
//
/////////////////////////////////////////////////////////////////////
//
//	Sometimes, requests for frequency (or VFO) changes are
//	from an external source, i.e. an "intelligent" rig
//	in that case we apply simpler functions
void	RadioInterface::set_ExtFrequency	(int newFreq) {
	if (!lockLO) 
	   tunedIF	= setFrequency (newFreq);
}

void	RadioInterface::set_ExtLO	(int newLO) {
	if (!lockLO) 
	   tunedIF	= setFrequency (newLO);
}

void	RadioInterface::set_lockLO	(void) {
	lockLO	= true;
}

void	RadioInterface::set_unlockLO	(void) {
	lockLO	= false;
}

void	RadioInterface::set_stopHW	(void) {
	if (runMode == RUNNING)
	   clickPause ();
}

void	RadioInterface::set_startHW	(void) {
	if (runMode == PAUSED)
	   clickPause ();
}

//
////////////////////////////////////////////////////////////////
void	RadioInterface::stop_Dumping (void) {
	if (hf_dumpFile != NULL) {
	   sf_close (hf_dumpFile);
	   hf_dumpFile	= NULL;
	}
	if (if_dumpFile != NULL) {
	   sf_close (if_dumpFile);
	   if_dumpFile	= NULL;
	}
	if (lf_dumpFile != NULL) {
	   sf_close (lf_dumpFile);
	   lf_dumpFile = NULL;
	}
}
//
//	abstraction for setting up the sound cards
//	Note that since version 6, we only set up soundcard out
//	The rigs are responsible for soundcard in (if they need it)
void	RadioInterface::setup_soundcards	(void) {
int16_t	i;
bool	soundSeemsOk;

	mySoundcardOut		= new paWriter (audioRate, 10);
	outTable		= new int16_t
	                           [mySoundcardOut -> numberofDevices () + 1];
	for (i = 0; i < mySoundcardOut -> numberofDevices (); i ++)
	   outTable [i] = -1;

	soundSeemsOk		= setupSoundOut (streamOutSelector,
	                                         mySoundcardOut,
	                                         audioRate,
	                                         outTable);
//	
	outputDevice		= mySoundcardOut-> invalidDevice ();
}
//
bool	RadioInterface::fitsinDisplay (int32_t freq) {
int32_t	vfo	= theDevice -> getVFOFrequency ();
int32_t	bandLow	= getbandLow ();
int32_t	bandHigh= getbandHigh ();
int32_t	lborder	= freq - vfo + bandLow;
int32_t	hborder	= freq - vfo + bandHigh;

	return  (lborder > - inputRate / 2 + Khz (5)) &&
	        (hborder < inputRate / 2 - Khz (5));
}
	   
int32_t	RadioInterface::setFrequency	(int32_t freq) {
int32_t	vfo	= theDevice -> getVFOFrequency ();
int32_t	tunedto;

	if (fitsinDisplay (freq)) {
	   hfScope	-> setNeedle	(freq - vfo);
	   set_Filters_hf (freq - vfo);
	   tunedto	= freq - vfo;
	}
	else {
	   theDevice	-> setVFOFrequency (freq - inputRate / 4);
	   hfScope	-> setZero	(freq - inputRate / 4);
	   hfScope	-> setNeedle    (inputRate / 4);
	   set_Filters_hf (inputRate / 4);
	   tunedto	= inputRate / 4;
	}

	lcdFrequency	-> display (freq);
	currentFrequency	= freq;
	return tunedto;
}

void	RadioInterface::showAgcGain		(float gain) {
	agc_Display	-> display (gain);
}

void	RadioInterface::showPeak (int i, double v) {
	(void)i;
	peakIndicator	-> display (v);
}
//
//	we only deal with integer upconversion
void	RadioInterface::outputSample	(float re, float im) {
DSPCOMPLEX b [10];
float	c [20];
int16_t	amount, i;

	if (theDecoder == NULL)
	   return;
	amount = convertRate (DSPCOMPLEX (re, im), b, theDecoder -> rateOut ());
	if (lf_dumpFile != NULL) {
	   for (i = 0; i < amount; i ++) {
	      c [2 * i] 	= real (b [i]);
	      c [2 * i + 1]	= imag (b [i]);
	   }
	   sf_writef_float (lf_dumpFile, c, amount);
	}
	mySoundcardOut	-> putSamples (b, amount);
}

//
//	Will be signaled from the deviceInterface
void	RadioInterface::sampleHandler (int amount) {
DSPCOMPLEX buffer [BUFFERSIZE];
DSPCOMPLEX v2	[BUFFERSIZE];
DSPCOMPLEX v3	[BUFFERSIZE];
float	dumpBuffer [2 * BUFFERSIZE];
int16_t	i, ifCount;

	while (theDevice -> Samples () >= BUFFERSIZE) {
	   theDevice -> getSamples (buffer, BUFFERSIZE, inputMode);
	   for (i = 0; i < BUFFERSIZE; i ++) {
	      v2 [i]	= theAttenuator -> shift_and_attenuate (buffer [i]);
	      hfScope	-> addElement (v2 [i]);
	      v2 [i]	= hfFilter	-> Pass (v2 [i]);
	      v2 [i]	= myOscillator	-> nextValue (tunedIF) * v2 [i];
	   }

	   if (hf_dumpFile != NULL) {
	      for (i = 0; i < BUFFERSIZE; i ++) {
	         dumpBuffer [2 * i] = real (buffer [i]);
	         dumpBuffer [2 * i + 1] = imag (buffer [i]);
	      }
	      sf_writef_float (hf_dumpFile, dumpBuffer, BUFFERSIZE);
	   }
//
//	end of HF processing
//	decimate and process if samples
	   ifCount	= ifResampler -> resample (v2, v3);
	   for (i = 0; i < ifCount; i ++) {
	      float agcGain;
	      v3 [i]	= lfFilter	-> Pass (v3 [i]);
	      agcGain	= agc		-> doAgc (v3 [i]);
	      v3 [i]	*= agcGain;
	      lfScope -> addElement (v3 [i]);
	      if (++delayCount > workingRate / 10) {
	         delayCount = 0;
	         showPeak (0, theMeter -> MeterValue (v3 [i]));
	         showAgcGain (agcGain);
	      }
	      else
	         (void)theMeter -> MeterValue (v3 [i]);
	      if (theDecoder != NULL)
	         theDecoder -> doDecode (v3 [i]);
	   }
	   if (if_dumpFile != NULL) {	// ifCount <= BUFFERSIZE
	      for (i = 0; i < ifCount; i ++) {
	         dumpBuffer [2 * i] = real (buffer [i]);
	         dumpBuffer [2 * i + 1] = imag (buffer [i]);
	      }
	      sf_writef_float (if_dumpFile, dumpBuffer, ifCount);
	   }
	   amount -= BUFFERSIZE;
	}
}

//	For filters we need a reasonable "bandLow" and "bandHigh"
//	relative to the selected frequency
void	RadioInterface::set_Filters_hf	(int32_t ifFreq) {
int32_t	filterLow, filterHigh;

	filterLow	= ifFreq + bandLow;
	filterHigh	= ifFreq + bandHigh;
	if (filterLow < - inputRate / 2)
	   filterLow = - inputRate / 2;
	if (filterHigh > inputRate / 2)
	   filterHigh = inputRate / 2;
	hfFilter	-> setBand (filterLow, filterHigh, inputRate);
}

void	RadioInterface::set_Filters_if	(int32_t ifFreq) {
	(void) ifFreq;
	lfFilter	-> setBand (bandLow, bandHigh, workingRate);
}


int32_t	RadioInterface::getbandHigh		(void) {
	return bandHigh;
}

int32_t	RadioInterface::getbandLow		(void) {
	return bandLow;
}
//
//
int16_t	RadioInterface::convertRate	(DSPCOMPLEX	s,
	                                 DSPCOMPLEX	*out,
	                                 int32_t	inrate) {
static	int32_t oldinrate = 0;
static	LowPassFIR	*theFilter = NULL;
int16_t	i;

	if (inrate != oldinrate) {
	   if (theFilter != NULL)
	      delete theFilter;
	   theFilter	= new LowPassFIR (11, inrate / 2, audioRate);
	   oldinrate = inrate;
	}

	out [0] = theFilter -> Pass (s);
	for (i = 1; i < audioRate / inrate; i ++) 
	   out [i] = theFilter -> Pass (DSPCOMPLEX (0, 0));

	return audioRate / inrate;
}

