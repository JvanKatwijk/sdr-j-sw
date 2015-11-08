#
/*
 *    Copyright (C) 2008, 2009, 2010
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Programming
 *
 *    This file is part of the SDR-J (JSDR).
 *    Many of the ideas as implemented in SDR-J are derived from
 *    other work, made available through the GNU general Public License. 
 *    All copyrights of the original authors are recognized.
 *
 *    This domino software is derived from sources
 *    from the domino decoder in fldigi.
 *
 *    Copyright (C) 2001, 2002, 2003
 *    Tomi Manninen (oh2bns@sral.fi)
 *    Copyright (C) 2006
 *    Hamish Moffatt (hamish@debian.org)
 *    Copyright (C) 2006
 *    David Freese (w1hkj@w1hkj.com)
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
#include	"domino-decoder.h"
#include	<stdint.h>
#ifndef	__MINGW32__
#include	"alloca.h"
#endif
#include	<QSettings>
/*
 *	Standard Nasa coefficients for the viterbi decoder
 */
#define	K			7
#define	POLY1			0x6d
#define	POLY2			0x4f

#define	BASEFREQ		500.0
#define	FIRSTIF			800.0

#define	DOMINOEX_BW		1.5
#define	NUMTESTS		10


QWidget	*dominoDecoder::createPluginWindow	(int32_t rate,
	                                         QSettings *s) {
QString temp;
int16_t	k;

	theRate			= rate;
	dominoSettings		= s;
	myFrame			= new QFrame;
	setupUi (myFrame);
	fprintf (stderr, "domino Decoder is loaded\n");
	setup_Device	(rate);
	
	dominoSettings	-> beginGroup ("dominoDecoder");
	temp	= dominoSettings -> value ("dominoMode",
	                            dominoMode -> currentText ()). toString ();
	k	= dominoMode	-> findText (temp);
	if (k != -1)
	   dominoMode -> setCurrentIndex (k);
	domino_setMode		(dominoMode -> currentText ());

	temp	= dominoSettings -> value ("dominoFec",
	                            dominoFec -> currentText ()). toString ();
	k	= dominoFec	-> findText (temp);
	if (k != -1)
	   dominoFec	-> setCurrentIndex (k);
	domino_setFec (dominoFec -> currentText ());

	temp	= dominoSettings -> value ("dominoReverse",
	                            dominoReverse -> currentText ()). toString ();
	k	= dominoReverse -> findText (temp);
	if (k != -1)
	   dominoReverse -> setCurrentIndex (k);
	domino_setReverse (dominoReverse -> currentText ());

	k	= dominoSettings -> value ("dominoSquelch",
	                            dominoSquelch -> value ()). toInt ();
	domino_setSquelch (k);
	dominoSettings	-> endGroup ();

	connect (dominoMode, SIGNAL (activated (const QString &)),
	         this, SLOT (domino_setMode (const QString &)));
	connect (dominoFec, SIGNAL (activated (const QString &)),
	         this, SLOT (domino_setFec (const QString &)));
	connect (dominoReverse, SIGNAL (activated (const QString &)),
	         this, SLOT (domino_setReverse (const QString &)));
	connect (dominoSquelch, SIGNAL (valueChanged (int)),
	         this, SLOT (domino_setSquelch (int)));
	domino_setSquelch	(dominoSquelch		-> value ());
	domino_setFec		(dominoFec		-> currentText ());
	domino_setReverse	(dominoReverse		-> currentText ());
	dominoText		= QString ();
	dominoText2		= QString ();

	return myFrame;
}

	decoderInterface::~decoderInterface	(void) {
}

	dominoDecoder::~dominoDecoder		(void) {
int16_t	i;

	for (i = 0; i < MAXFFTS; i ++)
	   if (binsfft [i])
	      delete binsfft [i];

	if (spectrumCache != NULL)
	   delete[] spectrumCache;
	dominoSettings	-> beginGroup ("dominoDecoder");
	dominoSettings	-> value ("dominoMode",
	                           dominoMode -> currentText ());
	dominoSettings	-> value ("dominoFec",
	                           dominoFec -> currentText ());
	dominoSettings	-> value ("dominoReverse",
	                           dominoReverse -> currentText ());
	dominoSettings	-> value ("dominoSquelch",
	                           dominoSquelch -> value ());
	dominoSettings	-> endGroup ();
	delete	dominoInterleaver;
	delete	dominoViterbi;
	delete	syncFilter;
	delete	bandFilter;
}

