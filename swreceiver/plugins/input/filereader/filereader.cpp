#
/*
 *    Copyright (C) 2008, 2009, 2010
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
 */
#include	<QLabel>
#include	<QFileDialog>
#include	"swradio-constants.h"
#include	"filehulp.h"
#include	"filereader.h"

QWidget	*fileReader::createPluginWindow	(int32_t rate, QSettings *s) {
	this	-> rate		= rate;
	this	-> rate		= 96000;	// default
	(void)s;
	myFrame		= new QFrame;
	setupUi		(myFrame);
	myReader	= NULL;
	nameofFile	-> setText (QString ("no file"));
	this	-> rate	= setup_Device	();
	rateDisplay	-> display (this -> rate);
	this	-> lastFrequency	= Khz (14070);
	connect (resetButton, SIGNAL (clicked (void)),
	         this, SLOT (reset (void)));
	connect (progressBar, SIGNAL (sliderMoved (int)),
	         this, SLOT (handle_progressBar (int)));
	return myFrame;
}
//
//	just a dummy to have ~fileReader executed
	rigInterface::~rigInterface	(void) {
}

	fileReader::~fileReader	(void) {
	if (myReader != NULL)
	   delete myReader;
	myReader	= NULL;
	delete	myFrame;
}

int32_t	fileReader::getRate		(void) {
	return rate;
}

int32_t	fileReader::setup_Device	(void) {
int32_t	rate	= 96000;
	QString	replayFile
	              = QFileDialog::
	                 getOpenFileName (myFrame,
	                                  tr ("load file .."),
	                                  QDir::homePath (),
	                                  tr ("sound (*.wav)"));
	replayFile	= QDir::toNativeSeparators (replayFile);
	myReader	= new fileHulp (replayFile, &rate);
	connect (myReader, SIGNAL (set_progressBar (int)),
	         this, SLOT (set_progressBar (int)));
	connect (myReader, SIGNAL (samplesAvailable (int)),
	         this, SIGNAL (samplesAvailable (int)));
	nameofFile	-> setText (replayFile);
	set_progressBar	(0);
	return rate;
}

void	fileReader::handle_progressBar		(int f) {
	myReader	-> setFileat (f);
}

void	fileReader::set_progressBar	(int f) {
	progressBar	-> setValue (f);
}

void	fileReader::setVFOFrequency		(int32_t f) {
	lastFrequency	= f;
}

int32_t	fileReader::getVFOFrequency		(void) {
	return lastFrequency;
}

bool	fileReader::legalFrequency		(int32_t f) {
	return f > 0;
}

int32_t	fileReader::defaultFrequency		(void) {
	return 14070000;
}

bool	fileReader::restartReader		(void) {
	if (myReader != NULL)
	   return myReader -> restartReader ();
	else
	   return false;
}

void	fileReader::stopReader			(void) {
	if (myReader != NULL)
	   myReader	-> stopReader ();
}

void	fileReader::exit			(void) {
	if (myReader != NULL)
	   myReader	-> stopReader ();
}

bool	fileReader::isOK			(void) {
	return true;
}

int32_t	fileReader::Samples			(void) {
	if (myReader != NULL)
	   return myReader	-> Samples ();
	else
	   return 0;
}

int32_t	fileReader::getSamples			(DSPCOMPLEX *v,
	                                         int32_t a, uint8_t m) {
	if (myReader != NULL)
	   return	myReader -> getSamples (v, a, m);
	else
	   return 0;
}

int16_t	fileReader::bitDepth			(void) {
	return 16;
}

void	fileReader::reset			(void) {
	if (myReader != NULL)
	   myReader -> reset ();
}

#if QT_VERSION < 0x050000
QT_BEGIN_NAMESPACE
Q_EXPORT_PLUGIN2(device_filereader, fileReader)
QT_END_NAMESPACE
#endif

