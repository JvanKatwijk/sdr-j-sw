#
/*
 *    Copyright (C) 2014
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Computing
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

#include	"sstv-worker.h"
#include	"iir-filters.h"
#include	"snr-calc.h"

		sstvWorker::sstvWorker (int32_t theRate,
	                                RingBuffer<DSPCOMPLEX> *b,
	                                QImage **im) {
	this	-> theRate	= theRate;
	theBuffer		= b;
	this	-> theImageP	= im;

	bandPassFreq0	= new BandPassIIR (3,
	                                   1050, 1150, theRate, S_BUTTERWORTH);
	lowPassFreq0	= new LowPassIIR (2, 50, theRate, S_BUTTERWORTH);
	bandPassFreq1	= new BandPassIIR (3,
	                                   1150, 1250, theRate, S_BUTTERWORTH);
	lowPassFreq1	= new LowPassIIR (2, 50, theRate, S_BUTTERWORTH);
	bandPassFreq2	= new BandPassIIR (3,
	                                   1250, 1350, theRate, S_BUTTERWORTH);
	lowPassFreq2	= new LowPassIIR (2, 50, theRate, S_BUTTERWORTH);
	bandPassFreq3	= new BandPassIIR (3,
	                                   1450, 1550, theRate, S_BUTTERWORTH);
	lowPassFreq3	= new LowPassIIR (2, 50, theRate, S_BUTTERWORTH);
	bandPassFreq4	= new BandPassIIR (3,
	                                   1850, 1950, theRate, S_BUTTERWORTH);
	lowPassFreq4	= new LowPassIIR (2, 50, theRate, S_BUTTERWORTH);

	LPM_Filter	= new LowPassIIR (2, 800, theRate, S_BUTTERWORTH);
	localOscillator	= new Oscillator (theRate);
	oneBitSampleCount	= 30 * theRate / 1000;
	computeSNR	= new SNRcalc (theRate);
	start ();
	running		= true;
}

		sstvWorker::~sstvWorker	(void) {
	running	= false;
	while (!isFinished ())
	   usleep (100);
	delete	bandPassFreq0;
	delete	bandPassFreq1;
	delete	bandPassFreq2;
	delete	bandPassFreq3;
	delete	bandPassFreq4;
	delete	lowPassFreq0;
	delete	lowPassFreq1;
	delete	lowPassFreq2;
	delete	lowPassFreq3;
	delete	lowPassFreq4;
	delete	LPM_Filter;
}
void	sstvWorker::stop	(void) {
	running = false;
}

DSPCOMPLEX	sstvWorker::getSample (void) {
int16_t	avail	= theBuffer	-> GetRingBufferReadAvailable ();
DSPCOMPLEX	sample;

	while ((avail < theRate / 100) && running) {
	   usleep (2000); // good for theRate / 500 samples
	   avail	= theBuffer -> GetRingBufferReadAvailable ();
	}

	if (!running)
	   throw (20);
	theBuffer	-> getDataFromBuffer (&sample, 1);
	return sample;
}

// this function returns the bit value of the current sample
// possible results:
// 1900 Hz 		->	4 (leader tone)
// 1500 Hz 		->	3 (sync porch, separator pulse)
// 1300 Hz 		->	2 (VIS bit 0)
// 1200 Hz 		->	1 (break, VIS start&stop bit)
// 1100 Hz 		->	0 (VIS bit 1)
uint8_t	sstvWorker::demodulator	(DSPCOMPLEX sample) {
double lines [5];
int16_t maxIndex = 0;
double maxVal;
int16_t	i;
DSPCOMPLEX	t1, t2, t3, t4, t5;

	t1		= bandPassFreq0 -> Pass (sample);
	lines [0]	= real (t1 * conj (t1));
	t2		= bandPassFreq1 -> Pass (sample);
	lines [1]	= real (t2 * conj (t2));
	t3		= bandPassFreq2 -> Pass (sample);
	lines [2]	= real (t3 * conj (t3));
	t4		= bandPassFreq3 -> Pass (sample);
	lines [3]	= real (t4 * conj (t4));
	t5		= bandPassFreq4 -> Pass (sample);
	lines [4]	= real (t5 * conj (t5));
//	lowpass filtering the lines
	lines[0]	= lowPassFreq0 -> Pass (lines[0]);
	lines[1]	= lowPassFreq1 -> Pass (lines[1]);
	lines[2]	= lowPassFreq2 -> Pass (lines[2]);
	lines[3] 	= lowPassFreq3 -> Pass (lines[3]);
	lines[4] 	= lowPassFreq4 -> Pass (lines[4]);

// deciding which line is the highest and returning it's index
	maxIndex = 0;
	maxVal = lines[0];
	for (i = 1; i < 5; i++) {
	   if (lines [i] > maxVal) {
	      maxVal = lines[i];
	      maxIndex = i;
	   }
	}
	return maxIndex;
}

//	this function returns at half of the bit, with the (assumed)
//	most reliable sample of the bit
uint8_t 	sstvWorker::getVisBit (void) {
uint8_t val = 0;
int16_t	i;

	for (i = 0; i < oneBitSampleCount; i++) 
	   val = demodulator (getSample ());
	return val;
}

uint8_t 	sstvWorker::getVisCode (void) {
uint16_t	bitResult;
int16_t		i;
int16_t		VIS		= 0;
uint16_t	parity		= 0;
bool		parityError	= false;
uint8_t		bit;
//
//	Note that take into account the fact that we used already
//	a couple of samples  (6) to detect the edge.
	displayStage (QString ("receiving VIS code"));
//	waiting half bit time (15ms)
	for (i = 0; i < oneBitSampleCount / 2 - 6; i++) 
	   (void)demodulator (getSample ());

	for (i = 0; i < 9; i++) {
	   bitResult	= getVisBit ();
	   bit		= bitResult == 0 ? 1 : 0;
	   switch (i) {
	      case 7:
	         if (parity != bit)
	            parityError = true;
	         break;

	      case 8:
	         break; // stop bit

	      default:
	         if (bitResult == 0)
	            VIS |= (bit << (i));
	         parity ^= bit;
	   }
	}

	if (parityError)
	   fprintf (stderr, " parity ERROR, vis = %o\n", VIS);
	else
	   fprintf (stderr, " parity OK, vis = %o\n", VIS);

// waiting to reach the end of the stop bit
	for (i = 0; i < oneBitSampleCount / 4 + 5; i++) 
	   (void)demodulator (getSample ());

	return VIS;
}
//
void	sstvWorker:: waitForEdge (uint8_t *result) {
int16_t	i;
	result [0] = result [1];

	while (running) {
	   if (result [1] != result [0])
	      break;
	   result [0] = result [1];
	   result [1] = demodulator (getSample ());
	};
}
//

int16_t	sstvWorker::fmDemodulateLuminance (DSPCOMPLEX sample) {
static DSPCOMPLEX prevSample = DSPCOMPLEX (0, 0);
int16_t	luminance	= 0;
float	temp;
	sample	= sample * localOscillator -> nextValue (1900);
//	sample	= LPM_Filter -> Pass (sample);
// 	then we apply slicing  and we compute the relative offst 0 .. 255
	temp	= arg (conj (prevSample) * sample);
//
//	the "bandwidth" of the signal is max 2000 Hz, so
//	the range for the arg here is roughly 2000 / theRate,
//	we amplify the outcome by the inverse to obtain a
//	full spectrum -PI .. PI
	luminance	= theRate / 2000.0 * temp / M_PI * 128 + 128;
	prevSample	= sample;
	return luminance & 0xFF;
}

//	Just a dispatcher
void	sstvWorker::receiveImage (visDescriptor *myDescriptor) {

	displayMode (myDescriptor -> Name);
	switch (myDescriptor -> ColorEnc) {
	   case GBR:
	      receiveGBRImage	(myDescriptor);
	      return;
	   case RGB: default:
	      receiveRGBImage	(myDescriptor);
	      return;
	   case YUV:
	      receiveYUVImage	(myDescriptor);
	      return;
	   case BW:
	      receiveBWImage	(myDescriptor);
	      return;
	}
}

void	sstvWorker::receiveGBRImage (visDescriptor *myDescriptor) {
int16_t	Hlength		= myDescriptor	-> ImgWidth;
int16_t	Vlength		= myDescriptor	-> ImgHeigth;
double	chanLength [3];
double	chanStart  [3];

double	lineLength	= myDescriptor -> LineLen;
double	imageLengthInSamples =
	             (lineLength * myDescriptor -> ImgHeigth) * theRate;
double		timeofSample, timeinLine;
double		nextSyncTime = 0;
DSPCOMPLEX	sample;
uint8_t		lum;
int32_t		s;

	switch (myDescriptor -> visCode) {
	   default:
	   case 0x2C:		//	Martin M1
	   case 0x28:		//	Martin M2
	   case 0x24:		//	Martin M3
	   case 0x20:		//	Martin M4
	      chanLength [0]	= myDescriptor -> PixelLen * 
	                                  myDescriptor -> ImgWidth;
	      chanLength [1]	= myDescriptor -> PixelLen * 
	                                  myDescriptor -> ImgWidth;
	      chanLength [2]	= myDescriptor -> PixelLen * 
	                                  myDescriptor -> ImgWidth;
	      chanStart [0]	= myDescriptor -> SyncLen +
	                                  myDescriptor -> PorchLen;
	      chanStart [1]	= chanStart [0] + chanLength [0] +
	                                  myDescriptor -> SeparatorLen;
	      chanStart [2]	= chanStart [1] + chanLength [1] +
	                                  myDescriptor -> SeparatorLen;
	      break;

	   case 0x3C:		//	Scottie S1
	   case 0x38:		//	Scottie S2
	   case 0x4C:		//	Scottie DX
	      chanLength [0]	= myDescriptor -> PixelLen *
	                                   myDescriptor -> ImgWidth;
	      chanLength [1] 	= myDescriptor -> PixelLen *
	                                   myDescriptor -> ImgWidth;
	      chanLength [2]	= myDescriptor -> PixelLen *
	                                   myDescriptor -> ImgWidth;
	      chanStart [0]	= myDescriptor -> SeparatorLen;
	      chanStart [1]	= chanStart[0] + chanLength[0] +
	                                   myDescriptor -> SeparatorLen;
	      chanStart [2]	= chanStart[1] + chanLength [1] +
	                                   myDescriptor -> SyncLen +
	                                   myDescriptor -> PorchLen;
	      break;
	}

//	first, resize the frame for the image
	if ((Hlength != (*theImageP) -> width ()) ||
	    (Vlength != (*theImageP) -> height ())) {
	   delete (*theImageP);
	   (*theImageP)	= new QImage (Hlength, Vlength, QImage::Format_RGB32);
	}

	for (s = 0; s < imageLengthInSamples && running; s++) {
	   float snr;
	   timeofSample	= s / (double)theRate;
	   timeinLine	= fmod (timeofSample, lineLength);
	   sample	= getSample ();
	   lum		= fmDemodulateLuminance (sample);
	   if (computeSNR	-> addElement (sample, &snr)) 
	      displaySNR (snr);

	   if (timeofSample >= nextSyncTime) {
	      displayImage ();
	      nextSyncTime += lineLength;
	   }

//	   if ((timeinLine >= chanStart [0] &&
//	       timeinLine < chanStart [0] + chanLength [0]) ||
//	       (timeinLine >= chanStart [1] &&
//	       timeinLine < chanStart [1] + chanLength [1]) ||
//	       (timeinLine >= chanStart [2] &&
//	       timeinLine < chanStart [2] + chanLength [2])) {

//	first: the line number
	      int y_pos = (int)floor (timeofSample / lineLength);
//
//	sample in Green line?
	      if (timeinLine >= chanStart [0] &&
	          timeinLine < chanStart [0] + chanLength [0]) {
//	compute the x coordinate
	         int x = (int)floor (
	                   ((timeinLine - chanStart [0])/chanLength [0]) * Hlength);
	         (*theImageP) -> setPixel (QPoint (x, y_pos), qRgb (0, lum, 0));
	      }
	      else
//
//	second line: are we in the Blue line?
	      if (timeinLine >= chanStart [1] &&
	          timeinLine < chanStart [1] + chanLength [1]) {
	         int x = (int)floor (
	                   ((timeinLine - chanStart [1])/chanLength [1]) * Hlength);
	         (*theImageP) -> setPixel (QPoint (x, y_pos),
	                               qRgb (0, qGreen ((*theImageP) -> pixel (x, y_pos)),
	                                     lum));
	      }
	      else

//	then: it should be red, however, we'll test it
	      if (timeinLine >= chanStart [2] &&
	          timeinLine < chanStart [2] + chanLength [2]) {
	         int x = (int)floor (
	                   ((timeinLine - chanStart [2])/chanLength [2]) * Hlength);
	         (*theImageP) -> setPixel (QPoint (x, y_pos),
	                               qRgb (lum,
                                             qGreen ((*theImageP) ->
	                                                     pixel (x, y_pos)),
	                                     qBlue  ((*theImageP) ->
	                                                     pixel (x, y_pos))
	                                     ));
	      }
//	   }
	}
	displayImage ();

	fprintf (stderr, "End of Picture\n");
}

void	sstvWorker::receiveRGBImage (visDescriptor *myDescriptor) {
}
void	sstvWorker::receiveYUVImage (visDescriptor *myDescriptor) {
}
void	sstvWorker::receiveBWImage (visDescriptor *myDescriptor) {
}

void	sstvWorker::run		(void) {
uint8_t	result [2];
int16_t syncpattern	= 0;
uint8_t	theVis		= 0;
int16_t	i;
visDescriptor	*myDescriptor;
	
	try {
L:
	   displayStage (QString ("Waiting for an Edge\n"));
	   result [1] = demodulator (getSample ());
	   while (running) {
	      result [0] = result [1];
	      result [1] = demodulator (getSample ());
	      while (running && (result [1] == result [0])) {
//	         result [0] = result [1];
	         result [1] = demodulator (getSample ());
	      };
	      if (!running)
	         return;

//	this part starts VIS receiving after receiving
//	41 -> 14 -> 41 transitions (bits 4141) in sequence
	      switch (syncpattern) {
	         case 0:
	            if (result [0] == 4 && result [1] == 1) {
	               syncpattern = 41;
	               continue;
	            }
	            break;

	         case 41:
	            if (result [0] == 1 && result [1] == 4) { // got 41 -> 14
	               syncpattern = 414;
	               continue;
	            }
	            break;

	         case 414:
	            if (result [0] == 4 &&
	               result [1] == 1) { // got 41 -> 14 -> 41
	               theVis	= getVisCode ();
	               if (theVis == 0) {
	                  syncpattern = 0;
	                  continue;
	               }
	               myDescriptor = getVis (theVis);
	               if (myDescriptor == NULL) {
	                  syncpattern	= 0;
	                  continue;
	               }
	               displayStage (QString ("Imaging"));
	               receiveImage (myDescriptor);
	               running	= false;
	               goto L;		// next run
	            }
	            break;
	      }
	      syncpattern = 0;
	   }
	}
	catch  (int e) {
	   fprintf (stderr, "end of run\n");
	}
}

