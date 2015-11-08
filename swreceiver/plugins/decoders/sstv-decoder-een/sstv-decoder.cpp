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
#include	"sstv-decoder.h"
#include	<QPushButton>
#include	<QObject>
#include	<QSettings>
#include	<QLabel>
#include	<QPixmap>
#include	<stdio.h>
#include	"iir-filters.h"
#include	"fir-filters.h"
#include	"sstv-worker.h"

QWidget *sstvDecoder::createPluginWindow (int32_t rate, QSettings *s) {
	theRate		= rate;
	(void)s;
	myFrame		= new QFrame;
	setupUi (myFrame);
	fprintf (stderr, "sstvDecoder is loaded\n");
	theBuffer	= new RingBuffer<DSPCOMPLEX> (4096);
	sstvBandFilter	= new BandPassIIR (4, 
	                                   1000, 2400,
	                                   theRate, S_BUTTERWORTH);
	theScroller	= new QScrollArea (NULL);
	myImage		= new QImage (320, 320, QImage::Format_RGB32);
	theImageP	= &myImage;
	theScroller	-> show ();
	theWorker	= NULL;
	theFilter	= new HilbertFilter (25, 0.25, theRate);
	mono		= monoButton -> currentText () == "mono";
	ifdisplay	-> display (1900);
	connect (startButton, SIGNAL (clicked (void)),
	         this, SLOT (handle_startButton (void)));
	connect (monoButton, SIGNAL (activated (const QString &)),
	         this, SLOT (setMono (const QString &)));
	return myFrame;

}

	decoderInterface::~decoderInterface	(void) {
}

	sstvDecoder::~sstvDecoder		(void) {
	if (theWorker != NULL) {
	   theWorker	-> stop ();
	   while (!theWorker -> isFinished ())
	      usleep (100);
	   delete theWorker;
	   theWorker = NULL;
	}
	delete	sstvBandFilter;
	delete	theScroller;
	delete	myImage;
	delete	theBuffer;
}

int32_t	sstvDecoder::rateOut		(void) {
	return theRate;
}

int16_t	sstvDecoder::detectorOffset	(void) {
	return 0;
}

void	sstvDecoder::doDecode		(DSPCOMPLEX v) {
	if (mono) 
	   v  = theFilter	-> Pass (real (v), real (v));
	v	= sstvBandFilter -> Pass (v);
	if (theWorker != NULL)
	   theBuffer -> putDataIntoBuffer (&v, 1);
}

bool	sstvDecoder::initforRate	(int32_t rate) {
	theRate	= rate;
	if (theWorker != NULL) {
	   theWorker	-> stop ();
	   while (!theWorker -> isFinished ())
	      usleep (100);
	   delete theWorker;
	}
	theWorker	= new sstvWorker (theRate, theBuffer, theImageP);
	connect (theWorker, SIGNAL (displayImage (void)),
	         this, SLOT (displayImage (void)));
	connect (theWorker, SIGNAL (displayMode (QString)),
	         this, SLOT (displayMode (QString)));
	connect (theWorker, SIGNAL (displayStage (QString)),
	         this, SLOT (displayStage (QString)));
	connect (theWorker, SIGNAL (displaySNR (float)),
	         this, SLOT (displaySNR (float)));
	return true;
}

void	sstvDecoder::handle_startButton		(void) {
	if (theWorker != NULL) {
	   theWorker	-> stop ();
	   while (!theWorker -> isFinished ())
	      usleep (100);
	   delete theWorker;
	   theWorker = NULL;
	   startButton	-> setText ("start");
	   stageLabel	-> setText ("Idle");
	   return;
	}
	theWorker	= new sstvWorker (theRate, theBuffer, theImageP);
	connect (theWorker, SIGNAL (displayImage (void)),
	         this, SLOT (displayImage (void)));
	connect (theWorker, SIGNAL (displayMode (QString)),
	         this, SLOT (displayMode (QString)));
	connect (theWorker, SIGNAL (displayStage (QString)),
	         this, SLOT (displayStage (QString)));
	connect (theWorker, SIGNAL (displaySNR (float)),
	         this, SLOT (displaySNR (float)));
	startButton	-> setText ("stop");
	stageLabel	-> setText ("waiting for sync");
}

void	sstvDecoder::displayImage	(void) {
QLabel	*imageLabel	= new QLabel;
	
	imageLabel	-> setPixmap (QPixmap::fromImage (*myImage));
	theScroller	-> setWidget (imageLabel);
	imageLabel	-> show ();
}

void	sstvDecoder::displayMode	(QString s) {
	pictureMode -> setText (s);
}

void	sstvDecoder::setMono		(const QString &s) {
	mono	= s == "mono";
}

void	sstvDecoder::displayStage	(QString s) {
	stageLabel	-> setText (s);
}

void	sstvDecoder::displaySNR	(float snr) {
	snrdisplay	-> display (snr);
}

#if QT_VERSION < 0x050000
QT_BEGIN_NAMESPACE
Q_EXPORT_PLUGIN2(sstv_decoder, sstvDecoder)
QT_END_NAMESPACE
#endif
