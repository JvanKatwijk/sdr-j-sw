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
#include	<QLabel>
#include	<QHBoxLayout>
#include	<QSettings>
#include	"pa-reader.h"
#include	"no-rig.h"

	bool no_rig::createPluginWindow	(int32_t rate,
	                                 QFrame * myFrame, QSettings *s) {
	inputRate	= rate;
	(void)s;
	QLabel	*myLabel	= new QLabel (myFrame);
	myLabel	-> setText ("NO RIG");
	QHBoxLayout *myLayout	= new QHBoxLayout (myFrame);
	myLayout	-> addWidget (myLabel);
	myFrame		-> setLayout (myLayout);
	fprintf (stderr, "creating a no_rig\n");
	return true;
}

	rigInterface::~rigInterface	(void) {
}

	no_rig::~no_rig			(void) {
	fprintf (stderr, "\"no-rig\" will be deleted now\n");
}

int32_t	no_rig::getRate			(void) {
	return 96000;
}

void	no_rig::setVFOFrequency		(int32_t f) {
	myFrequency	= f;
}

int32_t	no_rig::getVFOFrequency		(void) {
	return myFrequency;
}

bool	no_rig::legalFrequency		(int32_t f) {
	return f > 0;
}

int32_t	no_rig::defaultFrequency	(void) {
	return Khz (14070);
}

bool	no_rig::restartReader		(void) {
	return false;
}

void	no_rig::stopReader		(void) {
}

int32_t	no_rig::Samples			(void) {
	return 0;
}

int32_t	no_rig::getSamples		(DSPCOMPLEX *b, int32_t a, uint8_t m) {
	(void)b; (void)a; (void)m;
	return 0;
}

int16_t	no_rig::bitDepth		(void) {
	return 24;
}

void	no_rig::exit			(void) {
}

bool	no_rig::isOK			(void) {
	return true;
}

