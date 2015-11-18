#
/*
 *    Copyright (C) 2010, 2011, 2012
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Computing
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
 *     Communication via network to an sdrplay server
 */

#include	<QSettings>
#include	<QLabel>
#include	<QMessageBox>
#include	<QTcpSocket>
#include	<QHostAddress>
#include	"remote-dabstick.h"
//
#define	DEFAULT_FREQUENCY	(Khz (14070))

QWidget	*remoteDabstick::createPluginWindow (int32_t rate, QSettings *s) {
	(void)rate;		// not used here (starting at 6.0)
	remoteSettings		= s;
	theFrame		= new QFrame;
	setupUi (theFrame);

    //	setting the defaults and constants
	vfoOffset	= 0;
	remoteSettings	-> beginGroup ("remote-dabstick");
	theRate		= remoteSettings ->
	                          value ("remote-rate", 192000). toInt ();
	theGain		= remoteSettings ->
	                          value ("remote-gain", 50). toInt ();
	remoteSettings	-> endGroup ();
	gainSlider	-> setValue (theGain);
	rateDisplay	-> display (theRate);
	vfoFrequency	= DEFAULT_FREQUENCY;
	_I_Buffer	= new RingBuffer<DSPCOMPLEX>(4 * 32768);
	connected	= false;
//
	connect (connectButton, SIGNAL (clicked (void)),
	         this, SLOT (wantConnect (void)));
	connect (disconnectButton, SIGNAL (clicked (void)),
	         this, SLOT (setDisconnect (void)));
	connect (hzOffset, SIGNAL (valueChanged (int)),
	         this, SLOT (setOffset (int)));
	connect (gainSlider, SIGNAL (valueChanged (int)),
	         this, SLOT (sendGain (int)));
	connect (rateSelector, SIGNAL (activated (const QString &)),
	         this, SLOT (setRate (const QString &)));
	connect (checkAgc, SIGNAL (stateChanged (int)),
	         this, SLOT (setAgc (int)));
	state	-> setText ("waiting to start");
	return theFrame;
}
//
//	just a dummy, needed since the rigInterface is abstract
//	and we need to terminate properly
	rigInterface::~rigInterface	(void) {
}

	remoteDabstick::~remoteDabstick	(void) {
	remoteSettings ->  beginGroup ("remote-dabstick");
	if (connected) {		// close previous connection
	   stopReader	();
	   streamer. close ();
	   remoteSettings -> setValue ("remote-rate", theRate);
	   remoteSettings -> setValue ("remote-server",
	                               toServer. peerAddress (). toString ());
	   QByteArray datagram;
	   datagram. resize (5);
	   datagram [0] = 0125;
	   toServer. write (datagram. data (), datagram. size ());
	   toServer. waitForBytesWritten ();
	}
	remoteSettings -> setValue ("remote-gain", theGain);
	remoteSettings -> endGroup ();
	toServer. close ();
	delete	_I_Buffer;
}
//
void	remoteDabstick::wantConnect (void) {
QString ipAddress;
int16_t	i;
QList<QHostAddress> ipAddressesList = QNetworkInterface::allAddresses();

	if (connected)
	   return;
	// use the first non-localhost IPv4 address
	for (i = 0; i < ipAddressesList.size(); ++i) {
	   if (ipAddressesList.at (i) != QHostAddress::LocalHost &&
	      ipAddressesList. at (i). toIPv4Address ()) {
	      ipAddress = ipAddressesList. at(i). toString();
	      break;
	   }
	}
	// if we did not find one, use IPv4 localhost
	if (ipAddress. isEmpty())
	   ipAddress = QHostAddress (QHostAddress::LocalHost).toString ();
	remoteSettings -> beginGroup ("remote-dabstick");
	ipAddress = remoteSettings ->
	                value ("remote-server", ipAddress). toString ();
	remoteSettings	-> endGroup ();
	hostLineEdit	->  setText (ipAddress);

	hostLineEdit	-> setInputMask ("000.000.000.000");
//	Setting default IP address
	hostLineEdit	-> show ();
	state	-> setText ("Give IP address, return");
	connect (hostLineEdit, SIGNAL (returnPressed (void)),
	         this, SLOT (setConnection (void)));
}

