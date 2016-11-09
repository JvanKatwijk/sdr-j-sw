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
#ifndef	__DEVICE_LOADER
#define	__DEVICE_LOADER
#
#include	<QObject>
#include	<QTextStream>
#include	"swradio-constants.h"
#include	"rig-interface.h"

class	QHBoxLayout;
class	QWidget;
class	QPluginLoader;
class	QString;
class	QFrame;
class	QDir;
class	QSettings;
class	QComboBox;

class	deviceLoader: public QObject {
Q_OBJECT
public:
		deviceLoader	(QComboBox *,
	                         QString,
	                         QSettings *);
		~deviceLoader	(void);
void		setup_deviceTable	(QDir);
rigInterface	*loadDevice	(QString, int32_t);
void		unloadDevice	(void);
private:
	rigInterface	*new_noRig	(void);
	void		*myRadio;
	QString		searchPath;
	QFrame		*myFrame;
	QSettings	*mySettings;

	QHBoxLayout	*myLayout;
	QFrame		*rigFrame;
	rigInterface	*theRig;
	QPluginLoader	*myLoader;
	QComboBox	*box;

	bool		somethingLoaded;
};

#endif

