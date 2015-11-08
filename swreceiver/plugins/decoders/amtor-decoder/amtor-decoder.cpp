#
/*
 *    Copyright (C) 2011, 2012, 2013
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Programming
 *
 *    This file is part of the SDR-J (JSDR).
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

#include	"amtor-decoder.h"
#include	"oscillator.h"
#include	"fir-filters.h"
#include	"utilities.h"

CharMap CCIR_476_ITA4 [34] = {
{'A', '-',  0x71},
{'B', '?',  0x27},
{'C', ':',  0x5C},
{'D', '$',  0x65},
{'E', '3',  0x35},
{'F', '!',  0x6C},
{'G', '&',  0x56},
{'H', '#',  0x4B},
{'I', '8',  0x59},
{'J', '\\', 0x74},
{'K', '(',  0x3C},
{'L', ')',  0x53},
{'M', '.',  0x4E},
{'N', ',',  0x4D},
{'O', '9',  0x47},
{'P', '0',  0x5A},
{'Q', '1',  0x3A},
{'R', '4',  0x55},
{'S', '\'', 0x69},
{'T', '5',  0x17},
{'U', '7',  0x39},
{'V', '=',  0x1E},
{'W', '2',  0x72},
{'X', '/',  0x2E},
{'Y', '6',  0x6A},
{'Z', '+',  0x63},
{015, 015,  0x0F},		
{012, 012,  0x1B},
{040, 040 , 0x2D},		/* letter shift		*/
{040, 040,  0x36},		/* figure shift		*/
{040, 040,  0x1D},		/* space		*/
{040, 040,  0x33},		/* phasing 2 (Beta)	*/
{040, 040,  0x78},		/* phasing 1 (Alpha)	*/
{040, 040,  0x00}
};
/*
 *	The local states of the amtor machine
 */
#define	AMTOR_X0	0100
#define	AMTOR_X1	0101
#define	AMTOR_X2	0102
#define	AMTOR_X3	0103
#define	AMTOR_X4	0104
#define	AMTOR_X5	0105
#define	AMTOR_X6	0106
#define	AMTOR_X7	0107

#define	AMTOR_ALPHA	0x78
#define	AMTOR_BETA	0x33
#define	AMTOR_CR	0x0F
#define	AMTOR_LF	0x1B
#define	AMTOR_DIGIT	0x36
#define	AMTOR_LETTER	0x2D
#define	AMTOR_SCRAPPED	0xFF

#define	AMTOR_IF	800
//
//
QWidget	*amtorDecoder::createPluginWindow (int32_t rate,
	                                   QSettings *s) {
	theRate		= rate;
	amtorSettings		= s;

	myFrame			= new QFrame;
	setupUi (myFrame);
	fprintf (stderr, "amtorDecoder is loaded\n");
	setup_amtorDecoder	(rate);
	return myFrame;
}

	decoderInterface::~decoderInterface	(void) {
}

	amtorDecoder::~amtorDecoder		(void) {
	amtorSettings	-> beginGroup ("amtorDecoder");
	amtorSettings	-> setValue ("afc_on",
	                              amtorAfcon -> currentText ());
	amtorSettings	-> setValue ("reverse",
	                              amtorReverse -> currentText ()); 
	amtorSettings	-> setValue ("fecError",
	                               amtorFecError -> currentText ());
	amtorSettings	-> setValue ("message",
	                              amtorMessage -> currentText ());
	amtorSettings	-> endGroup ();
	delete	LPM_Filter;
	delete	amfilter;
}

