#
/*
 *    Copyright (C) 2015
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

#include	<QtNetwork>
#include	"dabstick-server.h"	// our header
#include	"swradio-constants.h"
#include	"fir-filters.h"
#include	"dongleselect.h"

//	The naming of functions for accessing shared libraries
//	differ between Linux and Windows
#ifdef __MINGW32__
#define	GETPROCADDRESS	GetProcAddress
#else
#define	GETPROCADDRESS	dlsym
#endif

#define	READLEN_DEFAULT	8192

//	for handling the events in libusb, we need a controlthread
//	whose sole purpose is to process the rtlsdr_read_async function
//	from the lib.

	dll_driver::dll_driver (dabstickServer *d) {
	theStick	= d;
	start ();
}

	dll_driver::~dll_driver (void) {
}
//	For the callback, we do need some environment 
//	This is the user-side call back function
void	dll_driver::RTLSDRCallBack (unsigned char *buf,
	                            uint32_t len, void *ctx) {
dabstickServer	*theStick = (dabstickServer *)ctx;

	if ((len != READLEN_DEFAULT) || (theStick == NULL))
	   return;
//	Note that our "context"  is the stick
	theStick-> _I_Buffer -> putDataIntoBuffer (buf, len);
	if ((int)(theStick -> _I_Buffer -> GetRingBufferReadAvailable ()) >
	   (int)(theStick -> inputRate / 10))
	   theStick -> workerHandle -> samplesAvailable ();
}

void	dll_driver::run (void) {
	(theStick -> rtlsdr_read_async) (theStick -> device,
	                          (rtlsdr_read_async_cb_t)&RTLSDRCallBack,
	                          (void *)theStick,
	                          0,
	                          READLEN_DEFAULT);
}
//
//	Currently, we do not have lots of settings
	dabstickServer::dabstickServer (QSettings	*s,
	                                QWidget *parent):QDialog (parent) {
	setupUi (this);
	dabserverSettings	= s;
	deviceOK		= false;
	outputRate		= 96000;
	inputRate		= getInputRate (outputRate);
	ratio			= inputRate / outputRate;
	rateDisplay		-> display (inputRate);
	outRateDisplay		-> display (outputRate);
	currentGain		= 25;
	actualAttenuation	-> display (currentGain);
//
//	the connects
	setupDevice (inputRate, outputRate);
	if (!libraryLoaded) {
	   fprintf (stderr, "couln't load rtlsdr lib, fatal\n");
	   exit (1);
	}
	_I_Buffer	= new RingBuffer<uint8_t>(16 * 32768);
	vfoFrequency	= Khz (94700);
	vfoOffset	= 0;
	workerHandle	= NULL;
	notConnected	= true;	// tells we have a client or not

	statusLabel	-> setText ("Loaded");
	connect (resetButton, SIGNAL (clicked (void)),
	         this, SLOT (handleReset (void)));
	connect (quitButton, SIGNAL (clicked (void)),
	         this, SLOT (handleQuit (void)));
//	Now for the communication
	connect (&server, SIGNAL (newConnection (void)),
	         this, SLOT (acceptConnection (void)));
	server. listen (QHostAddress::Any, 20060);
}
//
	dabstickServer::~dabstickServer	(void) {
	if (deviceOK && dabserverSettings != NULL) {
	   dabserverSettings	-> beginGroup ("dabstick");
	   dabserverSettings	-> setValue ("dab_correction",
	                                             ppmOffset -> value ());
	   dabserverSettings	-> setValue ("dab_khzOffset", khzOffset -> value ());
	   dabserverSettings	-> endGroup ();
	}

	if (deviceOK) 
	   stopReader ();
	if (deviceOK) 
	   rtlsdr_close (device);

	if (libraryLoaded)
#ifdef __MINGW32__
	   FreeLibrary (Handle);
#else
	   dlclose (Handle);
#endif
}

void	dabstickServer::acceptConnection (void) {
	if (!notConnected)
	   return;

	client = server. nextPendingConnection ();
	QHostAddress s = client -> peerAddress ();
	fprintf (stderr, "Accepted a client %s\n", s.toString (). toLatin1 (). data ());
	QByteArray a;
	a. resize (client -> bytesAvailable ());
	if (a. size () > 0) {
	   fprintf (stderr, "%d bytes in de queue\n", a. size ());
	   client -> read (a. data (), a. size ());
	}
	connectName -> setText (client -> peerAddress (). toString ());
	streamer. listen (QHostAddress::Any, 20060 + 10);
	if (!streamer. waitForNewConnection (5000, NULL)) 
	   return;

	connect (client, SIGNAL (readyRead (void)),
	         this, SLOT (processCommand (void)));

	QByteArray message;
	streamerAddress	= streamer. nextPendingConnection ();
//	fprintf (stderr, "Ook de streamer is connected\n");
	connect (&watchTimer, SIGNAL (timeout (void)),
	         this, SLOT (checkConnection (void)));
	watchTimer. start (5000);
}

void	dabstickServer::checkConnection (void) {
	if (streamerAddress -> state () ==
	                   QAbstractSocket::UnconnectedState) {
	   watchTimer. stop ();
	   handleReset ();
	}
}

//	Commands are packed in 5 bytes.
void	dabstickServer::processCommand (void) {
QByteArray a;
	a. resize (5);
	while (client -> bytesAvailable () >= 5) {
	   client -> read (a. data (), 5);

	   switch (a [0]) {
	   case 0170:		// set frequency;
	      setVFOFrequency (a);
	      break;
	   case 0171:		// set rate;
	      setRate (a);
	      break;
	   case 0172:		// setStart;
	      restartReader ();
	      break;
	   case 0173:		// setStop:
	      stopReader ();
	      break;
	   case 0174:		// set external gain
	      setAttenuation (a);
	      break;
	   case 0175:		// set agc mode
	      setAgc	(a);
	      break;
	   default:
	   case 0125:		// give up
	      streamer. close ();
	      notConnected	 = true;
	      connectName	-> setText ("No connection");
	   }
	}
}

void	dabstickServer::handleReset	(void) {
	stopReader	();
	notConnected		= true;
	outputRate		= 96000;
	inputRate		= 960000;
	rateDisplay		-> display (inputRate);
	outRateDisplay		-> display (outputRate);
	currentGain		= 25;
	actualAttenuation	-> display (currentGain);
	streamer. close ();
}

int32_t	dabstickServer::defaultFrequency	(void) {
	return Khz (94700);
}

void	dabstickServer::setVFOFrequency	(QByteArray a) {
int32_t	realFreq;
QByteArray message;
uint8_t f0, f1, f2, f3;

	f0	= a [1];
	f1	= a [2];
	f2	= a [3];
	f3	= a [4];
	realFreq = (f0 << 24) | (f1 << 16) | (f2 << 8) | f3;
//	fprintf (stderr, "request for freq -> %d\n", realFreq);
	if (!legalFrequency (realFreq) || !deviceOK)
	   return;

	vfoFrequency	= realFreq;
	(void)rtlsdr_set_center_freq (device, vfoFrequency + vfoOffset);
	freqDisplay -> display (vfoFrequency);

	message. resize (7);
	message [0] = 0377;	// start of message
	message [1] = 07;	// length of message
	message [2] = 'f';	// identity of message
	message [3] = (vfoFrequency >> 24) & 0xFF;
	message [4] = (vfoFrequency >> 16) & 0xFF;
	message [5] = (vfoFrequency >>  8) & 0xFF;
	message [6] =  vfoFrequency        & 0xFF;
	client -> write (message. data (), message. size ());
}

void	dabstickServer::setAttenuation	(QByteArray a) {
int16_t		newGain	= a [1];
int16_t		realGain	= 0;

	if (newGain == currentGain)
	   return;

	realGain	= gainsCount * newGain / 100;

	if ((newGain < 0) || (realGain >= gainsCount))
	   return;	

	rtlsdr_set_tuner_gain (device, gains [realGain]);
	int r = rtlsdr_get_tuner_gain (device);
	currentGain = newGain;
	actualAttenuation -> display (realGain);
}

void	dabstickServer::setAgc (QByteArray a) {
int16_t	on_off	= a [1];

	if (on_off == 0)
	   rtlsdr_set_agc_mode (device, 0);
	else
	   rtlsdr_set_agc_mode (device, 1);
}

bool	dabstickServer::restartReader	(void) {
int32_t	r;

	if ((workerHandle != NULL) || !deviceOK)
	   return true;

	r = rtlsdr_reset_buffer (device);
	if (r < 0)
	   return false;

	rtlsdr_set_center_freq (device, vfoFrequency + vfoOffset);
	workerHandle	= new dll_driver (this);
	connect (workerHandle, SIGNAL (samplesAvailable (void)),
	         this, SLOT (sendSamples (void)));
	return true;
}

void	dabstickServer::stopReader	(void) {

	if ((workerHandle == NULL) || !deviceOK)
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
//	we get "amount" samples in and have to send a smaller amount
//	to the client, and changed from uint8_t to uint16_t
//	we have this clumsy way since for decimation we filter

void	dabstickServer::sendSamples (void) {
uint8_t localBuffer [ratio * 2 * 512];
QByteArray datagram;
int16_t	i;
static	int cnt	= 0;
static	float	sum = 0;

	datagram. resize (4 * 512);	// 512 samples

	while ((int)(_I_Buffer -> GetRingBufferReadAvailable ()) >
	                                                 ratio * 512) {
	   _I_Buffer -> getDataFromBuffer (localBuffer, ratio * 512);
	   int count	= 0;
	   for (i = 0; i < ratio * 512; i ++) {
	      DSPCOMPLEX temp	= DSPCOMPLEX ((localBuffer [2 * i] - 127.0) / 128.0,
	                                      (localBuffer [2 * i + 1] - 127.0) / 128.0);
	      if (d_filter -> Pass (temp, &temp)) {
	         int16_t re	= real (temp) * 32768;
	         int16_t im	= imag (temp) * 32768;

	         datagram [count ++] = ((re & 0xFF00) >>   8) & 0xFF;
	         datagram [count ++] =  (re & 0x00FF) & 0xFF;

	         datagram [count ++] = ((im & 0xFF00) >>  8) & 0xFF;
	         datagram [count ++] =  (im & 0x00FF) & 0xFF;
	      }
	   }
	   if (streamerAddress -> state () ==
	                   QAbstractSocket::UnconnectedState) {
	      handleReset ();
	      return;
	   }

	   streamerAddress -> write (datagram. data (), datagram. size ());
	}
}

//
void	dabstickServer::setRate (QByteArray a) {
int32_t	newRate;
uint8_t r0, r1, r2, r3;
int	r;
	r0	= a [1];
	r1	= a [2];
	r2	= a [3];
	r3	= a [4];
	newRate = (r0 << 24) | (r1 << 16) | (r2 << 8) | r3;

	fprintf (stderr, "We request a rate %d\n", newRate);
	if (newRate == outputRate)
	   return;
	outputRate	= newRate;
	inputRate	= getInputRate (outputRate);
	ratio		= inputRate / outputRate;
	rateDisplay		-> display (inputRate);
	outRateDisplay		-> display (outputRate);

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

	rateDisplay		-> display (inputRate);
	outRateDisplay		-> display (outputRate);
}

void	dabstickServer::handleQuit	(void) {
	stopReader	();
	accept ();
}

void	dabstickServer::setupDevice (int32_t rateIn, int32_t rateOut) {
int	r;
int16_t	deviceIndex;
int16_t	i;
bool	open;

	libraryLoaded	= false;
	d_filter	= NULL;
	_I_Buffer	= NULL;

	statusLabel	-> setText ("setting up");
#ifdef	__MINGW32__
	Handle		= LoadLibrary ((wchar_t *)L"rtlsdr.dll");
#else
	Handle		= dlopen ("libusb-1.0.so", RTLD_NOW | RTLD_GLOBAL);
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
	   exit (1);
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
	rtlsdr_set_tuner_gain_mode	(device, 1);
	rtlsdr_set_tuner_gain		(device, gains [gainsCount / 2]);
	rateDisplay	-> display (rateIn);
	_I_Buffer	= new RingBuffer<uint8_t>(1024 * 1024);
	workerHandle	= NULL;

	d_filter	= new DecimatingFIR (rateIn / rateOut * 5 - 1,
	                                     - rateOut / 2,
	                                     rateOut / 2,
	                                     rateIn,
	                                     rateIn / rateOut);
	connect (ppmOffset, SIGNAL (valueChanged (int)),
	         this, SLOT (setCorrection (int)));
	connect (khzOffset, SIGNAL (valueChanged (int)),
	         this, SLOT (setOffset (int)));
	if (dabserverSettings != NULL) {
	   int k;
	   dabserverSettings	-> beginGroup ("dabStick");
	   k		= dabserverSettings	-> value ("dab_correction", 0). toInt ();
	   ppmOffset	-> setValue (k);
	   k		= dabserverSettings	-> value ("dab_khzOffset", 0). toInt ();
	   khzOffset	-> setValue (k);
	   dabserverSettings	-> endGroup ();
	}
	statusLabel	-> setText ("Loaded");
	deviceOK	= true;
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
	deviceOK	= false;
	return;
}

bool	dabstickServer::legalFrequency	(int32_t f) {
	(void)f;
	return true;
}
//
void	dabstickServer::setCorrection	(int ppm) {
	if (!deviceOK)
	   return;

	rtlsdr_set_freq_correction (device, ppm);
}
//
void	dabstickServer::setOffset	(int k) {
	vfoOffset	=  Khz (k);
}

bool	dabstickServer::load_rtlFunctions (void) {
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

int32_t	dabstickServer::getInputRate	(int32_t rate) {
int32_t temp 	= rate;

	while (temp < 960000)
	   temp += rate;

	return temp;
}

