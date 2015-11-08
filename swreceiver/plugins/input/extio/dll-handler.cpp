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
 */
#
#include	<QFileDialog>
#include	<QSettings>
#include	<QMessageBox>
#include	"swradio-constants.h"
#include	"dll-handler.h"
#include	"pa-reader.h"

#ifndef	__MINGW32__
#include	"dlfcn.h"
#endif
using namespace std;

#ifdef __MINGW32__
#define	GETPROCADDRESS	GetProcAddress
#define	FREELIBRARY	FreeLibrary
#else
#define	GETPROCADDRESS	dlsym
#define	FREELIBRARY	dlclose
#endif
//

#define	DEFAULT_RATE	96000
//	Interface routines
//	The callback from the dll does not contain a "context",
//	Our context here is the instance of the class, stored in
//	a static variable
static
dll_handler	*myContext	= NULL;

static
int	extioCallback (int cnt, int status, float IQoffs, void *IQData) {
	fprintf (stderr, "extio gave signal %d\n", status);
	if (cnt > 0) {
	   fprintf (stderr, "data received %d\n", cnt);
	}
	if (cnt < 0) {
	   switch (status) {
	      case extHw_Disconnected:
	      case extHw_READY:
	      case extHw_RUNNING:
	      case extHw_ERROR:
	      case extHw_OVERLOAD:
	             ;			// for now
	         break;

	      case extHw_Changed_SampleRate:	// sampling speed changed
	         if (myContext != NULL) 
	            myContext -> set_Changed_SampleRate (myContext -> GetHWSR ());
	         break;
	      case extHw_Changed_LO:		// LO frequency changed
	         if (myContext != NULL) 
	            myContext -> set_Changed_LO (myContext -> GetHWLO ()); 
	         break;
	      case extHw_Lock_LO:
	         if (myContext != NULL)
	            myContext -> set_Lock_LO ();
	         break;
	      case extHw_Unlock_LO:
	         if (myContext != NULL)
	            myContext -> set_Unlock_LO ();
	         break;
	      case extHw_Changed_LO_Not_TUNE:	// not implemented
	         break;
	      case extHw_Changed_TUNE:
	         if (myContext != NULL) {
	            long t = myContext -> GetTune ();
	            if (t != -1)
	               myContext -> set_Changed_TUNE (myContext -> GetTune ());
	         }
	         break;
	      case extHw_Changed_MODE:
	         fprintf (stderr, "Mode is changed to %d\n",
	                             myContext -> GetMode ());
	         break;
	      case extHw_Start:
	         if (myContext != NULL)
	            myContext -> set_StartHW ();
	         break;
	      case extHw_Stop:
	         if (myContext != NULL)
	            myContext -> set_StopHW ();
	         break;
	      case extHw_Changed_FILTER:
	         {  int x1, x2, x3;
	            myContext -> GetFilters (x1, x2, x3);
	            fprintf (stderr, "Filters to be changed %d %d %d\n",
	                                                x1, x2, x3);
	         }
	         break;
	      default:
	         break;
	   }
	   return 1;
	}
	return 1;
}
//
//	the seemingly additional complexity is caused by the fact
//	that naming of files in windows is in MultiBytechars
QWidget	*dll_handler::createPluginWindow (int32_t rate,
	                                  QSettings *s) {
	inputRate	= rate;
	(void)s;
	myReader	= NULL;
	myFrame		= new QFrame;
	
	setupUi	(myFrame);
	cardSelector	-> hide ();
	nameofdll	-> setText (QString ("no dll"));
	setup_Device (rate);
	this		-> lastFrequency	= KHz (14070);
	myContext	= (dll_handler *)this;
	return	myFrame;
}

	rigInterface::~rigInterface	(void) {
}

	dll_handler::~dll_handler (void) {
	if (dll_open) {
	   StopHW ();
	   CloseHW ();
	   FREELIBRARY (Handle);
	}
	delete myFrame;
}

int32_t	dll_handler::getRate	(void) {
	return inputRate;
}

void	dll_handler::setup_Device (int32_t rate) {
#ifdef	__MINGW32__
char	temp [256];
wchar_t	*windowsName;
int16_t	wchars_num;
#endif
	(void)rate;

	QString	dll_file
	              = QFileDialog::
	                 getOpenFileName (myFrame,
	                                  tr ("load file .."),
	                                  QDir::homePath (),
#ifdef	__MINGW32__
	                                  tr ("libs (*.dll)"));
#else
	                                  tr ("libs (*.so)"));
#endif
	dll_file	= QDir::toNativeSeparators (dll_file);
	if (dll_file == "") {
	   status	-> setText ("no file");
	   dll_open	= false;
	   return;
	}
