#
/*
 *    Copyright (C) 2014
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

#include	<QThread>
#include	<QSettings>
#include	<QHBoxLayout>
#include	<QLabel>

//	the header of the Mirics library, for windows and linux there
//	are different versions
#ifndef	__MINGW32__
#include	"mirsdrapi-rsp.h"
#else
#include	"mir_sdr-windows.h"	
#endif
#include	"sdrplay.h"	// our header
#include	"sdrplay-worker.h"	// the worker
#include	"sdrplay-loader.h"	// funtion loader
//
//	Currently, we do not have lots of settings
QWidget	*sdrplay::createPluginWindow (int32_t rate, QSettings *s) {
int	err;
float	ver;
bool	success;

	(void)rate;
	this	-> sdrplaySettings	= s;
	deviceOK		= false;
	myFrame			= new QFrame;
	setupUi (myFrame);
	_I_Buffer		= NULL;
	theLoader		= NULL;
	outputRate		= rateSelector -> currentText (). toInt ();
	inputRate		= getInputRate (outputRate);
	rateDisplay	-> display (inputRate);
//
//	sometimes problems with dynamic linkage of libusb, it is
//	loaded indirectly through the dll
	if (libusb_init (NULL) < 0) {
	   fprintf (stderr, "libusb problem\n");	// should not happen
	   return myFrame;
	}
	libusb_exit (NULL);

	theLoader	= new sdrplayLoader (&success);
	if (!success) {
	   fprintf (stderr, " No success in loading sdrplay lib\n");
	   statusLabel	-> setText ("no SDRplay lib");
	   delete theLoader;
	   theLoader = NULL;
	   return myFrame;
	}

	err			= theLoader -> mir_sdr_ApiVersion (&ver);
//	Note that the versions for windows and linux are not the same,
//	we are using the Linux ".h" file, so when loading a windows
//	dll we had a problem.
	if (ver != MIR_SDR_API_VERSION) {
	   fprintf (stderr, "Foute API: %f, %f\n", ver, MIR_SDR_API_VERSION);
	   statusLabel	-> setText ("mirics error");
	}

	statusLabel	-> setText ("Loaded");
	api_version	-> display (ver);
	_I_Buffer	= new RingBuffer<DSPCOMPLEX>(2 * 1024 * 1024);
	vfoFrequency	= Khz (94700);
	vfoOffset	= 0;
	theWorker	= NULL;

	currentGain	= sdrplaySettings -> value ("sdrplay_externalGain",
	                                                     25). toInt ();
	externalGain	-> setValue (currentGain);
//
	connect (externalGain, SIGNAL (valueChanged (int)),
	         this, SLOT (setExternalGain (int)));
	connect (khzOffset, SIGNAL (valueChanged (int)),
	         this, SLOT (setKhzOffset (int)));
	connect (rateSelector, SIGNAL (activated (const QString &)),
	         this, SLOT (set_rateSelector (const QString &)));
	deviceOK	= true;
	return myFrame;
}
//
//	just a dummy
	rigInterface::~rigInterface	(void) {
}
//
//	that will allow a decent destructor
	sdrplay::~sdrplay	(void) {
	stopReader ();
	sdrplaySettings	-> setValue ("sdrplay_externalGain",
	                                          externalGain -> value ());
	sdrplaySettings	-> setValue ("sdrplay_khzOffset",
	                                          khzOffset -> value ());
	if (_I_Buffer != NULL)
	   delete _I_Buffer;
	if (theLoader != NULL)
	   delete theLoader;
}

int32_t	sdrplay::getRate	(void) {
	return outputRate;
}

static inline
int16_t	bankFor_dongle (int32_t freq) {
	if (freq < 60 * MHz (1))
	   return -1;
	if (freq < 120 * MHz (1))
	   return 1;
	if (freq < 245 * MHz (1))
	   return 2;
	if (freq < 420 * MHz (1))
	   return -1;
	if (freq < 1000 * MHz (1))
	   return 3;
	return -1;
}

static inline
int16_t bankFor_sdr (int32_t freq) {
	if (freq < 100 * Khz (1))
	   return -1;
	if (MIR_SDR_API_VERSION > 1.5) {
	   if (freq < 12 * Mhz (1))
	      return 1;
	   if (freq < 30 * Mhz (1))
	      return 2;
	   if (freq < 60 * Mhz (1))
	      return 3;
	   if (freq < 120 * Mhz (1))
	      return 4;
	   if (freq < 250 * Mhz (1))
	      return 5;
	   if (freq < 420 * Mhz (1))
	      return 6;
	   if (freq < 1000 * Mhz (1))
	      return 7;
	   if (freq < 2000 * Mhz (1))
	      return 8;
	   return - 1;
	}
	else {   // old API
	   if (freq < 12 * MHz (1))
	      return 1;
	   if (freq < 30 * MHz (1))
	      return 2;
	   if (freq < 60 * MHz (1))
	      return 3;
	   if (freq < 120 * MHz (1))
	      return 4;
	   if (freq < 250 * MHz (1))
	      return 5;
	   if (freq < 380 * MHz (1))
	      return 6;
	   if (freq < 420 * MHz (1))
	      return -1;
	   if (freq < 1000 * MHz (1))
	      return 7;
	   return -1;
	}
}

bool	sdrplay::legalFrequency (int32_t f) {
	return bankFor_sdr (f) != -1;
}

int32_t	sdrplay::defaultFrequency	(void) {
	return Khz (14070);
}

void	sdrplay::setVFOFrequency	(int32_t newFrequency) {
int32_t	realFreq = newFrequency + vfoOffset;

	if (!legalFrequency (realFreq) || !deviceOK)
	   return;

	if (theWorker == NULL) {
	   vfoFrequency = newFrequency + vfoOffset;
	   return;
	}

	if (bankFor_sdr (realFreq) != bankFor_sdr (vfoFrequency)) {
	   stopReader ();
	   vfoFrequency	= realFreq;
	   restartReader ();	// restart will set a frequency
	   return;
	}
	else
	   theWorker -> setVFOFrequency (realFreq);
	vfoFrequency = realFreq;
}

int32_t	sdrplay::getVFOFrequency	(void) {
	return vfoFrequency - vfoOffset;
}

void	sdrplay::setExternalGain	(int newGain) {
	if (newGain < 0 || newGain > 101)
	   return;

	if (theWorker != NULL)
	   theWorker -> setExternalGain (newGain);
	currentGain = newGain;
}

bool	sdrplay::restartReader	(void) {
bool	success;

	if ((theWorker != NULL) || !deviceOK)
	   return true;

	_I_Buffer	-> FlushRingBuffer ();
	theWorker	= new sdrplayWorker (inputRate,
	                                    outputRate,
	                                    getBandwidth (outputRate),
	                                    vfoFrequency,
	                                    theLoader,
	                                    _I_Buffer,
	                                    &success);
	if (success) {
	   theWorker -> setExternalGain (currentGain);
	   connect (theWorker, SIGNAL (samplesAvailable (int)),
	            this, SIGNAL (samplesAvailable (int)));
	}
	return success;
}

void	sdrplay::stopReader	(void) {
	if ((theWorker == NULL) || !deviceOK)
	   return;

	theWorker	-> stop ();
	while (theWorker -> isRunning ())
	   usleep (100);
	delete theWorker;
	theWorker = NULL;
}

void	sdrplay::exit		(void) {
	stopReader ();
}

//
//	The brave old getSamples. For the mirics stick, most of the
//	work is done in the "worker", before putting data into the 
//	buffer
int32_t	sdrplay::getSamples (DSPCOMPLEX *V, int32_t size, uint8_t Mode) { 
int32_t		amount, i;
DSPCOMPLEX	buf [size];

	if (!deviceOK)
	   return 0;

	amount = _I_Buffer	-> getDataFromBuffer (buf, size);
	for (i = 0; i < amount; i ++)  {
	   switch (Mode) {
	      default:
	      case IandQ:
	         V [i] = buf [i];
	         break;

	      case QandI:
	         V [i] = DSPCOMPLEX (imag (buf [i]), real (buf [i]));
	         break;

	      case Q_Only:
	         V [i] = DSPCOMPLEX (real (buf [i]), 0.0);
	         break;

	      case I_Only:
	         V [i] = DSPCOMPLEX (imag (buf [i]), 0.0);
	         break;
	   }
	}
	return amount;
}

int32_t	sdrplay::Samples	(void) {
	if (!deviceOK)
	   return 0;
	return _I_Buffer	-> GetRingBufferReadAvailable ();
}

//
//	vfoOffset is in Hz, we have two spinboxes influencing the
//	settings
void	sdrplay::setCorrection	(int k) {
	(void)k;
//	not yet implemented
}

void	sdrplay::setKhzOffset	(int k) {
	vfoOffset	= (vfoOffset / 1000) + Khz (k);
}

bool	sdrplay::isValidRate	(int32_t rate) {
	return rate >= Khz (960) && rate <= Khz (8000);
}

int32_t	sdrplay::getBandwidth	(int32_t rate) {
static int validWidths [] = {200, 300, 600, 1536, 5000, 6000, 7000, 8000, -1};
int16_t	i;
	for (i = 1; validWidths [i] != -1; i ++)
	   if (rate / Khz (1) < validWidths [i] &&
	       rate / Khz (1) >= validWidths [i - 1])
	      return validWidths [i - 1] * Khz (1);
	return false;
}
//
//	find a decent rate for the sdrplay
//	such that it can be decimated to rate
int32_t	sdrplay::getInputRate (int32_t rate) {
int32_t	temp;

	temp	= rate;
	while (temp < Khz (1500))
	   temp += rate;
	return temp;
}

void	sdrplay::set_rateSelector (const QString &s) {
int32_t	newRate	= s. toInt ();

	if (newRate == inputRate)
	   return;
	stopReader	();
	outputRate	= newRate;
	inputRate	= getInputRate (outputRate);
//
//	Note that restart reader will be done by the main program,
//	creating a new worker with the right parameters
//	and filters
	emit set_changeRate (outputRate);
}

int16_t	sdrplay::bitDepth	(void) {
	return 10;
}

#if QT_VERSION < 0x050000
QT_BEGIN_NAMESPACE
Q_EXPORT_PLUGIN2(sdrplay, sdrplay)
QT_END_NAMESPACE
#endif