void	amtorDecoder::setup_amtorDecoder (int32_t WorkingR) {
float	H;
QString	temp;
int16_t	k;

	theRate		= WorkingR;

	LocalOscillator		= new Oscillator	(theRate);
	amtor_IF		= AMTOR_IF;
	amtor_shift		= 170;
	amtor_baudrate		= 100;
	amtor_afcon		= false;
/*
 *	filtering on the zeroed IF is only low pass filtering
 */
	H			= 0.5 * amtor_shift + amtor_baudrate;
        LPM_Filter		= new LowPassFIR (35,
	                                          (int)H, theRate);
	amtor_bitlen		= (uint16_t)(theRate / amtor_baudrate + 0.5);
	amfilter	= new average (amtor_bitlen);

//
//	set everything ready
//
	amtor_bitclk		= 0.0;
	amtor_phaseacc		= 0;
	fragment_cnt		= 0;
	amtor_oldz		= 0;
	amtor_state		= AMTOR_X0;
	amtor_decimator		= 0;
	amtor_oldfragment	= 0;
	amtor_shiftreg		= 0;
	amtor_bitCount		= 0;
	amtor_Lettershift	= 1;
	CycleCount		= 0;
	amtor_clrText ();
	amtor_showCorrection (amtor_IF);
	setDetectorMarker    (amtor_IF);
	amtor_afcon	= false;
	showAlways	= true;
//
//	settings from previous invocations
	amtorSettings	-> beginGroup ("amtorDecoder");
	temp	= amtorSettings -> value ("afc_con", 
	                         amtorAfcon -> currentText ()). toString ();
	k = amtorAfcon		-> findText (temp);
	if (k != -1)
	   amtorAfcon	-> setCurrentIndex (k);
	set_amtorAfcon		(amtorAfcon -> currentText ());

	temp	= amtorSettings -> value ("reverse",
	                          amtorReverse -> currentText ()). toString ();
	k = amtorReverse	-> findText (temp);
	if (k != -1)
	   amtorReverse -> setCurrentIndex (k);
	set_amtorReverse	(amtorReverse -> currentText ());

	temp	= amtorSettings -> value ("fecError",
	                          amtorFecError -> currentText ()). toString ();
	k = amtorFecError	-> findText (temp);
	if (k != -1)
	   amtorFecError	-> setCurrentIndex (k);
	set_amtorFecError	(amtorFecError -> currentText ());

	temp	= amtorSettings -> value ("message",
	                          amtorMessage -> currentText ()). toString ();
	k = amtorMessage	-> findText (temp);
	if (k != -1)
	   amtorMessage	-> setCurrentIndex (k);
	set_amtorMessage	(amtorMessage -> currentText ());
	amtorSettings	-> endGroup ();
	connect (amtorAfcon, SIGNAL (activated (const QString &)),
	         this, SLOT (set_amtorAfcon (const QString &)));
	connect (amtorReverse, SIGNAL (activated (const QString &)),
	         this, SLOT (set_amtorReverse (const QString &)));
	connect (amtorFecError, SIGNAL (activated (const QString &)),
	         this, SLOT (set_amtorFecError (const QString &)));
	connect (amtorMessage, SIGNAL (activated (const QString &)),
	         this, SLOT (set_amtorMessage (const QString &)));
	amtorTextstring		= QString ();
}

bool	amtorDecoder::initforRate	(int32_t workingRate) {
float	H;
	if (theRate == workingRate)
	   return true;

	theRate	= workingRate;
	delete	LocalOscillator;
	LocalOscillator	= new Oscillator (theRate);
	delete	LPM_Filter;
	H			= 0.5 * amtor_shift + amtor_baudrate;
        LPM_Filter		= new LowPassFIR (35, (int)H, theRate);
	amtor_bitlen		= (uint16_t)(theRate / amtor_baudrate + 0.5);
	delete	amfilter;
	amfilter		= new average (amtor_bitlen);
	return true;
}

int32_t	amtorDecoder::rateOut		(void) {
	return theRate;
}

int16_t	amtorDecoder::detectorOffset	(void) {
	return AMTOR_IF;
}

