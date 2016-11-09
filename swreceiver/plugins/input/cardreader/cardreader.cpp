#
/*
 *    Copyright (C) 2008, 2009, 2010
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
#include	"pa-reader.h"
#include	"cardreader.h"
#include	<QLabel>
#include	<QSettings>
#include	<QMessageBox>

bool	cardReader::createPluginWindow	(int32_t rate,
	                                 QFrame *myFrame, QSettings *s) {
	(void)rate;
	this	-> myFrame	= myFrame;
	(void)s;
	setupUi (myFrame);
	inputRate	= rateSelector -> currentText (). toInt ();
	gainValue	= gainSlider	-> value ();
	myReader	= new paReader (inputRate, cardSelector);
	connect (cardSelector, SIGNAL (activated (int)),
	         this, SLOT (set_streamSelector (int)));
	connect (rateSelector, SIGNAL (activated (const QString &)),
	         this, SLOT (set_rateSelector (const QString &)));
	connect (gainSlider, SIGNAL (valueChanged (int)),
	         this, SLOT (set_gainValue (int)));
	connect (myReader, SIGNAL (samplesAvailable (int)),
	         this, SIGNAL (samplesAvailable (int)));
	return true;
}

	rigInterface::~rigInterface	(void) {
}

	cardReader::~cardReader		(void) {
	fprintf (stderr, "\"cardReader\" will be deleted now\n");
	readerOwner. lock ();
	delete myReader;
	myReader	= NULL;
	readerOwner. unlock ();
}

int32_t	cardReader::getRate		(void) {
	return inputRate;
}

void	cardReader::setVFOFrequency		(int32_t f) {
	myFrequency	= f;
}

int32_t	cardReader::getVFOFrequency		(void) {
	return myFrequency;
}

bool	cardReader::legalFrequency		(int32_t f) {
	return f > 0;
}

int32_t	cardReader::defaultFrequency	(void) {
	return Khz (14070);
}

bool	cardReader::restartReader	(void) {
bool	b;
	readerOwner. lock ();
	b	=  myReader	-> restartReader ();
	readerOwner. unlock ();
	return b;
}

void	cardReader::stopReader	(void) {
	readerOwner. lock ();
	myReader -> stopReader ();
	readerOwner. unlock ();
}

void	cardReader::exit	(void) {
	stopReader ();
}

bool	cardReader::isOK	(void) {
	return true;
}

int32_t	cardReader::Samples	(void) {
int32_t	n;
	readerOwner. lock ();
	n =  myReader	-> Samples ();
	readerOwner. unlock ();
	return n;
}

int32_t	cardReader::getSamples	(DSPCOMPLEX *b, int32_t a, uint8_t m) {
int32_t i, n;
DSPCOMPLEX temp [a];

	readerOwner. lock ();
	n =  myReader -> getSamples (temp, a, m);
	readerOwner. unlock ();
	for (i = 0; i < n; i ++)
	   b [i] = cmul (temp [i], float (gainValue) / 100);
	return n;
}

void	cardReader::set_streamSelector (int idx) {
	readerOwner. lock ();
	if (!myReader	-> set_StreamSelector (idx))
	   QMessageBox::warning (myFrame, tr ("sdr"),
	                               tr ("Selecting  input stream failed\n"));
	readerOwner. unlock ();
}

void	cardReader::set_rateSelector (const QString &s) {
int32_t	newRate	= s. toInt ();
int16_t	i;
	if (newRate == inputRate)
	   return;

	readerOwner. lock ();
	myReader	-> stopReader ();
	disconnect (cardSelector, SIGNAL (activated (int)),
	            this, SLOT (set_streamSelector (int)));
	for (i = cardSelector -> count (); i > 0; i --)
	   cardSelector -> removeItem (cardSelector -> currentIndex ()); 
	delete myReader;

	inputRate	= newRate;
	myReader	= new paReader (inputRate, cardSelector);
	connect (cardSelector, SIGNAL (activated (int)),
	         this, SLOT (set_streamSelector (int)));
	readerOwner. unlock ();
	emit set_changeRate (newRate);
//	Now just wait until the caller starts again
}

void	cardReader::set_gainValue	(int n) {
	gainValue	= n;
	gainDisplay	-> display (n);
}

int16_t	cardReader::bitDepth	(void) {
	return 24;
}

#if QT_VERSION < 0x050000
QT_BEGIN_NAMESPACE
Q_EXPORT_PLUGIN2(device_cardReader, cardReader)
QT_END_NAMESPACE
#endif

