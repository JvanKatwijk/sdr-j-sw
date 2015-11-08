/*
 *
 *	Copyright (C) 2008, 2009, 2010
 *	Jan van Katwijk (J.vanKatwijk@gmail.com)
 *	Lazy Chair Programming
 *
 *	The code is adapted from code contained in gmfsk source code 
 *	distribution. Copyrights gratefully acknowledged
 *	Copyright (C) 2005
 *	Tomi Manninen (oh2bns@sral.fi)
 *
 *	The software is essentially a wrapper around the olivia decoder
 *	made by Pawel Jalocha (Copyright 2004)
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
 *
 */

#include	"olivia-decoder.h"
#include	"utilities.h"
#include	<QSettings>

#define	BUFSIZE	512
#define	IF	1500

QWidget	*oliviaDecoder::createPluginWindow	(int32_t rate,
	                                         QSettings *s) {
	_WorkingRate		= rate;
	ISettings		= s;
	myFrame			= new QFrame;
	setupUi (myFrame);
	fprintf (stderr, "olivia Decoder is loaded\n");
	setup_Device	(rate);
	connect (oliviaTones, SIGNAL (activated (const QString &)),
	         this, SLOT (olivia_setTones (const QString &)));
	connect (oliviaBW, SIGNAL (activated (const QString &)),
	         this, SLOT (olivia_setBW (const QString &)));
	connect (oliviaSyncMargin, SIGNAL (valueChanged (int)),
	         this, SLOT (olivia_setSyncMargin (int)));
	connect (oliviaSyncIntegLen, SIGNAL (valueChanged (int)),
	         this, SLOT (olivia_setSyncIntegLen (int)));
	connect (oliviaSquelch, SIGNAL (activated (const QString &)),
	         this, SLOT (olivia_setSquelch (const QString &)));
	connect (oliviasquelchValue, SIGNAL (valueChanged (int)),
	         this, SLOT (olivia_setSquelchvalue (int)));
	connect (oliviaReverse, SIGNAL (activated (const QString &)),
	         this, SLOT (olivia_setReverse (const QString &)));

	olivia_clrText ();
	olivia_put_Status1 (" ");
	olivia_put_Status2 (" ");
	return myFrame;
}

	decoderInterface::~decoderInterface	(void) {
}

	oliviaDecoder::~oliviaDecoder		(void) {
	if (Receiver)
	   delete Receiver;
	delete[]	oliviaBuffer;
	delete[]	msg1;
	delete[]	msg2;
}

void	oliviaDecoder::setup_Device (int32_t wr) {
	_WorkingRate			= wr;
	oliviaBuffer		= new DSPFLOAT [BUFSIZE];
	inp			= 0;
	baseFrequency		= IF;
	frequency		= baseFrequency;
	snr			= 1e-3;
	msg1			= new char [20];
	msg2			= new char [20];
//
//	The default values for the relevant parameters
	Tones			= 32;
	Bandwidth		= 1000;
	SyncMargin		= 8;
	SyncIntegLen		= 4;
	SyncThreshold		= 3.2;
	SquelchOn		= false;
	squelchValue		= 5;
	reverse			= false;

//	just instantiate a new receiver
	Receiver		= new MFSK_Receiver<DSPFLOAT>;
	Receiver -> Tones	= 32;	// number of tones
	Receiver -> Bandwidth	= 1000;	// Bandwidth (Hz)
	Receiver -> SyncMargin	= 8;	//synchronizer tune margin
	Receiver -> SyncIntegLen	= 4;	// synchronizer integration period
	Receiver -> SyncThreshold	= 3.2;	// S/N threshold for printing
	Receiver -> SampleRate	= 8000;
	Receiver -> InputSampleRate	= _WorkingRate;
	Receiver -> Reverse	= false;
	if (Receiver -> Preset () < 0)
	   fprintf (stderr, "Insufficient RAM or another problem\n");

	fprintf (stderr, "\nOlivia Rx parameters:\n%s",
	                   Receiver -> PrintParameters ());
	lastFreq		= 0;
	escape			= false;
}

int32_t	oliviaDecoder::rateOut		(void) {
	return _WorkingRate;
}

int16_t	oliviaDecoder::detectorOffset	(void) {
	return IF;
}


bool	oliviaDecoder::initforRate (int32_t workingRate) {
	if (_WorkingRate == workingRate)
	   return true;

	_WorkingRate		= workingRate;
	Receiver	-> SampleRate		= 8000;
	Receiver	-> InputSampleRate	= _WorkingRate;
	restart ();
	setDetectorMarker	(IF);
	return true;
}
	
void	oliviaDecoder::olivia_setTones (const QString &s) {
	Tones	= s. toInt ();
	restart	();
}

void	oliviaDecoder::olivia_setBW (const QString &s) {
	bw	= s. toInt ();
	Bandwidth = bw;
	restart ();
}

void	oliviaDecoder::olivia_setSyncMargin (int n) {
	SyncMargin	= n;
	restart	();
}