void	amtorDecoder::doDecode (DSPCOMPLEX z) {
float	power;
DSPCOMPLEX	localCopy;
float	f;
float	bf;

	if (++CycleCount > theRate / 8) {
	   amtor_showStrength   (amtor_strength);
	   amtor_showCorrection (amtor_IF);
	   setDetectorMarker	(amtor_IF);
	   CycleCount		= 0;
	}

	localCopy	= z;		/* keep a copy of the sample	*/

//	mix the sample with the LO and filter out unwanted products
	z	= z * LocalOscillator  -> nextValue (amtor_IF);
	z	= LPM_Filter -> Pass (z);

	power	= norm (z);
	if (power > 0) {
	   amtor_strength = get_db (power);
	}
	else
	   amtor_strength = 0;
/*
 *	slice and filter
 */
	f	= arg (z * conj (amtor_oldz)) * theRate / (2 * M_PI);
	amtor_oldz	= z;
	f	= amfilter -> filter (f);
	bf	= amtor_reversed? (f < 0.0) : (f > 0.0);

	if (++ amtor_decimator >= amtor_bitlen / 10) {
	   addbitfragment (bf ? 1 : 0);
	   if (amtor_afcon) 
	      amtor_IF += correctIF (f);
	   amtor_decimator = 0;
	}

	outputSample (real (localCopy), imag (localCopy));
}
/*
 *	according to the theory, f should equal rtty_shift / 2 when
 *	amtor_IF is in the middle. Check and adjust. The 256 is
 *	still experimental
 */
DSPFLOAT	amtorDecoder::correctIF (DSPFLOAT f) {
	if (f > 0.0)
	   f = f - amtor_shift / 2;
	else
	   f = f + amtor_shift / 2;

	if (fabs (f) < amtor_shift / 2) 
	   return  f / 256;
	return 0;
}
/*
 *	In this try, we make use of the fact that we
 *	deal with a continuous stream of samples. Each bit
 *	is exactly 10 Msec long.
 *	So, as with psk, we try to locate the beginning
 *	of the bits and determine the bit itself by the 
 *	middle of it. We call addbitfragment with a rate
 *	of 10 times per bit, i.e. 100 Hz
 */
void	amtorDecoder::addbitfragment (int8_t bf) {
	if (bf != amtor_oldfragment) {
	   fragment_cnt = 5;
	   amtor_oldfragment = bf;
	   return;
	}

	if (++fragment_cnt >= 10) {
	   addfullBit (bf);
	   fragment_cnt = 0;
	}
}
/*
 *	once we have a bit (or better, we think we have one)
 *	we add it to the current "word" which is kept in 
 *	"amtor_shiftreg", while the number of bits is
 *	kept in "amtor_bitCount"
 */	
void	amtorDecoder::addfullBit (int8_t bit) {
	amtor_shiftreg <<= 1;
	amtor_shiftreg |= (bit & 01);

	if (++amtor_bitCount >= 7) {
	   if (validate (amtor_shiftreg, 4)) {
	      decoder (amtor_shiftreg);
	      amtor_bitCount	= 0;
	      amtor_shiftreg	= 0;
	   }
	   else {
	      amtor_bitCount --;
	      amtor_shiftreg &= 077;
	   }
	}
}
/*
 *	whether or not the bits form a word in the
 *	characterset, we don't know
 */
bool	amtorDecoder::validate (uint16_t bits, int8_t cnt) {
int	i;
int	No1	= 0;

	for (i = 0; i < 7; i ++) {
	   if (bits & 01) No1 ++;
	   bits >>= 01;
	}

	return (No1 == cnt);
}
/*
 *	In decoder we process the input such that the raw
 *	characterstream that forms the messages remains
 *	and is sent to the character handler
 *	I.e., all synch data is processed and most
 *	of it is removed
 */
