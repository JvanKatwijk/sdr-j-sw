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

#include	<QtNetwork>
#include	<QThread>
#include	"mirsdrapi-rsp.h"	// the header of the Mirics library
#include	"sdrplay-server.h"	// our header
#include	"sdrplay-worker.h"	// the worker
#include	"sdrplay-loader.h"	// function loader
//
//	Currently, we do not have lots of settings
	sdrplayServer::sdrplayServer (int32_t rate,
	                              QWidget *parent):QDialog (parent) {
int	err;
float	ver;
bool	success;

	setupUi (this);
	(void)rate;
	deviceOK		= false;
	_I_Buffer		= NULL;
	theLoader		= NULL;
	outputRate		= 192000;
	inputRate		= getInputRate (outputRate);
	rateDisplay		-> display (inputRate);
	outRateDisplay		-> display (outputRate);
	currentGain		= 50;
	ppm			= 0;
	actualAttenuation	-> display (currentGain);
//
//	sometimes problems with dynamic linkage of libusb, it is
//	loaded indirectly through the dll
	if (libusb_init (NULL) < 0) {
	   fprintf (stderr, "libusb problem\n");	// should not happen
	   exit (1);
	}
	libusb_exit (NULL);

	theLoader	= new sdrplayLoader (&success);
	if (!success) {
	   fprintf (stderr, " No success in loading sdrplay lib\n");
	   statusLabel	-> setText ("no SDRplay lib");
	   delete theLoader;
	   theLoader = NULL;
	   exit (1);
	}

	err			= theLoader -> mir_sdr_ApiVersion (&ver);
//	Note that the versions for windows and linux are not the same,
//	we are using the Linux ".h" file, so when loading a windows
//	dll we had a problem.
	if (ver != MIR_SDR_API_VERSION) {
	   fprintf (stderr, "Foute API: %f, %d\n", ver, err);
	   statusLabel	-> setText ("library error");
	   exit (1);
	}

//	LibraryVersion	-> display (ver);
	statusLabel	-> setText ("Loaded");
	_I_Buffer	= new RingBuffer<DSPCOMPLEX>(2 * 32768);
	vfoFrequency	= Khz (94700);
	theWorker	= NULL;
	notConnected	= true;	// tells we have a client or not
	deviceOK	= true;
//
//	we only have 2 buttons
	connect (resetButton, SIGNAL (clicked (void)),
	         this, SLOT (handleReset (void)));
	connect (quitButton, SIGNAL (clicked (void)),
	         this, SLOT (handleQuit (void)));
//	Now for the communication
	connect (&server, SIGNAL (newConnection (void)),
	         this, SLOT (acceptConnection (void)));
	server. listen (QHostAddress::Any, 20040);
}
//
	sdrplayServer::~sdrplayServer	(void) {
	stopReader ();
	if (_I_Buffer != NULL)
	   delete _I_Buffer;
	if (theLoader != NULL)
	   delete theLoader;
}

void	sdrplayServer::acceptConnection (void) {
	if (!notConnected)
	   return;

	client = server. nextPendingConnection ();
	QHostAddress s = client -> peerAddress ();
	fprintf (stderr, "Accepted a client %s\n",
	                     s.toString (). toLatin1 (). data ());
	QByteArray a;
	a. resize (client -> bytesAvailable ());
	if (a. size () > 0) {
//	   fprintf (stderr, "%d bytes in de queue\n", a. size ());
	   client -> read (a. data (), a. size ());
	}

	connectName -> setText (client -> peerAddress (). toString ());
	streamer. listen (QHostAddress::Any, 20040 + 10);
	if (!streamer. waitForNewConnection (5000, NULL)) 
	   return;

	connect (client, SIGNAL (readyRead (void)),
	         this, SLOT (processCommand (void)));

	QByteArray message;
	streamerAddress	= streamer. nextPendingConnection ();
	fprintf (stderr, "Ook de streamer is connected\n");
	connect (&watchTimer, SIGNAL (timeout (void)),
	         this, SLOT (checkConnection (void)));
	watchTimer. start (5000);
}

void	sdrplayServer::checkConnection (void) {
	if (streamerAddress -> state () ==
	                   QAbstractSocket::UnconnectedState) {
	   watchTimer. stop ();
	   handleReset ();
	}
}

//	Commands are packed in 5 bytes.
void	sdrplayServer::processCommand (void) {
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
	   case 0175:		// toggle Agc
	      set_agcControl (a);
	      break;
	   case 0176:		// set ppm
	      set_ppmControl (a);
	      break;
	   default:
	      fprintf (stderr, "sorry %o not implemented\n", 
	                                        int32_t (a [0] & 0xFF));
	      break;
	   case 0125:		// give up
	      streamer. close ();
	      notConnected	 = true;
	      connectName	-> setText ("No connection");
	   }
	}
}

void	sdrplayServer::handleReset	(void) {
	stopReader	();
	notConnected		= true;
	outputRate		= 192000;
	inputRate		= getInputRate (outputRate);
	rateDisplay		-> display (inputRate);
	outRateDisplay		-> display (outputRate);
	currentGain		= 50;
	actualAttenuation	-> display (currentGain);
	streamer. close ();
}

