#
/*
 *    Copyright (C) 2014
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair programming 
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
#include	<QFileDialog>
#include	<QString>
#include	<QHBoxLayout>
#include	<QFrame>
#include	<QWidget>
#include	<QPluginLoader>
#include	<QDir>
#include	<QSettings>
#include	<QStringList>
#include	<QComboBox>
#include	"deviceloader.h"
#include	"no-rig.h"
#
	deviceLoader::deviceLoader (QComboBox	*box,
	                            QString 	searchPath,
	                            QFrame 	*myFrame,
	                            QSettings *s) {
	this	-> box		= box;
	this	-> searchPath	= searchPath;
	this	-> myFrame	= myFrame;
	this	-> mySettings	= s;
	myLayout		= new QHBoxLayout;
	theRig			= NULL;
	rigWindow		= NULL;
	myLoader		= NULL;
	somethingLoaded		= false;
}

	deviceLoader::~deviceLoader (void) {
	if (somethingLoaded)
	   unloadDevice ();
	delete myLayout;
}

void	deviceLoader::unloadDevice	(void) {
	if (!somethingLoaded) {
	   fprintf (stderr, "Unloading NOTHING\n");
	   return;
	}
//	first: ensure that previous incarnation(s) are terminated
	if (theRig != NULL) 
	   theRig	-> stopReader ();

//	first: get rid of the frame of a previous decoder
	if (rigWindow != NULL) {
	   myLayout	-> removeWidget (rigWindow);
	   rigWindow	-> deleteLater ();
	   rigWindow	= NULL;
	}

	if (theRig != NULL)
	   delete theRig;
	theRig	= NULL;
//
//	if there is an existing loader, try to unload
	if ((myLoader != NULL) &&
	    (myLoader -> isLoaded ())) {
	   myLoader -> unload ();
	   delete myLoader;
	}
	myLoader	= NULL;
	somethingLoaded	= false;
}

//	find and load the rig, the abbreviated name is in string s
rigInterface	*deviceLoader::loadDevice (QString s, int32_t rate) {
QObject	*plugin;
QString	fileName;

	if (s == "no rig") {	// 
	   theRig	= new_noRig ();
	   goto L2;
	}

	fileName	= searchPath;
#ifdef	__MINGW32__
	fileName. append ("/device_");
	fileName. append (s);
	fileName. append (".dll");
#else
	fileName. append ("/libdevice_");
	fileName. append (s);
	fileName. append (".so");
#endif
	fileName	= QDir::toNativeSeparators (fileName);
	fprintf (stderr, "found %s\n", fileName. toLatin1 (). data ());
//	OK, file for plugin exists
	myLoader	= new QPluginLoader (fileName);
	plugin		= myLoader	-> instance ();
	if (plugin == NULL) {
	   fprintf (stderr, "loading plugin for %s failed\n",
	                         fileName. toLatin1 (). data ());
	   fprintf (stderr, " error = %s\n",
	             (myLoader -> errorString ()). toLatin1 (). data ());
	   theRig	= new_noRig ();
	}
	else
	   theRig	=
	      qobject_cast<rigInterface *> (plugin);
L2:	rigWindow	= theRig	-> createPluginWindow (rate,
	                                                       mySettings);
	rigWindow	-> show ();
	rigWindow	-> setParent (myFrame);
	myLayout	-> addWidget (rigWindow);
	myFrame		-> setLayout (myLayout);
	somethingLoaded	= true;
	return theRig;
}

rigInterface	*deviceLoader::new_noRig	(void) {
	return new no_rig ();
}

//	The rig table is created dynamically,
void	deviceLoader::setup_deviceTable (QDir d) {
int16_t	i;
QStringList	filters;
QStringList	fileList;

#ifdef	__MINGW32__
	filters << "device_*.dll";
#else
	filters << "libdevice_*.so";
#endif
	d. setNameFilters (filters);
//
//	we make it "nice" names by removing the tail and the prefix
	fileList	= d. entryList ();
	box	-> insertItem (0, "no rig", QVariant (0));
	for (i = 1; i < fileList. size () + 1; i ++) {
	   QString temp = fileList. at (i - 1);
#ifdef	__MINGW32__
	   temp. remove (".dll");
	   temp. remove ("device_");
#else
	   temp. remove (".so");
	   temp. remove ("libdevice_");
#endif
	   box	-> insertItem (i, temp, QVariant (i));
	}
}