void	dominoDecoder::setup_Device (int32_t wr) {
int16_t	i;

	theRate			= wr;

	dominoSyncCounter	= 0;
	symcounter		= 0;
	Fec_symcounter		= 0;
	dominoNoiseValue	= 0;

	for (i = 0; i < MAXFFTS; i ++)
	   phase [i]	= 0;

	for (i = 0; i < MAXFFTS; i ++)
	   binsfft [i] = 0;

	spectrumCache		= NULL;
	bandFilter		= new fftFilter (1024, (uint16_t)127);
	syncFilter		= new average (8);
	Datashiftreg		= 0;
	staticburst		= false;
	outofRange		= false;
	dominoSquelch_on	= false;
	dominoFec_on		= false;
	dominoReverse_on	= false;
	dominoModus		= MODE_DOMINOEX8;	// default
	dominoViterbi		= new viterbi (K, POLY1, POLY2);
	dominoViterbi		-> viterbi_set_traceback (45);
	dominoViterbi		-> viterbi_set_chunksize ((int32_t)1);
	dominoInterleaver	= new Interleaver (4, 4,
	                                    Interleaver::INTERLEAVE_REV);
	dominoSignalValue	= 0;

	setupInterfaceforMode	(dominoModus);
}

bool	dominoDecoder::initforRate	(int32_t workingRate) {
	if (theRate == workingRate)
	   return true;

	theRate = workingRate;
	setupInterfaceforMode	(dominoModus);
	setDetectorMarker	((int32_t)FIRSTIF);
	return true;
}

int32_t	dominoDecoder::rateOut		(void) {
	return theRate;
}

int16_t	dominoDecoder::detectorOffset	(void) {
	return FIRSTIF;
}

void	dominoDecoder::domino_setMode (const QString &s) {
	if (s == "domino5")
	   dominoModus = dominoDecoder::MODE_DOMINOEX5;
	else
	if (s == "domino11")
	   dominoModus = dominoDecoder::MODE_DOMINOEX11;
	else
	if (s == "domino22")
	   dominoModus = dominoDecoder::MODE_DOMINOEX22;
	else
	if (s == "domino4")
	   dominoModus = dominoDecoder::MODE_DOMINOEX4;
	else
	if (s == "domino16")
	   dominoModus = dominoDecoder::MODE_DOMINOEX16;
	else
	   dominoModus = dominoDecoder::MODE_DOMINOEX8;
	
	setupInterfaceforMode	(dominoModus);
	setDetectorMarker	(FIRSTIF);
}

void	dominoDecoder::domino_setFec (const QString &s) {
	dominoFec_on	= s == "fec";
}

void	dominoDecoder::domino_setSquelch (int m) {
	squelchValue	= m;
	dominoSquelch_on	= (m != 0);
}

void	dominoDecoder::domino_setReverse (const QString &s) {
	dominoReverse_on	= s == "reverse";
}