void	amtorDecoder::decoder (int val) {

	switch (amtor_state) {
	   case AMTOR_X0:		/* waiting for ALPHA	*/
	      if (val == AMTOR_ALPHA)
	         amtor_state = AMTOR_X1;
	      if (val == AMTOR_BETA)
	         amtor_state = AMTOR_X2;
	      break;

	   case AMTOR_X1:		/* waiting for Beta or data	*/
	      if (val == AMTOR_BETA)
	         amtor_state = AMTOR_X2;
	      else
	      {  initMessage ();
	         addBytetoMessage (val);
	         amtor_state = AMTOR_X3;
	      }
	      break;

	   case AMTOR_X2:		/* waiting for Alfa	*/
	      if (val == AMTOR_ALPHA)
	         amtor_state = AMTOR_X1;
	      break;

	   case AMTOR_X3:		/* the message		*/
	      if (val == AMTOR_ALPHA) 	/* might be the end	*/
	         amtor_state = AMTOR_X4;
	      else
	      if (val == AMTOR_BETA)
	         amtor_state = AMTOR_X5;
	      addBytetoMessage (val);
	      break;

	   case AMTOR_X4:		/* we got a possible end */
	      if (val == AMTOR_BETA)
	         amtor_state = AMTOR_X6;
	      else
	         amtor_state = AMTOR_X3;
	      addBytetoMessage (val);
	      break;

	   case AMTOR_X5:		/* we got a possible end */
	      if (val == AMTOR_ALPHA)
	         amtor_state = AMTOR_X6;
	      else
	         amtor_state = AMTOR_X3;
	      addBytetoMessage (val);
	      break;

	   case AMTOR_X6:
	      flushBuffer ();
	      amtor_state = AMTOR_X0;
	      break;

	   default:;			/* does not happen	*/
	}
}
/*
 *	The raw data forming the characters are handled
 *	by the addBytetoMessage and byteinfo will be
 *	passed on to HandleChar
 */
void	amtorDecoder::addBytetoMessage (int val) {

	if ((amtor_queue [amtor_qp] == AMTOR_ALPHA) ||
	    (amtor_queue [amtor_qp] == AMTOR_BETA)) {	/* just skip	*/
	   amtor_qp = (amtor_qp + 1) % AMTOR_DECODERQUEUE;
	   amtor_queue [(amtor_qp + 5) % AMTOR_DECODERQUEUE] = val;
	   return;
	}

	if (amtor_queue [amtor_qp] == AMTOR_SCRAPPED) {	/* just skip	*/
	   amtor_qp = (amtor_qp + 1) % AMTOR_DECODERQUEUE;
	   amtor_queue [(amtor_qp + 5) % AMTOR_DECODERQUEUE] = val;
	   return;
	}

	if (amtor_queue [amtor_qp] == amtor_queue [(amtor_qp + 5) % AMTOR_DECODERQUEUE]) {
	   HandleChar (amtor_queue [amtor_qp]);
	   amtor_queue [(amtor_qp + 5) % AMTOR_DECODERQUEUE] = AMTOR_SCRAPPED;
	   amtor_qp = (amtor_qp + 1) % AMTOR_DECODERQUEUE;
	   amtor_queue [(amtor_qp + 5) % AMTOR_DECODERQUEUE] = val;
	   return;
	}
	else
	{
	   if (amtor_fecerror)
	      HandleChar ('x');
	   else
	      HandleChar (amtor_queue [amtor_qp]);
	   amtor_queue [(amtor_qp + 5) % AMTOR_DECODERQUEUE] = AMTOR_SCRAPPED;
	   amtor_qp   = (amtor_qp + 1) % AMTOR_DECODERQUEUE;
	   amtor_queue [(amtor_qp + 5) % AMTOR_DECODERQUEUE] = val;
	   return;
	}
}

void	amtorDecoder::HandleChar (int16_t val) {
int16_t	i;
CharMap	*p = CCIR_476_ITA4;

	if (val == -1) {	/* error, show		*/
	   amtorText ((int)'x');
	   return;
	}

	if (val == AMTOR_LETTER) {
	   amtor_Lettershift = 1;
	   return;
	}

	if (val == AMTOR_DIGIT) {
	   amtor_Lettershift = 0;
	   return;
	}

	for (i = 0; p [i]. key != 0; i ++) {
	   if (p [i]. key == val) {
	      if (amtor_Lettershift)
	         amtorText (p [i]. letter);
	      else
	         amtorText (p [i]. digit);
	      return;
	   }
	}
/*
 *	we shouldn't be here
 */
	amtorText ('?');
}

