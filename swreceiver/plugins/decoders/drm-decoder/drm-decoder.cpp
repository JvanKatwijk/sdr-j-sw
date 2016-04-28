#
/*
 *    Copyright (C) 2013
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
#
#include	<QSettings>
#include	<QFrame>
#include	<QWidget>
#include	<QSettings>
#include	<QMessageBox>
#include	<cstring>
#include	"drm-decoder.h"
#include	"utilities.h"
#include	"basics.h"
#include	"frame-processor.h"
#include	"iqdisplay.h"
#include	"math.h"
#include	"local-scope.h"
/*
 */
QWidget	*drmDecoder::createPluginWindow (int32_t rate, QSettings *s) {
QString	temp;
int16_t	i;
int16_t	symbs;

	theRate		= rate;
	drmSettings	= s;
	myFrame		= new QFrame;
	setupUi (myFrame);
	fprintf (stderr, "drm decoder is loaded\n");
	validFlag	= false;
	decimatorFlag	= false;
	timeSyncLabel	-> setStyleSheet ("QLabel {background-color:red}");
	facSyncLabel	-> setStyleSheet ("QLabel {background-color:red}");
	sdcSyncLabel	-> setStyleSheet ("QLabel {background-color:red}");
	faadSyncLabel	-> setStyleSheet ("QLabel {background-color:red}");
	if (rate != 12000 && rate != 24000) {
	   QMessageBox::warning (myFrame, tr ("sdr"),
	                        tr ("Will only work with 12 or 24 k input"));
	   theRate = 12000;
	}

	channel_1	-> hide ();
	channel_2	-> hide ();
	channel_3	-> hide ();
	channel_4	-> hide ();
	drmSettings	-> beginGroup ("drmDecoder");
	symbs		= drmSettings -> value ("searchLength", 15). toInt ();
	int8_t windowDepth
	                = drmSettings -> value ("windowDepth", 2). toInt ();
	int8_t qam64Roulette
	                = drmSettings -> value ("qam64Roulette", 6). toInt ();
	drmSettings	-> endGroup ();
	validFlag	= true;
	myIQDisplay	= new IQDisplay (iqDisplay, 2 * 322);
	myScope		= new localScope (iqDisplay, 300, 20);
	Y_values	= new double *[20];
	for (i = 0; i < 20; i ++)
	   Y_values [i] = new double [300];
	currentS	= 0;
	buffer		= new RingBuffer<DSPCOMPLEX> (12000);
	connect (channel_1, SIGNAL (clicked (void)),
	         this, SLOT (selectChannel_1 (void)));
	connect (channel_2, SIGNAL (clicked (void)),
	         this, SLOT (selectChannel_2 (void)));
	connect (channel_3, SIGNAL (clicked (void)),
	         this, SLOT (selectChannel_3 (void)));
	connect (channel_4, SIGNAL (clicked (void)),
	         this, SLOT (selectChannel_4 (void)));
	channelSelector		= 1;
	running			= true;
	my_frameProcessor	= new frameProcessor (this,
	                                              buffer,
	                                              symbs,
	                                              windowDepth,
	                                              qam64Roulette);
#ifdef	ESTIMATOR_1
	drmName	-> setText ("drm 0.15 straight");
#elif	ESTIMATOR_2
	drmName	-> setText ("drm 0.15 arma-1");
#else
	drmName	-> setText ("drm 0.15 arma-2");
#endif
	my_frameProcessor	-> start	();
	return myFrame;
}

	decoderInterface::~decoderInterface	(void) {
}

	drmDecoder::~drmDecoder		(void) {
	running		= false;
	my_frameProcessor -> stop ();
	while (my_frameProcessor -> isRunning ())
	   usleep (10);
	delete my_frameProcessor;
	delete	buffer;
	delete	myFrame;
}

int32_t	drmDecoder::rateOut	(void) {
	return 48000;
}

int16_t	drmDecoder::detectorOffset	(void) {
	return 0;
}
//
void	drmDecoder::doDecode (DSPCOMPLEX z) {
	if (!validFlag)
	   return;

	if (theRate != 24000 && theRate != 12000)
	   return;

	if ((theRate == 24000) && decimatorFlag) {
	   decimatorFlag = false;
	   return;
	}

	decimatorFlag = true;
	buffer	-> putDataIntoBuffer (&z, 1);
}

bool	drmDecoder::initforRate	(int workingRate) {
	if (theRate == workingRate)
	   return true;
	if (workingRate != 12000 && workingRate != 24000) {
	   QMessageBox::warning (myFrame, tr ("sdr"),
	                        tr ("Will only work with 12 or 24 k input"));
	   return false;
	}
	theRate = workingRate;
	return true;
}

bool	drmDecoder::haltSignal		(void) {
	return !running;
}
//
void	drmDecoder::show_coarseOffset (float offset) {
	show_int_offset	-> display (offset);
}

void	drmDecoder::show_fineOffset (float offset) {
	show_small_offset -> display (offset);
}