#ifdef	__MINGW32__
	wchars_num = MultiByteToWideChar (CP_UTF8, 0,
	                              dll_file. toLatin1 (). data (),
	                              -1, NULL, 0);
	windowsName = new wchar_t [wchars_num];
	MultiByteToWideChar (CP_UTF8, 0,
	                     dll_file. toLatin1 (). data (),
	                     -1, windowsName, wchars_num);
	wcstombs (temp, windowsName, 128);
	Handle		= LoadLibrary (windowsName);
#else
	Handle		= dlopen (dll_file. toLatin1 (). data (), RTLD_NOW);
#endif

	if (Handle == NULL) {
	   nameofdll	-> setText (dll_file);
	   status	-> setText ("loading failed");
	   dll_open	= false;
	   return;
	}
//	apparently, the library is open, so record that
	dll_open	= true;

//	start binding addresses
	InitHW		= (pfnInitHW)GETPROCADDRESS (Handle, "InitHW");
	if (InitHW == NULL) {
	   fprintf (stderr, "Failed to load InitHW\n");
	   status	-> setText ("InitHW not there");
	   goto err;
	}

	OpenHW		= (pfnOpenHW)GETPROCADDRESS (Handle, "OpenHW");
	if (OpenHW == NULL) {
	   fprintf (stderr, "Failed to load OpenHW\n");
	   goto err;
	}

	StartHW		= (pfnStartHW)GETPROCADDRESS (Handle, "StartHW");
	if (StartHW == NULL) {
	   fprintf (stderr, "Failed to load StartHW\n");
	   goto err;
	}

	StopHW		= (pfnStopHW)GETPROCADDRESS (Handle, "StopHW");
	if (StopHW == NULL) {
	   fprintf (stderr, "Failed to load StopHW\n");
	   goto err;
	}

	CloseHW		= (pfnCloseHW)GETPROCADDRESS (Handle, "CloseHW");
	if (CloseHW == NULL) {
	   fprintf (stderr, "Failed to load CloseHW\n");
	   goto err;
	}

	GetHWLO		= (pfnGetHWLO)GETPROCADDRESS (Handle, "GetHWLO");
	if (GetHWLO == NULL) {
	   fprintf (stderr, "Failed to load GetHWLO\n");
	   goto err;
	}

	SetHWLO		= (pfnSetHWLO)GETPROCADDRESS (Handle, "SetHWLO");
	if (SetHWLO == NULL) {
	   fprintf (stderr, "Failed to load SetHWLO\n");
	   goto err;
	}

	GetStatus	= (pfnGetStatus)GETPROCADDRESS (Handle, "GetStatus");
	if (GetStatus == NULL) {
	   fprintf (stderr, "Failed to load GetStatus\n");
	   goto err;
	}

	SetCallback	= (pfnSetCallback)
	                      GETPROCADDRESS (Handle, "SetCallback");
	if (SetCallback == NULL) {
	   fprintf (stderr, "Failed to load SetCallback\n");
	   goto err;
	}


	L_ShowGUI	= (pfnShowGUI)GETPROCADDRESS (Handle, "ShowGUI");
	L_HideGUI	= (pfnHideGUI)GETPROCADDRESS (Handle, "HideGUI");
	L_GetHWSR	= (pfnGetHWSR)GETPROCADDRESS (Handle, "SetHWSR");
	L_GetFilters	= (pfnGetFilters)GETPROCADDRESS (Handle, "GetFilters");
	L_GetTune	= (pfnGetTune)GETPROCADDRESS (Handle, "GetTune");
	L_GetMode	= (pfnGetMode)GETPROCADDRESS (Handle, "GetMode");
//
//	and start the rig
	rigName		= new char [128];
	rigModel	= new char [128];
	if (!((*InitHW) (rigName, rigModel, hardwareType))) {
	   fprintf (stderr, "Sorry cannot init hardware\n");
	   status	-> setText ("Init failure");
	   fprintf (stderr, " hardware type = %d\n", hardwareType);
	   goto err;
	}

	nameofdll	-> setText (QString (rigName));

	switch (hardwareType) {
	   case exthwNone:
	   case exthwSDRX:
	   case exthwUSBdata24:
	   case exthwUSBdata32:
	   case exthwHPSDR:
	   case exthwUSBdata16:
	   case exthwSDR14:
	   default:
	      fprintf (stderr, "Sorry, mode %d not (yet) supported\n",
	                                                       hardwareType);
	      goto err;
	      break;

	   case exthwUSBfloat32:
	   case exthwSCdata:
	      break;
	}

	SetCallback (extioCallback);

	if ((*OpenHW)()) {
	   fprintf (stderr, "OpenHW successfull\n");
	   status -> setText (QString ("OpenHW success"));
	}
	else {
	   fprintf (stderr, "OpenHW not successful");
	   status -> setText (QString ("OpenHW fail"));
	   goto err;
	}

	if (hardwareType == exthwSCdata) {
	   cardSelector	-> show ();
	   myReader	= new paReader (inputRate, cardSelector);
	   connect (cardSelector, SIGNAL (activated (int)),
	            this, SLOT (set_Streamselector (int)));
	   connect (myReader, SIGNAL (samplesAvailable (int)),
	            this, SIGNAL (samplesAvailable (int)));
	}
	else
	if (hardwareType == exthwUSBfloat32);