void	dominoDecoder::setupInterfaceforMode (int16_t md) {
int16_t	lowend, highend;
int16_t	i;

	dominoModus	= md;
	switch (dominoModus) {
	   case MODE_DOMINOEX4:
	      doublespaced	= 2;
	      dominoBaudrate	= 3.90625;
	      break;

	   case MODE_DOMINOEX5:
	      doublespaced	= 2;
	      dominoBaudrate	= 5.3833;
	      break;

	   case MODE_DOMINOEX8:
	   default:
	      doublespaced	= 2;
	      dominoBaudrate	= 7.8125;
	      break;

	   case MODE_DOMINOEX11:
	      doublespaced	= 1;
	      dominoBaudrate	= 10.766;
	      break;

	   case MODE_DOMINOEX16:
	      doublespaced	= 1;
	      dominoBaudrate	= 15.625;
	      break;

	   case MODE_DOMINOEX22:
	      doublespaced	= 1;
	      dominoBaudrate	= 21.533;
	      break;
	}

	dominoSamplesperSymbol	= (int)(theRate / dominoBaudrate);
	toneSpacing		= doublespaced * dominoBaudrate;
	bandWidth		= NUMTONES * toneSpacing;
	basetone		= (int)floor (BASEFREQ * dominoSamplesperSymbol / theRate + 0.5);
	lowend			= FIRSTIF - DOMINOEX_BW * bandWidth;
	highend			= FIRSTIF + DOMINOEX_BW * bandWidth;
	bandFilter		-> setBand (lowend, highend,
	                                     (uint16_t)theRate);

	extones			= NUMTONES / 2;
	paths			= 5;
	lotone			= basetone - extones * doublespaced;
	hitone			= basetone +
	                             (NUMTONES + extones) * doublespaced;
	numofBins		= hitone - lotone + 1;

	for (i = 0; i < MAXFFTS; i ++)
	   if (binsfft [i])
	      delete binsfft [i];

	for (i = 0; i < paths; i ++) 
	   binsfft [i] =
	           new slidingFFT (dominoSamplesperSymbol, lotone, hitone);

	if (spectrumCache != NULL)
	   delete[] spectrumCache;
	spectrumCache		= new CacheLine [2 * dominoSamplesperSymbol];
	dominoCacheLineP	= 0;
	symcounter		= 0;
	dominoSyncCounter	= 0;
	Fec_symcounter		= 0;
	metric			= 0;
	s2n			= 0;

	prev1Tone		= 0;
	prev2Tone		= 0;

	MuSymbolpair [0]	= 0;
	MuSymbolpair [1]	= 0;
	Datashiftreg		= 1;
	emit domino_clrText	();
}

DSPCOMPLEX	dominoDecoder::Mixer (int16_t n, DSPCOMPLEX in) {
DSPCOMPLEX	z;
double	f;

	z	= DSPCOMPLEX (cos (phase [n]), sin (phase [n])) * in;
//	update the phase for the oscillator for tone n
	f	= FIRSTIF - BASEFREQ - bandWidth / 2.0 +
	             float(toneSpacing * n) / paths;
	phase [n]	-= 2 * M_PI * f / theRate;
	if (phase [n] < 0)
	   phase [n] += 2 * M_PI;
	if (phase [n] >= 2 * M_PI)
	   phase [n] -= 2 * M_PI;

	return z;
}

void	dominoDecoder::doDecode (DSPCOMPLEX refz) {
DSPCOMPLEX	z;
int16_t	i, c, k;
DSPCOMPLEX	*spectrum	= spectrumCache [dominoCacheLineP]. vector;
DSPCOMPLEX	bins [numofBins];

//	OK, now we start
	refz	= bandFilter	-> Pass (refz);

	for (i = 0; i < paths; i ++) {
	   z = Mixer (i, refz);
	   binsfft [i] -> do_FFT (z, bins);
	   for (k = 0; k < numofBins; k ++)
	      spectrum [k * paths + i] = bins [k];
	}
/*
 *	see whether we are ready for decoding
 */
	if (--dominoSyncCounter <= 0) {
	   dominoSyncCounter	= dominoSamplesperSymbol;
	   currTone		= findBestTone ();
	   c			= MapTonetoNibble (currTone);
	   decodeDomino		(c);
	   decodeforFec		(c);
/*
 *	adjust the syncCounter (i.e. synchronize) if possible
 */
	   if (!staticburst &&
	       (currTone != prev1Tone) &&
	       (prev1Tone != prev2Tone)) 
	      dominoSyncCounter	+= adjustSyncCounter (currTone, prev1Tone);

	   eval_s2n (currTone);
	   prev2Tone	= prev1Tone;
	   prev1Tone	= currTone;
	}

	dominoCacheLineP ++;
	if (dominoCacheLineP >=  (int16_t)(2 * dominoSamplesperSymbol))
	   dominoCacheLineP = 0;

	outputSample (real (refz), imag (refz));
}

