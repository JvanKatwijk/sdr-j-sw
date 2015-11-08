#
/*
 *    Copyright (C) 2013, 2014
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
#include	"analog-decoder.h"
#include	<QPushButton>
#include	<QObject>
#include	<QSettings>
#include	<stdio.h>
#include	"fir-filters.h"
#include	"iir-filters.h"
#include	"pllC.h"
#include	"squelchClass.h"

QWidget *analogDecoder::createPluginWindow (int32_t rate,
	                                    QSettings *s) {
QString h;
int32_t	k;
	theRate		= rate;
	analogSettings	= s;
	myFrame		= new QFrame;
	setupUi (myFrame);
	fprintf (stderr, "Analog Decoder is loaded\n");
	setup_analogDecoder (rate);
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
	delete	SSB_Filter;
	delete	am_pll;
	delete	mySquelch;
}

void	analogDecoder::setup_analogDecoder	(int32_t rate) {
int16_t squelchFreq;

	theRate		= rate;
	adaptiveFiltersize	= 15;		/* the default	*/
	adaptive		= new adaptiveFilter (adaptiveFiltersize, 0.2);
	analog_Volume		= 30;
/*
 *	for the FM decoder we need:
 */
	Imin1			= 1;
	Imin2			= 1;
	Qmin1			= 1;
	Qmin2			= 1;

	detectorType		= AM_DECODER;
	SSB_Filter		= new HilbertFilter (31, 0.25, theRate);

	am_pll			= new pllC (theRate,
	                                   0,		// center frequency
	                                   - 500,	// low offset
	                                   + 500,	// high offset
	                                   4000);	// bandwidth

	if (theRate <= 8000)
	   squelchFreq = theRate / 3;
	else
	   squelchFreq = 3000;
	mySquelch		= new squelch (1, squelchFreq,
	                                       theRate / 20, theRate);
	currLock		= 0.5;
	prevLock		= 1.0;
	amDc			= 0;
	amSmooth		= 0;
	detectorType		= AM_DECODER;
        showDetectorType ();
	adaptiveFiltering = false;
	CycleCount	= 0;
	analog_IF	= 0;
}

bool	analogDecoder::initforRate	(int32_t workingRate) {
int16_t	squelchFreq;

	if (theRate == workingRate)
	   return true;

	theRate	= workingRate;
//	change some filters
	delete	adaptive;
	adaptive	= new adaptiveFilter	(adaptiveFiltersize, 0.2);

	delete	SSB_Filter;
	SSB_Filter		= new HilbertFilter (31, 0.25, theRate);

	delete	am_pll;
	am_pll			= new pllC (theRate,
	                                   0,
	                                   - 500,
	                                   + 500,
	                                   0.9 * theRate);

	squelchON		= true;
	squelchFreq		= theRate <= 8000 ?
	                                       theRate / 3 : 3000;
	mySquelch		= new squelch (0.1,
	                                       squelchFreq, 512, theRate);
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
	if (s == "am")
	   d	= analogDecoder::AM_DECODER;
	else
	if (s == "fm")
	   d	= analogDecoder::FM_DECODER;
	else
	if (s == "usb")
	   d	= analogDecoder::USB_DECODER;
	else
	if (s == "lsb")
	   d	= analogDecoder::LSB_DECODER;
	else
	if (s == "isb")
	   d	= analogDecoder::ISB_DECODER;
	else
	if (s == "coherent")
	   d	= analogDecoder::COHERENT;
	else
	   d	= analogDecoder::AM_DECODER;

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

	switch (detectorType) {
	   case AM_DECODER:
	      lf = amDecoder (z, (float)analog_Volume / 10.0);
	      break;

	   case FM_DECODER:
	      lf = fmDecoder (z, (float)analog_Volume / 10.0);
	      break;

	   case USB_DECODER:
	      lf = usbDecoder (z, (float)analog_Volume / 10.0);
	      break;

	   case LSB_DECODER:
	      lf = lsbDecoder (z, (float)analog_Volume / 10.0);
	      break;

	   case ISB_DECODER:
	      lf = isbDecoder (z, (float)analog_Volume / 10.0);
	      break;

	   case COHERENT:
	      lf = syncDecoder (z, (float)analog_Volume / 10.0);
	      break;

	   default:
	      lf = z;
	      break;
	}

	if (adaptiveFiltering)
	   lf	= adaptive	-> Pass (lf);
	outputSample (real (lf), imag (lf));
}
/*
 */
DSPCOMPLEX	analogDecoder::fmDecoder (DSPCOMPLEX z, double Vol) {
DSPFLOAT	res;
double	I	= real (z);
double	Q	= imag (z);

	res		= Imin1 * (Q - Qmin2) - Qmin1 * (I - Imin2);
	res		/= Imin1 * Imin1 + Qmin1 * Qmin1 ;
	Imin2		= Imin1;
	Qmin2		= Qmin1;
	Imin1		= I;
	Qmin1		= Q;
	res		= mySquelch -> do_squelch (res);
	return 	DSPCOMPLEX (Vol * res, Vol * res);
}

DSPCOMPLEX	analogDecoder::usbDecoder (DSPCOMPLEX z, double Vol) {
DSPCOMPLEX H;
	z	= SSB_Filter	-> Pass (z);
	H	= DSPCOMPLEX (Vol * (real (z) - imag (z)),
		           Vol * (real (z) - imag (z)));
	return H;
}

DSPCOMPLEX	analogDecoder::lsbDecoder (DSPCOMPLEX z, double Vol) {
DSPCOMPLEX H;
	z	= SSB_Filter	-> Pass (z);
	H	= DSPCOMPLEX (Vol * (real (z) + imag (z)),
			   Vol * (real (z) + imag (z)));
	return H;
}

DSPCOMPLEX	analogDecoder::isbDecoder (DSPCOMPLEX z, double Vol) {
DSPCOMPLEX H;
	z	= SSB_Filter -> Pass (z);
	H	= DSPCOMPLEX (Vol * (real (z) - imag (z)),
	                   Vol * (real (z) + imag (z)));
	return H;
}

#define	pllBeta	0.5
#define	pllGain	0.3
#define	NcoFreqHLimit	 0.8
#define	NcoFreqLLimit	 - 0.8
#define	DC_ALPHA	0.85

DSPCOMPLEX	analogDecoder::amDecoder (DSPCOMPLEX z, double Vol) {
	currLock	= Vol * abs (z);
	amDc		= 0.9999f * amDc + 0.0001f * currLock;
	amSmooth	= 0.5 * amSmooth + 0.5 * (currLock - amDc);
	return DSPCOMPLEX (amSmooth, amSmooth);
}
//
//	
DSPCOMPLEX	analogDecoder::syncDecoder (DSPCOMPLEX z, double Vol) {
DSPFLOAT	res;

	am_pll		-> do_pll (z);
	currLock	= 0.999 * currLock +
	                  0.001 * fabs (imag (am_pll -> getDelay ()));
	prevLock	= currLock;
	amDc		= 0.9999 * amDc + 0.0001 * real (am_pll -> getDelay ());
	res		= Vol * (real (am_pll -> getDelay ()) - amDc);
	return DSPCOMPLEX (res, res);
}

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

