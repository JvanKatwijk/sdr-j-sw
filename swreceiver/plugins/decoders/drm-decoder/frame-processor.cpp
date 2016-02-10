#
/*
 *    Copyright (C) 2015
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Computing
 *
 *    This file is part of SDR-J.
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
#include	"frame-processor.h"
#include	"fac-data.h"
#include	"fac-processor.h"
#include	"sdc-processor.h"
#include	"msc-processor.h"
#include	"drm-decoder.h"
#include	"equalizer-1.h"
#include	"ofdm-processor.h"
#include	"referenceframe.h"
#include	"basics.h"
#include	"syncer.h"
#include	"fac-tables.h"
#include	"viterbi-drm.h"
#include	"oscillator.h"

//	The main driving class is the frameprocessor. It is
//	implemented as a thread and will control:
//	a. initial synchronization and resynchronization after sync failures
//	b. getting synchronized data for the frames
//	b. assembling the FAC and SDC,
//	c. collecting Muxes and passing the Mux-es to the  MSC handler
//
	frameProcessor::frameProcessor (drmDecoder	*mr,
	                                RingBuffer<DSPCOMPLEX> *buffer,
	                                int16_t length,
	                                int8_t	windowDepth,
	                                int8_t qam64Roulette) {
	this	-> mr 		= mr;
	this	-> buffer	= buffer;
	this	-> my_Syncer	= new Syncer (buffer, mr, length * 320);
	this	-> windowDepth	= windowDepth;
	this	-> qam64Roulette = qam64Roulette;
//
//	One viterbidecoder for all deconvolutions
	viterbiDecoder		= new viterbi (0);
//
//	defaults, will be overruled almost immediately
	this	-> Mode		= 1;
	this	-> Spectrum	= 3;
	my_Equalizer		= NULL;
	this	-> facTable	= getFacTableforMode (Mode);

	createProcessors	();
	connect (this, SIGNAL (setTimeSync (bool)),
	         mr, SLOT (executeTimeSync (bool)));
	connect (this, SIGNAL (setFACSync (bool)),
	         mr, SLOT (executeFACSync (bool)));
	connect (this, SIGNAL (setSDCSync (bool)),
	         mr, SLOT (executeSDCSync (bool)));
	connect (this, SIGNAL (show (float, float)),
	         mr, SLOT (showIQ (float, float)));
	connect (this, SIGNAL (show_Mode (int)),
	         mr, SLOT (execute_showMode (int)));
	connect (this, SIGNAL(show_Spectrum (int)),
	         mr, SLOT (execute_showSpectrum (int)));
	connect (this, SIGNAL (show_services (int, int)),
	         mr, SLOT (show_channels (int, int)));
	connect (this, SIGNAL (show_frames (int, int)),
	         mr, SLOT (show_frames (int, int)));
	connect (this, SIGNAL (show_audioMode (QString)),
	         mr, SLOT (show_audioMode (QString)));
	connect (this, SIGNAL (newYvalue (int, float)),
	         mr, SLOT (newYvalue (int, float)));
	connect (this, SIGNAL (showEq (int)),
	         mr, SLOT (showEq (int)));
	connect (this, SIGNAL (showSNR (float)),
	         mr, SLOT (showSNR (float)));
//
//	The driver is a separate task, therefore we
//	just set a flag when changes are to be applied
//	The thread will look regularly to see whether changes
//	are to be applied
}

	frameProcessor::~frameProcessor (void) {
	taskMayRun	= false;
	while (isRunning ())
	   usleep (100);
//	deleteProcessors	(Mode);	 need to be addressed!!!
}

void	frameProcessor::stop (void) {
	taskMayRun	= false;
}

void	frameProcessor::deleteProcessors (uint8_t Mode) {
	(void) Mode;
	if (my_Equalizer != NULL)
	   delete	my_Equalizer;
	my_Equalizer = NULL;
	delete	my_facProcessor;
	delete	my_sdcProcessor;
	delete	my_mscProcessor;
	delete	my_facData;
	delete	my_mscConfig;
}

void	frameProcessor::createProcessors (void) {

	my_mscConfig		= new mscConfig	(Mode, Spectrum);
	my_facData		= new facData (mr, my_mscConfig);
//
	my_Equalizer 		= new equalizer_1 (Mode, Spectrum, windowDepth);

	my_facProcessor		= new facProcessor (Mode,
	                                            Spectrum,
	                                            viterbiDecoder);
	my_sdcProcessor		= new sdcProcessor (Mode,
	                                            Spectrum,
	                                            viterbiDecoder,
	                                            my_facData,
	                                            sdcCells ());
	my_mscProcessor		= new mscProcessor  (my_mscConfig,
	                                             mr,
	                                             qam64Roulette,
	                                             viterbiDecoder);
}

//
//	The frameprocessor is the driving force. It will continuously
//	ask for ofdm symbols, build frames and send frames to the
//	various processors (performing some validity checks as part of the
//	process)
void	frameProcessor::run	(void) {
int16_t		lc;		// index of first word in inbank
uint8_t		oldMode		= 6,
		oldSpectrum	= 7;
int16_t		i;
int16_t		blockCount	= 0;
bool		inSync;
bool		superframer	= false;
int16_t		goodFrames	= 0;
int16_t		badFrames	= 0;
int16_t		threeinaRow;
smodeInfo	modeInf;
int16_t		symbol_no	= 0;
bool		frameReady;
int16_t		missers;
//
//	inbank and outbank have dimensions satisfying the need of the
//	most "hungry" modes
	inbank		= new DSPCOMPLEX *[symbolsperFrame (Mode_C)];
	for (i = 0; i < symbolsperFrame (Mode_C); i ++) {
	   int16_t t = Kmax (Mode_A, Spectrum) - Kmin (Mode_A, Spectrum) + 1;
	   inbank [i] = new DSPCOMPLEX [t];
	   memset (inbank [i], 0, t * sizeof (DSPCOMPLEX));
	}

	outbank			= new theSignal *[symbolsperFrame (Mode_C)];
	for (i = 0; i < symbolsperFrame (Mode_C); i ++) {
	   int16_t t = Kmax (Mode_A, Spectrum) - Kmin (Mode_A, Spectrum) + 1;
	   outbank [i] = new theSignal [t];
	   memset (outbank [i], 0, t * sizeof (theSignal));
	}

	taskMayRun	= true;		// will be set from elsewhere to false
	Spectrum	= 7;		// current default
//
//	It is cruel, but alas. Task termination is quite complex
//	in Qt (a task has to commit suicide on a notification from
//	elsewhere, We just raise an exception when the time has come ...)
	try {
restart:
	
	   if (!taskMayRun)
	      throw (0);
	   setTimeSync		(false);
	   setFACSync		(false);
	   setSDCSync		(false);
	   show_services	(0, 0);
	   superframer		= false;	
	   goodFrames		= 0;
	   badFrames		= 0;
	   threeinaRow		= 0;
	   show_audioMode	(QString ("  "));
	   lc			= 0;
	   missers		= 0;
//
//	First step: find mode and starting point
	   modeInf. mode_indx = -1;

	   while ((modeInf. mode_indx == -1 ||
	           modeInf. mode_indx == Mode_D) && taskMayRun) {
	      my_Syncer -> shiftBuffer (Ts_of (Mode_A) / 2);
	      my_Syncer	-> getMode (&modeInf);
	   }
	   if (!taskMayRun) throw (0);

	   fprintf (stderr, "found 1: Mode = %d, time_offset = %f, sampleoff = %f freqoff = %f\n",
	        modeInf. mode_indx,
	        modeInf. time_offset,
	        modeInf. sample_rate_offset,
	        modeInf. freq_offset_fract);
//
//	adjust the bufferpointer:
	   my_Syncer -> shiftBuffer (floor (modeInf. time_offset + 0.5));
	   Mode			= modeInf. mode_indx;
	   int16_t time_offset_integer = floor (modeInf. time_offset + 0.5);
//	fractional time offset:
	   float time_offset_fractional	= modeInf. time_offset -
	                                         time_offset_integer;

//	and create a reader/processor
	   my_ofdmProcessor	= new ofdmProcessor (my_Syncer,
	                                             Mode, Spectrum, mr);
//	look for a spectrum
	   int32_t intOffset;
	   my_ofdmProcessor	-> frequencySync (my_Syncer,
	                                          time_offset_fractional,
	                                          &Spectrum,
	                                          &intOffset);
	   setTimeSync	(true);
//
//	if the newly detected mode differs from the one we
//	were dealing with, we have to rebuild a proper
//	working environment.

	   if ((Mode != oldMode) ||
	       (Spectrum != oldSpectrum) ||
	        abs (intOffset) > Khz (2)) {
//	sadly, we have to delete the various processors
	      deleteProcessors (oldMode);
	      delete my_ofdmProcessor;
//	establish a new set of workers and tables

	      facTable		= getFacTableforMode (Mode);
	      createProcessors ();
	      my_ofdmProcessor	= new ofdmProcessor (my_Syncer,
	                                             Mode, Spectrum, mr);
	      oldSpectrum	= Spectrum;
	      oldMode		= Mode;
	   }
	   show_Mode	(Mode); show_Spectrum (Spectrum);
//
//	Ok, once here, we know the intoffset, we adjusted the buffer
//	for the coarse timing and we know the "time_offset_fractional"
//	OK, all set, first compute some shorthands
	   carriersinSymbol	= Kmax (Mode, Spectrum) -
	                          Kmin (Mode, Spectrum) + 1;
	   symbolsinFrame	= symbolsperFrame (Mode);
	   K_min		= Kmin (Mode, Spectrum);
	   K_max		= Kmax (Mode, Spectrum);
//
//	we know that - when starting - we are not "in sync"
	   inSync		= false;
//
//	start here with reading ofdm words until we have a
//	match with the timesync. We start with reading in "symbolsinFrame"
//	ofdm symbols
	   for (i = 0; i < symbolsinFrame; i ++) {
	      my_ofdmProcessor -> getWord (Mode,
	                                   Spectrum,
	                                   inbank [i],
	                                   i == 0,
	                                   intOffset,
	                                   time_offset_fractional);
	      corrBank [i] = getCorr (inbank [i]);
	      time_offset_fractional -=  Ts_of (Mode) *
	                                      modeInf. sample_rate_offset;
	   }
	   int16_t errors	= 0;
	   lc	= 0;
//
//	We keep on reading here until we are satisfied that the
//	frame that is in, is indeed a valid, synchronized frame

	   while (!inSync && taskMayRun) {
	      while (true) {
	         my_ofdmProcessor -> getWord (Mode,
	                                      Spectrum,
	                                      inbank [lc],
	                                      false,
	                                      intOffset,
	                                      time_offset_fractional);
	         time_offset_fractional -=  Ts_of (Mode) *
	                                     modeInf. sample_rate_offset;
	         corrBank [lc] = getCorr (inbank [lc]);
	         lc = (lc + 1) % symbolsperFrame (Mode);
	         if (is_bestIndex (lc))  {
	            break;
	         }
	      }
//
//	from here on, we know that in the input bank, the frames occupy the
//	rows "lc" ... "(lc + symbolsinFrame) % symbolsinFrame"
//	so, once here, we know that the frame starts with index lc,
//	so let us equalize the last symbolsinFrame words in the buffer
	      for (symbol_no = 0; symbol_no < symbolsinFrame; symbol_no ++) 
	         (void) my_Equalizer ->
	            equalize (inbank [(lc + symbol_no) % symbolsinFrame],
	                   symbol_no, outbank);
//
//	Once here, we are waiting for the equalizer to signal that a full
//	frame is equalized. Since we did exactly symbolsinFrame symbols,
//	symbol_no is set to 0 again
	      lc		= (lc + symbol_no) % symbol_no;
	      symbol_no		= 0;
	      frameReady	= false;
	      while (!frameReady) {
	         my_ofdmProcessor ->
	              getWord (Mode,
	                       Spectrum,
	                       inbank [lc],
	                       false,
	                       intOffset,
	                       time_offset_fractional);
	         time_offset_fractional -=  Ts_of (Mode) *
	                                        modeInf. sample_rate_offset,
	         corrBank [lc] = getCorr (inbank [lc]);
	         frameReady = my_Equalizer -> equalize (inbank [lc],
	                                                symbol_no,
	                                                outbank);
	         lc = (lc + 1) % symbolsinFrame;
	         symbol_no = (symbol_no + 1) % symbolsinFrame;
	      }

//	so here, we might expect that "lc" indicates the location in
//	the inputbank to store the next word, while symbol_no denotes
//	the next symbol in the output bank
//	      showEq_data (1);
	      showEq_data (2);
//
//	when we are here, we do have a full "frame".
//	so, we will be convinced that we are OK when we have a decent FAC
	      if (getFACdata (outbank, my_facData,
	                      my_Equalizer   -> getMeanEnergy  (),
	                      my_Equalizer   -> getChannels ())) {
	         inSync = true;
	         break;		
	      }
	      else 	// if no fac, keep on trying or, start all over again
	         if (++errors > 5)
	            goto restart;
	   }

	   bool	firstTime	= true;
	   if (!taskMayRun)
	      throw (21);
//
//	when here, it feels like we have passed an exam,
//	we have in the buffer an equalized frame (the first we met)
//	ready for further processing
//
//	Loop is such that when we are here, we have
//	decent FAC data
	float	offsetFractional	= 0;	//
	int16_t	offsetInteger		= 0;
	float	deltaFreqOffset		= 0;
	float	sampleclockOffset	= 0;
	   while (taskMayRun) {
	      setFACSync (true);
//	this should not happen really
//	      if (getSpectrum (my_facData) != Spectrum) {
//	         fprintf (stderr, "Ik snap er nix van\n");
//	         goto restart;
//	      }
//	when we are here, we can start thinking about  SDC's and superframes
//	The first frame of a superframe has an SDC part
	      if (isFirstFrame (my_facData)) {
	         bool sdcOK;
	         theSignal sdcVector [sdcCells ()];
	         int16_t nSDC = extractSDC (outbank, sdcVector);
//	if needed create a new sdcProcessor
	         if (my_facData -> myChannelParameters. getSDCmode () !=
	                               my_sdcProcessor -> getSDCmode ()) {
	            delete my_sdcProcessor;
	            my_sdcProcessor = new sdcProcessor (Mode, Spectrum, 
	                                                viterbiDecoder,
	                                                my_facData,
	                                                sdcCells ());
	         }
//
	         sdcOK = my_sdcProcessor -> processSDC (sdcVector); 
	         setSDCSync (sdcOK);
	         if (sdcOK) {
	            goodFrames ++;
	            threeinaRow ++;
	         }
	         else
	            badFrames ++;
	               
	         show_services (my_mscConfig -> getnrAudio (),
	                        my_mscConfig -> getnrData ());
	         show_frames (goodFrames, badFrames);

	         if (sdcOK) 
	            my_mscProcessor -> check_mscConfig (my_mscConfig);
//
//	we allow one sdc to fail, but only after having at least
//	three frames correct
	         blockCount	= 0;
	         superframer	= sdcOK || threeinaRow >= 3;
	         if (!sdcOK)
	            threeinaRow	= 0;
	      }
//
//	when here, add the current frame to the superframe.
//	Obviously, we cannot garantee that all data is in order
	      if (superframer)
	         addtoSuperFrame (blockCount ++);
//
//	when we are here, it is time to build the next frame
	      frameReady	= false;
	      for (i = 0; i < symbolsinFrame; i ++) {
	         my_ofdmProcessor ->
	              getWord (Mode, Spectrum,
	                       inbank [(lc + i) % symbolsinFrame],
	                       intOffset,	// initial value
	                       time_offset_fractional,	// initial value
	                       firstTime,
	                       offsetInteger,	// not used
	                       offsetFractional,	// tracking value
	                       deltaFreqOffset,		// tracking value
	                       sampleclockOffset	// tracking value
	                      );
	         firstTime = false;
	         corrBank [(lc + i) % symbolsinFrame] =
	                          getCorr (inbank [(lc + i) % symbolsinFrame]);
	         frameReady =
	                  my_Equalizer -> equalize (inbank [(lc + i) % symbolsinFrame],
	                                            (symbol_no + i) % symbolsinFrame,
	                                            outbank,
	                                            &offsetInteger,
	                                            &offsetFractional,
	                                            &deltaFreqOffset,
	                                            &sampleclockOffset);
	      }

	      showEq_data (0);
	      showEq_data (symbolsinFrame - 1);
//	OK, let us check the FAC
	      if (!getFACdata (outbank,
	                       my_facData,
	                       my_Equalizer   -> getMeanEnergy  (),
	                       my_Equalizer   -> getChannels ())) {
	            setFACSync (false);
	            setSDCSync (false);
	            superframer		= false;
	            if (missers++ < 3)
	               continue;
	            goto restart;	// ... or give up and start all over
	      }
	      else
	         missers = 0;
	   }	// end of main loop
	}
	catch (int e) {
	   fprintf (stderr, "frameprocessor will halt with %d\n", e);
	}
}
//
//	adding the contents of a frame to a superframe
void	frameProcessor::addtoSuperFrame (int16_t blockno) {
static	int	teller	= 0;
int16_t	symbol, carrier;

	if (blockno == 0)	// new superframe
	   my_mscProcessor	-> newFrame ();
	for (symbol = 0; symbol < symbolsinFrame; symbol ++)
	   for (carrier = K_min; carrier <= K_max; carrier ++)
	      if (isDatacell (symbol, carrier, blockno)) {
	         my_mscProcessor -> addtoMux (blockno, teller ++,
	                                     outbank [symbol][carrier - K_min]);
	      }

	if (isLastFrame (my_facData)) {
	   my_mscProcessor	-> endofFrame ();
	   teller = 0;
	}
}

bool	frameProcessor::isLastFrame (facData *f) {
uint8_t	val	= f -> myChannelParameters. getIdentity ();
	return ((val & 03) == 02);
}

bool	frameProcessor::isFirstFrame (facData *f) {
uint8_t	val	= f -> myChannelParameters. getIdentity ();
	return ((val & 03) == 0) || ((val & 03) == 03);
}

bool	frameProcessor::isDatacell (int16_t symbol,
	                            int16_t carrier, int16_t blockno) {
	if (carrier == 0)
	   return false;
	if (Mode == 1 && (carrier == -1 || carrier == 1))
	   return false;
//
//	these are definitely SDC cells
	if ((blockno == 0) && ((symbol == 0) || (symbol == 1)))
	   return false;
//
	if ((blockno == 0) && ((Mode == 3 ) || (Mode == 4)))
	   if (symbol == 2)
	      return false;
	if (isFreqCell (Mode, symbol, carrier))
	   return false;
	if (isPilotCell (Mode, symbol, carrier))
	   return false;
	if (isTimeCell (Mode, symbol, carrier))
	   return false;
	if (isFACcell (symbol, carrier))
	   return false;

	return true;
}

int16_t	frameProcessor::sdcCells (void) {
int16_t	nQAM_SDC_cells	= 0;

	switch (Mode) {
	   case 1:
	      switch (Spectrum) {	
	         case 0:	nQAM_SDC_cells = 167; break;
	         case 1:	nQAM_SDC_cells = 190; break;
	         case 2:	nQAM_SDC_cells = 359; break;
	         case 3:	nQAM_SDC_cells = 405; break;
	         default:
	         case 4:	nQAM_SDC_cells = 754; break;
	      }
	      break;

	   default:
	   case 2:
	      switch (Spectrum) {
	         case 0:	nQAM_SDC_cells	= 130; break;
	         case 1:	nQAM_SDC_cells	= 150; break;
	         case 2:	nQAM_SDC_cells	= 282; break;
	         default:
	         case 3:	nQAM_SDC_cells	= 322; break;
	         case 4:	nQAM_SDC_cells	= 588; break;
	      }
	      break;

	   case 3:
	      nQAM_SDC_cells	= 288; break;
	
	   case 4:
	      nQAM_SDC_cells	= 152; break;
	}

	return nQAM_SDC_cells;
}

int16_t	frameProcessor::mscCells  (void) {
	switch (Mode) {
	   case 1:
	      switch (Spectrum) {
	         case	0:	return 3778;
	         case	1:	return 4368;
	         case	2:	return 7897;
	         default:
	         case	3:	return 8877;
	         case	4:	return 16394;
	      }

	   default:
	   case 2:
	      switch (Spectrum) {
	         case	0:	return 2900;
	         case	1:	return 3330;
	         case	2:	return 6153;
	         default:
	         case	3:	return 7013;
	         case	4:	return 12747;
	      }

	   case 3:
	      return 5532;

	   case 4:
	      return 3679;
	}
}
//
//	If the FAC fails, we just check that the
//	frames are still what we expect them to be
bool	frameProcessor::stillSynchronized (int16_t *lc) {
int16_t	i;
	for (i = 1; i < symbolsinFrame; i ++)
	   if (corrBank [*lc] < corrBank [(*lc + i) % symbolsinFrame])
	      return false;
	return true;
}

bool	frameProcessor::is_bestIndex (int16_t index) {
int16_t	i;

	for (i = 0; i < symbolsperFrame (Mode); i ++)
	   if (corrBank [i] > corrBank [index])
	      return false;
	return true;
}

bool	frameProcessor::isFACcell (int16_t symbol, int16_t carrier) {
int16_t	i;
//
//	we know that FAC cells are always in positive carriers
	if (carrier < 0)
	   return false;
	for (i = 0; facTable [i]. symbol != -1; i ++) {
	   if (facTable [i]. symbol > symbol)
	      return false;
	   if ((facTable [i]. symbol == symbol) &&
	       (facTable [i]. carrier == carrier))
	      return true;
	}
	return false;
}

bool	frameProcessor::isSDCcell (int16_t symbol, int16_t carrier) {

	if (carrier == 0)
	   return false;
	if (Mode == 1 && ((carrier == -1) || (carrier == 1)))
	   return false;

	if (isTimeCell (Mode, symbol, carrier))
	   return false;
	if (isFreqCell (Mode, symbol, carrier))
	   return false;
	if (isPilotCell (Mode, symbol, carrier))
	   return false;
	if (isFACcell (symbol, carrier))
	   return false;
	return true;
}

int16_t	frameProcessor::extractSDC (theSignal	**bank,
	                            theSignal	*outV) {
int16_t	valueIndex	= 0;
int16_t	carrier;

	for (carrier = Kmin (Mode, Spectrum);
	     carrier <= Kmax (Mode, Spectrum); carrier ++)
	   if (isSDCcell (0, carrier)) {
	      outV [valueIndex ++] = 
	           bank [0][carrier - Kmin (Mode, Spectrum)];
	   }

	for (carrier = Kmin (Mode, Spectrum);
	     carrier <= Kmax (Mode, Spectrum); carrier ++) 
	   if (isSDCcell (1, carrier)) {
	      outV [valueIndex ++] = 
	           bank [1][carrier - Kmin (Mode, Spectrum)];
	   }

	if ((Mode == 3) || (Mode == 4)) {
	   for (carrier = Kmin (Mode, Spectrum);
	        carrier <= Kmax (Mode, Spectrum); carrier ++) 
	      if (isSDCcell (2, carrier)) {
	         outV [valueIndex ++] = 
	              bank [2][carrier - Kmin (Mode, Spectrum)];
	      }
	}

//	fprintf (stderr, "for Mode %d we have %d sdcCells\n",
//	                   Mode, valueIndex);
	return valueIndex;
}
//
//	just for readability
uint8_t	frameProcessor::getSpectrum	(facData *f) {
uint8_t val = f -> myChannelParameters. getSpectrumBits ();
	return val <= 3 ? val : 3;
}
//
//	just a handle
DSPFLOAT frameProcessor::getCorr (DSPCOMPLEX *v) {
	return timeCorrelate (v, Mode, Spectrum);
}

DSPCOMPLEX frameProcessor::facError (DSPCOMPLEX *src,
	                             DSPCOMPLEX *ref,
	                             int16_t amount) {
int16_t	i;
DSPCOMPLEX ldist	= DSPCOMPLEX (0, 0);

	for (i = 0; i < amount; i ++) {
	   ldist += src [i] / ref [i];
	}
	return cdiv (ldist, amount);
}

bool	frameProcessor::getFACdata (theSignal **bank,
	                            facData *d, float meanEnergy,
	                            DSPCOMPLEX **H) {
theSignal	facVector [100];
float		squaredNoiseSequence [100];
float		squaredWeightSequence [100];
int16_t		i;
bool		result;
static	int	count	= 0;
static	int	goodcount = 0;
float	sum_MERFAC	= 0;
float	sum_WMERFAC	= 0;
float	sum_weight_FAC	= 0;
int16_t	nFac		= 0;
float	WMERFAC;

	for (i = 0; facTable [i]. symbol != -1; i ++) {
	   int16_t symbol	= facTable [i]. symbol;
	   int16_t index	= facTable [i]. carrier - Kmin (Mode, Spectrum);
	   facVector [i]	= bank [symbol][index];
	   squaredNoiseSequence [i]	= abs (facVector [i]. signalValue) - sqrt (0.5);
	   squaredNoiseSequence [i]	*= squaredNoiseSequence [i];
	   DSPCOMPLEX theH	= H [symbol][index];
	   squaredWeightSequence [i] = real (theH * conj (theH));
	   sum_MERFAC		+= squaredNoiseSequence [i];
	   sum_WMERFAC		+= squaredNoiseSequence [i] * 
	                                (squaredWeightSequence [i] + 1.0E-10);
	   sum_weight_FAC	+= squaredWeightSequence [i];
	}
	nFac		= i;
//	MERFAC		= log10 (sum_MERFAC / i + 1.0E-10);
	WMERFAC		= log10 (sum_WMERFAC /
	                         (meanEnergy * (sum_weight_FAC + nFac * 1.0E-10)));
	WMERFAC		*= -10.0;
	showSNR (WMERFAC);

//	processFAC will do two things:
//	decode the FAC samples and check the CRC and, if the CRC
//	check is OK, it will construct the FAC samples as they should have been
	result = my_facProcessor -> processFAC (facVector, d);
	if (result) {
	   goodcount ++;
	}
	if (++count == 100) {
	   printf ("%d FACs, success = %d\n", count, goodcount);
	   count = 0;
	   goodcount = 0;
	}
	return result;
}

void	frameProcessor::showEq_data (int16_t first) {
int16_t	i;
DSPCOMPLEX	eqVec [400];

	my_Equalizer	-> getEq (first, eqVec);
	for (i = 0; i < carriersinSymbol; i ++)
	   newYvalue (i, abs (eqVec [i]));
	showEq (carriersinSymbol);
}

