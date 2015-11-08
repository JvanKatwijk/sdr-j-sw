#
/*
 *
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
 *	Main program configurator
 */
#include	<QApplication>
#include	<QSettings>
#include	"configurator.h"
#include	<unistd.h>
#include	<QDir>
#include	<Qt>

int	main (int argc, char **argv) {
configurator	*myConfigurator;
/*
 *	The default values
 */
QString	homeDir = QDir::homePath ();
	homeDir. append ("/");
QString	initsw	= homeDir;
QString	initfm	= homeDir;
QString	initbe	= homeDir;
	initsw. append (".jsdr-sw.ini");
	initsw	= QDir::toNativeSeparators (initsw);
	
QSettings *swSettings	= new QSettings (initsw, QSettings::IniFormat);
/*
 *	Before we connect control to the gui, we have to
 *	instantiate
 */
	QApplication a (argc, argv);
	myConfigurator = new configurator (swSettings);
	myConfigurator -> show ();
	a. exec ();
/*
 *	done:
 */
	fflush (stdout);
	fflush (stderr);
	qDebug ("It is done\n");
	myConfigurator	-> ~configurator ();
	swSettings	-> ~QSettings ();
	exit (1);
}

