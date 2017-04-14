#
/*
 *    Copyright (C) 2014
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Programming
 *
 *    This file is part of the SDR-J.
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

#include	<QThread>
#include	<QSettings>
#include	<QHBoxLayout>
#include	<QLabel>
#include	"sdrplay.h"	// our header
#include	"sdrplayselect.h"

#define	DEFAULT_GRED	40

//
//	Currently, we do not have lots of settings
static
void myStreamCallback (int16_t		*xi,
	               int16_t		*xq,
	               uint32_t		firstSampleNum, 
	               int32_t		grChanged,
	               int32_t		rfChanged,
	               int32_t		fsChanged,
	               uint32_t		numSamples,
	               uint32_t		reset,
	               void		*cbContext) {
int16_t	i;
sdrplay	*p	= static_cast<sdrplay *> (cbContext);
DSPCOMPLEX *localBuf = (DSPCOMPLEX *)alloca (numSamples * sizeof (DSPCOMPLEX));

	for (i = 0; i <  (int)numSamples; i ++)
	   localBuf [i] = DSPCOMPLEX (float (xi [i]) / 2048.0,
	                              float (xq [i]) / 2048.0);
	p -> _I_Buffer -> putDataIntoBuffer (localBuf, numSamples);
	p -> dataRead += numSamples;
	if (p -> dataRead > p -> inputRate / 8) {
	   p -> sendSignal ();
	   p -> dataRead	= 0;
	}
	(void)	firstSampleNum;
	(void)	grChanged;
	(void)	rfChanged;
	(void)	fsChanged;
	(void)	reset;
}

void	myGainChangeCallback (uint32_t	gRdB,
	                      uint32_t	lnaGRdB,
	                      void	*cbContext) {
	(void)gRdB;
	(void)lnaGRdB;	
	(void)cbContext;
}

bool	sdrplay::createPluginWindow (int32_t rate,
	                             QFrame *myFrame, QSettings *s) {
int	err;
float	ver;
bool	success;
mir_sdr_DeviceT devDesc [4];
mir_sdr_GainValuesT gainDesc;
sdrplaySelect   *sdrplaySelector;

	(void)rate;
	this	-> myFrame		= myFrame;
	this	-> sdrplaySettings	= s;
	deviceOK		= false;
	setupUi (myFrame);
	_I_Buffer		= NULL;
	Handle			= NULL;
	libraryLoaded		= false;
	outputRate		= rateSelector -> currentText (). toInt ();
	inputRate		= getInputRate (outputRate);
	decimationFactor	= inputRate / outputRate;
//
//	The largest decimation is app 42, the smallest 4.
	d_filter		= new DecimatingFIR (decimationFactor * 2 + 1,
	                                             - outputRate / 2,
	                                             + outputRate / 2,
	                                             inputRate,
	                                             decimationFactor);
	rateDisplay	-> display (inputRate);
//
#ifdef	__MINGW32__
HKEY APIkey;
wchar_t APIkeyValue [256];
ULONG APIkeyValue_length = 255;
	if (RegOpenKey (HKEY_LOCAL_MACHINE,
	                TEXT("Software\\MiricsSDR\\API"),
	                &APIkey) != ERROR_SUCCESS) {
          fprintf (stderr,
	           "failed to locate API registry entry, error = %d\n",
	           (int)GetLastError());
	   return false;
	}
	RegQueryValueEx (APIkey,
	                 (wchar_t *)L"Install_Dir",
	                 NULL,
	                 NULL,
	                 (LPBYTE)&APIkeyValue,
	                 (LPDWORD)&APIkeyValue_length);
//	Ok, make explicit it is in the 64 bits section
	wchar_t *x = wcscat (APIkeyValue, (wchar_t *)L"\\x86\\mir_sdr_api.dll");
//	wchar_t *x = wcscat (APIkeyValue, (wchar_t *)L"\\x64\\mir_sdr_api.dll");
//	fprintf (stderr, "Length of APIkeyValue = %d\n", APIkeyValue_length);
//	wprintf (L"API registry entry: %s\n", APIkeyValue);
	RegCloseKey (APIkey);

	Handle	= LoadLibrary (x);
	if (Handle == NULL) {
	  fprintf (stderr, "Failed to open mir_sdr_api.dll\n");
	  return false;
	}
#else
//	Ǹote that under Ubuntu, the Mirics shared object does not seem to be
//	able to find the libusb. That is why we explicity load it here
	Handle		= dlopen ("libusb-1.0.so", RTLD_NOW | RTLD_GLOBAL);

	Handle		= dlopen ("libmirsdrapi-rsp.so", RTLD_NOW);
	if (Handle == NULL)
	   Handle	= dlopen ("libmir_sdr.so", RTLD_NOW);

	if (Handle == NULL) {
	   fprintf (stderr, "error report %s\n", dlerror ());
	   return false;
	}
#endif
	libraryLoaded	= true;

	success = loadFunctions ();
	if (!(success)) {
	   fprintf (stderr, " No success in loading sdrplay lib\n");
	   return false;
	}

	err			= my_mir_sdr_ApiVersion (&ver);
	if (ver < 2.05) {
	   fprintf (stderr, "sorry, library too old\n");
	   statusLabel	-> setText ("mirics error");
	   return false;
	}

	api_version	-> display (ver);

	_I_Buffer	= new RingBuffer<DSPCOMPLEX>(2 * 1024 * 1024);
	vfoFrequency	= Khz (14700);
	sdrplaySettings	-> beginGroup ("sdrplaySettings");
	gainSlider              -> setValue (
                    sdrplaySettings -> value ("externalGain", 10). toInt ());
	ppmControl	-> setValue (
	            sdrplaySettings -> value ("ppmOffset", 0). toInt ());
        sdrplaySettings -> endGroup ();

        setExternalGain (gainSlider     -> value ());
	set_ppmControl  (ppmControl	-> value ());

        connect (gainSlider, SIGNAL (valueChanged (int)),
                 this, SLOT (setExternalGain (int)));
        connect (agcControl, SIGNAL (stateChanged (int)),
                 this, SLOT (agcControl_toggled (int)));
        connect (ppmControl, SIGNAL (valueChanged (int)),
                 this, SLOT (set_ppmControl (int)));
	connect (rateSelector, SIGNAL (activated (const QString &)),
	         this, SLOT (set_rateSelector (const QString &)));
        connect (debugBox, SIGNAL (stateChanged (int)),
                 this, SLOT (set_debugBox (int)));
        running         = false;

	my_mir_sdr_GetDevices (devDesc, &numofDevs, uint32_t (4));
	if (numofDevs == 0) {
	   fprintf (stderr, "Sorry, no device found\n");
	   return false;
	}
	if (numofDevs > 1) {
           sdrplaySelector       = new sdrplaySelect ();
           for (deviceIndex = 0; deviceIndex < numofDevs; deviceIndex ++) {
#ifndef	__MINGW32__
              sdrplaySelector ->
                   addtoList (devDesc [deviceIndex]. DevNm);
#else
              sdrplaySelector ->
                   addtoList (devDesc [deviceIndex]. SerNo);
#endif
           }
           deviceIndex = sdrplaySelector -> QDialog::exec ();
           delete sdrplaySelector;
        }
	else
	   deviceIndex = 0;

	serialNumber -> setText (devDesc [deviceIndex]. SerNo);
	hwVersion = devDesc [deviceIndex]. hwVer;
        fprintf (stderr, "hwVer = %d\n", hwVersion);

	if (hwVersion < 2) {
	   fprintf (stderr, "Sorry, we need an RSP II for this option\n");
	   return false;
	}

	int a = my_mir_sdr_SetDeviceIdx (deviceIndex);
	fprintf (stderr, "setDeviceIdx (%d) gives %d\n", deviceIndex, a);
	a = my_mir_sdr_AmPortSelect (1);
	fprintf (stderr, "port selection gives %d\n", a);
	unsigned char text;
	(void)my_mir_sdr_GetHwVersion (&text);

	deviceOK	= true;
	agcMode		= 0;
	return true;
}
//

	sdrplay::~sdrplay	(void) {
	stopReader ();
	sdrplaySettings	-> beginGroup ("sdrplaySettings");
	sdrplaySettings	-> setValue ("externalGain",
	                                          gainSlider -> value ());
	sdrplaySettings	-> setValue ("ppmOffset",
	                                          ppmControl -> value ());
	sdrplaySettings	-> endGroup ();

	if (!libraryLoaded)
	   return;
	if (numofDevs > 0)
	   my_mir_sdr_ReleaseDeviceIdx (deviceIndex);

	if (_I_Buffer != NULL)
	   delete _I_Buffer;

#ifdef __MINGW32__
	FreeLibrary (Handle);
#else
	dlclose (Handle);
#endif
	if (d_filter != NULL)
	   delete d_filter;
	d_filter	= NULL;
}

int32_t	sdrplay::getRate	(void) {
	return outputRate;
}

bool	sdrplay::legalFrequency (int32_t f) {
	return f < Mhz (60);
}

int32_t	sdrplay::defaultFrequency	(void) {
	return Khz (14070);
}

void	sdrplay::setVFOFrequency	(int32_t newFrequency) {
int     gRdBSystem;
int     samplesPerPacket;
mir_sdr_ErrT    err;
int localGred	= currentGred;

	if (!legalFrequency (newFrequency) || !deviceOK)
	   return;

	if (!running) {
	   vfoFrequency = newFrequency;
	   return;
	}

	my_mir_sdr_SetRf (newFrequency, 1, 0);

	vfoFrequency	= newFrequency;
        my_mir_sdr_SetPpm (float (ppmControl -> value ()));
}

int32_t	sdrplay::getVFOFrequency	(void) {
	return vfoFrequency;
}

int16_t	sdrplay::maxGain	(void) {
	return 101;
}

void	sdrplay::setExternalGain	(int newGain) {
	if (newGain < 0 || newGain > 101)
	   return;

	currentGred = maxGain () - newGain;
	int a = my_mir_sdr_SetGr (currentGred, 1, 0);
	fprintf (stderr, "gain set (%d) -> %d\n", maxGain () - newGain, a);
	gainDisplay -> display (newGain);
}

bool	sdrplay::restartReader	(void) {
int     gRdBSystem;
int     samplesPerPacket;
mir_sdr_ErrT    err;
int	localGred	= currentGred;

	if (running)
	   return true;
	_I_Buffer	-> FlushRingBuffer ();

        err     = my_mir_sdr_StreamInit (&localGred,
                                         double (inputRate) / MHz (1),
                                         double (vfoFrequency) / Mhz (1),
	                                 getBandwidth (outputRate),
                                         mir_sdr_IF_Zero,
                                         0,     // lnaEnable do not know yet
                                         &gRdBSystem,
                                         agcMode, // useGrAltMode,do not know yet
                                         &samplesPerPacket,
                                         (mir_sdr_StreamCallback_t)myStreamCallback,
                                         (mir_sdr_GainChangeCallback_t)myGainChangeCallback,
                                         this);
	fprintf (stderr, "streamInit %d (%d)\n", err, samplesPerPacket);
	if (err != mir_sdr_Success)
           return false;
        my_mir_sdr_DebugEnable (0);
	my_mir_sdr_SetPpm (double (ppmControl -> value ()));

        err             = my_mir_sdr_SetDcMode (4, 1);
        err             = my_mir_sdr_SetDcTrackTime (63);
//
        my_mir_sdr_SetSyncUpdatePeriod ((int)(inputRate / 2));
        my_mir_sdr_SetSyncUpdateSampleNum (samplesPerPacket);
//      my_mir_sdr_AgcControl (1, -30, 0, 0, 0, 0, 0);
        my_mir_sdr_DCoffsetIQimbalanceControl (0, 1);
        running         = true;
        return true;
}

void	sdrplay::stopReader	(void) {
	if (!running)
           return;

        my_mir_sdr_StreamUninit       ();
	msleep (1);		// cooling off period
        running         = false;
}

void	sdrplay::exit		(void) {
	stopReader ();
}

bool	sdrplay::isOK		(void) {
	return true;
}

//
//	Decimation is done here, i.e. for X samples, we
//	require the availability of X * decimationFactor samples
int32_t	sdrplay::getSamples (DSPCOMPLEX *V, int32_t size, uint8_t Mode) { 
int32_t		amount, i;
DSPCOMPLEX	buf [size * decimationFactor];
DSPCOMPLEX	temp;
int16_t		next	= 0;
	if (!deviceOK)
	   return 0;

	amount = _I_Buffer -> getDataFromBuffer (buf, size * decimationFactor);
	for (i = 0; i < amount; i ++)  {
	   if (!d_filter -> Pass (buf [i], &temp))
	      continue;
	   switch (Mode) {
	      default:
	      case IandQ:
	         V [next ++] = temp;
	         break;

	      case QandI:
	         V [next ++] = DSPCOMPLEX (imag (temp), real (temp));
	         break;

	      case Q_Only:
	         V [next ++] = DSPCOMPLEX (real (temp), 0.0);
	         break;

	      case I_Only:
	         V [next ++] = DSPCOMPLEX (imag (temp), 0.0);
	         break;
	   }
	}
	return amount / decimationFactor;
}

int32_t	sdrplay::Samples	(void) {
	if (!deviceOK)
	   return 0;
	return _I_Buffer	-> GetRingBufferReadAvailable () / decimationFactor;
}

//
bool	sdrplay::isValidRate	(int32_t rate) {
	return rate >= Khz (2000) && rate <= Khz (8000);
}

mir_sdr_Bw_MHzT	sdrplay::getBandwidth	(int32_t rate) {
static
	mir_sdr_Bw_MHzT validWidths [] = {
	mir_sdr_BW_0_200,
	mir_sdr_BW_0_300,
	mir_sdr_BW_0_600,
	mir_sdr_BW_1_536,
	mir_sdr_BW_5_000,
	mir_sdr_BW_6_000,
	mir_sdr_BW_7_000,
	mir_sdr_BW_8_000
};

uint16_t	i;
	for (i = 1; i < sizeof (validWidths) / sizeof (mir_sdr_Bw_MHzT); i ++)
	   if (rate / Khz (1) < (int)(validWidths [i]) &&
	       rate / Khz (1) >= (int)(validWidths [i - 1]))
	      return validWidths [i - 1];
	return mir_sdr_BW_0_200;
}
//
//	find a decent rate for the sdrplay
//	such that it can be decimated to rate. I just learned
//	that the legal rates start at 2M
int32_t	sdrplay::getInputRate (int32_t rate) {
int32_t	temp;

	temp	= rate;
	while (temp < Khz (2000))
	   temp += rate;
	return temp;
}
//
//	Note that we set the outputRate
//	The protocol is that the main program will
//	stop the reader, change the rate and restart
//	so all we have to do is to set a new filter
//	and the new rate
void	sdrplay::set_rateSelector (const QString &s) {
int32_t	newRate	= s. toInt ();

	if (newRate == outputRate)
	   return;

	stopReader ();
	outputRate	= newRate;
	inputRate	= getInputRate (outputRate);
	decimationFactor	= inputRate / outputRate;
	delete	d_filter;
	d_filter	= new DecimatingFIR (decimationFactor * 2 + 1,
	                                     - outputRate / 2,
	                                     + outputRate / 2,
	                                     inputRate,
	                                     decimationFactor);
	rateDisplay	-> display (inputRate);
	_I_Buffer	-> FlushRingBuffer ();
//
//	Note that restart reader will be done by the main program,
	emit set_changeRate (outputRate);
}

int16_t	sdrplay::bitDepth	(void) {
	return 12;
}

void    sdrplay::agcControl_toggled (int agcMode) {
	(void)agcMode;
        this    -> agcMode      = agcControl -> isChecked ();
	if (agcMode == 0)
	   setExternalGain (gainSlider -> value ());
}

void    sdrplay::set_debugBox   (int debugMode) {
	(void)debugMode;
        my_mir_sdr_DebugEnable (debugBox        -> isChecked ());
}

void    sdrplay::set_ppmControl (int ppm) {
        my_mir_sdr_SetPpm (float (ppm));
	my_mir_sdr_SetRf  (float (vfoFrequency), 1, 0);
}

bool	sdrplay::loadFunctions	(void) {

	my_mir_sdr_StreamInit	= (pfn_mir_sdr_StreamInit)
	                    GETPROCADDRESS (this -> Handle,
	                                    "mir_sdr_StreamInit");
	if (my_mir_sdr_StreamInit == NULL) {
	   fprintf (stderr, "Could not find mir_sdr_StreamInit\n");
	   return false;
	}

	my_mir_sdr_StreamUninit	= (pfn_mir_sdr_StreamUninit)
	                    GETPROCADDRESS (this -> Handle,
	                                    "mir_sdr_StreamUninit");
	if (my_mir_sdr_StreamUninit == NULL) {
	   fprintf (stderr, "Could not find mir_sdr_StreamUninit\n");
	   return false;
	}

	my_mir_sdr_SetRf	= (pfn_mir_sdr_SetRf)
	                    GETPROCADDRESS (Handle, "mir_sdr_SetRf");
	if (my_mir_sdr_SetRf == NULL) {
	   fprintf (stderr, "Could not find mir_sdr_SetRf\n");
	   return false;
	}

	my_mir_sdr_SetFs	= (pfn_mir_sdr_SetFs)
	                    GETPROCADDRESS (Handle, "mir_sdr_SetFs");
	if (my_mir_sdr_SetFs == NULL) {
	   fprintf (stderr, "Could not find mir_sdr_SetFs\n");
	   return false;
	}

	my_mir_sdr_SetGr	= (pfn_mir_sdr_SetGr)
	                    GETPROCADDRESS (Handle, "mir_sdr_SetGr");
	if (my_mir_sdr_SetGr == NULL) {
	   fprintf (stderr, "Could not find mir_sdr_SetGr\n");
	   return false;
	}

	my_mir_sdr_SetGrParams	= (pfn_mir_sdr_SetGrParams)
	                    GETPROCADDRESS (Handle, "mir_sdr_SetGrParams");
	if (my_mir_sdr_SetGrParams == NULL) {
	   fprintf (stderr, "Could not find mir_sdr_SetGrParams\n");
	   return false;
	}

	my_mir_sdr_SetDcMode	= (pfn_mir_sdr_SetDcMode)
	                    GETPROCADDRESS (Handle, "mir_sdr_SetDcMode");
	if (my_mir_sdr_SetDcMode == NULL) {
	   fprintf (stderr, "Could not find mir_sdr_SetDcMode\n");
	   return false;
	}

	my_mir_sdr_SetDcTrackTime	= (pfn_mir_sdr_SetDcTrackTime)
	                    GETPROCADDRESS (Handle, "mir_sdr_SetDcTrackTime");
	if (my_mir_sdr_SetDcTrackTime == NULL) {
	   fprintf (stderr, "Could not find mir_sdr_SetDcTrackTime\n");
	   return false;
	}

	my_mir_sdr_SetSyncUpdateSampleNum = (pfn_mir_sdr_SetSyncUpdateSampleNum)
	               GETPROCADDRESS (Handle, "mir_sdr_SetSyncUpdateSampleNum");
	if (my_mir_sdr_SetSyncUpdateSampleNum == NULL) {
	   fprintf (stderr, "Could not find mir_sdr_SetSyncUpdateSampleNum\n");
	   return false;
	}

	my_mir_sdr_SetSyncUpdatePeriod	= (pfn_mir_sdr_SetSyncUpdatePeriod)
	                GETPROCADDRESS (Handle, "mir_sdr_SetSyncUpdatePeriod");
	if (my_mir_sdr_SetSyncUpdatePeriod == NULL) {
	   fprintf (stderr, "Could not find mir_sdr_SetSyncUpdatePeriod\n");
	   return false;
	}

	my_mir_sdr_ApiVersion	= (pfn_mir_sdr_ApiVersion)
	                GETPROCADDRESS (Handle, "mir_sdr_ApiVersion");
	if (my_mir_sdr_ApiVersion == NULL) {
	   fprintf (stderr, "Could not find mir_sdr_ApiVersion\n");
	   return false;
	}

	my_mir_sdr_AgcControl	= (pfn_mir_sdr_AgcControl)
	                GETPROCADDRESS (Handle, "mir_sdr_AgcControl");
	if (my_mir_sdr_AgcControl == NULL) {
	   fprintf (stderr, "Could not find mir_sdr_AgcControl\n");
	   return false;
	}

	my_mir_sdr_Reinit	= (pfn_mir_sdr_Reinit)
	                GETPROCADDRESS (Handle, "mir_sdr_Reinit");
	if (my_mir_sdr_Reinit == NULL) {
	   fprintf (stderr, "Could not find mir_sdr_Reinit\n");
	   return false;
	}

	my_mir_sdr_SetPpm	= (pfn_mir_sdr_SetPpm)
	                GETPROCADDRESS (Handle, "mir_sdr_SetPpm");
	if (my_mir_sdr_SetPpm == NULL) {
	   fprintf (stderr, "Could not find mir_sdr_SetPpm\n");
	   return false;
	}

	my_mir_sdr_DebugEnable	= (pfn_mir_sdr_DebugEnable)
	                GETPROCADDRESS (Handle, "mir_sdr_DebugEnable");
	if (my_mir_sdr_DebugEnable == NULL) {
	   fprintf (stderr, "Could not find mir_sdr_DebugEnable\n");
	   return false;
	}

	my_mir_sdr_DCoffsetIQimbalanceControl	=
	                     (pfn_mir_sdr_DCoffsetIQimbalanceControl)
	                GETPROCADDRESS (Handle, "mir_sdr_DCoffsetIQimbalanceControl");
	if (my_mir_sdr_DCoffsetIQimbalanceControl == NULL) {
	   fprintf (stderr, "Could not find mir_sdr_DCoffsetIQimbalanceControl\n");
	   return false;
	}


	my_mir_sdr_ResetUpdateFlags	= (pfn_mir_sdr_ResetUpdateFlags)
	                GETPROCADDRESS (Handle, "mir_sdr_ResetUpdateFlags");
	if (my_mir_sdr_ResetUpdateFlags == NULL) {
	   fprintf (stderr, "Could not find mir_sdr_ResetUpdateFlags\n");
	   return false;
	}

	my_mir_sdr_GetDevices		= (pfn_mir_sdr_GetDevices)
	                GETPROCADDRESS (Handle, "mir_sdr_GetDevices");
	if (my_mir_sdr_GetDevices == NULL) {
	   fprintf (stderr, "Could not find mir_sdr_GetDevices");
	   return false;
	}

	my_mir_sdr_GetCurrentGain	= (pfn_mir_sdr_GetCurrentGain)
	                GETPROCADDRESS (Handle, "mir_sdr_GetCurrentGain");
	if (my_mir_sdr_GetCurrentGain == NULL) {
	   fprintf (stderr, "Could not find mir_sdr_GetCurrentGain");
	   return false;
	}

	my_mir_sdr_GetHwVersion	= (pfn_mir_sdr_GetHwVersion)
	                GETPROCADDRESS (Handle, "mir_sdr_GetHwVersion");
	if (my_mir_sdr_GetHwVersion == NULL) {
	   fprintf (stderr, "Could not find mir_sdr_GetHwVersion");
	   return false;
	}

	my_mir_sdr_RSPII_AntennaControl	= (pfn_mir_sdr_RSPII_AntennaControl)
	                GETPROCADDRESS (Handle, "mir_sdr_RSPII_AntennaControl");
	if (my_mir_sdr_RSPII_AntennaControl == NULL) {
	   fprintf (stderr, "Could not find mir_sdr_RSPII_AntennaControl");
	   return false;
	}

	my_mir_sdr_SetDeviceIdx	= (pfn_mir_sdr_SetDeviceIdx)
	                GETPROCADDRESS (Handle, "mir_sdr_SetDeviceIdx");
	if (my_mir_sdr_SetDeviceIdx == NULL) {
	   fprintf (stderr, "Could not find mir_sdr_SetDeviceIdx");
	   return false;
	}

	my_mir_sdr_ReleaseDeviceIdx	= (pfn_mir_sdr_ReleaseDeviceIdx)
	                GETPROCADDRESS (Handle, "mir_sdr_ReleaseDeviceIdx");
	if (my_mir_sdr_ReleaseDeviceIdx == NULL) {
	   fprintf (stderr, "Could not find mir_sdr_ReleaseDeviceIdx");
	   return false;
	}

	my_mir_sdr_AmPortSelect	= (pfn_mir_sdr_AmPortSelect)
	                GETPROCADDRESS (Handle, "mir_sdr_AmPortSelect");
	if (my_mir_sdr_AmPortSelect == NULL) {
	   fprintf (stderr, "Could not find mir_sdr_AmPortSelect");
	   return false;
	}

	return true;
}

void	sdrplay::sendSignal (void) {
	emit samplesAvailable (10);
}

#if QT_VERSION < 0x050000
QT_BEGIN_NAMESPACE
Q_EXPORT_PLUGIN2(sdrplay, sdrplay)
QT_END_NAMESPACE
#endif