void	oliviaDecoder::olivia_setSyncIntegLen (int n) {
	SyncIntegLen	= n;
	restart	();
}

void	oliviaDecoder::olivia_setSquelch (const QString &s) {
	SquelchOn	= s == "squelch on";
}

void	oliviaDecoder::olivia_setSquelchvalue (int n) {
	squelchValue = n;
}

void	oliviaDecoder::olivia_setReverse (const QString &s) {
	reverse	= s == "reverse";
}

void	oliviaDecoder::doDecode (DSPCOMPLEX v) {
int c;
unsigned char ch = 0;
int fc_offset = Bandwidth * (1.0 - 0.5 / Tones) / 2.0;

	oliviaBuffer [inp] = real (v);
	if (++inp < BUFSIZE)
	   return;

	inp	= 0;
	if ((lastFreq != frequency || Receiver -> Reverse) && !reverse) {
	   Receiver -> FirstCarrierMultiplier =
	                         (frequency - fc_offset)/500.0; 
	   Receiver -> Reverse = 0;
	   lastFreq = frequency;
	   Receiver -> Preset();
	}
	else if ((lastFreq != frequency || !Receiver -> Reverse) && reverse) {
	   Receiver -> FirstCarrierMultiplier =
	                              (frequency + fc_offset)/500.0; 
	   Receiver -> Reverse = 1;
	   lastFreq = frequency;
	   Receiver -> Preset();
	}

	Receiver -> SyncThreshold = SquelchOn ? 
		clamp(squelchValue / 5.0 + 3.0, 0, 90.0) : 0.0;

	Receiver -> Process(oliviaBuffer, BUFSIZE);
//
//	We ould aks here for the s/n
//
	bool gotchar = false;
	while (Receiver -> GetChar (ch) > 0) {
	   if ((c = unescape(ch)) != -1 && c > 7) {
	      olivia_addChar (c);
	      gotchar = true;
	   }
	}
	if (gotchar) {
	   snprintf(msg1, sizeof(msg1), "s/n: %4.1f dB", 10*log10(snr) - 20);
	   olivia_put_Status1 (msg1);
	   DSPFLOAT t = Receiver -> FrequencyOffset ();
	   snprintf(msg2, sizeof(msg2), "f/o %+4.1f Hz", t);
	   olivia_put_Status2 (msg2);
	}
	outputSample (real (v), imag (v));
}

void oliviaDecoder::restart (void) {
int32_t	fc_offset;

	Receiver -> Tones		= Tones;
	Receiver -> Bandwidth		= Bandwidth;
	Receiver -> SyncMargin		= SyncMargin;
	Receiver -> SyncIntegLen	= SyncIntegLen;

fprintf (stderr, "tones = %d, bandwidth = %d, margin = %d, integLen = %d\n",
	 Tones, Bandwidth, SyncMargin, SyncIntegLen);

	Receiver -> SyncThreshold	= SquelchOn ? 
		        clamp(squelchValue / 5.0 + 3.0, 0, 90.0) : 0.0;

	fc_offset = Receiver -> Bandwidth * (1.0 - 0.5 / Receiver -> Tones) / 2.0;
	if (reverse) { 
	   Receiver -> FirstCarrierMultiplier =
	                               (frequency + fc_offset)/500.0; 
	   Receiver -> Reverse = 1; 
	} else { 
	   Receiver -> FirstCarrierMultiplier =
	                               (frequency - fc_offset)/500.0; 
	   Receiver -> Reverse = 0; 
	}

	if (Receiver -> Preset() < 0) {
		fprintf (stderr, "olivia: receiver preset failed!");
		return;
	}

	fprintf (stderr, "\nOlivia Rx parameters:\n%s",
	                   Receiver -> PrintParameters ());

	olivia_clrText	();
}

int16_t oliviaDecoder::unescape (int16_t c) {

	if (escape) {
	   escape = false;
	   return c + 128;
	}

	if (c == 127) {
	   escape = 1;
	   return -1;
	}

	return c;
}

//	and the slots
void	oliviaDecoder::olivia_clrText	(void) {
	oliviaText = QString ("");
	oliviatextBox	-> setText (oliviaText);
}

void	oliviaDecoder::olivia_addChar	(char c) {
	if (c < ' ') c = ' ';
	oliviaText.append (QString (QChar (c)));
	if (oliviaText. length () > 120)
	   oliviaText. remove (0, 1);
	oliviatextBox	-> setText (oliviaText);
}

void	oliviaDecoder::olivia_put_Status1	(char *s) {
	olivia_status1_textBox -> setText (QString (s));
}

void	oliviaDecoder::olivia_put_Status2	(char *s) {
	olivia_status2_textBox -> setText (QString (s));
}

#if QT_VERSION < 0x050000
QT_BEGIN_NAMESPACE
Q_EXPORT_PLUGIN2(oliviadecoder, oliviaDecoder)
QT_END_NAMESPACE
#endif

