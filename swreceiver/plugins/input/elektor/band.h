#
/*
 *
 *    Copyright (C) 2008, 2009
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

//
//	very simple class for recording the mappings for the
//	elektor preselector.
class	band {
private:
	int	amount;
	int	upperBound;
	int	lowerBound;
	int	*FreqTable;
	int	*VoltageTable;
	int	bandNumber;
public:
		band 		(int n, int m);
		~band		(void);
	int	getBandNumber	(void);
	void	setBandNumber	(int b);
	int	getAmount	(void);
	void	setAmount	(int);
	int	getLowerBound	(void);
	void	setLowerBound	(int b);
	int	getUpperBound	(void);
	void	setUpperBound	(int b);
	int	getFrequency	(int n);
	void	setFrequency	(int n, int v);
	int	getVoltage	(int n);
	void	setVoltage	(int n, int v);
	void	sortBand	(void);
};

