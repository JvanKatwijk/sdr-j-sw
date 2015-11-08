#
/*
 *    Copyright (C) 2013
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
#include	<QSettings>
#include	"throb-decoder.h"
#include	"throb.h"

//
//	The throb decoder will delegate the actual work to the
//	rewrite of the code of Tomi Manninen for the throbdecoding.
//
QWidget	*throbDecoder::createPluginWindow (int32_t rate, QSettings *s) {
int16_t	k;

	theRate			= rate;
	throbSettings		= s;

	myFrame			= new QFrame;
	setupUi (myFrame);
	fprintf (stderr, "throbDecoder is loaded\n");
	throbMode	= MODE_THROB1;
	throbText 	= QString ("");
	throbTextbox	-> setText (throbText);
	my_Throb	= new throb (rate, throbMode, throbscope);
	connect	(my_Throb, SIGNAL (showChar (int16_t)),
	         this, SLOT (showChar (int16_t)));
	connect	(my_Throb, SIGNAL (clrText (void)),
	         this, SLOT (clrText (void)));
	connect	(my_Throb, SIGNAL (setMarker (float)),
	         this, SLOT (setMarker (float)));
	connect	(my_Throb, SIGNAL (showMetrics (double)),
	         this, SLOT (showMetrics (double)));
	connect (my_Throb, SIGNAL (showS2N (double)),
	         this, SLOT (showS2N (double)));
	throbSettings	-> beginGroup ("throbDecoder");
	k		= throbSettings -> value ("throbThreshold", 5). toInt ();
	threshold	-> setValue (k);
	throbSettings	-> endGroup ();

	connect (modeSelector, SIGNAL (activated (QString)),
	         this, SLOT (selectMode (QString)));
	connect (reverseSwitch, SIGNAL (clicked (void)),
	         this, SLOT (setReverse (void)));
	connect (afcSwitch_button, SIGNAL (clicked (void)),
	         this, SLOT (set_Afc (void)));
	connect (threshold, SIGNAL (valueChanged (int)),
	         this, SLOT (setThreshold (int)));
	throbReverse	= false;
	afcSwitch	= false;
	my_Throb	-> setReverse (throbReverse);
	my_Throb	-> setThreshold (threshold -> value ());
	return myFrame;
}

	decoderInterface::~decoderInterface	(void) {
}

	throbDecoder::~throbDecoder		(void) {
	throbSettings	-> beginGroup ("throbDecoder");
	throbSettings	-> setValue ("throbThreshold", 
	                              threshold -> value ());
	throbSettings	-> endGroup ();
	delete	my_Throb;
	delete	myFrame;
}

int32_t	throbDecoder::rateOut			(void) {
	return theRate;
}

int16_t	throbDecoder::detectorOffset		(void) {
	return THROB_IF;
}

//	a different setting for the rate
bool	throbDecoder::initforRate		(int32_t rate) {
	theRate	= rate;
	delete my_Throb;
	my_Throb	= new throb	(rate, throbMode, throbscope);
	connect	(my_Throb, SIGNAL (showChar (int16_t)),
	         this, SLOT (showChar (int16_t)));
	connect	(my_Throb, SIGNAL (clrText (void)),
	         this, SLOT (clrText (void)));
	connect	(my_Throb, SIGNAL (setMarker (float)),
	         this, SLOT (setMarker (float)));
	connect	(my_Throb, SIGNAL (showMetrics (double)),
	         this, SLOT (showMetrics (double)));
	connect (my_Throb, SIGNAL (showS2N (double)),
	         this, SLOT (showS2N (double)));
	my_Throb	-> setReverse	(throbReverse);
	my_Throb	-> setAfc	(afcSwitch);
	my_Throb	-> setThreshold (threshold -> value ());
	return true;
}
//
//	The most important one is the simplest of them all
void	throbDecoder::doDecode (DSPCOMPLEX z) {
DSPCOMPLEX temp = my_Throb -> doDecode (z);
	outputSample (real (z), imag (z));
}

void	throbDecoder::selectMode (QString s) {
	throbMode	= Modefor (s);
	delete my_Throb;
	my_Throb	= new throb (theRate, throbMode, throbscope);
	connect	(my_Throb, SIGNAL (showChar (int16_t)),
	         this, SLOT (showChar (int16_t)));
	connect	(my_Throb, SIGNAL (clrText (void)),
	         this, SLOT (clrText (void)));
	connect	(my_Throb, SIGNAL (setMarker (float)),
	         this, SLOT (setMarker (float)));
	connect	(my_Throb, SIGNAL (showMetrics (double)),
	         this, SLOT (showMetrics (double)));
	connect (my_Throb, SIGNAL (showS2N (double)),
	         this, SLOT (showS2N (double)));
	my_Throb	-> setReverse (throbReverse);
	my_Throb	-> setAfc (afcSwitch);
	my_Throb	-> setThreshold (threshold -> value ());
}

void	throbDecoder::setReverse (void) {
	throbReverse	= !throbReverse;
	reverseSwitch	-> setText (throbReverse ? "reverse" : "normal");
	my_Throb		-> setReverse (throbReverse);
}

void	throbDecoder::showS2N	(double s) {
	s2n_display -> display ((int)(floor (s)));
}

void	throbDecoder::showMetrics (double s) {
	metrics_display -> display ((int)(floor (s)));
}

void	throbDecoder::clrText (void) {
	throbText = QString ("");
	throbTextbox	-> setText (throbText);
}

void	throbDecoder::showChar (int16_t c) {
	c	= c & 0377;
	if (c < ' ') c = ' ';
	throbText.append (QString (QChar (c)));
	if (throbText. length () > 70)
	   throbText. remove (0, 1);
	throbTextbox	-> setText (throbText);
}

void	throbDecoder::setMarker (DSPFLOAT mark) {
	throb_ifMarker	-> display (mark);
	setDetectorMarker ((int)mark);
}

uint8_t	throbDecoder::Modefor (QString s) {
	if (s == "throb1")
	   return MODE_THROB1;
	if (s == "throb2")
	   return MODE_THROB2;
	if (s == "throb4")
	   return MODE_THROB4;
	if (s == "throbx1")
	   return MODE_THROBX1;
	if (s == "throbx2")
	   return MODE_THROBX2;
//	if (s == "throbx4")
	   return MODE_THROBX4;
}

void	throbDecoder::set_Afc	(void) {
	afcSwitch = !afcSwitch;
	afcSwitch_button	-> setText (afcSwitch ? "afc on" : "afc off");
	my_Throb	-> setAfc (afcSwitch);
}

void	throbDecoder::setThreshold	(int t) {
	my_Throb	-> setThreshold ((int16_t)t);
}

#if QT_VERSION < 0x050000
QT_BEGIN_NAMESPACE
Q_EXPORT_PLUGIN2(throbdecoder, throbDecoder)
QT_END_NAMESPACE
#endif

