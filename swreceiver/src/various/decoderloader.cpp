#
/*
 *    Copyright (C) 2014
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
#include	"decoderloader.h"
#include	<QHBoxLayout>
#include	<QWidget>
#include	<QPluginLoader>
#include	<QString>
#include	<QFrame>
#include	<QDir>
#include	<gui.h>
#include	<QFileDialog>
#include	<QComboBox>
#include	<QStringList>
//
//	A "factory" for creating decoder objects
	decoderLoader::decoderLoader (QComboBox	*box,
	                              QString	searchPath,
	                              QFrame	*myFrame,
	                              QSettings *s) {
	this	-> box		= box;
	this	-> searchPath	= searchPath;
	this	-> myFrame	= myFrame;
	this	-> settings	= s;
	myLayout		= new QHBoxLayout;		// or whatever
	theDecoder		= NULL;
	decoderWindow		= NULL;
	myLoader		= NULL;
	somethingLoaded		= false;
}

	decoderLoader::~decoderLoader	(void) {
	unloadDecoder ();
	delete myLayout;
}

void		decoderLoader::unloadDecoder (void) {
	if (!somethingLoaded)
	   return;

	if (decoderWindow != NULL) {
	   myLayout		-> removeWidget (decoderWindow);
	   decoderWindow	-> deleteLater ();
	   decoderWindow	= NULL;
	}
	if (theDecoder != NULL) {
	   delete theDecoder;
	   theDecoder	= NULL;
	}

//	if there is an existing loader, unload first
	if ((myLoader != NULL) &&
	     (myLoader -> isLoaded ())) {
	   myLoader -> unload ();
	   delete myLoader;
	}
	myLoader	= NULL;
	somethingLoaded	= false;
}

decoderInterface	*decoderLoader::loadDecoder (QString s, int32_t rate) {
QString	fileName;

	if (somethingLoaded) {
	   fprintf (stderr, "Please unload first\n");
	   return NULL;
	}

	if (s == "no decoder")
	   return NULL;

	QTextStream out (stdout);

	fileName	= searchPath;
#ifdef	__MINGW32__
	fileName. append	("/");
#else
	fileName. append	("/lib");
#endif
	fileName. append	(s);
#ifdef	__MINGW32__
	fileName. append	(".dll");
#else
	fileName. append	(".so");
#endif
	fileName	= QDir::toNativeSeparators (fileName);

	fprintf (stderr, "found (decoder) %s\n",
	                 fileName. toLatin1 (). data ());
	out << QString ("going to load ") << fileName << endl;
	myLoader	= new QPluginLoader (fileName);
	QObject *plugin	= myLoader -> instance ();

	if (plugin == NULL) {
	   out << myLoader -> errorString () << endl;
           out << QString ("loading plugin file ") << fileName <<
	                          QString (" failed") << endl;
	   return NULL;
	}
//
//	OK, it seems the plugin was loaded, so let us connect

	theDecoder = qobject_cast<decoderInterface *>(plugin);
        if (theDecoder == NULL) {
            out << QString ("Plugin file ") << fileName <<
	                          QString (" is invalid") << endl;
	   return NULL;
	}

	decoderWindow	= theDecoder -> createPluginWindow (rate, settings);
	decoderWindow	-> setParent (myFrame);
	myLayout	-> addWidget (decoderWindow);
	myFrame		-> setLayout (myLayout);
	somethingLoaded	= true;
	return theDecoder;
}
//
//	The decoder table is created dynamically,
void	decoderLoader::setup_decoderTable (QDir d) {
int16_t	i;
QStringList	filters;
QStringList	fileList;

#ifdef	__MINGW32__
	filters << "*.dll";
#else
	filters << "*.so";
#endif
	d. setNameFilters (filters);
//
//	we make it "nice" names by removing the tail and the prefix
	fileList	= d. entryList ();
	box	-> insertItem (0, "no decoder", QVariant (0));
	for (i = 1; i < fileList. size () + 1; i ++) {
	   QString temp = fileList. at (i - 1);
fprintf (stderr, "%s \n", temp. toLatin1 (). data ());
#ifdef	__MINGW32__
	   temp. remove (".dll");
#else
	   temp. remove (".so");
	   temp. remove ("lib");
#endif
	   box	-> insertItem (i, temp, QVariant (i));
	}
}

