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
 *
 */
// throb.cxx  --  throb modem
//
//	This file is part of the sdr-j sw receiver.
//	This throb decoder is a rewrite of the throb modem by
//	Dave Freese
// Copyright (C) 2006-2010
//		Dave Freese, W1HKJ
// ThrobX additions by Joe Veldhuis, KD8ATU
//
// This file is part of fldigi.  Adapted from code contained in gmfsk source code
// distribution.
//  gmfsk Copyright (C) 2001, 2002, 2003
//  Tomi Manninen (oh2bns@sral.fi)
//
// This file is part of fldigi.
//
// Fldigi is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Fldigi is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with fldigi.  If not, see <http://www.gnu.org/licenses/>.
// ----------------------------------------------------------------------------

#include	"throb.h"
#include	"oscillator.h"
#include	<QSettings>
#include	"throbTables.h"

#define	HALF	0
#define	FULL	1
static inline
int16_t	tonesformode (uint8_t throb_mode) {
	switch (throb_mode) {
	   default:
	   case MODE_THROB1:
	   case MODE_THROB2:
	   case MODE_THROB4:
	      return 9;

	   case MODE_THROBX1:
	   case MODE_THROBX2:
	      return 11;
	}
}

static inline
int16_t charsformode (uint8_t throb_mode) {
	switch (throb_mode) {
	   default:
	   case MODE_THROB1:
	   case MODE_THROB2:
	   case MODE_THROB4:
	      return 45;

	   case MODE_THROBX1:
	   case MODE_THROBX2:
	      return 55;
	}
}

static	inline
bool	isThrob (uint8_t throb_mode) {
	if ((throb_mode == MODE_THROB1) ||
	    (throb_mode == MODE_THROB2) ||
	    (throb_mode == MODE_THROB4))
	   return true;
	return false;
}
//
	throb::throb (int32_t rate, uint8_t mode, QwtPlot *scope) {
double		half_bw;
float		*fp;
int16_t		i;

	sampleRate	= rate;
	downRate	= sampleRate / 250;
	throbMode 	= mode;
	throbscope	= scope;
	throbIF		= THROB_IF;
	afcOn		= false;
	throbReverse 	= false;
//
	for (i = 0; i < 55; i ++)
	   rxtone [i]	= NULL;

	switch (throbMode) {
	case MODE_THROB1:
	   symlen	= (SYMLEN_1 * rate / 8000);
	   rxsymlen 	= symlen / downRate;
//
//	pulse for filter
	   fp = (float *)alloca (rxsymlen * sizeof (float));
	   mk_pulse (fp, rxsymlen, HALF);
	   
	   idlesym	= 0;
	   spacesym	= 44;
	   freqs	= ThrobToneFreqsNarrow;
	   half_bw	= 36.0;
	   break;

	case MODE_THROB2:
	   symlen	= (SYMLEN_2 * rate) / 8000;
	   rxsymlen 	= symlen / downRate;
	   fp = (float *)alloca (rxsymlen * sizeof (float));
	   mk_pulse (fp, rxsymlen, HALF);

	   idlesym 	= 0;
	   spacesym 	= 44;
	   freqs	= ThrobToneFreqsNarrow;
	   half_bw	= 36.0;
	   break;

	case MODE_THROB4:
	default:
	   symlen 	= (SYMLEN_4 * rate) / 8000;
	   rxsymlen 	= symlen / downRate;
	   fp = (float *)alloca (rxsymlen * sizeof (float));
	   mk_pulse (fp, rxsymlen, FULL);
	   idlesym	= 0;
	   spacesym	= 44;
	   freqs	= ThrobToneFreqsWid;
	   half_bw	= 72.0;
	   break;

        case MODE_THROBX1:
           symlen	= (SYMLEN_1 * rate) / 8000;
	   rxsymlen 	= symlen / downRate;
	   fp = (float *)alloca (rxsymlen * sizeof (float));
           mk_pulse (fp, rxsymlen, HALF);
	   idlesym	= 0;
	   spacesym	= 1;
	   freqs	= ThrobXToneFreqsNarrow;
           half_bw	= 47.0;
	   break;

        case MODE_THROBX2:
           symlen 	= (SYMLEN_2 * rate / 8000);
	   rxsymlen 	= symlen / downRate;
	   fp = (float *)alloca (rxsymlen * sizeof (float));
           mk_pulse (fp, rxsymlen, HALF);
	   idlesym	= 0;
	   spacesym	= 1;
	   freqs	= ThrobXToneFreqsNarrow;
           half_bw	= 47.0;
           break;
	}

	ourScope	= new fft_scope (throbscope,
	                                 64,
	                                 1,
	                                 50,
	                                 256,
	                                 rxsymlen,
	                                 4);
	ourScope	-> SelectView (WATERFALL_MODE);
	fft_filter = new fftFilter (2048, 255);
	fft_filter -> setLowPass (half_bw, sampleRate);

	syncFilter	= new Basic_FIR (symlen / downRate);
	syncFilter	-> setTaps (rxsymlen, fp, NULL);

	for (i = 0; i < tonesformode (throbMode); i++)
	   rxtone[i] = mk_rxtone (freqs[i], symlen, downRate);

	metrics		= 0.0;
	thresHold	= 0;
	symptr 		= 0;
	localOscillator	= new Oscillator (sampleRate);
	for (i = 0; i < 1000; i++)
	   syncbuf [i] =  0.0;
	rxcntr 		= rxsymlen;
	waitsync 	= 1;
	deccntr 	= 0;
	symptr 		= 0;
	shift 		= 0;
	lastchar 	= '\0';
	clrText	();
}

	throb::~throb	(void) {
int16_t	i;
	delete	ourScope;
	delete	syncFilter;
	delete	localOscillator;
	delete	fft_filter;
	for (i = 0; i < 55; i ++)
	   if (rxtone [i] != NULL)
	      delete[]  rxtone[i];
}

