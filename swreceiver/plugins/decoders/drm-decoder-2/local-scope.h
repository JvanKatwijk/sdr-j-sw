#
/*
 *    Copyright (C) 2008, 2009, 2010
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

#ifndef	__LOCAL_SCOPE
#define	__LOCAL_SCOPE

#ifndef	QT_STATIC_CONST
#define	QT_STATIC_CONST
#endif
#include	<QObject>
#include	<qwt.h>
#include	<qwt_plot.h>
#include	<qwt_plot_curve.h>
#include	<qwt_plot_marker.h>
#include	<qwt_plot_grid.h>
#include	<qwt_color_map.h>
#include	<stdint.h>

class localScope: public QObject {
Q_OBJECT
public:
	localScope (QwtPlot *, uint16_t, uint16_t);
	~localScope (void);
void	Show 	(double **, double);
private:
	QwtPlot		*plotgrid;
	int16_t		displaySize;
	int16_t		nrCurves;
	QwtPlotGrid	*grid;
	QwtPlotCurve	**Curves;
	QwtPlotMarker	*Marker;
	double		*X_axis;
	double		**Y_values;
};
#endif