//	if/when a return is pressed in the line edit,
//	a signal appears and we are able to collect the
//	inserted text. The format is the IP-V4 format.
//	Using this text, we try to connect,
void	remoteDabstick::setConnection (void) {
QString s	= hostLineEdit -> text ();
QHostAddress theAddress	= QHostAddress (s);

	serverAddress	= QHostAddress (s);
	basePort	= 20060;
	disconnect (hostLineEdit, SIGNAL (returnPressed (void)),
	            this, SLOT (setConnection (void)));
	toServer. connectToHost (serverAddress, basePort);
	if (!toServer. waitForConnected (2000)) {
	   QMessageBox::warning (theFrame, tr ("sdr"),
	                                   tr ("connection failed\n"));
	   return;
	}

//
//	The streamer will provide us with the raw data
	streamer. connectToHost (serverAddress, basePort + 10);
	if (!streamer. waitForConnected (2000)) {
	   QMessageBox::warning (theFrame, tr ("sdr"),
	                                   tr ("setting up stream failed\n"));
	   toServer. disconnectFromHost ();
	   return;
	}

	connected	= true;
	sendGain (theGain);
	sendRate (theRate);
	sendVFO	(DEFAULT_FREQUENCY - theRate / 4);
	toServer. waitForBytesWritten ();
	state -> setText ("Connected");
	connect (&streamer, SIGNAL (readyRead (void)),
	         this, SLOT (readData (void)));
	connect (&toServer, SIGNAL (readyRead (void)),
	         this, SLOT (readControl (void)));
}

int32_t	remoteDabstick::getRate	(void) {
	return theRate;
}

void	remoteDabstick::setRate	(const QString &s) {
int32_t	localRate	= s. toInt ();

	if (localRate == theRate)
	   return;

//	communicate change in rate to the device
	theRate	= localRate;
	rateDisplay -> display (theRate);
    	set_changeRate (theRate);	// signal the main program
	                                // to alter some settings
	if (connected)
	   sendRate (theRate);
}

bool	remoteDabstick::legalFrequency (int32_t f) {
	(void)f;
	return true;
}

int32_t	remoteDabstick::defaultFrequency (void) {
	return DEFAULT_FREQUENCY;	// choose any legal frequency here
}

void	remoteDabstick::setVFOFrequency	(int32_t newFrequency) {
	if (!connected)
	   return;
//	here the command to set the frequency
	vfoFrequency = newFrequency + vfoOffset;
//	fprintf (stderr, "We gaan naar %d\n", vfoFrequency);
	sendVFO (vfoFrequency);
}

int32_t	remoteDabstick::getVFOFrequency	(void) {
	return vfoFrequency - vfoOffset;
}

bool	remoteDabstick::restartReader	(void) {
	if (!connected)
	   return false;
	sendStart	();
	return true;
}

void	remoteDabstick::stopReader	(void) {
	if (connected)
	   sendStop	();
}

//
//	Note that, while the server sends "bytes", the
//	packing function here makes it into nice samples
//	to ease the life of the getSamples function
int32_t	remoteDabstick::getSamples (DSPCOMPLEX *V, int32_t size, uint8_t Mode) { 
DSPCOMPLEX buf [size];
int32_t	amount;
int32_t	i;

	if (!connected) 
	   return size;

	amount = _I_Buffer	-> getDataFromBuffer (buf, size);

	for (i = 0; i < amount ; i ++)  {
	   switch (Mode) {
	      default:
	         case IandQ:
	            V [i] = buf [i]; break;

	         case QandI:
	            V [i] = DSPCOMPLEX (imag (buf [i]), real (buf [i]));
	            break;

	         case I_Only:
	            V [i] = DSPCOMPLEX (real (buf [i]), 0);
	            break;

	         case Q_Only:
	            V [i] = DSPCOMPLEX (imag (buf [i]), 0);
	            break;
	   }
	}
	return amount;
}

int32_t	remoteDabstick::Samples	(void) {
	if (!connected)
	   return 0;
	return _I_Buffer	-> GetRingBufferReadAvailable ();
}

//
//	bitDepth is required in the forthcoming 6.1 release
//	In 6.0 it is not called
//	It is used to set the scale for the spectrum
int16_t	remoteDabstick::bitDepth	(void) {
	return 8;
}

//
//	the offset is in Hz
void	remoteDabstick::setOffset	(int k) {
	vfoOffset	= k;
}
//
//	Slots, will not be called as long as there is no connection
void	remoteDabstick::readControl	(void) {
QByteArray d;

	while (toServer. bytesAvailable () > 0) {
	   d. resize (3);
	   toServer. read (d. data (), d. size ());
	   if (uint8_t (d [0]) !=  uint8_t (0xFF)) {	
	      d. resize (toServer. bytesAvailable ());
	      toServer. read (d. data (), d. size ());
	      break;
	   }
	   switch (d [2]) {
	      case 'f':		// new frequency
	         d. resize (d [1] - 3);
	         toServer. read (d. data (), d. size ());
	         vfoFrequency = ((d [0] << 24) & 0xFF000000) |
	                        ((d [1] << 16) & 0x00FF0000) |
	                        ((d [2] <<  8) & 0x0000FF00) |
	                         (d [3] & 0x000000FF);
//	         fprintf (stderr, "new vfo = %d\n", vfoFrequency);
	         break;

	      default:
	         d. resize (d [1] - 3);
	         toServer. read (d. data (), d. size ());
	         break;
	   }
	}
}

//	These functions are typical for network use
void	remoteDabstick::readData	(void) {
QByteArray d;

	d. resize (4 * 512);
	while (streamer. bytesAvailable () > 4 * 512) {
	   streamer. read (d. data (), d. size ());
	   toBuffer (d);
	}
}