DSPCOMPLEX throb::doDecode (DSPCOMPLEX z) {
static	int cnt	= 0;
//	shift down to 0 +- 32 (64) Hz
//	and lowpass

	if (--cnt < 0) {
	   cnt = sampleRate / 10;
	   setMarker (throbIF);
	}

	z = z * localOscillator -> nextValue (throbIF); 
	z = fft_filter	-> Pass (z);

//	downRate to 250 and push to the receiver */
	if (++deccntr >= downRate) {
	   rxcntr -= 1.0;
	   ourScope	-> addElement (z);
//	do symbol sync 
	   throb_sync (z);
//	decode
	   throb_rx (z);
	   symptr = (symptr + 1) % rxsymlen;
	   deccntr = 0;
	}

	return z;
}

void	throb::throb_sync (DSPCOMPLEX z) {
float f, maxval = 0;
int16_t i, maxpos = 0;

//	"rectify", filter and store input 
	f = syncFilter -> Pass  (abs (z));
	syncbuf [symptr] = f;

	/* check counter if we are waiting for sync */
	if (waitsync == 0 || rxcntr > (rxsymlen / 2.0))
	   return;

	for (i = 0; i < rxsymlen; i ++) {
	   f = syncbuf [(i + symptr + 1) % rxsymlen];
	   if (f > maxval) {
	      maxpos = i;
	      maxval = syncbuf [(i + symptr + 1) % rxsymlen];
	   }
	}

//	good guess for the sync point
	rxcntr	+= (maxpos - rxsymlen / 2) / (tonesformode (throbMode) - 1);
	waitsync = 0;
}
//
//	Tone correlator, filter a tone component out through convolving
//	with the predefined tone
static inline 
DSPCOMPLEX  cmac (const DSPCOMPLEX *a, const DSPCOMPLEX *b,
	          int16_t ptr, int16_t len) {
DSPCOMPLEX	tmp	= 0;
int16_t		i;

	ptr =  ptr % len;
	for (i = 0; i < len; i++) {
	   tmp += a[i] * b[ptr];
	   ptr = (ptr + 1) % len;
	}
	return tmp;
}
//
//	Just as a gadget, I implemented tone decoding by taking the
//	fft of the last rxsymbol samples. Cannot say that it was
//	better, so it was deleted after all.
void	throb::throb_rx (DSPCOMPLEX in) {
DSPCOMPLEX rxword [MAX_TONES];
int16_t i, tone1, tone2, maxtone;
static	int cnt	= 0;

	symbol [symptr] = in;
	if (rxcntr > 0.0)
	   return;

// correlate against the predefined tones
	for (i = 0; i < tonesformode (throbMode); i++)
	   rxword [i] = cmac (rxtone[i], symbol, symptr + 1, rxsymlen);
//
//	and find the strongest tones
	maxtone = findtones(rxword, &tone1, &tone2);
// decode
	if (!throbReverse)
	   decodechar (tone1, tone2);
	else
	   decodechar (tonesformode (throbMode) - 1 - tone2,
	               tonesformode (throbMode) - 1 - tone1);

	if (afcOn && (metrics >= thresHold)) {
	   DSPCOMPLEX z1, z2;
	   float f;
//
//	for error correction, we compute the freq difference
//	between the current maxtone and the previous one
//	Note that correction basically works for only half
//	the distance of two subsequent tones.
	   z1	= rxword [maxtone];
	   z2	= cmac (rxtone [maxtone], symbol, symptr + 2, rxsymlen);
	   f	= arg (conj (z2) * z1) /
	                       ((2 * downRate * M_PI) / sampleRate);
	   f	+= freqs [maxtone];
	   throbIF	-= f / (tonesformode (throbMode) - 1);
	}

//	done with this symbol, start over 
	rxcntr 		= rxsymlen;
	waitsync 	= 1;
	if (++cnt > 10) {
	   cnt = 0;
	   showS2N (s2n);
	   showMetrics (metrics);
	}
}

