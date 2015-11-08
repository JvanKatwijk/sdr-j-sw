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
#include	"sstv-buffer.h"
#include	"oscillator.h"
#include	"snr-calc.h"

static inline
bool	inRange (float v, float l, float h) {
	return l <= v && v <= h;
}

#define	SYNCING	0100
#define	IMAGING	0101

		sstvWorker::sstvWorker (int32_t theRate,
	                                RingBuffer<DSPCOMPLEX> *b,
	                                QImage **im) {
	this	-> theRate	= theRate;
	theBuffer		= b;
	this	-> theImageP	= im;
	State			= SYNCING;
	filterBuffer		= new sstvBuffer (45, theRate);
	localOscillator		= new Oscillator (theRate);
	oneBitSampleCount	= 30 * theRate / 1000;
	tones			= new float [45];
	computeSNR		= new SNRcalc (theRate);
	start ();

	running			= true;
	
}

		sstvWorker::~sstvWorker	(void) {
	running	= false;
	while (!isFinished ())
	   usleep (100);
	delete		localOscillator;
	delete[]	tones;
	delete 		computeSNR;
	delete		filterBuffer;
}

void	sstvWorker::stop	(void) {
	running = false;
}
//
//	"tone"  contains the tones as registered during the
//	last period. Each "tone" represents 10 msec of samples,
//	so here we have 450 mseconds of data.
bool	sstvWorker::getVisCode	(float *tones, uint8_t *VIS) {
int16_t	i, j;
uint8_t	Bit [10];
uint8_t	parity, parityBit;

//	Is there a pattern that looks like (the end of)
//	a calibration header + VIS?
	for (i = 0; i < 3; i++) {
	   for (j = 0; j < 3; j++) {
	      if (!inRange (tones [1 * 3 + i], tones [0 + j] - 25, // 1900 Hz
	                                       tones [0 + j] + 25))
	         continue;
	      if (!inRange (tones [2 * 3 + i], tones [0 + j] - 25, // 1900 Hz
	                                       tones [0 + j] + 25))
	         continue;
	      if (!inRange (tones [3 * 3 + i], tones [0 + j] - 25, // 1900 Hz
	                                       tones [0 + j] + 25))
	         continue;
	      if (!inRange (tones [4 * 3 + i], tones [0 + j] - 25, // 1900 Hz
	                                       tones [0 + j] + 25))
	         continue;
//
//	Now for start bit
	      if (!inRange (tones [5 * 3 + i], tones [0 + j] - 725, // 1900 Hz
	                                       tones [0 + j] - 675))
	         continue;
// ...8 VIS bits...
//	and the stop bits
	      if (!inRange (tones [14 * 3 + i], tones [0 + j] - 725, // 1900 Hz
	                                        tones [0 + j] - 675))
	         continue;

	      if (!makeVIS (&tones [6 * 3 + i], tones [0 + j], Bit))
	         return false;	// next try

	      currentShift = tones [0 + j] - 1900;
	      *VIS = Bit[0] + (Bit[1] << 1) + (Bit[2] << 2) +
	             (Bit[3] << 3) + (Bit[4] << 4) + (Bit[5] << 5) +
	             (Bit[6] << 6);
	      parityBit = Bit [7];
	      fprintf(stderr, "  VIS %o (%02Xh) @ %+d Hz\n",
	                                 *VIS, *VIS, currentShift);

	      parity = Bit[0] ^ Bit[1] ^ Bit[2] ^
	               Bit[3] ^ Bit[4] ^ Bit[5] ^ Bit[6];

	      if (parity != parityBit) {
	         fprintf (stderr, " Parity fail\n");
	         return false;
	      }
//	skip over the remainder of the stopbit
	//      for (i = 0; i < oneBitSampleCount / 2; i ++)
//	         (void)getSample ();
	      return true;
	   }		// end of j loop
	}		// end of i-loop
	return false;
}

bool 	sstvWorker:: makeVIS (float *vistone, float refTone, uint8_t *bits) {
int16_t	k;
// Attempt to reconstruct VIS
	fprintf (stderr, "Making a VIS\n");
	for (k = 0; k < 8; k++) {
	   if (inRange (vistone [3 * k], refTone - 625, refTone - 575))
	      bits [k] = 0;
	   else
	   if (inRange (vistone [3 * k], refTone - 825, refTone - 775))
	      bits [k] = 1;
	   else  // erroneous bit
	      return false;
	}
	return true;
}

void	sstvWorker::run		(void) {
uint8_t	VIS	= 0;
visDescriptor	*myDescriptor;
	try {
	   fprintf (stderr, "sstv is started\n");
	   displayStage (QString ("SYNCING"));
	   while (running) {
	      while ((State == SYNCING) && running) {
	         filterBuffer -> addElement (getSample ());
	         if (filterBuffer -> tonesAvailable () < 45)
	            continue;

	         filterBuffer -> getTones (tones);
	         if (!getVisCode (tones, &VIS)) {
	            filterBuffer -> shift (3);
	            continue;
	         }
	         myDescriptor = getVis (VIS);	
	         if (myDescriptor == NULL)
	            continue;
	         State = IMAGING;
	      }	
	      if (!running)
                 break;
	      displayStage (QString ("Imaging"));
	      receiveImage (myDescriptor);
	      State = SYNCING;
	      filterBuffer	-> reset ();
	      displayStage (QString ("SYNCING"));
	      fprintf (stderr, "Op weg naar de volgende\n");
	   }
	}
	catch  (int e) {
	   fprintf (stderr, "end of run\n");
	}
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
//
//	Just a dispatcher
void	sstvWorker::receiveImage (visDescriptor *myDescriptor) {

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
	      fprintf (stderr, "snr = %f\n", snr);
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