void	amtorDecoder::amtorText (uint8_t c) {
	if (showAlways) {
	   amtor_addText ((char)c);
	   return;
	}
//	in case we only want to see the official messages, we
//	just wait until we see a "ZCZC" sequence of characters	
	switch (messageState) {
	   case NOTHING:
	      if ((char)c == 'Z') 
	         messageState = STATE_Z;
	      return;

	   case STATE_Z:
	      if ((char)c == 'C')
	         messageState = STATE_ZC;
	      else
	         messageState = NOTHING;
	      return;

	   case STATE_ZC:
	      if ((char)c == 'Z')
	         messageState = STATE_ZCZ;
	      else
	         messageState = NOTHING;
	      return;

	   case STATE_ZCZ:
	      if ((char)c == 'C') {
	         messageState = STATE_ZCZC;
	         amtor_addText ((char)'Z');
	         amtor_addText ((char)'C');
	         amtor_addText ((char)'Z');
	         amtor_addText ((char)'C');
	      }
	      else
	         messageState = NOTHING;
	      return;

	   case STATE_ZCZC:
	      if ((char)c == 'N')
	         messageState = STATE_N;
	      amtor_addText ((char)c);
	      return;

	   case STATE_N:
	      if ((char)c == 'N')
	         messageState = STATE_NN;
	      else
	         messageState = STATE_ZCZC;
	      amtor_addText ((char)c);
	      return;

	   case STATE_NN:
	      if ((char)c == 'N')
	         messageState = STATE_NNN;
	      else
	         messageState = STATE_ZCZC;
	      amtor_addText ((char)c);
	      return;

	   case STATE_NNN:
	      if ((char)c == 'N')
	         messageState = STATE_NNNN;
	      else
	         messageState = STATE_ZCZC;
	      amtor_addText ((char)c);
	      return;

	   case STATE_NNNN:
	      messageState = NOTHING;
	      amtor_addText ((char)c);
	      return;
	}
}

void	amtorDecoder::initMessage () {
int	i;

	for (i = 0; i < AMTOR_DECODERQUEUE; i ++)
	   amtor_queue [i] = AMTOR_BETA;
	amtor_qp = 0;
	messageState	= NOTHING;
}

void	amtorDecoder::flushBuffer () {
	amtorText ((int)' ');
	amtorText ((int)' ');
	amtorText ((int)' ');
	amtorText ((int)' ');
}

/*
 *	functions for the amtor decoder
 */
void	amtorDecoder::set_amtorAfcon (const QString &s) {
	amtor_afcon = s == "afc on";
	if (!amtor_afcon) {
	   amtor_IF	= AMTOR_IF;
	   amtor_showCorrection (amtor_IF);
	   setDetectorMarker	(amtor_IF);
	}
}

void	amtorDecoder::set_amtorReverse (const QString &s) {
	amtor_reversed = s != "normal";
}

void	amtorDecoder::set_amtorFecError (const QString &s) {
	amtor_fecerror = s == "strict fec";
}

void	amtorDecoder::set_amtorMessage (const QString &s) {
	showAlways = !(s == "message");
	messageState = NOTHING;
}

void	amtorDecoder::amtor_showStrength (float f) {
	amtorStrengthMeter	-> display (f);
}

void	amtorDecoder::amtor_showCorrection (float f) {
	amtorFreqCorrection	-> display (f);
}

void	amtorDecoder::amtor_clrText () {
	amtorTextstring = QString ("");
	amtortextBox	-> setText (amtorTextstring);
}

void	amtorDecoder::amtor_addText (char c) {
	if (c < ' ') c = ' ';
	amtorTextstring.append (QString (QChar (c)));
	if (amtorTextstring. length () > 70)
	   amtorTextstring. remove (0, 1);
	amtortextBox	-> setText (amtorTextstring);
}

#if QT_VERSION < 0x050000
QT_BEGIN_NAMESPACE
Q_EXPORT_PLUGIN2(amtordecoder, amtorDecoder)
QT_END_NAMESPACE
#endif

