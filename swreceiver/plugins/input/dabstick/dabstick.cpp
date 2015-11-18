#
/*
 *    Copyright (C) 2010, 2011, 2012, 2013
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
 *
 * 	This particular driver is a very simple wrapper around the
 * 	librtlsdr. The librtlsdr is osmocom software and all rights
 * 	are greatly acknowledged
 *
 *	To keep things simple, we just load the librtlsdr library
 *	and wrap around it
 */

#define READLEN_DEFAULT 8192

#include	<QThread>
#include	"dabstick.h"
#include	<stdio.h>
#include	<QSettings>
//
//	The naming of functions for accessing shared libraries
//	differ between Linux and Windows
#ifdef __MINGW32__
#define	GETPROCADDRESS	GetProcAddress
#else
#define	GETPROCADDRESS	dlsym
#endif
//
//	for handling the events in libusb, we need a controlthread
//	whose sole purpose is to process the rtlsdr_read_async function
//	from the lib.

	dll_driver::dll_driver (dabStick *d) {
	theStick	= d;
	start ();
}

	dll_driver::~dll_driver (void) {
}
//	For the callback, we do need some environment 
//	This is the user-side call back function
void	dll_driver::RTLSDRCallBack (unsigned char *buf,
	                            uint32_t len, void *ctx) {
dabStick	*theStick = (dabStick *)ctx;

	if ((len != READLEN_DEFAULT) || (theStick == NULL))
	   return;
//	Note that our "context"  is the stick
	theStick-> _I_Buffer -> putDataIntoBuffer (buf, len);
	if (theStick -> _I_Buffer -> GetRingBufferReadAvailable () >
	   theStick -> inputRate / 10)
	   emit theStick -> samplesAvailable (theStick -> inputRate / 10);
}

void	dll_driver::run (void) {
	(theStick -> rtlsdr_read_async) (theStick -> device,
	                          (rtlsdr_read_async_cb_t)&RTLSDRCallBack,
	                          (void *)theStick,
	                          0,
	                          READLEN_DEFAULT);
}
//
//	Our wrapper is a simple classs
//	to keep things simple, we just load the osmocom function
//	dynamically
#include	<QHBoxLayout>
#include	<QLabel>

QWidget	*dabStick::createPluginWindow (int32_t rate,
	                               QSettings *s) {
	(void)rate;
	this	-> dabSettings	= s;
	myFrame			= new QFrame;
	setupUi	(myFrame);

	outputRate		= rateSelector -> currentText (). toInt ();
	inputRate		= getInputRate (outputRate);
	gains			= NULL;
//	the connects
	setupDevice (inputRate, outputRate);
//	The device itself generates the data, so the reader is
//	just doing nothing
	return myFrame;
}

	rigInterface::~rigInterface	(void) {
}

	dabStick::~dabStick	(void) {
	if (open && libraryLoaded && dabSettings != NULL) {
	   dabSettings	-> beginGroup ("dabstick");
	   dabSettings	-> setValue ("dab_externalGain",
	                                             gainSlider -> value ());
	   dabSettings	-> setValue ("dab_correction",
	                                             f_correction -> value ());
	   dabSettings	-> setValue ("dab_khzOffset", khzOffset -> value ());
	   dabSettings	-> setValue ("dab_hzOffset", hzOffset -> value ());
	   dabSettings	-> endGroup ();
	}

	if (open && libraryLoaded) 
	   stopReader ();
	if (open) 
	   rtlsdr_close (device);

	if (libraryLoaded)
#ifdef __MINGW32__
	   FreeLibrary (Handle);
#else
	   dlclose (Handle);
#endif
	if (_I_Buffer != NULL)
	   delete _I_Buffer;
	if (d_filter != NULL)
	   delete d_filter;
	d_filter	= NULL;
	delete	myFrame;
	if (gains != NULL)
	   delete gains;
}

int32_t	dabStick::getRate	(void) {
	return outputRate;
}