int16_t	dominoDecoder::MapTonetoNibble (int16_t tone) {
int16_t	c;
float	fdiff	= tone - prev1Tone;

	if (dominoReverse_on)
	   fdiff = - fdiff;

	fdiff	/= doublespaced;
	fdiff	/= paths;

	outofRange	= fabs (fdiff) > 17;
	c		= (int)floor (fdiff + 0.5) - 2;
	if (c < 0)
	   c += NUMTONES;

	return c;
}

int16_t	dominoDecoder::adjustSyncCounter (int16_t curr, int16_t prev) {
int16_t	i;
int16_t	syn = 0;
double	max = -100000;
/*
 *	run through the pipe belonging to the previous symbol
 *	and find the max value
 */
	for (i = 1; i < 2 * dominoSamplesperSymbol; i ++) {
           int16_t index =
	              (dominoCacheLineP + i) % (2 * dominoSamplesperSymbol);
	   double val = abs (spectrumCache [index]. vector [prev]);
	   if (val > max) {
	      max = val;
	      syn = i;
	   }
	}

	syn	= syncFilter -> filter (syn);
	return  (int16_t) floor (float(syn - dominoSamplesperSymbol) / NUMTONES + 0.5);
}

int16_t	dominoDecoder::findBestTone (void) {
double	x, max	= 0.0;
double	avg	= 0;
bool	cwi [paths * numofBins];
double	cwmag;
int16_t	count;
int16_t	i, j;
int16_t	besttone	= 0;

	for (i = 0; i < paths * numofBins; i ++)
	   avg += abs (spectrumCache [dominoCacheLineP]. vector [i]);
	avg /= (paths * numofBins);

	if (avg < 1e-10)
	   avg = 1e-10;

	for (i = 0; i < paths * numofBins; i ++) {
	   cwmag	= 0;
	   count	= 0;

	   for (j = 1; j <= NUMTESTS; j ++) {
	      int p = dominoCacheLineP - j;
	      if (p < 0)
	         p += 2 * dominoSamplesperSymbol;
	      cwmag = abs (spectrumCache [p]. vector [i]) / NUMTESTS;
	      if (cwmag >= 50.0 * (1.0 - 0.5) * avg)
	         count ++;
	   }
	   cwi [i] = (count == NUMTESTS);
	}

	avg	= 0;
	for (i = 0; i < paths * numofBins; i ++) {
	   if (!cwi [i]) {
	      x = abs (spectrumCache [dominoCacheLineP]. vector [i]);
	      avg += x;
	      if (x > max) {
	         max = x;
	         besttone = i;
	      }
	   }
	}

	avg 		/= (paths * numofBins);
	staticburst	= (max / avg < 1.2);

	return besttone;
}

void	dominoDecoder::decodeDomino (int16_t c) {
int16_t	sym, ch;
int16_t	i;
/*
 *	if the new symbol is the start of a new
 *	character (MSB is low), we first complete
 *	the previous character
 */
	if (!(c & 0x8)) {
	   if (symcounter <= MAX_VARICODE_LEN) {
	      sym = 0;
	      for (i = 0; i < symcounter; i ++)
	         sym |= dominoSymbolbuffer [i] << (4 * i);
	      ch = varidec (sym);
	      if (!dominoFec_on)
	         if (!staticburst && !outofRange)
	            recvchar (ch);
	   }
	   symcounter = 0;
	}
//	shift
	for (i = MAX_VARICODE_LEN - 1; i > 0; i --)
	   dominoSymbolbuffer [i] = dominoSymbolbuffer [i - 1];
	dominoSymbolbuffer [0] = c;

	symcounter ++;
	if (symcounter > MAX_VARICODE_LEN + 1)
	   symcounter = MAX_VARICODE_LEN + 1;
}

