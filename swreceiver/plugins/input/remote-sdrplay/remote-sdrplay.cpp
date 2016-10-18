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
#include	"remote-sdrplay.h"
//
#define	DEFAULT_FREQUENCY	(Khz (14070))

QWidget	*remote::createPluginWindow (int32_t rate, QSettings *s) {
	(void)rate;		// not used here (starting at 6.0)
	remoteSettings		= s;
	theFrame		= new QFrame (NULL);
	setupUi (theFrame);

    //	setting the defaults and constants
	remoteSettings	-> beginGroup ("remote-sdrPlay");
	theGain		= remoteSettings ->
	                          value ("remote-gain", 50). toInt ();
	gainSlider	-> setValue (theGain);
	QString h	= remoteSettings ->
	                           value ("remote-rate", 96000). toString ();
	int k		= rateSelector	-> findText (h);
	if (k != -1) 
	   rateSelector -> setCurrentIndex (k);
	theRate		= rateSelector	-> currentText (). toInt ();
	ppm		= remoteSettings ->
	                           value ("remote-ppm", 0). toInt ();
	ppmControl	-> setValue (ppm);
	remoteSettings	-> endGroup ();

	vfoFrequency	= DEFAULT_FREQUENCY;
	_I_Buffer	= new RingBuffer<DSPCOMPLEX>(4 * 32768);
	connected	= false;
//
	connect (connectButton, SIGNAL (clicked (void)),
	         this, SLOT (wantConnect (void)));
	connect (disconnectButton, SIGNAL (clicked (void)),
	         this, SLOT (setDisconnect (void)));
	connect (gainSlider, SIGNAL (valueChanged (int)),
	         this, SLOT (sendGain (int)));
	connect (rateSelector, SIGNAL (activated (const QString &)),
	         this, SLOT (setRate (const QString &)));
	connect (agcControl, SIGNAL (stateChanged (int)),
                 this, SLOT (set_agcControl (int)));
        connect (ppmControl, SIGNAL (valueChanged (int)),
                 this, SLOT (set_ppmControl (int)));

	state	-> setText ("waiting to start");
	return theFrame;
}
//
//	just a dummy, needed since the rigInterface is abstract
//	and we need to terminate properly
	rigInterface::~rigInterface	(void) {
}

	remote::~remote	(void) {
	remoteSettings ->  beginGroup ("remote-sdrPlay");
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
	remoteSettings -> setValue ("remote-ppm", ppmControl -> value ());
	remoteSettings -> endGroup ();
	toServer. close ();
	delete	_I_Buffer;
}
//
void	remote::wantConnect (void) {
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
	remoteSettings -> beginGroup ("remote-sdrPlay");
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
void	remote::setConnection (void) {
QString s	= hostLineEdit -> text ();
QHostAddress theAddress	= QHostAddress (s);

	serverAddress	= QHostAddress (s);
	basePort	= 20040;
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
	sendPpm  (ppm);
	
	sendVFO	(DEFAULT_FREQUENCY - theRate / 4);
	toServer. waitForBytesWritten ();
	state -> setText ("Connected");
	connect (&streamer, SIGNAL (readyRead (void)),
	         this, SLOT (readData (void)));
	connect (&toServer, SIGNAL (readyRead (void)),
	         this, SLOT (readControl (void)));
}

int32_t	remote::getRate	(void) {
	return theRate;
}

void	remote::setRate	(const QString &s) {
int32_t	localRate	= s. toInt ();

	if (localRate == theRate)
	   return;

//	communicate change in rate to the device
	theRate	= localRate;
    	set_changeRate (theRate);	// signal the main program
	                                // to alter some settings
	if (connected)
	   sendRate (theRate);
}

void	remote::set_ppmControl	(int c) {
//
//	communicate change in ppm setting to the device
	if (connected) {
	   sendPpm (c);
	   sendVFO (vfoFrequency);
	}
}

void	remote::set_agcControl	(int f) {
	(void)f;
	if (connected)
	   sendAgc (agcControl -> isChecked ());
}

bool	remote::legalFrequency (int32_t f) {
	(void)f;
	return true;
}

int32_t	remote::defaultFrequency (void) {
	return DEFAULT_FREQUENCY;	// choose any legal frequency here
}

void	remote::setVFOFrequency	(int32_t newFrequency) {
	if (!connected)
	   return;
//	here the command to set the frequency
	vfoFrequency = newFrequency;
//	fprintf (stderr, "We gaan naar %d\n", vfoFrequency);
	sendVFO (vfoFrequency);
}

int32_t	remote::getVFOFrequency	(void) {
	return vfoFrequency;
}

bool	remote::restartReader	(void) {
	if (!connected)
	   return false;
	sendStart	();
	return true;
}

void	remote::stopReader	(void) {
	if (connected)
	   sendStop	();
}

//
//	Note that, while the server sends "bytes", the
//	packing function here makes it into nice samples
//	to ease the life of the getSamples function
int32_t	remote::getSamples (DSPCOMPLEX *V, int32_t size, uint8_t Mode) { 
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

int32_t	remote::Samples	(void) {
	if (!connected)
	   return 0;
	return _I_Buffer	-> GetRingBufferReadAvailable ();
}

//
//	It is used to set the scale for the spectrum
int16_t	remote::bitDepth	(void) {
	return 14;
}

//
//	Slots, will not be called as long as there is no connection
void	remote::readControl	(void) {
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
//
//	Transfer is in blocks of 512 * 4 bytes
void	remote::readData	(void) {
QByteArray d;

	d. resize (4 * 512);
	while (streamer. bytesAvailable () > 4 * 512) {
	   streamer. read (d. data (), d. size ());
	   toBuffer (d);
	}
}
//
static inline
DSPCOMPLEX	makeSample (uint8_t *buf) {
int16_t ii = 0; int16_t qq = 0;
int16_t	i = 0;

	uint8_t i0 = buf [i++];
	uint8_t i1 = buf [i++];

	uint8_t q0 = buf [i++];
	uint8_t q1 = buf [i++];

	ii = (i0 << 8) | i1;
	qq = (q0 << 8) | q1;

	return  DSPCOMPLEX ((float (ii)) / 2048.0, (float (qq)) / 2048.0);
}

//	we get in 16 bits int16_t, packed in unsigned bytes.
//	so, for one I/Q sample we need 4 bytes
void	remote::toBuffer (QByteArray d) {
int32_t	i;
int32_t j;
int32_t	length	= d. size ();
DSPCOMPLEX buffer [length / 4];
int	size	= 0;

	for (i = 0; i < length / 4; i++) {
	   uint8_t lbuf [4];
	   for (j = 0; j < 4; j ++)
	      lbuf [j] = d [4 * i + j];
	   buffer [i] = makeSample (&lbuf [0]);
	   size ++;
	}

	_I_Buffer -> putDataIntoBuffer (buffer, size);
	if (_I_Buffer -> GetRingBufferReadAvailable () >
	                                  uint32_t (theRate / 10))
	   emit samplesAvailable (theRate / 10);
}
//
//	commands are packed in 5 bytes
void remote::sendRate (int32_t theRate) {
QByteArray datagram;

	datagram. resize (5);
	datagram [0] = 0171;		// command to set rate
	datagram [4] = theRate & 0xFF;  //lsb last
	datagram [3] = (theRate >> 8) & 0xFF;
	datagram [2] = (theRate >> 16) & 0xFF;
	datagram [1] = (theRate >> 24) & 0xFF;

	toServer. write (datagram. data (), datagram. size ());
}

void	remote::sendPpm (int32_t ppm) {
QByteArray datagram;

	datagram. resize (5);
	datagram [0] = 0176;
	datagram [4] = ppm & 0xFF;  //lsb last
	datagram [3] = (ppm >> 8) & 0xFF;
	datagram [2] = (ppm >> 16) & 0xFF;
	datagram [1] = (ppm >> 24) & 0xFF;

	toServer. write (datagram. data (), datagram. size ());
}

void	remote:: sendAgc (uint8_t f) {
QByteArray datagram;

	datagram. resize (5);
	datagram [0] = 0175;
	datagram [1] = f;
	toServer. write (datagram. data (), datagram. size ());
}

void remote::sendVFO (int32_t frequency) {
QByteArray datagram;

	datagram. resize (5);
	datagram [0] = 0170;		// command to set freq
	datagram [4] = frequency & 0xFF;  //lsb last
	datagram [3] = (frequency >> 8) & 0xFF;
	datagram [2] = (frequency >> 16) & 0xFF;
	datagram [1] = (frequency >> 24) & 0xFF;

	toServer. write (datagram. data (), datagram. size ());
}

void remote::sendStart (void) {
QByteArray datagram;
int16_t	i;

	datagram. resize (5);
	for (i = 0; i < 5 ; i++)
	   datagram[i] = 0;
	datagram [0] = 0172;
//	no other setting required... keep it 0!

	toServer. write (datagram. data (), datagram. size ());
}

void remote::sendStop (void) {
QByteArray datagram;
int16_t	i;

	datagram. resize (5);
	for (i = 0; i < 5 ; i++)
	   datagram[i] = 0;
	datagram [0] = 0173;
//	no other setting required... keep it 0!

	toServer. write (datagram. data (), datagram. size ());
}

void	remote::sendGain (int gain) {
QByteArray datagram;
int16_t	i;

	datagram. resize (5);
	for (i = 0; i < 5 ; i++)
	   datagram[i] = 0;
	datagram [0]	= 0174;
	datagram [1]	= gain;
	toServer. write (datagram. data (), datagram. size ());
	theGain		= gain;
	gainDisplay	-> display (theGain);
}

void	remote::setDisconnect (void) {
	if (connected) {		// close previous connection
	   stopReader	();
	   streamer. close ();
	   remoteSettings -> beginGroup ("remote-sdrPlay");
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

bool	remote::isvalidRate (int32_t rate) {
	return rate == 96000 || rate == 192000 || rate == 256000 ||
	       rate == 384000 || rate == 512000 || rate == 768000;
}

void	remote::exit	(void) {
}

bool	remote::isOK	(void) {
	return true;
}

#if QT_VERSION < 0x050000
QT_BEGIN_NAMESPACE
Q_EXPORT_PLUGIN2 (device_remore, remote)
QT_END_NAMESPACE
#endif