void	dabStick::setupDevice (int32_t rateIn, int32_t rateOut) {
int	r;
int16_t	deviceIndex;
int16_t	i;
//
	libraryLoaded	= false;
	workerHandle	= NULL;
	d_filter	= NULL;
	_I_Buffer	= NULL;
	open		= false;

	statusLabel	-> setText ("setting up");
#ifdef	__MINGW32__
	Handle		= LoadLibrary ((wchar_t *)L"rtlsdr.dll");
#else
	Handle		= dlopen ("librtlsdr.so", RTLD_NOW);
#endif
	if (Handle == NULL) {
	   fprintf (stderr, "Failed to open rtlsdr.dll\n");
	   statusLabel	-> setText ("no rtlsdr lib");
	   return;
	}

	libraryLoaded	= true;
	fprintf (stderr, "rtlsdr is loaded\n");

	if (!load_rtlFunctions	()) {
	   statusLabel	-> setText ("load failed");
	   goto err;
	}

//	vfoFrequency is the VFO frequency
	vfoFrequency		= KHz (94700);	// just a dummy
	vfoOffset		= 0;
	deviceCount 		= (this -> rtlsdr_get_device_count) ();
	if (deviceCount == 0) {
	   statusLabel	-> setText ("no device");
	   fprintf (stderr, "No devices found\n");
	   goto err;
	}

	deviceIndex = 0;	// default
	if (deviceCount > 1) {
	   dongleSelector	= new dongleSelect ();
	   for (deviceIndex = 0; deviceIndex < deviceCount; deviceIndex ++) {
	      dongleSelector ->
	           addtoDongleList (rtlsdr_get_device_name (deviceIndex));
	   }
	   deviceIndex = dongleSelector -> QDialog::exec ();
	   delete dongleSelector;
	}
//
//	OK, now open the hardware
	r			= this -> rtlsdr_open (&device, deviceIndex);
	if (r < 0) {
	   fprintf (stderr, "Opening dabstick failed\n");
	   statusLabel	-> setText ("opening failed");
	   goto err;
	}

	open	= true;
	r	= (this -> rtlsdr_set_sample_rate) (device, rateIn);
	if (r < 0) {
	   fprintf (stderr, "Setting samplerate failed\n");
	   statusLabel	-> setText ("samplerate");
	   goto err;
	}

	gainsCount = rtlsdr_get_tuner_gains (device, NULL);
	fprintf(stderr, "Supported gain values (%d): ", gainsCount);
	gains		= new int [gainsCount];
	gainsCount = rtlsdr_get_tuner_gains (device, gains);
	for (i = 0; i < gainsCount; i++)
	   fprintf(stderr, "%.1f ", gains [i] / 10.0);
	rtlsdr_set_tuner_gain_mode (device, 1);
	rtlsdr_set_tuner_gain (device, gains [gainsCount / 2]);
	gainSlider	-> setMaximum (gainsCount);
	gainSlider	-> setValue (8);
	_I_Buffer	= new RingBuffer<uint8_t>(1024 * 1024);
	workerHandle	= NULL;

	d_filter	= new DecimatingFIR (rateIn / rateOut * 5 - 1,
	                                     - rateOut / 2,
	                                     rateOut / 2,
	                                     rateIn,
	                                     rateIn / rateOut);
	connect (gainSlider, SIGNAL (valueChanged (int)),
	         this, SLOT (setExternalGain (int)));
	connect (f_correction, SIGNAL (valueChanged (int)),
	         this, SLOT (setCorrection (int)));
	connect (khzOffset, SIGNAL (valueChanged (int)),
	         this, SLOT (setKhzOffset (int)));
	connect	(hzOffset, SIGNAL (valueChanged (int)),
	         this, SLOT (setHzOffset (int)));
	connect (rateSelector, SIGNAL (activated (const QString &)),
	         this, SLOT (set_rateSelector (const QString &)));
	connect (checkAgc, SIGNAL (stateChanged (int)),
	         this, SLOT (setAgc (int)));
	if (dabSettings != NULL) {
	   dabSettings	-> beginGroup ("dabStick");
	   int	k	= dabSettings	-> value ("dab_externalGain", 24). toInt ();
	   gainSlider	-> setValue (k);
	   k		= dabSettings	-> value ("dab_correction", 0). toInt ();
	   f_correction	-> setValue (k);
	   k		= dabSettings	-> value ("dab_khzOffset", 0). toInt ();
	   khzOffset	-> setValue (k);
	   k		= dabSettings	-> value ("dab_hzOffset", 0). toInt ();
	   hzOffset		-> setValue (k);
	   dabSettings	-> endGroup ();
	}
	statusLabel	-> setText ("Loaded");
	return;
err:
	if (open)
	   rtlsdr_close (device);
#ifdef __MINGW32__
	FreeLibrary (Handle);
#else
	dlclose (Handle);
#endif
	libraryLoaded	= false;
	open		= false;
	return;
}
//
//
int32_t	dabStick::getVFOFrequency	(void) {
	if (!open || !libraryLoaded)
	   return defaultFrequency ();
	return (int32_t)rtlsdr_get_center_freq (device) - vfoOffset;
}