void	dominoDecoder::decodeforFec (int16_t ch) {
uint8_t symbols [4];
int16_t	i;

	for (i = 0; i < 4; i ++)
	   symbols [i] = 0;

	for (i = 0; i < 4; i ++) {
	   if ((ch & 1) == 1)
	      symbols [3 - i] = 255;
	   else
	      symbols [3 - i] = 1;
	   ch >>= 1;
	}

	if (staticburst || outofRange)
	   for (i = 0; i < 4; i ++)
	      symbols [i] = 0;

	dominoInterleaver	-> InterleaveSyms (symbols);
	for (i = 0; i < 4; i ++) {
	   int16_t	c	= 0;
	   int16_t	ch2	= 0;
	   int32_t	met	= 0;
	   MuSymbolpair [0] = MuSymbolpair [1];
	   MuSymbolpair [1] = symbols [i];

	   Fec_symcounter = Fec_symcounter ? 0 : 1;
	   if (Fec_symcounter)
	      continue;

	   c = dominoViterbi -> viterbi_decode (MuSymbolpair, &met);
	   if (c == -1)
	      continue;

	   if (dominoSquelch_on && (metric < squelchValue))
	      continue;

	   Datashiftreg = (Datashiftreg << 1) | !!c;
	   if ((Datashiftreg & 07) == 001) {
	      ch2 = varidec (Datashiftreg >> 1);
	      if (dominoFec_on)
	         recvchar (SplittoSecChar (ch2));
	      Datashiftreg = 1;
	   }
	}
}

// Convert Primary to Split Primary / Secondary character
uint16_t dominoDecoder::SplittoSecChar (uint16_t c) {
	if (c >= 127 && c < 153)
	   c += ('A' - 127) + 0x100;
	else if (c >=14 && c < 24)
	   c += ('0' - 14) + 0x100;
	else if (c >= 1 && c < 4)
	   c += (' ' - 1) + 0x100;
	else if (c == 4)
	   c = '_' + 0x100;
	else if (c >= 5 && c < 8)
	   c += ('$' - 5) + 0x100;
	else if (c >= 9 && c < 13)
	   c += ('\'' - 9) + 0x100;
	else if (c >= 24 && c < 29)
	   c += ('+' - 24) + 0x100;
	else if (c >= 29 && c < 32)
	   c += (':' - 29) + 0x100;
	else if (c >= 153 && c < 157)
	   c += ('=' - 153) + 0x100;
	else if (c >= 157 && c < 160)
	   c += ('[' - 157) + 0x100;
	return c;
}

void	dominoDecoder::eval_s2n (int16_t bestTone) {
double	s = abs (spectrumCache [dominoCacheLineP]. vector [bestTone]);
int16_t	index
	  = (dominoCacheLineP + dominoSamplesperSymbol) %
	                              (2 * dominoSamplesperSymbol);
double	n
	  = abs (spectrumCache [index]. vector [bestTone]) * (NUMTONES - 1);

	dominoSignalValue
	          = decayingAverage (dominoSignalValue,
	                               s, abs (s - dominoSignalValue) > 4 ? 4 : 32);
	dominoNoiseValue
	          = decayingAverage (dominoNoiseValue, n, 64);

	if (dominoNoiseValue)
	   s2n = 20 * log10 (dominoSignalValue / dominoNoiseValue) - 6;
	else
	   s2n	= 0;

//	metric = 4 * s2n;
//	To partially offset the increase of noise by (THORNUMTONES - 1)
//	in the noise calculations above, add
//	15 * log10 (THORNUMTONES - 1) = 18.4, and multiply by 6
	metric	= 6 * (s2n + 18.4);
	metric	= clamp (metric, 0, 100);

	domino_doshowS2N (metric);
}

void	dominoDecoder::domino_clrText () {
	dominoText = QString ("");
	dominoText2 = QString ("");
	dominotextBox	-> setText (dominoText);
	dominotextBox2	-> setText (dominoText2);
}

void	dominoDecoder::domino_addText (char c) {
	if (c < ' ') c = ' ';
	dominoText.append (QString (QChar (c)));
	if (dominoText. length () > 100)
	   dominoText. remove (0, 1);
	dominotextBox	-> setText (dominoText);
}

