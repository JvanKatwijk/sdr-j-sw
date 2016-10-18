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

#include	"swradio-constants.h"	// some general definitions
#include	"sdrplay-worker.h"	// our header
#include	"ringbuffer.h"		// the buffer

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
sdrplayWorker	*p	= static_cast<sdrplayWorker *> (cbContext);
DSPCOMPLEX *localBuf = (DSPCOMPLEX *)alloca (numSamples * sizeof (DSPCOMPLEX));

	for (i = 0; i <  (int)numSamples; i ++)
	   localBuf [i] = DSPCOMPLEX (float (xi [i]) / 4096.0,
	                              float (xq [i]) / 4096.0);
	p -> _I_Buffer -> putDataIntoBuffer (localBuf, numSamples);
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

//	The sdrplayWorker is the actual wrapper around the mir-sdr
//	interface. It is a pretty simple thread performing the
//	basic functions.
//	

#define	NO_CHANGE	0000
#define	FREQ_CHANGE	0001
#define	GAIN_CHANGE	0002
#define	RATE_CHANGE	0004
#define	AGC_CONTROL_CHANGE	0010
#define	PPM_CHANGE	0020
#define	SEGMENT		1024

	sdrplayWorker::sdrplayWorker (int32_t	inputRate,
	                              int32_t	outputRate,
	                              int32_t	gain,
	                              int32_t	defaultFreq,
	                              sdrplayLoader	*f,
	                              RingBuffer<DSPCOMPLEX> *buf,
	                              bool	*OK) {
int     gRdBSystem;
int     samplesPerPacket;
mir_sdr_ErrT    err;

	fprintf (stderr, "nieuwe worker opgestart\n");
	this	-> inputRate	= inputRate;
	this	-> outputRate	= outputRate;
	this	-> currentGain	= gain;
//	as a sanity check we could verify that dongleRate % outputRate == 0
	this	-> vfoFrequency	= defaultFreq;
	this	-> newFrequency	= 0;
	this	-> agcMode	= 0;
	this	-> ppm		= 0;
	this	-> functions	= f;
	_O_Buffer		= buf;
	_I_Buffer		= new RingBuffer<DSPCOMPLEX> (1024 * 1024);
	*OK			= false;	// just the default

	decimationFactor		= inputRate / outputRate;
	dataRead		= 0;
	d_filter		= new DecimatingFIR (decimationFactor * 5,
	                                             - outputRate / 2,
	                                             + outputRate / 2,
	                                             inputRate,
	                                             decimationFactor);

 	err     = functions -> mir_sdr_StreamInit (&currentGain,
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

	if (err != mir_sdr_Success) {
	   fprintf (stderr, "Probleem init\n");
	   return;
	}

	err			= functions -> mir_sdr_SetDcMode (5, 0);
	err			= functions -> mir_sdr_SetDcTrackTime (63);
//
//	some defaults:
        functions -> mir_sdr_SetSyncUpdatePeriod ((int)(inputRate / 2));
        functions -> mir_sdr_SetSyncUpdateSampleNum (samplesPerPacket);
        functions -> mir_sdr_DCoffsetIQimbalanceControl (0, 1);
	anyChange		= 0;
	start ();
	*OK			= true;
}

//	As usual, killing objects containing a thread need to
//	be done carefully.
void	sdrplayWorker::stop	(void) {
	running	= false;
}

	sdrplayWorker::~sdrplayWorker	(void) {
	running	= false;
	while (isRunning ())
	   msleep (1);
	functions -> mir_sdr_Uninit ();
	delete _I_Buffer;
}
//
//	The actual thread responds on the callback, collects
//	samples and handles changes in the state of the device
//	through appropriate API functions
//
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

void	sdrplayWorker:: run (void) {
DSPCOMPLEX	localBuf [SEGMENT * decimationFactor];
DSPCOMPLEX	secondBuf [SEGMENT];
int32_t		bufferContent	= 0;
int16_t		i;
int     gRdBSystem;
int     samplesPerPacket;
mir_sdr_ErrT    err;

	running		= true;

	while (running) {
	   while (running && (_I_Buffer -> GetRingBufferReadAvailable () <
	                                uint32_t(SEGMENT * decimationFactor)))
	      msleep (1);
	   if (!running)
	      break;
//	OK, when here, there is something to be done,
//	either a signal that data is available, a
//	request to change settings or both
	   while (_I_Buffer -> GetRingBufferReadAvailable () >
	                             uint32_t (SEGMENT * decimationFactor)) {
	      int16_t next	= 0;
	      DSPCOMPLEX temp;
	      int32_t amount = _I_Buffer -> getDataFromBuffer (localBuf, 
	                                        SEGMENT * decimationFactor);
	      for (i = 0; i < amount; i ++) 
	         if (d_filter -> Pass (localBuf [i], &temp)) 
	            secondBuf [next ++] = temp;
	      _O_Buffer	-> putDataIntoBuffer (secondBuf, next);
	      bufferContent += next;
	      if (bufferContent > outputRate / 10) {
	         emit sendSamples (bufferContent);
	         bufferContent = 0;
	      }
	   }
	   
//	OK, data is now stored, now checking for updates
	   while (anyChange != NO_CHANGE) {
	      if (anyChange & GAIN_CHANGE) {
	         functions -> mir_sdr_SetGr (currentGain, 1, 0);
	         anyChange &= ~GAIN_CHANGE;
	         continue;
	      }
	      if (anyChange & AGC_CONTROL_CHANGE) {
	         functions -> mir_sdr_AgcControl (agcMode, -currentGain, 0, 0, 0, 1, 0);
                 functions -> mir_sdr_SetGr (currentGain, 1, 0);
	         anyChange &=  ~AGC_CONTROL_CHANGE;
	         continue;
	      }
	      if (anyChange & PPM_CHANGE) {
	         functions -> mir_sdr_SetPpm ((float)ppm);
	         anyChange	&= ~PPM_CHANGE;
	         continue;
	      }
	
	      if (anyChange & FREQ_CHANGE) {
	         if (bankFor_sdr (newFrequency) !=
	                     bankFor_sdr (vfoFrequency)) {
                    err = functions -> mir_sdr_Reinit (&currentGain,
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
                 }
                 else
                    err = functions -> mir_sdr_SetRf (newFrequency, 1, 0);
	         (void)err;
                 vfoFrequency = newFrequency;
	         anyChange &= ~FREQ_CHANGE;
	         continue;
	      }
//
//	This actually does not happen
	      if (anyChange & RATE_CHANGE) {
	         outputRate	= newRate;
	         inputRate	= getInputRate (outputRate);
	         err = functions -> mir_sdr_Reinit (&currentGain,
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
	         delete	d_filter;
	         d_filter	= new DecimatingFIR (decimationFactor * 5,
	                                             - outputRate / 2,
	                                             + outputRate / 2,
	                                             inputRate,
	                                             decimationFactor);
	         _I_Buffer	-> FlushRingBuffer ();
	         dataRead	= 0;
	         anyChange &= ~RATE_CHANGE;
	      }
	   }
	}
	fprintf (stderr, "sdrplayWorker now stopped\n");
}

void	sdrplayWorker::setVFOFrequency	(int32_t f) {
	newFrequency	= f;
	anyChange 	|= FREQ_CHANGE;
}

void	sdrplayWorker::setExternalGain	(int16_t gain) {
	if (gain < 0)
	   return;
	if (gain > 102)	// should not happen
	   return;
	if ((newFrequency > MHz (430)) && (gain > 82))
	   return;
	currentGain		= gain;
	anyChange 	|= GAIN_CHANGE;
}

void	sdrplayWorker::set_agcControl (int16_t c) {
	anyChange	|= AGC_CONTROL_CHANGE;
	agcMode		= c;

}

void	sdrplayWorker::set_ppmControl (int32_t ppm) {
	anyChange	|= PPM_CHANGE;
	this -> ppm	= ppm;
}

void	sdrplayWorker::set_outputRate	(int32_t newrate) {
	anyChange	|= RATE_CHANGE;
	newRate		= newrate;
}

mir_sdr_Bw_MHzT	sdrplayWorker::getBandwidth	(int32_t rate) {
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
//      find a decent rate for the sdrplay
//      such that it can be decimated to rate
int32_t sdrplayWorker::getInputRate (int32_t rate) {
int32_t temp;

        temp    = rate;
        while (temp < Khz (1200))
           temp += rate;
        return temp;
}

