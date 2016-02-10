#
/*
 *    Copyright (C) 2013
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair programming
 *
 *    This file is part of the SDR-J (JSDR).
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
#
#ifndef	__EQUALIZER_1
#define	__EQUALIZER_1

#include	"equalizer-base.h"
#include	<fftw3.h>

class	estimate_1;

class	equalizer_1: public equalizer_base {
public:
			equalizer_1 	(uint8_t Mode,
	                                 uint8_t Spectrum);
			~equalizer_1 	(void);
	bool		equalize	(DSPCOMPLEX *,
	                                 int16_t,
	                                 theSignal **,
	                                 int16_t *,
	                                 float *);
	bool		equalize	(DSPCOMPLEX *,
	                                 int16_t,
	                                 theSignal **);
private:
	void		getRelAddress	(int16_t, int16_t *, int16_t *);
	int16_t		buildTrainers	(int16_t, int16_t *);
	estimate_1	**Estimators;
	int16_t		rndcnt;
	int16_t		windowsinFrame;
	double		**W_symbol_blk [10];
	float		f_cut_t;
	float		f_cut_k;
	int16_t		**myTrainers;
	DSPCOMPLEX	**pilotEstimates;
	DSPCOMPLEX	**oldEstimates;
	int16_t		gain_cells_per_group [10];
	int16_t		symbols_to_delay;
	int16_t		symbols_per_2D_window;
	void		processSymbol	(int16_t, theSignal *);
	float		delta_time_offset_I;
	float		delta_time_offset_P;
	float		delta_time_offset;
	int16_t		delta_time_offset_integer;
	float		time_offset_fractional;
	int16_t		Ts;
	int16_t		Tu;
	int16_t		Tg;
	int16_t		lH;
	int16_t		Tgh;
	int16_t		Tc;
	int16_t		Tgs;
	int16_t		Tgsh;
	DSPCOMPLEX	*fftVector;
	fftwf_plan	plan_1;
	float		*sinFilter;

	void		drmfilter1 (float *,
	                            float *, float *, int32_t, int32_t);
const	float kP_small_timing_controller = 0.005;
const	float kP_large_timing_controller = 0.01;
const	float threshold_timing_small_large = 2.0;
const	float kI_timing_controller = 0.000005;
};

#endif

