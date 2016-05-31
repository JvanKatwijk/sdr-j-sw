#
/*
 *    Copyright (C) 2013, 2014, 2015
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Programming
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
#include	<QPushButton>
#include	<QObject>
#include	<QSettings>
#include	<stdio.h>
#include	"analog-decoder.h"
#include	"fir-filters.h"
#include	"pllC.h"
#include	"squelchClass.h"

#include	"decoders.h"

QWidget *analogDecoder::createPluginWindow (int32_t rate,
	                                    QSettings *s) {
QString h;
int32_t	k;
int32_t	squelchFreq;

	theRate		= rate;
	analogSettings	= s;

	myFrame		= new QFrame;
	setupUi (myFrame);
	fprintf (stderr, "Analog Decoder is loaded\n");

	lowpass			= new LowPassFIR (5, 0.45 * rate, rate);
	adaptiveFiltersize	= 15;		/* the default	*/
	adaptive		= new adaptiveFilter (adaptiveFiltersize, 0.2);
	analog_Volume		= 30;
	ourDecoder		= new decoderBase (theRate);
	adaptiveFiltering 	= false;
	CycleCount		= 0;
	analog_IF		= 0;
/*
 *	for the decoders we need:
 */
	if (theRate <= 8000)
	   squelchFreq = theRate / 3;
	else
	   squelchFreq = 3000;
	mySquelch		= new squelch (1, squelchFreq,
	                                       theRate / 20, theRate);
	currLock		= 0.5;
	set_decoder		(analogDecoderSelect -> currentText ());
//
//	first set the sliders to the previous settings
	analogSettings	-> beginGroup ("analogDecoder");
	h	= analogSettings -> value ("analogDecoderSelect", "am"). toString ();
	k	= analogDecoderSelect -> findText (h);
	if (k != -1) {
	   analogDecoderSelect	-> setCurrentIndex (k);
	   set_decoder (h);
	}
	analogSettings	-> endGroup ();
	connect (analogDecoderSelect, SIGNAL (activated (const QString &)),
	         this, SLOT (set_decoder (const QString &)));
	connect (adaptiveFilterButton, SIGNAL (clicked (void)),
	         this, SLOT (set_adaptiveFilter (void)));
	connect (squelchSlider, SIGNAL (valueChanged (int)),
	         this, SLOT (set_squelch (int)));
	connect (adaptiveFilterslider, SIGNAL (valueChanged (int)),
	         this, SLOT (set_adaptiveFiltersize (int)));
	adaptiveFiltering	= false;
	return myFrame;
}

	decoderInterface::~decoderInterface	(void) {
}

	analogDecoder::~analogDecoder		(void) {
	analogSettings	-> beginGroup ("analogDecoder");
	analogSettings	-> setValue ("analogDecoderSelect", 
	                              analogDecoderSelect -> currentText ());
	analogSettings	-> endGroup ();
	delete	adaptive;
	delete	mySquelch;
	delete	myFrame;
	delete	lowpass;
}


bool	analogDecoder::initforRate	(int32_t workingRate) {
int16_t	squelchFreq;

	if (theRate == workingRate)
	   return true;

	theRate	= workingRate;
//	change some filters
	delete	adaptive;
	adaptive	= new adaptiveFilter	(adaptiveFiltersize, 0.2);
	squelchON		= false;
	squelchFreq		= theRate <= 8000 ?
	                                       theRate / 3 : 3000;
	mySquelch		= new squelch (0.1,
	                                       squelchFreq, 512, theRate);
	set_decoder (analogDecoderSelect -> currentText ());
	CycleCount		= theRate / 10;
	return true;
}

int32_t	analogDecoder::rateOut	(void) {
	return theRate;
}

int16_t	analogDecoder::detectorOffset	(void) {
	return analog_IF;
}

void	analogDecoder::set_decoder (const QString &s) {
int16_t	d;
	delete ourDecoder;

	if (s == "am") {
	   d	= analogDecoder::AM_DECODER;
	   ourDecoder	= new amDecoder (theRate);
	}
	else
	if (s == "fm") {
	   d	= analogDecoder::FM_DECODER;
	   ourDecoder	= new fmDecoder (theRate, mySquelch);
	}
	else
	if (s == "usb") {
	   d	= analogDecoder::USB_DECODER;
	   ourDecoder	= new usbDecoder (theRate);
	}
	else
	if (s == "lsb") {
	   d	= analogDecoder::LSB_DECODER;
	   ourDecoder	= new lsbDecoder (theRate);
	}
	else
	if (s == "isb") {
	   d	= analogDecoder::ISB_DECODER;
	   ourDecoder	= new isbDecoder (theRate);
	}
	else
	if (s == "coherent") {
	   d	= analogDecoder::COHERENT;
	   ourDecoder	= new syncDecoder (theRate);
	}
	else {
	   d	= analogDecoder::AM_DECODER;
	   ourDecoder	= new amDecoder (theRate);
	}

	detectorType	= d;
	this -> showDetectorType ();
	analog_IF = 0;
	setDetectorMarker (analog_IF);
}

void	analogDecoder::set_adaptiveFilter (void) {
	adaptiveFiltering	= !adaptiveFiltering;
	if (adaptiveFiltering)
	   adaptiveFilterButton -> setText ("on");
	else
	   adaptiveFilterButton	-> setText ("filter");
}

void	analogDecoder::set_adaptiveFiltersize (int d) {
	if (d < 2)
	   return;
	adaptiveFiltersize	= d;
	delete	adaptive;
	adaptive	= new adaptiveFilter	(adaptiveFiltersize, 0.2);
}

void	analogDecoder::showDetectorType () {
	switch (detectorType) {
	   case AM_DECODER:
	      analogMode -> setText (QString("Decoding AM"));
	      break;

	   case FM_DECODER:
	      analogMode -> setText (QString ("Decoding FM"));
	      break;

	   case USB_DECODER:
	      analogMode -> setText (QString ("Decoding USB"));
	      break;

	   case LSB_DECODER:
	      analogMode -> setText (QString ("Decoding LSB"));
	      break;

	   case ISB_DECODER:
	      analogMode -> setText (QString ("Decoding ISB"));
	      break;

	   case COHERENT:
	      analogMode  -> setText (QString ("Decoding AM with pll"));
	      break;

	   default:
	      analogMode -> setText (QString ("Do not know"));
	      break;
	}
}

void	analogDecoder::doDecode (DSPCOMPLEX z) {
double	power	=  abs (z);
double	analogStrength;
DSPCOMPLEX	lf;

	if (++CycleCount > theRate / 10) {
	   CycleCount = 0;
	   setDetectorMarker (analog_IF);
	   if (power > 0) {
	      analogStrength = (int)get_db (power);
	   }
	   else
	      analogStrength = 0;

	   signalStrength-> display (analogStrength);

	   if (detectorType == COHERENT)
	      showIF -> display	(currLock);
	   else
	      showIF -> display	(0);
	}

	lf	= ourDecoder -> decode (z, (float)analog_Volume / 10.0);

	if (adaptiveFiltering)
	   lf	= adaptive	-> Pass (lf);
	else
	   lf	= lowpass	->  Pass (lf);
	outputSample (real (lf), imag (lf));
}
/*
 */
void		analogDecoder::set_squelch (int n) {
	if (n < 2)
	   return;
	mySquelch	-> setSquelchLevel (n);
}

#if QT_VERSION < 0x050000
QT_BEGIN_NAMESPACE
Q_EXPORT_PLUGIN2(analogdecoder, analogDecoder)
QT_END_NAMESPACE
#endif

