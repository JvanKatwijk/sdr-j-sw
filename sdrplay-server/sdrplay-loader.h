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

//
//	To keep things simple, we just load the functions from the
//	mirics dll. To ease multipltform work, we converted
//	the "mir_sdr_api" for Linux into a ".so" file.
//	Behaviour therefore is the same under the
//	different systems.
//	Note that the ".a" file can be converted simply by
//	ar -x ...a
//	gcc -shared *.o -o mir_sdr_api.so
//
//	We use a straightforward class "miricsLoader"  both as
//	loader and as container	for the dll functions.

#ifndef	__SDRPLAY_LOADER
#define	__SDRPLAY_LOADER
#include	"swradio-constants.h"
#include	"mirsdrapi-rsp.h"

//	The naming of functions for accessing shared libraries
//	differ between Linux and Windows
#ifdef __MINGW32__
#define	GETPROCADDRESS	GetProcAddress
#else
#define	GETPROCADDRESS	dlsym
#endif

// Dll and ".so" function prototypes
typedef mir_sdr_ErrT (*pfn_mir_sdr_StreamInit) (int *gRdB, double fsMHz,
double rfMHz, mir_sdr_Bw_MHzT bwType, mir_sdr_If_kHzT ifType, int LNAEnable, int *gRdBsystem, int useGrAltMode, int *samplesPerPacket, mir_sdr_StreamCallback_t StreamCbFn, mir_sdr_GainChangeCallback_t GainChangeCbFn, void *cbContext); 

typedef mir_sdr_ErrT (*pfn_mir_sdr_Reinit) (int *gRdB, double fsMHz,
double rfMHz, mir_sdr_Bw_MHzT bwType, mir_sdr_If_kHzT ifType,
mir_sdr_LoModeT, int, int*, int, int*, mir_sdr_ReasonForReinitT);

typedef mir_sdr_ErrT (*pfn_mir_sdr_Uninit)(void);
typedef mir_sdr_ErrT (*pfn_mir_sdr_SetRf)(double drfHz, int abs, int syncUpdate);
typedef mir_sdr_ErrT (*pfn_mir_sdr_SetFs)(double dfsHz, int abs, int syncUpdate, int reCal);
typedef mir_sdr_ErrT (*pfn_mir_sdr_SetGr)(int gRdB, int abs, int syncUpdate);
typedef mir_sdr_ErrT (*pfn_mir_sdr_SetGrParams)(int minimumGr, int lnaGrThreshold);
typedef mir_sdr_ErrT (*pfn_mir_sdr_SetDcMode)(int dcCal, int speedUp);
typedef mir_sdr_ErrT (*pfn_mir_sdr_SetDcTrackTime)(int trackTime);
typedef mir_sdr_ErrT (*pfn_mir_sdr_SetSyncUpdateSampleNum)(unsigned int sampleNum);
typedef mir_sdr_ErrT (*pfn_mir_sdr_SetSyncUpdatePeriod)(unsigned int period);
typedef mir_sdr_ErrT (*pfn_mir_sdr_ApiVersion)(float *version);   
typedef mir_sdr_ErrT (*pfn_mir_sdr_ResetUpdateFlags)(int resetGainUpdate, int resetRfUpdate, int resetFsUpdate);   
typedef mir_sdr_ErrT (*pfn_mir_sdr_AgcControl)(uint32_t, int, int, uint32_t,
	                                       uint32_t, int, int);
typedef mir_sdr_ErrT (*pfn_mir_sdr_DCoffsetIQimbalanceControl) (uint32_t, uint32_t);
typedef mir_sdr_ErrT (*pfn_mir_sdr_SetPpm)(double);   
typedef mir_sdr_ErrT (*pfn_mir_sdr_DebugEnable)(uint32_t);   


class	sdrplayLoader {
public:

		sdrplayLoader		(bool *);
		~sdrplayLoader		(void);

	pfn_mir_sdr_StreamInit	mir_sdr_StreamInit;
	pfn_mir_sdr_Reinit	mir_sdr_Reinit;
	pfn_mir_sdr_Uninit	mir_sdr_Uninit;
	pfn_mir_sdr_SetRf	mir_sdr_SetRf;
	pfn_mir_sdr_SetFs	mir_sdr_SetFs;
	pfn_mir_sdr_SetGr	mir_sdr_SetGr;
	pfn_mir_sdr_SetGrParams	mir_sdr_SetGrParams;
	pfn_mir_sdr_SetDcMode	mir_sdr_SetDcMode;
	pfn_mir_sdr_SetDcTrackTime mir_sdr_SetDcTrackTime;
	pfn_mir_sdr_SetSyncUpdateSampleNum
	                        mir_sdr_SetSyncUpdateSampleNum;
	pfn_mir_sdr_SetSyncUpdatePeriod
	                        mir_sdr_SetSyncUpdatePeriod;
	pfn_mir_sdr_ApiVersion	mir_sdr_ApiVersion;
	pfn_mir_sdr_ResetUpdateFlags
	                        mir_sdr_ResetUpdateFlags;
	pfn_mir_sdr_AgcControl	mir_sdr_AgcControl;
	pfn_mir_sdr_DCoffsetIQimbalanceControl
	                        mir_sdr_DCoffsetIQimbalanceControl;
	pfn_mir_sdr_SetPpm	mir_sdr_SetPpm;
	pfn_mir_sdr_DebugEnable	mir_sdr_DebugEnable;
private:
	HINSTANCE	Handle;
	bool		libraryLoaded;
};

#endif


