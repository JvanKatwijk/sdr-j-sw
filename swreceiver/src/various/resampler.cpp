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
#include	"resampler.h"

	resampler::resampler	(int32_t	inputRate,
	                         int32_t	outputRate,
	                         int32_t	size) {
	this	-> inputRate	= inputRate;
	this	-> outputRate	= outputRate;
	this	-> size		= size;
	data.	data_in		= new float [2 * size];
	data.	input_frames	= size;
	data.	data_out	= new float [2 * size];
	data.	output_frames	= size;
	data.	end_of_input	= 0;
	data.	src_ratio	= (float)outputRate / inputRate;
	cnt			= 0;
}

	resampler::~resampler	(void) {
	delete []  data. data_in;
	delete []  data. data_out;
}

int32_t	resampler::resample	(DSPCOMPLEX *inVec, DSPCOMPLEX *outVec) {
int32_t	i;
int32_t	n;
float *data_in = (float*)data.data_in;

	for (i = 0; i < size; i ++) {
	   data_in [2 * i]	= real (inVec [i]);
	   data_in [2 * i + 1]	= imag (inVec [i]);
	}

	n = src_simple (&data, SRC_LINEAR, 2);
	for (i = 0; i < data. output_frames_gen; i ++) 
	   outVec [i] = DSPCOMPLEX (data. data_out [2 * i],
	                            data. data_out [2 * i + 1]);
	return data. output_frames_gen;
}

bool	resampler::doResample	(DSPCOMPLEX v,
	                         DSPCOMPLEX *outVec, int16_t *amount) {
int16_t i, n;
float *data_in = (float*)data.data_in;

	data_in [2 * cnt]		= real (v);
	data_in [2 * cnt + 1]	= imag (v);
	if (++cnt < size)
	   return false;

	n = src_simple (&data, SRC_LINEAR, 2);
	for (i = 0; i < data. output_frames_gen; i ++) 
	   outVec [i] = DSPCOMPLEX (data. data_out [2 * i],
	                            data. data_out [2 * i + 1]);
	*amount = data. output_frames_gen;
	fprintf (stderr, "in %d, out %d\n", cnt, data. output_frames_gen);
	cnt = 0;
	return true;
}
	
