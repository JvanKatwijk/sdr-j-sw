#
/*
 *
 *    Copyright (C) 2008, 2009, 2010, 2011, 2012
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair programming
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
 *	Main program
 */
#include	<QApplication>
#include	<QSettings>
#include	<QDir>
#include	"swradio-constants.h"
#include	"utilities.h"
#include	"gui.h"

void	fullPathfor (const char *v, char *out) {
int16_t	i;
QString	homeDir;

	if (v == NULL) {
	   sprintf (out, "%s", MISERY);
	   return;	// should not happen
	}

	if (v [0] == '/') {		// full path specified
	   sprintf (out, "%s", v);
	   return;
	}

	homeDir	= QDir::homePath ();
	homeDir. append ("/");
	homeDir. append (v);
	homeDir	= QDir::toNativeSeparators (homeDir);
	sprintf (out, "%s", homeDir. toLatin1 (). data ());
	fprintf (stderr, "ini file = %s\n", out);

	for (i = 0; out [i] != 0; i ++);
	if (out [i - 4] != '.' ||
	    out [i - 3] != 'i' ||
	    out [i - 2] != 'n' ||
	    out [i - 1] != 'i') {
	    out [i] = '.';
	    out [i + 1] = 'i';
	    out [i + 2] = 'n';
	    out [i + 3] = 'i';
	    out [i + 4] = 0;
	}
}

bool	fileExists (char *v) {
FILE *f;

	f = fopen (v, "r");
	if (f == NULL)
	   return false;
	fclose (f);
	return true;
}

int	main (int argc, char **argv) {
QSettings	*ISettings;			/* persistent settings	*/
int32_t		CardRate	= -1;
int32_t		OutputRate	= 96000;
RadioInterface	*MyRadioInterface;
char		*initFilename	= NULL;
char		*defaultInit	= (char *)alloca (1024 * sizeof (char));

	fullPathfor (".jsdr-sw.ini", defaultInit);

/*
 *	... and the settings of the "environment"
 */
	initFilename = defaultInit;
fprintf (stderr, "The init staat %s\n", initFilename);
	ISettings	= new QSettings (initFilename, QSettings::IniFormat);
/*
 *	Before we connect control to the gui, we have to
 *	instantiate
 */
	QApplication a (argc, argv);
	MyRadioInterface = new RadioInterface (ISettings,
	                                       CardRate,
	                                       OutputRate);

	MyRadioInterface -> show ();
	a. exec ();
/*
 *	done:
 */
	fflush (stdout);
	fflush (stderr);
	qDebug ("It is done\n");

	if (ISettings != NULL)
	   ISettings	-> ~QSettings ();
	delete MyRadioInterface;
	exit (1);
}

