#
/*
 *    Copyright (C) 2012, 2013, 2014
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

#include	"local-scope.h"
#include	"swradio-constants.h"

	localScope::localScope (QwtPlot *plot,
	                        uint16_t displaysize,
	                        uint16_t nrCurves) {
int16_t	i;
	this		-> plotgrid	= plot;
	this		-> displaySize	= displaysize;
	this		-> nrCurves	= nrCurves;

	plotgrid	-> setCanvasBackground (Qt::white);
	grid		= new QwtPlotGrid;
#if defined QWT_VERSION && ((QWT_VERSION >> 8) < 0x0601)
	grid	-> setMajPen (QPen(Qt::white, 0, Qt::DotLine));
#else
	grid	-> setMajorPen (QPen(Qt::white, 0, Qt::DotLine));
#endif
	grid	-> enableXMin (true);
	grid	-> enableYMin (true);
#if defined QWT_VERSION && ((QWT_VERSION >> 8) < 0x0601)
	grid	-> setMinPen (QPen(Qt::white, 0, Qt::DotLine));
#else
	grid	-> setMinorPen (QPen(Qt::white, 0, Qt::DotLine));
#endif
	grid		-> attach (plotgrid);

	Marker		= new QwtPlotMarker ();
	Marker		-> setLineStyle (QwtPlotMarker::VLine);
	Marker		-> setLinePen (QPen (Qt::red));
	Marker		-> attach (plotgrid);

	Curves		= new QwtPlotCurve *[nrCurves];
	for (i = 0; i < nrCurves; i ++) {
	   Curves [i]	= new QwtPlotCurve ("");
   	   Curves [i]	-> setPen (QPen(Qt::red));
	   Curves [i]	-> setOrientation (Qt::Horizontal);
	   Curves [i]	-> attach (plotgrid);
	}
	
	plotgrid	-> enableAxis (QwtPlot::yLeft);

	X_axis		= new double [displaySize];
	for (i = 0; i < displaySize; i ++)
	   X_axis [i] = - displaySize / 2 + i;
	Y_values	= new double * [nrCurves];
	for (i = 0; i < nrCurves; i ++)
	   Y_values [i] = new double [2 * displaySize];
	Marker		-> setXValue	(0);
}

	localScope::~localScope (void) {
int16_t	i;

	Marker		-> detach ();
	for (i = 0; i < nrCurves; i ++)
	   Curves [i]	-> detach ();
	grid		-> detach ();
	delete		Marker;
//	for (i = 0; i < nrCurves; i ++)
//	   delete [] Curves [i];
//	delete [] Curves;
	delete    grid;
	delete [] X_axis;
}

void	localScope::Show (double **Values,
	                  double amp) {
uint16_t	i, j;

	amp		= amp / 100 * (-get_db (0));
	plotgrid	-> setAxisScale (QwtPlot::xBottom,
				         (double)X_axis [0],
				         X_axis [displaySize - 1]);
	plotgrid	-> enableAxis (QwtPlot::xBottom);
	plotgrid	-> setAxisScale (QwtPlot::yLeft,
				         get_db (0) + 10, get_db (0) + 150);

	for (i = 0; i < nrCurves; i ++) 
	   for (j = 0; j < displaySize; j ++)
	      Y_values [i] [j] = get_db (Values [i] [j]) + 5 * i; 

	for (i = 0; i < nrCurves; i ++)
	   Curves [i] -> setSamples (X_axis, Y_values [i], displaySize);
	plotgrid	-> replot(); 
}
