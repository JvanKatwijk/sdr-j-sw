#
/*
 *    Copyright (C) 2014
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

#include	<QThread>
#include	<QSettings>
#include	<QHBoxLayout>
#include	<QLabel>

#include	"sdrplay.h"	// our header
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
static	int	dataRead	= 0;
sdrplay	*p	= static_cast<sdrplay *> (cbContext);
DSPCOMPLEX *localBuf = (DSPCOMPLEX *)alloca (numSamples * sizeof (DSPCOMPLEX));

	for (i = 0; i <  (int)numSamples; i ++)
	   localBuf [i] = DSPCOMPLEX (float (xi [i]) / 2048.0,
	                              float (xq [i]) / 2048.0);
	p -> _I_Buffer -> putDataIntoBuffer (localBuf, numSamples);
	dataRead += numSamples;
	if (dataRead > p -> inputRate / 8) {
	   p -> sendSignal ();
	   dataRead	= 0;
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

QWidget	*sdrplay::createPluginWindow (int32_t rate, QSettings *s) {
int	err;
float	ver;
bool	success;

	(void)rate;
	this	-> sdrplaySettings	= s;
	deviceOK		= false;
	myFrame			= new QFrame;
	setupUi (myFrame);
	_I_Buffer		= NULL;
	outputRate		= rateSelector -> currentText (). toInt ();
	inputRate		= getInputRate (outputRate);
	decimationFactor	= inputRate / outputRate;
	d_filter		= new DecimatingFIR (decimationFactor * 5,
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
	   return;
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
	RegCloseKey(APIkey);

	Handle	= LoadLibrary (x);
	if (Handle == NULL) {
	  fprintf (stderr, "Failed to open mir_sdr_api.dll\n");
	  return;
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
	   return NULL;
	}
#endif
	libraryLoaded	= true;

	success = loadFunctions ();
	if (!(success)) {
	   fprintf (stderr, " No success in loading sdrplay lib\n");
	   return NULL;
	}

	err			= my_mir_sdr_ApiVersion (&ver);
	if (ver != MIR_SDR_API_VERSION) {
	   fprintf (stderr, "Foute API: %f, %d\n", ver, err);
	   statusLabel	-> setText ("mirics error");
	}

	api_version	-> display (ver);

	_I_Buffer	= new RingBuffer<DSPCOMPLEX>(2 * 1024 * 1024);
	vfoFrequency	= Khz (94700);
	sdrplaySettings	-> beginGroup ("sdrplaySettings");
	gainSlider              -> setValue (
                    sdrplaySettings -> value ("externalGain", 10). toInt ());
	ppmControl	-> setValue (
	            sdrplaySettings -> value ("ppmOffset", 0). toInt ());
        sdrplaySettings -> endGroup ();

        setExternalGain (gainSlider     -> value ());
        connect (gainSlider, SIGNAL (valueChanged (int)),
                 this, SLOT (setExternalGain (int)));
        connect (agcControl, SIGNAL (stateChanged (int)),
                 this, SLOT (agcControl_toggled (int)));
        connect (ppmControl, SIGNAL (valueChanged (int)),
                 this, SLOT (set_ppmControl (int)));
        connect (debugBox, SIGNAL (stateChanged (int)),
                 this, SLOT (set_debugBox (int)));
        running         = false;

	deviceOK	= true;
	agcMode		= 0;
	return myFrame;
}
//
//	just a dummy
	rigInterface::~rigInterface	(void) {
}
//
//	that will allow a decent destructor
	sdrplay::~sdrplay	(void) {
	stopReader ();
	sdrplaySettings	-> beginGroup ("sdrplaySettings");
	sdrplaySettings	-> setValue ("externalGain",
	                                          gainSlider -> value ());
	sdrplaySettings	-> setValue ("ppmOffset",
	                                          ppmControl -> value ());
	sdrplaySettings	-> endGroup ();
	
	if (_I_Buffer != NULL)
	   delete _I_Buffer;
}

int32_t	sdrplay::getRate	(void) {
	return outputRate;
}

static inline
int16_t	bankFor_dongle (int32_t freq) {
	if (freq < 60 * MHz (1))
	   return -1;
	if (freq < 120 * MHz (1))
	   return 1;
	if (freq < 245 * MHz (1))
	   return 2;
	if (freq < 420 * MHz (1))
	   return -1;
	if (freq < 1000 * MHz (1))
	   return 3;
	return -1;
}

static inline
int16_t bankFor_sdr (int32_t freq) {
	if (freq < 100 * Khz (1))
	   return -1;
	if (MIR_SDR_API_VERSION > 1.5) {
	   if (freq < 12 * Mhz (1))
	      return 1;
	   if (freq < 30 * Mhz (1))
	      return 2;
	   if (freq < 60 * Mhz (1))
	      return 3;
	   if (freq < 120 * Mhz (1))
	      return 4;
	   if (freq < 250 * Mhz (1))
	      return 5;
	   if (freq < 420 * Mhz (1))
	      return 6;
	   if (freq < 1000 * Mhz (1))
	      return 7;
	   if (freq < 2000 * Mhz (1))
	      return 8;
	   return - 1;
	}
	else {   // old API
	   if (freq < 12 * MHz (1))
	      return 1;
	   if (freq < 30 * MHz (1))
	      return 2;
	   if (freq < 60 * MHz (1))
	      return 3;
	   if (freq < 120 * MHz (1))
	      return 4;
	   if (freq < 250 * MHz (1))
	      return 5;
	   if (freq < 380 * MHz (1))
	      return 6;
	   if (freq < 420 * MHz (1))
	      return -1;
	   if (freq < 1000 * MHz (1))
	      return 7;
	   return -1;
	}
}

bool	sdrplay::legalFrequency (int32_t f) {
	return bankFor_sdr (f) != -1;
}

int32_t	sdrplay::defaultFrequency	(void) {
	return Khz (14070);
}

void	sdrplay::setVFOFrequency	(int32_t newFrequency) {
int     gRdBSystem;
int     samplesPerPacket;
mir_sdr_ErrT    err;

	if (!legalFrequency (newFrequency) || !deviceOK)
	   return;

	if (!running) {
	   vfoFrequency = newFrequency;
	   return;
	}

	if (bankFor_sdr (newFrequency) != bankFor_sdr (vfoFrequency)) {
	   err = my_mir_sdr_Reinit (&currentGain,
	                            float (inputRate) / (MHz (1)),
	                            float (newFrequency) / Mhz (1),
	                            getBandwidth (outputRate),
                                    mir_sdr_IF_Zero,
	                            mir_sdr_LO_Undefined,
                                    0,     // lnaEnable do not know yet
                                    &gRdBSystem,
                                    agcMode, // useGrAltMode,
                                    &samplesPerPacket,
	                            mir_sdr_CHANGE_RF_FREQ);
	   fprintf (stderr, "err = %d\n", err);
	   vfoFrequency	= newFrequency;
	   return;
	}
	else
	   my_mir_sdr_SetRf (newFrequency, 1, 0);
	vfoFrequency = newFrequency;
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

	currentGain = newGain;
	my_mir_sdr_SetGr (currentGain, 1, 0);
	gainDisplay -> display (currentGain);
}

bool	sdrplay::restartReader	(void) {
int     gRdBSystem;
int     samplesPerPacket;
mir_sdr_ErrT    err;

	if (running)
	   return true;
	_I_Buffer	-> FlushRingBuffer ();

        err     = my_mir_sdr_StreamInit (&currentGain,
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

        my_mir_sdr_Uninit       ();
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
	return rate >= Khz (960) && rate <= Khz (8000);
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

int16_t	i;
	for (i = 1; i < sizeof (validWidths) / sizeof (mir_sdr_Bw_MHzT); i ++)
	   if (rate / Khz (1) < (int)(validWidths [i]) &&
	       rate / Khz (1) >= (int)(validWidths [i - 1]))
	      return validWidths [i - 1];
	return mir_sdr_BW_0_200;
}
//
//	find a decent rate for the sdrplay
//	such that it can be decimated to rate
int32_t	sdrplay::getInputRate (int32_t rate) {
int32_t	temp;

	temp	= rate;
	while (temp < Khz (1200))
	   temp += rate;
	return temp;
}
//
//	Note that we set the outputRate
void	sdrplay::set_rateSelector (const QString &s) {
int32_t	newRate	= s. toInt ();
int     gRdBSystem;
int     samplesPerPacket;
mir_sdr_ErrT    err;

	if (newRate == outputRate)
	   return;
	outputRate	= newRate;
	inputRate	= getInputRate (outputRate);
	err = my_mir_sdr_Reinit (&currentGain,
	                         float (inputRate) / (MHz (1)),
	                         float (vfoFrequency) / Mhz (1),
	                         getBandwidth (outputRate),
                                 mir_sdr_IF_Zero,
	                         mir_sdr_LO_Undefined,
                                 0,     // lnaEnable do not know yet
                                 &gRdBSystem,
                                 agcMode, // useGrAltMode,
                                 &samplesPerPacket,
	                         mir_sdr_ReasonForReinitT (
	                             mir_sdr_CHANGE_FS_FREQ |
	                             mir_sdr_CHANGE_BW_TYPE)
	                         );
	fprintf (stderr, "err = %d\n", err);
	delete	d_filter;
	d_filter	= new DecimatingFIR (decimationFactor * 5,
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
        this    -> agcMode      = agcControl -> isChecked ();
        my_mir_sdr_AgcControl (this -> agcMode, 70, 0, 0, 0, 1, 0);
	my_mir_sdr_SetGr (currentGain, 1, 0);
}

void    sdrplay::set_debugBox   (int debugMode) {
        my_mir_sdr_DebugEnable (debugBox        -> isChecked ());
}

void    sdrplay::set_ppmControl (int ppm) {
        my_mir_sdr_SetPpm ((float)ppm);
}

bool	sdrplay::loadFunctions	(void) {

	my_mir_sdr_StreamInit	= (pfn_mir_sdr_StreamInit)
	                    GETPROCADDRESS (this -> Handle,
	                                    "mir_sdr_StreamInit");
	if (my_mir_sdr_StreamInit == NULL) {
	   fprintf (stderr, "Could not find mir_sdr_StreamInit\n");
	   return false;
	}

	my_mir_sdr_Uninit	= (pfn_mir_sdr_Uninit)
	                    GETPROCADDRESS (this -> Handle,
	                                    "mir_sdr_Uninit");
	if (my_mir_sdr_Uninit == NULL) {
	   fprintf (stderr, "Could not find mir_sdr_Uninit\n");
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


//	my_mir_sdr_ResetUpdateFlags	= (pfn_mir_sdr_ResetUpdateFlags)
//	                GETPROCADDRESS (Handle, "mir_sdr_ResetUpdateFlags");
//	if (my_mir_sdr_ResetUpdateFlags == NULL) {
//	   fprintf (stderr, "Could not find mir_sdr_ResetUpdateFlags\n");
//	   return false;
//	}
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