//
//	The external world sets a virtual VFO frequency
//	The real VFO frequency can be influences by an externally
//	set vfoOffset
void	dabStick::setVFOFrequency	(int32_t f) {
	if (!open || !libraryLoaded)
	   return;

	vfoFrequency	= f;
	(void)rtlsdr_set_center_freq (device, f + vfoOffset);
//fprintf (stderr, "request to set on %d, offset = %d, actual set to %d\n",
//	                  f, vfoOffset, (int32_t)rtlsdr_get_center_freq (device));
}

bool	dabStick::legalFrequency	(int32_t f) {
	(void)f;
	return true;
}

int32_t	dabStick::defaultFrequency	(void) {
	return KHz (94700);
}
//
//
bool	dabStick::restartReader	(void) {
int32_t	r;

	if (!open ||!libraryLoaded)
	   return false;

	if (workerHandle != NULL)	// running already
	   return true;

	r = rtlsdr_reset_buffer (device);
	if (r < 0)
	   return false;

	rtlsdr_set_center_freq (device, vfoFrequency + vfoOffset);
	workerHandle	= new dll_driver (this);
	return true;
}

void	dabStick::stopReader	(void) {
	if (workerHandle == NULL)
	   return;

	rtlsdr_cancel_async (device);
	if (workerHandle != NULL) {
	   while (!workerHandle -> isFinished ()) 
	      usleep (10);

	   delete	workerHandle;
	}
	workerHandle	= NULL;
}
//
//
//	Note that selecting a new rate implies
//	restarting the program
void	dabStick::set_rateSelector	(const QString &s) {
int32_t	newRate	= s. toInt ();
int32_t	r;

	if (outputRate == newRate)
	   return;
	outputRate	= newRate;

	inputRate	= getInputRate (outputRate);

	if (workerHandle == NULL) {
	   r	= rtlsdr_set_sample_rate (device, inputRate);
	   (void)rtlsdr_set_center_freq (device, vfoFrequency + vfoOffset);
	}
	else {	
//	we stop the transmission!!!
	   stopReader ();
	   r	= rtlsdr_set_sample_rate (device, inputRate);
	   r = rtlsdr_reset_buffer (device);
	   rtlsdr_set_center_freq (device, vfoFrequency + vfoOffset);
	}
	(void)r;
	delete d_filter;
	d_filter	= new DecimatingFIR (inputRate / outputRate * 5 - 1,
	                                     - outputRate / 2,
	                                     outputRate / 2,
	                                     inputRate,
	                                     inputRate / outputRate);

	emit set_changeRate (outputRate);
}

void	dabStick::setExternalGain	(int gain) {
static int	oldGain	= 0;

	if (gain == oldGain)
	   return;

	if ((gain < 0) || (gain >= gainsCount))
	   return;	

	oldGain	= gain;
	rtlsdr_set_tuner_gain (device, gains [gain]);
	(void)rtlsdr_get_tuner_gain (device);
	gainDisplay	-> display (gain);
}
//
//	Good old getSamples
//	Note: n * rateIn / rateOut might give a wrong answer.
//	Keep the brackets!!
//
int32_t	dabStick::getSamples (DSPCOMPLEX *V,
	                                 int32_t n, uint8_t Mode) {
int32_t	size	= n * (inputRate / outputRate);
uint8_t	*buf	= (uint8_t *)alloca (2 * size * sizeof (uint8_t));
int32_t	i, index;

	if (!open || !libraryLoaded)
	   return	0;

//	first step: get data from the buffer
	(void) _I_Buffer	-> getDataFromBuffer (buf, 2 * size);

	for (i = 0, index = 0; i < size; i ++)  {
	   DSPCOMPLEX tmp = DSPCOMPLEX ((buf [2 * i] - 127.0) / 128.0,
	                                (buf [2 * i + 1] - 127.0) / 128.0);
	   if (d_filter -> Pass (tmp, &tmp)) {
	      switch (Mode) {
	         default:
	         case IandQ:
	            V [index ++] = tmp;
	            break;

	         case QandI:
	            V [index ++] = DSPCOMPLEX (imag (tmp), real (tmp));
	            break;

	         case Q_Only:
	            V [index ++] = DSPCOMPLEX (real (tmp), 0.0);
	            break;

	         case I_Only:
	            V [index ++] = DSPCOMPLEX (imag (tmp), 0.0);
	            break;
	      }
	   }
	}

	return index;
}

