#
/*
 *
 *    Copyright (C) 2008, 2009, 2010
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair programming
 *
 *    This file is part of the SDR-J (JSDR).
 *    Many of the ideas as implemented in ESDR are derived from
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

#include	"band.h"

	band::band (int n, int m) {
	   bandNumber	= n;
	   amount	= m;
	   lowerBound	= 0;
	   upperBound	= 0;
	   FreqTable	= new int	[amount];
	   VoltageTable	= new int	[amount];
	}

	band::~band () {
	   delete	FreqTable;
	   delete	VoltageTable;
	}

int	band::getBandNumber	(void) {
	return bandNumber;
}

void	band::setBandNumber	(int b) {
	bandNumber = b;
}

int	band::getLowerBound	(void) {
	return lowerBound;
}

void	band::setLowerBound	(int b) {
	lowerBound	= b;
}

int	band::getUpperBound	(void) {
	return upperBound;
}

void	band::setUpperBound	(int b) {
	upperBound	= b;
}

int	band::getFrequency	(int n) {
	if (n >= 0 && n < amount)
	   return FreqTable [n];
	else
	   return -1;
}

void	band::setFrequency	(int n, int v) {
	if (n >= 0 && n < amount)
	   FreqTable [n] = v;
}

int	band::getVoltage	(int n) {
	if (n >=0 && n < amount) 
	   return VoltageTable [n];
	else
	   return -1;
}

void	band::setVoltage	(int n, int v) {
	if (n >= 0 && n < amount)
	   VoltageTable [n] = v;
}

int	band::getAmount		(void) {
	return amount;
}
/*
 * 	to sort a band, we have
 */
void	band::sortBand (void) {
int	MadeChange = true;
int	i;
/*
 * 	just a simple bubblesort, since the amount of elements
 * 	is limited, and the function is only called on set up
 */
	while (MadeChange) {
	   MadeChange = false;
	   for (i = 0; i < amount - 1; i ++)
	      if (getFrequency (i) > getFrequency (i + 1)) {
	         int f1, v1;
	         f1 = getFrequency (i + 1);
	         v1 = getVoltage   (i + 1);
	         setFrequency (i + 1, getFrequency (i));
	         setVoltage   (i + 1, getVoltage   (i));
	         setFrequency (i, f1);
	         setVoltage   (i, v1);
	         MadeChange = true;
	      }
	}

	setLowerBound (getFrequency (0));
	setUpperBound (getFrequency (amount - 1));
}