static inline
int16_t bankFor_sdr (int32_t freq) {
	if (freq < 10 * Khz (1))
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

bool	sdrplayServer::legalFrequency (int32_t f) {
	return bankFor_sdr (f) != -1;
}

int32_t	sdrplayServer::defaultFrequency	(void) {
	return Khz (14070);
}

void	sdrplayServer::setVFOFrequency	(QByteArray a) {
int32_t	realFreq;
QByteArray message;
uint8_t f0, f1, f2, f3;

	f0	= a [1];
	f1	= a [2];
	f2	= a [3];
	f3	= a [4];
	realFreq = (f0 << 24) | (f1 << 16) | (f2 << 8) | f3;
	if (!legalFrequency (realFreq) || !deviceOK)
	   return;

	vfoFrequency = realFreq;
	if (theWorker != NULL)
	   theWorker -> setVFOFrequency (realFreq);
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

void	sdrplayServer::setAttenuation	(QByteArray a) {
int16_t	newGain	= a [1];

	if (newGain < 0 || newGain > 101)
	   return;

	if (theWorker != NULL)
	   theWorker -> setExternalGain (newGain);
	currentGain = newGain;
	actualAttenuation -> display (currentGain);
}

bool	sdrplayServer::restartReader	(void) {
bool	success;

	if ((theWorker != NULL) || !deviceOK)
	   return true;

	_I_Buffer	-> FlushRingBuffer ();
	theWorker	= new sdrplayWorker (inputRate,
	                                    outputRate,
	                                    currentGain,
	                                    vfoFrequency,
	                                    theLoader,
	                                    _I_Buffer,
	                                    &success);
	if (!success) 
	   return false;
	connect (theWorker, SIGNAL (sendSamples (int)),
	            this, SLOT (sendSamples (int)));

	theWorker -> setExternalGain (currentGain);
	theWorker -> set_ppmControl (ppm);
	return success;
}

void	sdrplayServer::stopReader	(void) {
	if ((theWorker == NULL) || !deviceOK)
	   return;

	theWorker	-> stop ();
	while (theWorker -> isRunning ())
	   usleep (100);
	delete theWorker;
	theWorker = NULL;
}
//
//	This slot is called from the worker
//	on arrival of a new bunch of samples
#define	SCALE_FACTOR 4096.0
void	sdrplayServer::sendSamples (int amount) {
QByteArray datagram;
DSPCOMPLEX buffer [1024];
int16_t	i;

	datagram. resize (4 * 1024);
	while (_I_Buffer -> GetRingBufferReadAvailable () > 1024) {
	   amount = _I_Buffer -> getDataFromBuffer (buffer, 1024);
	   for (i = 0; i < amount; i ++) {
	      int16_t re = real (buffer [i]) * SCALE_FACTOR;
	      int16_t im = imag (buffer [i]) * SCALE_FACTOR;
	      datagram [4 * i + 0] = ((re & 0xFF00) >>  8) & 0xFF;
	      datagram [4 * i + 1] =  (re & 0x00FF) & 0xFF;

	      datagram [4 * i + 2] = ((im & 0xFF00) >>  8) & 0xFF;
	      datagram [4 * i + 3] =  (im & 0x00FF) & 0xFF;
	   }

	   if (streamerAddress -> state () ==
	                   QAbstractSocket::UnconnectedState) {
	      handleReset ();
	      return;
	   }

	   streamerAddress -> write (datagram. data (), datagram. size ());
	}
}

//	find a decent inputRate for the sdrplay
//	such that it can be decimated to the requested outputrate
int32_t	sdrplayServer::getInputRate (int32_t rate) {
int32_t	temp;

	temp	= rate;
	while (temp < Khz (1200))
	   temp += rate;
	return temp;
}

void	sdrplayServer::setRate (QByteArray a) {
int32_t	newRate;
uint8_t r0, r1, r2, r3;

	r0	= a [1];
	r1	= a [2];
	r2	= a [3];
	r3	= a [4];
	newRate = (r0 << 24) | (r1 << 16) | (r2 << 8) | r3;

	if (newRate == outputRate)
	   return;
	if (theWorker != NULL) {
	   outputRate	= newRate;
	   inputRate	= getInputRate (outputRate);
	   rateDisplay	-> display (inputRate);
	   outRateDisplay -> display (outputRate);
	   theWorker	-> set_outputRate (newRate);
	}
	else {
	   outputRate	= newRate;
	   inputRate	= getInputRate (outputRate);
	   rateDisplay	-> display (inputRate);
	   outRateDisplay	-> display (outputRate);
	}
}

void	sdrplayServer::set_agcControl (QByteArray a) {
	if (theWorker != NULL)
	   theWorker -> set_agcControl (a[1]);
}

void	sdrplayServer::set_ppmControl	(QByteArray a) {
uint8_t f0, f1, f2, f3;
	f0	= a [1];
	f1	= a [2];
	f2	= a [3];
	f3	= a [4];
	ppm = (f0 << 24) | (f1 << 16) | (f2 << 8) | f3;
	if (theWorker != NULL)
	   theWorker -> set_ppmControl (ppm);
}

void	sdrplayServer::handleQuit	(void) {
	stopReader	();
	accept ();
}