int32_t	dabStick::Samples	(void) {
	return _I_Buffer -> GetRingBufferReadAvailable () /
	                          ((int)(2 * inputRate / outputRate));
}

void	dabStick::setCorrection	(int ppm) {
	if (!open || !libraryLoaded)
	   return;

	rtlsdr_set_freq_correction (device, ppm);
}
//
//	vfoOffset is in Hz, we have two spinboxes influencing the
//	settings
void	dabStick::setKhzOffset	(int k) {
	vfoOffset	= (vfoOffset / 1000) + Khz (k);
}

void	dabStick::setHzOffset	(int h) {
	vfoOffset	= 1000 * (vfoOffset / 1000) + h;
}

bool	dabStick::load_rtlFunctions (void) {
//	link the required procedures
	rtlsdr_open	= (pfnrtlsdr_open)
	                       GETPROCADDRESS (Handle, "rtlsdr_open");
	if (rtlsdr_open == NULL) {
	   fprintf (stderr, "Could not find rtlsdr_open\n");
	   return false;
	}
	rtlsdr_close	= (pfnrtlsdr_close)
	                     GETPROCADDRESS (Handle, "rtlsdr_close");
	if (rtlsdr_close == NULL) {
	   fprintf (stderr, "Could not find rtlsdr_close\n");
	   return false;
	}

	rtlsdr_set_sample_rate =
	    (pfnrtlsdr_set_sample_rate)GETPROCADDRESS (Handle, "rtlsdr_set_sample_rate");
	if (rtlsdr_set_sample_rate == NULL) {
	   fprintf (stderr, "Could not find rtlsdr_set_sample_rate\n");
	   return false;
	}

	rtlsdr_get_sample_rate	=
	    (pfnrtlsdr_get_sample_rate)
	               GETPROCADDRESS (Handle, "rtlsdr_get_sample_rate");
	if (rtlsdr_get_sample_rate == NULL) {
	   fprintf (stderr, "Could not find rtlsdr_get_sample_rate\n");
	   return false;
	}

	rtlsdr_set_agc_mode	=
	    (pfnrtlsdr_set_agc_mode)
	               GETPROCADDRESS (Handle, "rtlsdr_set_agc_mode");
	if (rtlsdr_set_agc_mode == NULL) {
	   fprintf (stderr, "Could not find rtlsdr_set_agc_mode\n");
	   return false;
	}

	rtlsdr_get_tuner_gains		= (pfnrtlsdr_get_tuner_gains)
	                     GETPROCADDRESS (Handle, "rtlsdr_get_tuner_gains");
	if (rtlsdr_get_tuner_gains == NULL) {
	   fprintf (stderr, "Could not find rtlsdr_get_tuner_gains\n");
	   return false;
	}

	rtlsdr_set_tuner_gain_mode	= (pfnrtlsdr_set_tuner_gain_mode)
	                     GETPROCADDRESS (Handle, "rtlsdr_set_tuner_gain_mode");
	if (rtlsdr_set_tuner_gain_mode == NULL) {
	   fprintf (stderr, "Could not find rtlsdr_set_tuner_gain_mode\n");
	   return false;
	}

	rtlsdr_set_tuner_gain	= (pfnrtlsdr_set_tuner_gain)
	                     GETPROCADDRESS (Handle, "rtlsdr_set_tuner_gain");
	if (rtlsdr_set_tuner_gain == NULL) {
	   fprintf (stderr, "Cound not find rtlsdr_set_tuner_gain\n");
	   return false;
	}

	rtlsdr_get_tuner_gain	= (pfnrtlsdr_get_tuner_gain)
	                     GETPROCADDRESS (Handle, "rtlsdr_get_tuner_gain");
	if (rtlsdr_get_tuner_gain == NULL) {
	   fprintf (stderr, "Could not find rtlsdr_get_tuner_gain\n");
	   return false;
	}
	rtlsdr_set_center_freq	= (pfnrtlsdr_set_center_freq)
	                     GETPROCADDRESS (Handle, "rtlsdr_set_center_freq");
	if (rtlsdr_set_center_freq == NULL) {
	   fprintf (stderr, "Could not find rtlsdr_set_center_freq\n");
	   return false;
	}

	rtlsdr_get_center_freq	= (pfnrtlsdr_get_center_freq)
	                     GETPROCADDRESS (Handle, "rtlsdr_get_center_freq");
	if (rtlsdr_get_center_freq == NULL) {
	   fprintf (stderr, "Could not find rtlsdr_get_center_freq\n");
	   return false;
	}

	rtlsdr_reset_buffer	= (pfnrtlsdr_reset_buffer)
	                     GETPROCADDRESS (Handle, "rtlsdr_reset_buffer");
	if (rtlsdr_reset_buffer == NULL) {
	   fprintf (stderr, "Could not find rtlsdr_reset_buffer\n");
	   return false;
	}

	rtlsdr_read_async	= (pfnrtlsdr_read_async)
	                     GETPROCADDRESS (Handle, "rtlsdr_read_async");
	if (rtlsdr_read_async == NULL) {
	   fprintf (stderr, "Cound not find rtlsdr_read_async\n");
	   return false;
	}

	rtlsdr_get_device_count	= (pfnrtlsdr_get_device_count)
	                     GETPROCADDRESS (Handle, "rtlsdr_get_device_count");
	if (rtlsdr_get_device_count == NULL) {
	   fprintf (stderr, "Could not find rtlsdr_get_device_count\n");
	   return false;
	}

	rtlsdr_cancel_async	= (pfnrtlsdr_cancel_async)
	                     GETPROCADDRESS (Handle, "rtlsdr_cancel_async");
	if (rtlsdr_cancel_async == NULL) {
	   fprintf (stderr, "Could not find rtlsdr_cancel_async\n");
	   return false;
	}

	rtlsdr_set_direct_sampling = (pfnrtlsdr_set_direct_sampling)
	                  GETPROCADDRESS (Handle, "rtlsdr_set_direct_sampling");
	if (rtlsdr_set_direct_sampling == NULL) {
	   fprintf (stderr, "Could not find rtlsdr_set_direct_sampling\n");
	   return false;
	}

	rtlsdr_set_freq_correction = (pfnrtlsdr_set_freq_correction)
	                  GETPROCADDRESS (Handle, "rtlsdr_set_freq_correction");
	if (rtlsdr_set_freq_correction == NULL) {
	   fprintf (stderr, "Could not find rtlsdr_set_freq_correction\n");
	   return false;
	}

	rtlsdr_get_device_name = (pfnrtlsdr_get_device_name)
	                   GETPROCADDRESS (Handle, "rtlsdr_get_device_name");
	if (rtlsdr_get_device_name == NULL) {
	   fprintf (stderr, "Could not find rtlsdr_get_device_name\n");
	   return false;
	}

	fprintf (stderr, "OK, functions seem to be loaded\n");
	return true;
}

int32_t	dabStick::getInputRate	(int32_t rate) {
int32_t temp 	= rate;

	while (temp < 960000)
	   temp += rate;

	return temp;
}

int16_t	dabStick::bitDepth	(void) {
	return 8;
}

void	dabStick::exit		(void) {
	stopReader ();
}

void	dabStick::newdataAvailable (int n) {
	samplesAvailable (n);
}

void	dabStick::setAgc	(int s) {
	(void) s;
	if (checkAgc -> isChecked ())
	   (void)rtlsdr_set_agc_mode (device, 1);
	else
	   (void)rtlsdr_set_agc_mode (device, 0);
}

#if QT_VERSION < 0x050000
QT_BEGIN_NAMESPACE
Q_EXPORT_PLUGIN2(device_dabstick, dabStick)
QT_END_NAMESPACE
#endif

