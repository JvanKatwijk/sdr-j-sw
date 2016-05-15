#
/*
 *    Copyright (C) 2013
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Computing
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
#ifndef	__FRAME_PROCESSOR
#define	__FRAME_PROCESSOR

#include	<QThread>
#include	<QString>
#include	"swradio-constants.h"
#include	"ringbuffer.h"
#include	"drm-decoder.h"
#include	"basics.h"
#include	"syncer.h"
#include	"viterbi-drm.h"

class	ofdmProcessor;
class	equalizer_base;
class	facData;
class	referenceFrame;
class	facProcessor;
class	sdcProcessor;
class	mscProcessor;
class	drmDecoder;
class	mscConfig;

struct facElement {
	int16_t	symbol;
	int16_t	carrier;
};

extern	
bool	isGaincell	(uint8_t, int16_t, int16_t);

class	frameProcessor: public QThread {
Q_OBJECT
public:
			frameProcessor	(drmDecoder *,
	                                 RingBuffer<DSPCOMPLEX> *,
	                                 int16_t,
	                                 int8_t,	// windowDepth
	                                 int8_t);	// qam64
			~frameProcessor (void);
	void		stop		(void);
private:
	drmDecoder	*mr;
	RingBuffer<DSPCOMPLEX> *buffer;
	int8_t		windowDepth;
	int8_t		qam64Roulette;

	bool		taskMayRun;
	DSPCOMPLEX	facError		(DSPCOMPLEX *,
	                                         DSPCOMPLEX *, int16_t);
	void		deleteProcessors	(uint8_t);
	void		createProcessors	(void);
	void		run		(void);
	int16_t		sdcCells	(void);
	int16_t		mscCells	(void);
	bool		stillSynchronized	(int16_t *lc);
	bool		is_bestIndex	(int16_t);
	uint8_t		getSpectrum	(facData *);
	DSPFLOAT	getCorr		(DSPCOMPLEX *);
	bool		isFirstFrame	(facData *);
	bool		isLastFrame	(facData *);
	bool		isSDCcell	(int16_t, int16_t);
	bool		isFACcell	(int16_t, int16_t);
	int16_t		extractSDC	(theSignal **, theSignal *);
	bool		getFACdata	(theSignal **, facData *,
	                                 float, DSPCOMPLEX **);
	void		addtoSuperFrame	(int16_t);
	bool		isDatacell	(int16_t, int16_t, int16_t);
	struct facElement *facTable;
	viterbi		viterbiDecoder;

	Syncer		my_Syncer;
	ofdmProcessor	*my_ofdmProcessor;
	referenceFrame	*my_referenceFrame;
	facProcessor	*my_facProcessor;
	sdcProcessor	*my_sdcProcessor;
	mscProcessor	*my_mscProcessor;
	mscConfig	*my_mscConfig;
	uint8_t		Mode;
	uint8_t		Spectrum;
	int16_t		K_min;
	int16_t		K_max;
	int16_t		carriersinSymbol;
	int16_t		symbolsinFrame;
	equalizer_base	*my_Equalizer;
	facData		*my_facData;
	DSPFLOAT	corrBank [30];
	DSPCOMPLEX	**inbank;
	theSignal	**outbank;
	DSPCOMPLEX	**refBank;
//
signals:
	void		setTimeSync	(bool);
	void		setFACSync	(bool);
	void		setSDCSync	(bool);
	void		show_Mode	(int);
	void		show_Spectrum	(int);
	void		show_services	(int, int);
	void		show_audioMode	(QString);
	void		showEq		(int);
	void		showSNR		(float);
	void		show_freq2	(float);
};

#endif

