#
/*
 *    Copyright (C) 2010, 2011, 2012
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
//
//	Simple HELL decoder.
//	Parameter (Mode) values taken from fldigi
//
#include	"hell-decoder.h"
#include	"oscillator.h"
#include	<QSettings>

#define	BufferSize	590	// pixels for display
#define	HELL_IF		800

QWidget	*hellDecoder::createPluginWindow (int32_t rate, QSettings *s) {
	theRate		= rate;
	hellSettings	= s;
	myFrame		= new QFrame;
	setupUi (myFrame);
	fprintf (stderr, "hellDecoder is loaded\n");
	setup_hellDecoder (rate);
	return myFrame;
}

	decoderInterface::~decoderInterface	(void) {
}

	hellDecoder::~hellDecoder		(void) {
	hellSettings	-> beginGroup ("hellDecoder");
	hellSettings	-> setValue ("hellMode",
	                             hell_modeSelector -> currentText ());
	hellSettings	-> setValue ("hell_rateCorrector",
	                             hell_rateCorrector -> value ());
	hellSettings	-> endGroup ();
	delete	smoothener;
	if (averager != NULL)
	   delete averager;
	delete	imageLabel;
	delete	localOscillator;
	delete	myFrame;
}

void	hellDecoder::setup_hellDecoder (int32_t rate) {
int16_t	i, j;

	theRate		= rate;
	bandpass		= new fftFilter (1024, 127);
	averager		= NULL;	// depends on settings
	smoothener		= new average (8);
	hellIF			= HELL_IF;
	localOscillator		= new Oscillator (theRate);
	sampleCounter		= 0.0;
	maxValue 		= 0.0;
	blackBackground		= false;
	reverse			= false;
	memset (columnBuffer, 0, 2 * RxColumnLen * sizeof (int16_t));
	columnBufferp		= 0;
	agc			= 0.0;
	rateCorrector		= 0;
	fullWidth		= true;	
	hellSettings		-> beginGroup ("hellDecoder");
	rateCorrector		= hellSettings ->
	                             value ("hell_rateCorrector", 0).
	                                                         toInt ();
	hell_rateCorrector	-> setValue (rateCorrector);
	QString temp		= hellSettings -> value ("hellMode",
	                               hell_modeSelector -> currentText ()).
	                                                          toString ();
	int16_t k	= hell_modeSelector -> findText (temp);
	if (k != -1)
	   hell_modeSelector	-> setCurrentIndex (k);
	selectMode		(hell_modeSelector -> currentText ());
	hellSettings		-> endGroup ();

	connect (hell_modeSelector, SIGNAL (activated (QString)),
	         this, SLOT (selectMode (QString)));
	connect (hell_switchBlackback, SIGNAL (clicked (void)),
	         this, SLOT (switchBlackback (void)));
	connect (hell_switchWidth, SIGNAL (clicked (void)),
	         this, SLOT (switchWidth (void)));
	connect (hell_rateCorrector, SIGNAL (valueChanged (int)),
	         this, SLOT (set_rateCorrector (int)));
//
//	and for the dot canvas we have
	image			= QImage (BufferSize,
	                                  2 * RxColumnLen,
	                                  QImage::Format_RGB32);
	for (i = 0; i < image. width (); i ++)
	   for (j = 0; j < image. height (); j ++)
	      image. setPixel (QPoint (i, j), qRgb (i % 256, i % 256, i % 256));

	imageLabel		= new QLabel;
	imageLabel		-> setPixmap (QPixmap::fromImage (image));
	hellText		-> setWidget (imageLabel);
	imageLabel		-> show ();
}

bool	hellDecoder::initforRate	(int32_t rate) {
	if (theRate == rate)
	   return true;
	theRate	= rate;
	selectMode (hellMode);
	return true;
}

int32_t	hellDecoder::rateOut	(void) {
	return theRate;
}

int16_t	hellDecoder::detectorOffset	(void) {
	return hellIF;
}

void	 hellDecoder::doDecode (DSPCOMPLEX z){
//	as usual: first filter and shift
	z	= bandpass	-> Pass (z);
	z	= z * localOscillator -> nextValue (hellIF);

	switch (hellMode) {
	   case MODE_FSKHELL:
	   case MODE_FSKH105:
	   case MODE_HELL80:
	      decodeFSK (z);
	      break;

	   default:
	      decodeAM(z);
	      break;
	}

	outputSample (real (z), imag (z));
}
//
//	FSK: i.e. slicing
void hellDecoder::decodeFSK (DSPCOMPLEX z) {
float	f;
int16_t	pixel;
double avg;
//	first slicing
	f		= arg (conj (prevSample) * z) *
	                   theRate / M_PI / (hell_bandwidth / 2.0);
	prevSample	= z;
	f 		= smoothener -> filter (f);
	avg 		= averager -> filter (abs (z));

	sampleCounter += PixelsperSample;
	if (sampleCounter < 1.0)
	   return;
	sampleCounter -= 1.0;

	if (avg > agc)
	   agc = avg;
	else
	   agc *= (1.0 - 0.01 / RxColumnLen);
	metrics = clamp (1000 * agc, 0.0, 100.0);

	pixel = (int16_t)(255.0 *  clamp (f + 0.5, 0.0, 1.0));
	if (hellMode == MODE_HELL80)
	   pixel = 255 - pixel;
	if (reverse)
	   pixel = 255 - pixel;
	if (blackBackground)
	   pixel = 255 - pixel;

	storeDataPoint (pixel);
}
//
//	AM: envelope detection
void hellDecoder::decodeAM (DSPCOMPLEX z) {
double x, avg;
int16_t	pixel;

	x 	= abs (z); 	// x is the envelope
	avg	= averager -> filter (x);

	if (x > currentMax)
	   currentMax = x;

	sampleCounter += PixelsperSample;
	if (sampleCounter < 1.0)
	   return;
	sampleCounter -= 1.0;

	x = currentMax;
	currentMax = 0;
	if (x > maxValue)
	   maxValue = x;
	else
	   maxValue *= (1.0 - 0.02 / RxColumnLen);
	x = x / maxValue;
	pixel = (int16_t)(255.0 * clamp (x, 0.0, 1.0));

	if (avg > agc)
	   agc = avg;
	else
	   agc *= (1.0 - 0.01 / RxColumnLen);
	metrics = clamp (1000 * agc, 0.0, 100.0);

	storeDataPoint (blackBackground ? 255 - pixel : pixel);
}

void	hellDecoder::selectMode (QString s) {
	if (s == "feldhell")
	   hellMode	= MODE_FELDHELL;
	else
	if (s == "slowhell")
	   hellMode	= MODE_SLOWHELL;
	else
	if (s == "hellx5")
	   hellMode	= MODE_HELLX5;
	else
	if (s == "hellx9")
	   hellMode	= MODE_HELLX9;
	else
	if (s == "fskhell")
	   hellMode	= MODE_FSKHELL;
	else
	if (s == "fskh105")
	   hellMode	= MODE_FSKH105;
	else
	if (s == "fskh245")
	   hellMode	= MODE_FSKH245;
	else
	   hellMode	= MODE_HELL80;

	selectMode (hellMode);
}

void	hellDecoder::selectMode (int16_t mode) {
double	PixelsperSecond;
double	ColumsperSecond;

	hellMode = mode;
 	switch (mode) {
// Amplitude modulated:
	   default :
	   case MODE_FELDHELL:
 	      ColumsperSecond	= 17.5;
	      filter_bandwidth	= HELL_BW_FH;
	      break;

	   case MODE_SLOWHELL:
	      ColumsperSecond	= 2.1875;
	      filter_bandwidth	= HELL_BW_SH;
	      break;

	   case MODE_HELLX5:
	      ColumsperSecond	= 87.5;
	      filter_bandwidth	= HELL_BW_X5;
	      break;

	   case MODE_HELLX9:
	      ColumsperSecond	= 157.5;
	      filter_bandwidth	= HELL_BW_X9;
	      break;
// FSK based
	   case MODE_FSKHELL:
	      ColumsperSecond	= 17.5;
	      hell_bandwidth	= 122.5;
	      filter_bandwidth	= HELL_BW_FSK;
	      break;

	   case MODE_FSKH105:
	      ColumsperSecond	= 17.5;
	      hell_bandwidth	= 52.5;
	      filter_bandwidth	= HELL_BW_FSK105;
	      break;

	   case MODE_HELL80:
	      ColumsperSecond	= 35;
	      hell_bandwidth	= 300;
	      filter_bandwidth	= HELL_BW_HELL80;
	      break;

	   case MODE_FSKH245:
	      ColumsperSecond	= 17.5;
	      hell_bandwidth	= 122.5;
	      filter_bandwidth	= HELL_BW_FSK245;
	      break;
	}

	PixelsperSecond = (RxColumnLen * ColumsperSecond);
	PixelsperSample = (PixelsperSecond + rateCorrector) / theRate;
//	set the band for the fft filter
	bandpass -> setBand ( hellIF - filter_bandwidth / 2,
	                      hellIF + filter_bandwidth / 2, theRate);
	if (averager != NULL)
	   delete averager;
	averager	= new average (static_cast<int>(500 / PixelsperSample));
	displayColumn	= 0;
}

void	hellDecoder::displayMetrics (double v) {
	metricsDisplay -> display (v);
}

void	hellDecoder::addColumn (int16_t *v) {
int16_t	i;

	if (displayColumn > BufferSize - 1)
	   displayColumn = 0;

	for (i = 0; i < 2 * RxColumnLen; i ++) 
	   image. setPixel (QPoint (displayColumn,
	                    2 * RxColumnLen - i - 1),
	                    qRgb (v [i], v [i], v [i]));
	imageLabel		-> setPixmap (QPixmap::fromImage (image));
	hellText		-> setWidget (imageLabel);
	imageLabel		-> show ();
	setDetectorMarker	(hellIF);
	displayColumn ++;
	displayMetrics (metrics);
}

void	hellDecoder::storeDataPoint (int16_t v) {
int16_t	i;

	columnBuffer [RxColumnLen + columnBufferp] = v;

	if (++columnBufferp >= RxColumnLen) {
	   addColumn (columnBuffer);
	   if (fullWidth) 
	      addColumn (columnBuffer);

	   columnBufferp = 0;
	   for (i = 0; i < RxColumnLen; i++)
	      columnBuffer [i] = columnBuffer [RxColumnLen + i];
	}
}

void	hellDecoder::switchBlackback	(void) {
	blackBackground	= !blackBackground;
}

void	hellDecoder::switchWidth	(void) {
	fullWidth	= !fullWidth;
}

void	hellDecoder::set_rateCorrector	(int c) {
	rateCorrector = c;
	selectMode (hellMode);
}

#if QT_VERSION < 0x050000
QT_BEGIN_NAMESPACE
Q_EXPORT_PLUGIN2(helldecoder, hellDecoder)
QT_END_NAMESPACE
#endif

