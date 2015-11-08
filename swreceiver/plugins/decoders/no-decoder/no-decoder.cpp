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
 *
 */
#
#include	"no-decoder.h"
#include	<QPushButton>
#include	<QObject>
#include	<stdio.h>
#include	<QSettings>

QWidget *noDecoder::createPluginWindow (int32_t rate, QSettings *s) {
	_WorkingRate		= rate;
	(void)s;
	myFrame			= new QFrame;
	setupUi (myFrame);
	fprintf (stderr, "noDecoder is loaded\n");
	return myFrame;
}

	decoderInterface::~decoderInterface	(void) {
}

	noDecoder::~noDecoder			(void) {
}

int32_t	noDecoder::rateOut		(void) {
	return _WorkingRate;
}

int16_t	noDecoder::detectorOffset	(void) {
	return 0;
}

void	noDecoder::doDecode	(DSPCOMPLEX v) {
	outputSample (real (v), imag (v));
}

bool	noDecoder::initforRate	(int32_t rate) {
	_WorkingRate	= rate;
	return true;
}

#if QT_VERSION < 0x050000
QT_BEGIN_NAMESPACE
Q_EXPORT_PLUGIN2(no_decoder, noDecoder)
QT_END_NAMESPACE
#endif

