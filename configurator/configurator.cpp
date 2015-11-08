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
#include	<Qt>
#include	<QSettings>
#include	<QMessageBox>
#include	<QFileDialog>
#include	<QObject>
#include	"configurator.h"

	configurator::configurator (QSettings	*swreceiver,
	                            QWidget	*parent) {

	setupUi (this);
	this	-> swreceiver	= swreceiver;

	connect (devices, SIGNAL (clicked (void)),
	         this, SLOT (setDeviceDirectory (void)));
	connect (decoders, SIGNAL (clicked (void)),
	         this, SLOT (setDecoderDirectory (void)));
	connect	(saveButton, SIGNAL (clicked (void)),
	         this, SLOT (saveSettings (void)));
	connect	(quitButton, SIGNAL (clicked (void)),
	         this, SLOT (terminate (void)));
	deviceBase	= swreceiver -> value ("deviceBase", ""). toString ();
	decoderBase	= swreceiver -> value ("decoderBase", ""). toString ();
	if (deviceBase. isEmpty ()) 
	   deviceBase = QDir::currentPath (). append ("/../input-plugins");
	if (decoderBase. isEmpty ())
	   decoderBase = QDir::currentPath (). append ("/../decoder-plugins");
	deviceBase	= QDir::toNativeSeparators (deviceBase);
	decoderBase	= QDir::toNativeSeparators (decoderBase);
	deviceDirectory		-> setText (deviceBase);
	decoderDirectory	-> setText (decoderBase);
}

	configurator::~configurator	(void) {
}

void	configurator::saveSettings	(void) {
	if (!deviceBase. isEmpty ()) {
	   swreceiver	-> setValue ("deviceBase", deviceBase);
	}

	if (!decoderBase. isEmpty ()) {
	   swreceiver	-> setValue ("decoderBase", decoderBase);
	}
}

void	configurator::setDeviceDirectory	(void) {
	deviceBase = QFileDialog::getExistingDirectory (this,
	                          tr("Directory"), QDir::currentPath ());
	if (!deviceBase. isEmpty ())
	   deviceDirectory	-> setText (deviceBase);
}

void	configurator::setDecoderDirectory	(void) {
	decoderBase = QFileDialog::getExistingDirectory (this,
	                          tr("Directory"), QDir::currentPath ());
	if (!decoderBase. isEmpty ())
	   decoderDirectory	-> setText (decoderBase);
}

void	configurator::terminate (void) {
	accept ();
}

