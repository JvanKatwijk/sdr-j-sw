#
/*
 *    Copyright (C) 2008, 2009, 2010
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
#include	<stdio.h>
#include	"frequencytables.h"

#define	LPAR	'('
#define	RPAR	')'

#define	is_space(x)	(x < '0')
#define	is_digit(x)	('0' <= (x) && (x) <= '9')
/*
 *	when either no or a wrong file with the frequency
 *	tables is given, there is no need to stop, all answers\
 *	will be consistent (and zero).
 */
	frequencytables::frequencytables (const char *fn) {
int16_t	i;

	for (i = 0; i < 4; i ++)
	   Bands [i] = NULL;
	nr_of_Bands	= 0;
	Filename	= fn;
	FreqFILE	= fopen (fn, "r");
	if (FreqFILE == NULL) {
	   fprintf (stderr, "Unable to read frequency tables %s\n", fn);
	   return;
	}

	getAchar	();
	readPredefinedFrequencySettings ();
}

	frequencytables::~frequencytables (void) {
int16_t	i;

	if (FreqFILE != NULL)
	   fclose (FreqFILE);

	for (i = 0; i < nr_of_Bands; i ++)
	   delete Bands [i];
}
/*
 * 	This function will be called to look for a (band, voltage)
 * 	pair, given a frequency, such that the elektor-front end can be set
 */
int16_t	frequencytables::findIndexofBand (int32_t freq) {
int16_t	i;

	freq = freq / 1000;
	for (i = 0; i < nr_of_Bands; i ++) 
	   if (Bands [i] != NULL)
	      if (Bands [i] -> getLowerBound () <= freq &&
	          Bands [i] -> getUpperBound () >= freq)
	      return i;

	return -1;
}
/*
 * 	Most likely, we need to interpolate between two frequencies
 * 	in subsequent entries to find the frequency,
 * 	and then interpolate to generate the approximate voltage value
 */
bool	frequencytables::findTuningData (int32_t freq,
	                                 int16_t *rband,
	                                 int16_t *voltage) {
int16_t	b;
int16_t	j;
int32_t	lower, higher;

	b = findIndexofBand (freq);
	if (b == -1) {
	   *rband = 0;
	   *voltage = 0;
	   return false;
	}

	*rband = Bands [b] -> getBandNumber ();
	freq = freq / 1000;		/* make it KHz	*/

	higher = Bands [b] -> getFrequency (0);
	for (j = 0; j < Bands [b] -> getAmount () - 1; j ++) {
	   lower = higher;		/* previous one		*/
	   higher = Bands [b] -> getFrequency (j + 1);
	   if (lower <= freq && freq <= higher) {
	      float fraction =
	             ((float)(freq - lower)) / (float)(higher - lower);
	      int lv  = Bands [b] -> getVoltage   (j);
	      int hv  = Bands [b] -> getVoltage (j + 1);
	      *voltage = (int)(lv + fraction * (hv - lv)) & 0377;
	      return true;
	   }
	}
/*
 * 	we did not find a match,  which only happens if the
 * 	table is incorrect, which obviously is possible
 */
	return false;
}
/*
 * 	FILE format
 *	(bandnr amount (freq voltage)*)*
 */
void	frequencytables::readPredefinedFrequencySettings (void) {
band	*t;
int16_t	i;

	checkinput (LPAR);
	for (i = 0; i < 4; i ++) {
	   t = readBand ();
	   if (t == NULL) 		/* just forget it	*/
	      return;
	   Bands [i] = t;
	   nr_of_Bands ++;
	}
	checkinput (RPAR);
}

band	*frequencytables::readBand	(void) {
int16_t	i;
int16_t	bandNumber;
int16_t	amount;
band	*Band;
int32_t	Frequency;
int16_t	Voltage;
/*
 * 	format is: (bandnr amount (f1, v1) ...)
 */
	checkinput (LPAR);
	bandNumber	= readNumber	();
	if (bandNumber == -1)		/* we passed EOF	*/
	   return NULL;
	amount		= readNumber	();
	if (amount == -1)	/* we passed EOF	*/
	   return NULL;

	Band	= new band (bandNumber, amount);
/*
 * 	Here we keep on reading, even if we erroneously passed an EOF
 */
	for (i = 0; i < Band -> getAmount (); i ++) {
	   readEntry (&Frequency, &Voltage);
	   Band -> setFrequency (i, Frequency);
	   Band -> setVoltage (i, Voltage);
	}

	checkinput (RPAR);
	Band -> sortBand ();
	return Band;
}

void	frequencytables::readEntry	(int32_t *a, int16_t *b) {
	checkinput (LPAR);
	*a = readNumber	();
	*b = readNumber ();
	checkinput (RPAR);
}

void	frequencytables::checkinput	(char t) {
	while (currentChar != EOF && currentChar != t)
	   getAchar ();
}

int32_t	frequencytables::readNumber	(void) {
int32_t	res	= 0;

	while (currentChar != EOF && is_space (currentChar))
	   getAchar ();

	if (!is_digit (currentChar))
	   return -1;

	res = currentChar - '0';
	getAchar ();
	while (is_digit (currentChar)) {
	   res = 10 * res + currentChar - '0';
	   getAchar ();
	}

	return res;
}

void	frequencytables::getAchar	(void) {
	currentChar	= fgetc (FreqFILE);
}