int16_t throb::findtones (DSPCOMPLEX *word,
	                  int16_t *tone1, int16_t *tone2) {
float max1, max2;
int16_t maxtone, i;
float	signalStrength, noise;
//
//	first look at "strongest" one
	max1	= 0;
	*tone1	= 0;
	for (i = 0; i < tonesformode (throbMode); i++) {
	   if (abs (word [i]) > max1 ) {
	       max1 =  abs (word[i]);
	       *tone1 = i;
	   }
	}
//
//	then the second in strength
	maxtone	= *tone1;
	max2	= 0;
	*tone2	= 0;
	for (i = 0; i < tonesformode (throbMode); i++) {
	   if ((i != *tone1) && (abs (word [i]) > max2)) {
 	      max2 = abs (word[i]);
	      *tone2 = i;
	   }
	}
//
//	Now, *tone1, *tone2 are the indices of the two strongst tones
//	in the set
//
//	handle single-tone symbols (Throb only)
	if (isThrob (throbMode))
	   if (max1 > max2 * 2)
	      *tone2 = *tone1;
//	exchange
	if (*tone1 > *tone2) {
	   int16_t temp = *tone1;
	   *tone1 = *tone2;
	   *tone2 = temp;
	}
//
//	Determine the signalstrength 
	signalStrength = (abs (word [*tone1]) + abs (word [*tone2])) / 2; 
//
//	Noise is defined by the other tone frequencies
	noise = 0.0;
	for (i = 0; i < tonesformode (throbMode); i++) {
	   if ((i != *tone1) && (i != *tone2))
	      noise += abs (word[i]);
	}
//
//	average noise level is therefore
	noise	= noise / (tonesformode (throbMode) - 2);
	metrics	= decayingAverage (metrics, signalStrength / (noise + 1e-6), 8);
	s2n = clamp (10.0 * log10 (metrics) - 3.0, 0.0, 100.0);
	return maxtone;
}

//call this whenever a space or idle is sent or received
void throb::flip_syms (void)  {
	if (isThrob (throbMode)) 
	   return;

	if (idlesym == 0) {
	   idlesym = 1;
	   spacesym = 0;
	} else {
	   idlesym = 0;
	   spacesym = 1;
	}
}

void	throb::decodeThrob (int16_t tone1, int16_t tone2) {
int16_t	i;
	if (shift == true) {
	   if (tone1 == 0 && tone2 == 8)
	      showChar ('?');

	   if (tone1 == 1 && tone2 == 7)
	      showChar ('@');

	   if (tone1 == 2 && tone2 == 6)
	      showChar ('=');

           if (tone1 == 4 && tone2 == 4)
	      showChar ('\n');

	   shift = false;
	   return;
	}
//
//	no shift
	if (tone1 == 3 && tone2 == 5) {
	   shift = true;
	   return;
	}
//
//	nothing special here, Look up in table
	for (i = 0; i < charsformode (throbMode); i++) {
	   if (throbTonePairs[i][0] == tone1 + 1 &&
	       throbTonePairs[i][1] == tone2 + 1) {
	      showChar (ThrobCharSet[i]);
	      return;
	   }
	}
}

void	throb::decodeThrobx (int16_t tone1, int16_t tone2) {
int16_t	i;

	for (i = 0; i < charsformode (throbMode); i++) {
	   if (ThrobXTonePairs[i][0] == tone1 + 1 &&
	       ThrobXTonePairs[i][1] == tone2 + 1) {
	      if (i == spacesym || i == idlesym) {
	         if (lastchar != '\0' && lastchar != ' ') {
	            showChar (ThrobXCharSet[1]);
	            lastchar = ' ';
	         }
	         else {
	            lastchar = '\0';
	         }
	         flip_syms();
	      } else {
	         showChar (ThrobXCharSet[i]);
	         lastchar = ThrobXCharSet[i];
	      }
	   }
	}
}

void throb::decodechar (int16_t tone1, int16_t tone2) {
	if (isThrob (throbMode))
	   decodeThrob  (tone1, tone2);
//ThrobX mode. No shifted case, but idle and space symbols alternate
	else
	   decodeThrobx (tone1, tone2);
}

void	throb::mk_pulse (float *pulse, int16_t len, int16_t mode) {
int16_t	i;
int16_t	div	= mode == HALF ? 2 : 1;
	for (i = 0; i < len; i++) 
	   pulse [i] = 0.5 * (1 - cos (2 * M_PI * i / (len / div)));
}

// Make a "downRate" times down sampled complex prototype tone for rx.
DSPCOMPLEX *throb::mk_rxtone (float freq, int16_t len, int16_t downrate) {
float		baseTone;
DSPCOMPLEX	*tone;		// the output vector
double x;
int16_t i;

	tone = new DSPCOMPLEX [len / downrate + 1];

	for (i = 0; i < len; i += downrate) {
	   baseTone = 0.5 * (1 - cos (2 * M_PI * i / (len / 2)));
//
//	modulate with the frequency
	   x = -2.0 * M_PI * freq * i / sampleRate;
	   tone [i / downrate] =
	      DSPCOMPLEX (baseTone * cos(x), baseTone * sin(x));
	}

	return tone;
}

void	throb::setReverse	(bool b) {
	throbReverse	= b;
}

void	throb::setAfc		(bool b) {
	afcOn	= b;
	if (!b)
	   throbIF = THROB_IF;
}

void	throb::setThreshold	(int16_t t) {
	thresHold	= t;
}

