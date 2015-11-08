#
/*
 *
 *    Copyright (C) 2008, 2009, 2010, 2011, 2012, 2013
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Programming
 *
 *    This file is part of the SDR-J (JSDR).
 *    Many of the ideas as implemented in JSDR are derived from
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
#include	"iqdisplay.h"
#include	"spectrogramdata.h"
/*
 *	iq circle plotter
 */
SpectrogramData	*IQData	= NULL;

	IQDisplay::IQDisplay (QwtPlot *plot, int16_t x):
	                                QwtPlotSpectrogram () {
QwtLinearColorMap *colorMap  = new QwtLinearColorMap (Qt::black, Qt::white);
int32_t	i, j;

	setRenderThreadCount	(1);
	Radius		= 100;
	plotgrid	= plot;
	x_amount	= x;
	CycleCount	= 0;
	Points		= new DSPCOMPLEX [x_amount];
	memset (Points, 0, x_amount * sizeof (DSPCOMPLEX));
	this		-> setColorMap (colorMap);
	plotData	= new double [2 * Radius * 2 * Radius];
	plot2		= new double [2 * Radius * 2 * Radius];
	memset (plotData, 0, 2 * 2 * Radius * Radius * sizeof (double));
	IQData		= new SpectrogramData (plot2,
	                                       0,
	                                       2 * Radius,
	                                       2 * Radius,
	                                       2 * Radius,
	                                       50.0);
	this		-> setData (IQData);
	plot		-> enableAxis (QwtPlot::xBottom, 0);
	plot		-> enableAxis (QwtPlot::yLeft, 0);
	this		-> setDisplayMode (QwtPlotSpectrogram::ImageMode, true);
	plotgrid	-> replot();
}

	IQDisplay::~IQDisplay () {
	this		-> detach ();
	delete[]	plotData;
	delete []	Points;
//	delete		IQData;
}

void	IQDisplay::DisplayIQ (DSPCOMPLEX z, float scale) {
int	x, y;
int	i;

        x = (int)(scale * real (z));
        y = (int)(scale * imag (z));
	if (x >= Radius)
	   x = Radius - 1;
	if (y >= Radius)
	   y = Radius - 1;

	if (x <= - Radius)
	   x = -(Radius - 1);
	if (y <= - Radius)
	   y = -(Radius - 1);

	Points [CycleCount] = DSPCOMPLEX (x, y);
	plotData [(x + Radius - 1) * 2 * Radius + y + Radius - 1] = 50;

	if (++ CycleCount < x_amount)
	   return;

	memcpy (plot2, plotData, 2 * 2 * Radius * Radius * sizeof (double));
	this		-> detach	();
	this		-> setData	(IQData);
	this		-> setDisplayMode (QwtPlotSpectrogram::ImageMode, false);
	this		-> attach     (plotgrid);
	plotgrid	-> replot();

	memset (plotData, 0, 2 * 2 * Radius * Radius * sizeof (double));
	CycleCount	= 0;
}
