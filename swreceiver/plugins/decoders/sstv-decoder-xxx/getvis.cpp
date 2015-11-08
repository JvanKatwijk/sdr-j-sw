#
#include <stdlib.h>
#include <math.h>
#include <fftw3.h>
#include "common.h"
#include	"sstv.h"
/* 
 *
 * Detect VIS & frequency shift
 *
 * Each bit lasts 30 ms (30 * theRate / 1000 samples)
 */

	visDetector::visDetector (int32_t theRate) {
	Power	= new double [1024];
	HedrBuf	= new double [100];
	tone	= new double [100];
	fftLength	= 1024;
	windowLength	= 20 * theRate / 1000;
	theBuffer	= new DSPCOMPLEX [windowLength / 2];
	theWindow	= new double [windowLength];
  // Create 20ms Hann window
	for (i = 0; i < windowLength; i ++)
	   theWindow [i] = 0.5 * (1 - cos ((2 * M_PI * (double)i) / (windowLength - 1)) );
	the_fft		= new common_fft (fftLength);
	theVector	= the_fft -> getVector ();
}

	visDetector::~visDetector	(void) {
	delete [] Power;
	delete [] HedrBuf;
	delete [] tone;
	delete [] theBuffer;
	delete [] theWindow;
	delete    the_fft;
}


uint8_t visDetector::GetVIS (void) {
int16_t	selmode, ptr=0;
int32_t	VIS = 0, Parity = 0;
int32_t	 HedrPtr = 0;
int32_t	i=0, j=0, k=0, MaxBin = 0;
bool	gotVis;
uint8_t	Bit[8] = {0},
	ParityBit = 0;

	for (i = 0; i < fftLength; i++)
	   theVector [i] = DSPCOMPLEX (0, 0);

	while (true ) {
	   if (Abort || ManualResync)
	      return(0);

//	Read 10 ms from sound card
//	shift contents of the buffer into the fft structure
	   memcpy (fftBuffer,
	           theBuffer, 10 * theRate / 1000 * sizeof (DSPCOMPLEX));
	   for (i = 0; i < 10 * theRate / 1000; i ++)
	      fftBuffer [windowSize / 2 + i] = theBuffer [i] = getSample ();

	   for (i = 0; i < fftSize - windowSize; i ++)
	      fftBuffer [windowSize + i] = DSPCOMPLEX (0, 0);
//	Apply Hann window
	   for (i = 0; i < windowSize; i ++)
	      fftBuffer = cmul (fftBuffer [i], theWindow [i]);

//	FFT of last 20 ms
	   the_fft	-> do_FFT ();

//	Find the bin with most power in the positive frequencies
	   MaxBin = 0;
	   for (i = 0; i < fftSize / 2; i ++) {
	      Power[i] =  abs (fftBuffer [i]);
	      if (( freq (i) >= 500 && freq (i) < 3300) &&
	          (MaxBin == 0 || Power[i] > Power[MaxBin]))
	         MaxBin = i;
	   }

//	Find the peak frequency by Gaussian interpolation
	   if (freq (MaxBin) > 500 &&  freq (MaxBin) < 3300 &&
	       Power[MaxBin] > 0 &&
	       Power[MaxBin+1] > 0 && Power[MaxBin-1] > 0)
	      HedrBuf [HedrPtr] = MaxBin +
	              (log( Power[MaxBin + 1] / Power[MaxBin - 1] )) /
                      (2 * log( pow(Power[MaxBin], 2) /
	                        (Power[MaxBin + 1] * Power[MaxBin - 1])));
	   else
	      HedrBuf[HedrPtr] = HedrBuf[(HedrPtr-1) % 45];

//	In Hertz, HedrBuf holds 45 * samples, each representing 10 msec
	   HedrBuf [HedrPtr] = HedrBuf[HedrPtr] / fftSize * theRate;
	   HedrPtr = (HedrPtr + 1) % 45;

//	Frequencies in the last 450 msec
	   for (i = 0; i < 45; i++)
	      tone[i] = HedrBuf[(HedrPtr + i) % 45];

//	Is there a pattern that looks like (the end of) a
//	calibration header + VIS?
//	Tolerance ±25 Hz
	   CurrentPic.HedrShift = 0;
	   gotvis    = FALSE;
	   for (i = 0; i < 3; i++) {
	      if (CurrentPic.HedrShift != 0)
	         break;
	      for (j = 0; j < 3; j++) {
                 if ( (tone [1*3+i]  > tone [0+j] - 25  &&
	               tone [1*3+i]  < tone [0+j] + 25)  && // 1900 Hz leader
                      (tone [2*3+i]  > tone [0+j] - 25  &&
	               tone [2*3+i]  < tone [0+j] + 25)  && // 1900 Hz leader
                      (tone [3*3+i]  > tone [0+j] - 25  &&
	               tone [3*3+i]  < tone [0+j] + 25)  && // 1900 Hz leader
                      (tone [4*3+i]  > tone [0+j] - 25  &&
	               tone [4*3+i]  < tone [0+j] + 25)  && // 1900 Hz leader
	              (tone [5*3+i]  > tone [0+j] - 725 &&
	               tone [5*3+i]  < tone [0+j] - 675) && // 1200 Hz start bit
// ...8 VIS bits...
	              (tone[14*3+i] > tone[0+j] - 725 && tone[14*3+i] < tone[0+j] - 675)    // 1200 Hz stop bit
	            ) {
// Attempt to reconstruct VIS
	            gotVis = TRUE;
	            for (k = 0; k < 8; k++) {
	               if (tone[6*3+i+3*k] > tone[0+j] - 625 &&
	                   tone[6*3+i+3*k] < tone[0+j] - 575)
	                  Bit[k] = 0;
	               else
	               if (tone[6*3+i+3*k] > tone[0+j] - 825 &&
	                   tone[6*3+i+3*k] < tone[0+j] - 775)
	                  Bit[k] = 1;
	               else { // erroneous bit
	                  gotVis = FALSE;
	                  break;
	               }
	            }

	            if (gotVis) {
	               HedrShift = tone [0 + j] - 1900;
	               VIS = Bit [0] +
	                    (Bit [1] << 1) +
	                    (Bit [2] << 2) +
	                    (Bit [3] << 3) +
	                    (Bit [4] << 4) +
	                    (Bit [5] << 5) +
	                    (Bit [6] << 6);
	               ParityBit = Bit[7];
	               printf("  VIS %d (%02Xh) @ %+d Hz\n",
	                         VIS, VIS, CurrentPic.HedrShift);

	               Parity = Bit[0] ^ Bit[1] ^ Bit[2] ^
	                        Bit[3] ^ Bit[4] ^ Bit[5] ^ Bit[6];

	               if (Parity != ParityBit) {
	                  printf("  Parity fail\n");
	                  gotvis = FALSE;
	               }
	               else
	               if (VISmap [VIS] == UNKNOWN) {
	                  printf("  Unknown VIS\n");
	                  gotvis = FALSE;
	               }
	               else {	// read the rest of the stopbit
	                  for (i = 0; i < halfBitSamples; i ++)
	                     (void)getSample ();
	                  return true;
                       }
                    }
                 }
	      }
	   }
	}
	return false;
}