void	dominoDecoder::domino_addText2 (char c) {
	if (c < ' ') c = ' ';
	dominoText2.append (QString (QChar (c)));
	if (dominoText2. length () > 96)
	   dominoText2. remove (0, 1);
	dominotextBox2	-> setText (dominoText2);
}

void	dominoDecoder::domino_doshowS2N (int n) {
	domino_showS2N	-> display (n);
}

void dominoDecoder::recvchar (int16_t c) {
	if (!dominoSquelch_on || metric > squelchValue) {
	   if (c == -1)
	      return;
	   if (c & 0x100)
	      put_sec_char (c & 0xFF);
	   else
	      put_rx_char (c & 0xFF);
	}
}

void	dominoDecoder::put_sec_char (int16_t t) {
	emit domino_addText2 ((char)t);
}

void	dominoDecoder::put_rx_char (int16_t t) {
	emit domino_addText ((char)t);
}

/*
 *	The index is the varicode symbol, being 1-3 nibbles, with no padding
 *	(ie a single-symbol character uses bits 3:0 only). The value is the
 *	ASCII character. Since continuation nibbles are required to have the
 *	MSB set, significant portions of the table are unused and should not
 *	be reached.
 */
static int16_t varidecode[] = {
    32,101,116,111, 97,105,110,114,115,108,104,100, 99,117,109,102,
    -1, -1, -1, -1, -1, -1, -1, -1,112,103,121, 98,119,118,107,120,
    -1, -1, -1, -1, -1, -1, -1, -1,113,122,106, 44,  8, 13, 84, 83,
    -1, -1, -1, -1, -1, -1, -1, -1, 69, 65, 73, 79, 67, 82, 68, 48,
    -1, -1, -1, -1, -1, -1, -1, -1, 77, 80, 49, 76, 70, 78, 66, 50,
    -1, -1, -1, -1, -1, -1, -1, -1, 71, 51, 72, 85, 53, 87, 54, 88,
    -1, -1, -1, -1, -1, -1, -1, -1, 52, 89, 75, 56, 55, 86, 57, 81,
    -1, -1, -1, -1, -1, -1, -1, -1, 74, 90, 39, 33, 63, 46, 45, 61,
    -1, -1, -1, -1, -1, -1, -1, -1, 43, 47, 58, 41, 40, 59, 34, 38,
    -1, -1, -1, -1, -1, -1, -1, -1, 64, 37, 36, 96, 95, 42,124, 62,
    -1, -1, -1, -1, -1, -1, -1, -1, 60, 92, 94, 35,125, -1, 91, 93,
    -1, -1, -1, -1, -1, -1, -1, -1,126,160,161,162,163,164,165,166,
    -1, -1, -1, -1, -1, -1, -1, -1,167,168,169,170,171,172,173,174,
    -1, -1, -1, -1, -1, -1, -1, -1,175,176,177,178,179,180,181,182,
    -1, -1, -1, -1, -1, -1, -1, -1,183,184,185,186,187,188,189,190,
    -1, -1, -1, -1, -1, -1, -1, -1,191,192,193,194,195,196,197,198,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1,199,200,201,202,203,204,205,206,
    -1, -1, -1, -1, -1, -1, -1, -1,207,208,209,210,211,212,213,214,
    -1, -1, -1, -1, -1, -1, -1, -1,215,216,217,218,219,220,221,222,
    -1, -1, -1, -1, -1, -1, -1, -1,223,224,225,226,227,228,229,230,
    -1, -1, -1, -1, -1, -1, -1, -1,231,232,233,234,235,236,237,238,
    -1, -1, -1, -1, -1, -1, -1, -1,239,240,241,242,243,244,245,246,
    -1, -1, -1, -1, -1, -1, -1, -1,247,248,249,250,251,252,253,254,
    -1, -1, -1, -1, -1, -1, -1, -1,255,  0,  1,  2,  3,  4,  5,  6,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1,  7,  9, 10, 11, 12, 14, 15, 16,
    -1, -1, -1, -1, -1, -1, -1, -1, 17, 18, 19, 20, 21, 22, 23, 24,
    -1, -1, -1, -1, -1, -1, -1, -1, 25, 26, 27, 28, 29, 30, 31,127,
    -1, -1, -1, -1, -1, -1, -1, -1,128,129,130,131,132,133,134,135,
    -1, -1, -1, -1, -1, -1, -1, -1,136,137,138,139,140,141,142,143,
    -1, -1, -1, -1, -1, -1, -1, -1,144,145,146,147,148,149,150,151,
    -1, -1, -1, -1, -1, -1, -1, -1,152,153,154,155,156,157,158,159,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1,288,357,372,367,353,361,366,370,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1,371,364,360,356,355,373,365,358,
    -1, -1, -1, -1, -1, -1, -1, -1,368,359,377,354,375,374,363,376,
    -1, -1, -1, -1, -1, -1, -1, -1,369,378,362,300,264,269,340,339,
    -1, -1, -1, -1, -1, -1, -1, -1,325,321,329,335,323,338,324,304,
    -1, -1, -1, -1, -1, -1, -1, -1,333,336,305,332,326,334,322,306,
    -1, -1, -1, -1, -1, -1, -1, -1,327,307,328,341,309,343,310,344,
    -1, -1, -1, -1, -1, -1, -1, -1,308,345,331,312,311,342,313,337,
    -1, -1, -1, -1, -1, -1, -1, -1,330,346,295,289,319,302,301,317,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1,299,303,314,297,296,315,290,294,
    -1, -1, -1, -1, -1, -1, -1, -1,320,293,292,352,351,298,380,318,
    -1, -1, -1, -1, -1, -1, -1, -1,316,348,350,291,381, -1,347,349,
    -1, -1, -1, -1, -1, -1, -1, -1,382,416,417,418,419,420,421,422,
    -1, -1, -1, -1, -1, -1, -1, -1,423,424,425,426,427,428,429,430,
    -1, -1, -1, -1, -1, -1, -1, -1,431,432,433,434,435,436,437,438,
    -1, -1, -1, -1, -1, -1, -1, -1,439,440,441,442,443,444,445,446,
    -1, -1, -1, -1, -1, -1, -1, -1,447,448,449,450,451,452,453,454,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1,455,456,457,458,459,460,461,462,
    -1, -1, -1, -1, -1, -1, -1, -1,463,464,465,466,467,468,469,470,
    -1, -1, -1, -1, -1, -1, -1, -1,471,472,473,474,475,476,477,478,
    -1, -1, -1, -1, -1, -1, -1, -1,479,480,481,482,483,484,485,486,
    -1, -1, -1, -1, -1, -1, -1, -1,487,488,489,490,491,492,493,494,
    -1, -1, -1, -1, -1, -1, -1, -1,495,496,497,498,499,500,501,502,
    -1, -1, -1, -1, -1, -1, -1, -1,503,504,505,506,507,508,509,510,
    -1, -1, -1, -1, -1, -1, -1, -1,511,256,257,258,259,260,261,262,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1,263,265,266,267,268,270,271,272,
    -1, -1, -1, -1, -1, -1, -1, -1,273,274,275,276,277,278,279,280,
    -1, -1, -1, -1, -1, -1, -1, -1,281,282,283,284,285,286,287,383,
    -1, -1, -1, -1, -1, -1, -1, -1,384,385,386,387,388,389,390,391,
    -1, -1, -1, -1, -1, -1, -1, -1,392,393,394,395,396,397,398,399,
    -1, -1, -1, -1, -1, -1, -1, -1,400,401,402,403,404,405,406,407,
    -1, -1, -1, -1, -1, -1, -1, -1,408,409,410,411,412,413,414,415,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
};

int16_t	dominoDecoder::varidec (uint16_t symbol) {
//
//	The caller is responsible for ensuring
//	symbols are properly delineated. 
	return varidecode [symbol & 0xFFF];
}

#if QT_VERSION < 0x050000
QT_BEGIN_NAMESPACE
Q_EXPORT_PLUGIN2(dominodecoder, dominoDecoder)
QT_END_NAMESPACE
#endif