static inline
DSPCOMPLEX	makeSample (uint8_t *buf) {
int16_t re =  0; int16_t im = 0;
int16_t	i = 0;

	uint8_t i0 = buf [i++];
	uint8_t i1 = buf [i++];

	uint8_t q0 = buf [i++];
	uint8_t q1 = buf [i++];

	re = (i0 << 8) | i1;
	im = (q0 << 8) | q1;
	
	return  DSPCOMPLEX ((float (re)) / 32767.0, (float (im)) / 32767.0);
}

//	we get in int16_t packed as bytes
//	so, for one I/Q sample we need 4 bytes
void	remoteDabstick::toBuffer (QByteArray d) {
int32_t	i;
int32_t j;
int32_t	length	= d. size ();
DSPCOMPLEX buffer [length / 4 ];
int	size	= 0;

	for (i = 0; i < length / 4; i++) {
	   uint8_t lbuf [4];
	   for (j = 0; j < 4; j ++)
	      lbuf [j] = d [4 * i + j];
	   buffer [i] = makeSample (&lbuf [0]);
	   size ++;
	}
	_I_Buffer -> putDataIntoBuffer (buffer, size);
	if (_I_Buffer -> GetRingBufferReadAvailable () > theRate / 10)
	   emit samplesAvailable (theRate / 10);
}
//
//	commands are packed in 5 bytes
void remoteDabstick::sendRate (int32_t theRate) {
QByteArray datagram;

	datagram. resize (5);
	datagram [0] = 0171;		// command to set rate
	datagram [4] = theRate & 0xFF;  //lsb last
	datagram [3] = (theRate >> 8) & 0xFF;
	datagram [2] = (theRate >> 16) & 0xFF;
	datagram [1] = (theRate >> 24) & 0xFF;

	toServer. write (datagram. data (), datagram. size ());
}

void remoteDabstick::sendVFO (int32_t frequency) {
QByteArray datagram;

	datagram. resize (5);
	datagram [0] = 0170;		// command to set freq
	datagram [4] = frequency & 0xFF;  //lsb last
	datagram [3] = (frequency >> 8) & 0xFF;
	datagram [2] = (frequency >> 16) & 0xFF;
	datagram [1] = (frequency >> 24) & 0xFF;

	toServer. write (datagram. data (), datagram. size ());
}

void remoteDabstick::sendStart (void) {
QByteArray datagram;
int16_t	i;

	datagram. resize (5);
	for (i = 0; i < 5 ; i++)
	   datagram[i] = 0;
	datagram [0] = 0172;
//	no other setting required... keep it 0!

	toServer. write (datagram. data (), datagram. size ());
}

void remoteDabstick::sendStop (void) {
QByteArray datagram;
int16_t	i;

	datagram. resize (5);
	for (i = 0; i < 5 ; i++)
	   datagram[i] = 0;
	datagram [0] = 0173;
//	no other setting required... keep it 0!

	toServer. write (datagram. data (), datagram. size ());
}

void	remoteDabstick::sendGain (int gain) {
QByteArray datagram;
int16_t	i;

	datagram. resize (5);
	for (i = 0; i < 5 ; i++)
	   datagram[i] = 0;
	datagram [0]	= 0174;
	datagram [1]	= gain;
	toServer. write (datagram. data (), datagram. size ());
	theGain		= gain;
	showGain	-> display (theGain);
}

void	remoteDabstick::setDisconnect (void) {
	if (connected) {		// close previous connection
	   stopReader	();
	   streamer. close ();
	   remoteSettings -> beginGroup ("remote-dabstick");
	   remoteSettings -> setValue ("remote-rate", theRate);
	   remoteSettings -> setValue ("remote-server",
	                               toServer. peerAddress (). toString ());
	   remoteSettings -> endGroup ();
	   QByteArray datagram;
	   datagram. resize (5);
	   datagram [0] = 0125;
	   toServer. write (datagram. data (), datagram. size ());
	   toServer. close ();
	}
	connected	= false;
	connectedLabel	-> setText (" ");
	state		-> setText ("disconnected");
}

bool	remoteDabstick::isvalidRate (int32_t rate) {
	return rate == 96000 || rate == 192000 || rate == 256000 ||
	       rate == 384000 || rate == 512000 || rate == 768000;
}

void	remoteDabstick:: exit (void) {
}

void	remoteDabstick::setAgc	(int state) {
QByteArray datagram;
int16_t	i;

	datagram. resize (5);
	for (i = 0; i < 5 ; i++)
	   datagram[i] = 0;
	datagram [0]	= 0175;
	datagram [1]	= checkAgc -> isChecked () ? 1 : 0;
	toServer. write (datagram. data (), datagram. size ());
}

#if QT_VERSION < 0x050000
QT_BEGIN_NAMESPACE
Q_EXPORT_PLUGIN2 (device_remote, remote)
QT_END_NAMESPACE
#endif