void	drmDecoder::show_angle		(float angle) {
	angleDisplay	-> display (angle);
}

void	drmDecoder::show_timeOffset	(float offset) {
	timeOffsetDisplay	-> display (offset);
}

void	drmDecoder::show_timeDelay	(float del) {
	timeDelayDisplay	-> display (del);
}

void	drmDecoder::show_clockOffset	(float o) {
	clockOffsetDisplay	-> display (o);
}

void	drmDecoder::showMessage (QString m) {
	messageLabel -> setText (m);
}

void	drmDecoder::executeTimeSync	(bool f) {
	if (f)
	   timeSyncLabel -> setStyleSheet ("QLabel {background-color:green}");
	else {
	   timeSyncLabel -> setStyleSheet ("QLabel {background-color:red}");
	   faadSyncLabel -> setStyleSheet ("QLabel {background-color:red}");
	}
}

void	drmDecoder::executeFACSync	(bool f) {
	if (f)
	   facSyncLabel -> setStyleSheet ("QLabel {background-color:green}");
	else {
	   facSyncLabel -> setStyleSheet ("QLabel {background-color:red}");
	   faadSyncLabel -> setStyleSheet ("QLabel {background-color:red}");
	}
}

void	drmDecoder::executeSDCSync	(bool f) {
	if (f)
	   sdcSyncLabel -> setStyleSheet ("QLabel {background-color:green}");
	else {
	   sdcSyncLabel -> setStyleSheet ("QLabel {background-color:red}");
	   faadSyncLabel -> setStyleSheet ("QLabel {background-color:red}");
	}
}

void	drmDecoder::show_stationLabel (const QString &s) {
	stationLabel -> setText (s);
}

void	drmDecoder::execute_showMode		(int l) {
	if (1 <= l && l <= 4)
	   modeIndicator	-> setText (QString (char ('A' + (l - 1))));
}

void	drmDecoder::execute_showSpectrum	(int l) {
	if (0 <= l && l < 4)
	   spectrumIndicator	-> setText (QString (char ('0' + l)));
}

//
void	drmDecoder::showIQ (float x, float y) {
static	int	phaseCounter = 0;

float	phase	= phaseCounter / 322.0 * 2 * M_PI;
	myIQDisplay	-> DisplayIQ (DSPCOMPLEX (sqrt (2.0) * cos (phase),
	                                          sqrt (2.0) * sin (phase)),
	                              40.0);
	phaseCounter = (phaseCounter + 1) % 322;
	myIQDisplay	-> DisplayIQ (DSPCOMPLEX (x, y), 40.0);
}

void	drmDecoder::show_channels	(int audio, int data) {
	if (audio > 0)
	   channel_1	-> show ();
	if (audio > 1)
	   channel_2	-> show ();
	else
	   channel_2	-> hide ();
	if (audio == 0)
	   channel_1	-> hide ();

	if (data > 0)
	   channel_3	-> show ();
	if (data > 1)
	   channel_4	-> show ();
	else
	   channel_4	-> hide ();
	if (data == 0)
	   channel_3	-> hide ();
}

void	drmDecoder::show_frames		(int good, int bad) {
	goodFrames	-> display (good);
	badFrames	-> display (bad);
}

void	drmDecoder::show_audioMode	(QString s) {
	audioModelabel	-> setText (s);
}

void	drmDecoder::sampleOut		(float re, float im) {
	outputSample (re, im);
}

void	drmDecoder::newYvalue		(int n1, float z) {

	Y_values [currentS] [n1] = z;
}

void	drmDecoder::showEq		(int n) {
static	int oldN = 300;
int16_t	i, j;
	if (n != oldN) {
	   delete myScope;
	   myScope	= new localScope (iqDisplay, n, 20);
	   oldN = n;
	}
	myScope	-> Show (Y_values, 100);
	for (i = 1; i <= 19; i ++)
	   for (j = 0; j < 300; j ++)
	      Y_values [i - 1][j] = Y_values [i][j];
	if (currentS < 19) currentS ++;
}

void	drmDecoder::showSNR		(float snr) {
	snrDisplay	-> display (snr);
}

void	drmDecoder::selectChannel_1	(void) {
	channelSelector	= 1;
}

void	drmDecoder::selectChannel_2	(void) {
	channelSelector	= 2;
}

void	drmDecoder::selectChannel_3	(void) {
	channelSelector	= 3;
}

void	drmDecoder::selectChannel_4	(void) {
	channelSelector	= 4;
}

bool	drmDecoder::isSelectedChannel	(int16_t c) {
	return channelSelector == c;
}

void	drmDecoder::faadSuccess		(bool b) {
	if (b)
	   faadSyncLabel -> setStyleSheet ("QLabel {background-color:green}");
	else
	   faadSyncLabel -> setStyleSheet ("QLabel {background-color:red}");
}


#if QT_VERSION < 0x050000
QT_BEGIN_NAMESPACE
Q_EXPORT_PLUGIN2 (drmdecoder, drmDecoder)
QT_END_NAMESPACE
#endif

