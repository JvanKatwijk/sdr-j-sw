#
/*
 *    Copyright (C)  2014
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
 */

#ifndef	__HF_SCOPE
#define	__HF_SCOPE

#include	<QObject>
#include	"swradio-constants.h"
#include	"scope.h"
#

class	HFScope: public Scope {
Q_OBJECT
public:
			HFScope		(QwtPlot*,	// the canvas
	                         	 int16_t,	// displayWidth
	                         	 int16_t);	// raster size
			~HFScope	(void);
	void		setAmplification	(int16_t);
	void		SelectView		(int8_t);
	void		setXaxis		(int32_t low,
	                                         int32_t high,
	                                         int32_t freq);
	void		Display			(double *buffer);
	void		setMarker		(int32_t);
private:
	int16_t		displaySize;
	int16_t		rasterSize;
	int32_t		low;
	int32_t		high;
	int32_t		Freq;
	int16_t		amplification;
	double		*X_axis;
	double		*displayBuffer;
//	inherits adjustFrequency  and RightClick from Scope
};
#endif