//
//	we moeten dus eigenlijk een geunificeerde reader hebben,
//	aan envelope over de verschillende readers
	
	return;
err:
	FREELIBRARY (Handle);
	dll_open = false;
	return;
}

void	dll_handler::setVFOFrequency (int32_t f) {
	lastFrequency = f;
	(void) (*SetHWLO) ((int)f);
}

int32_t	dll_handler::getVFOFrequency (void) {
	lastFrequency	= (*GetHWLO)();
	return lastFrequency;
}

bool	dll_handler::legalFrequency	(int32_t f) {
	return true;
	return minFreq <= f && f <= maxFreq;
}

int32_t	dll_handler::defaultFrequency	(void) {
	return Khz (14070);
}
//
//	envelopes for functions that might or might not
//	be available
void	dll_handler::ShowGUI		(void) {
	if (L_ShowGUI != NULL)
	   (*L_ShowGUI) ();
}

void	dll_handler::HideGUI		(void) {
	if (L_HideGUI != NULL)
	   (*L_HideGUI) ();
}

long	dll_handler::GetHWSR		(void) {
	return L_GetHWSR != NULL ? (*L_GetHWSR) () : 0;
}

void	dll_handler::GetFilters		(int &x, int &y, int &z) {
	if (L_GetFilters != NULL)
	   L_GetFilters (x, y, z);
}

long	dll_handler::GetTune		(void) {
	return L_GetTune != NULL ? (*L_GetTune) () : -1;
}

uint8_t	dll_handler::GetMode		(void) {
	return L_GetMode != NULL ? (*L_GetMode)() : 0;
}
//
//
//	Handling the data
bool	dll_handler::restartReader	(void) {
fprintf (stderr, "restart is called\n");
	return myReader != NULL ?  myReader -> restartReader () : false;
}

void	dll_handler::stopReader		(void) {
fprintf (stderr, "stop is called\n");
	if (myReader != NULL)
	   myReader -> stopReader ();
}

void	dll_handler::exit		(void) {
	stopReader ();
}

int32_t	dll_handler::Samples		(void) {
	return myReader != NULL ? myReader -> Samples () : 0;
}

int32_t	dll_handler::getSamples		(DSPCOMPLEX *buffer,
	                                 int32_t number, uint8_t mode) {
	return myReader != NULL ? 
	                   myReader -> getSamples (buffer, number, mode) : 0;
}

void	dll_handler::set_Streamselector (int idx) {
	fprintf (stderr, "Setting stream %d\n", idx);
	if (!myReader	-> set_StreamSelector (idx))
	   QMessageBox::warning (myFrame, tr ("sdr"),
	                               tr ("Selecting  input stream failed\n"));
}
//
//	Signalling the main program:
void	dll_handler::set_Changed_SampleRate (int32_t newRate) {
	emit set_changeRate (newRate);
	inputRate	= newRate;
	myReader	-> stopReader ();
	delete myReader;
	myReader	= new paReader (inputRate, cardSelector);
	connect (cardSelector, SIGNAL (activated (int)),
	               this, SLOT (set_Streamselector (int)));
	return;
}

void	dll_handler::set_Changed_LO	(int32_t newFreq) {
	emit set_ExtLO (newFreq);
	lastFrequency	= newFreq + inputRate / 4;
}

void	dll_handler::set_Lock_LO	(void) {
	emit set_lockLO ();
}

void	dll_handler::set_Unlock_LO	(void) {
	emit set_Unlock_LO ();
}

void	dll_handler::set_Changed_TUNE	(int32_t newFreq) {
	emit set_ExtFrequency (newFreq);
	lastFrequency	= newFreq;
}

void	dll_handler::set_StartHW	(void) {
	emit set_startHW ();
}

void	dll_handler::set_StopHW		(void) {
	emit set_stopHW ();
}

int16_t	dll_handler::bitDepth		(void) {
	return 16;
}

#if QT_VERSION < 0x050000
QT_BEGIN_NAMESPACE
Q_EXPORT_PLUGIN2(device_dllhandler, dll_handler)
QT_END_NAMESPACE
#endif

