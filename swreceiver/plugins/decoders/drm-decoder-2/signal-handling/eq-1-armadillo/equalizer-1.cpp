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
#include	<QDebug>
#include	"referenceframe.h"
#include	"basics.h"
#include	"equalizer-1.h"
#include	"estimate-1.h"
#include	<armadillo>
using namespace	arma;

		equalizer_1::equalizer_1 (uint8_t Mode,
	                                  uint8_t Spectrum):
	                                     equalizer_base (Mode, Spectrum) {
int16_t	i, window;
float	sigmaq_noise_list [] = {16.0, 14.0, 14.0, 12.0};
float	sigmaq_noise	= pow (10.0, - sigmaq_noise_list [Mode - 1] / 10.0);
arma::mat	PHI, PHI_INV;
arma::vec	THETA;
//	Based on table 92 ETSI ES 201980
//
//	the symbol we are looking at.
//int16_t		symbols_per_2D_window_list []	= {10, 6, 8, 6};
//int16_t		symbols_to_delay_list []	= {5, 3, 4, 3};
int16_t		symbols_per_2D_window_list []	= {10, 6, 8, 6};
int16_t		symbols_to_delay_list []	= {5, 3, 4, 3};
int16_t		*currentTrainers;
//
//	first shorthands 
//	Note that "Mode" is coded 1 .. 4
	symbols_per_2D_window	= symbols_per_2D_window_list [Mode - 1];
	symbols_to_delay	= symbols_to_delay_list [Mode - 1];
	windowsinFrame		= groupsperFrame (Mode);
	myTrainers		= new int16_t *[windowsinFrame];
	for (i = 0; i < windowsinFrame; i ++) 
	   myTrainers [i] = new int16_t [500];
	pilotEstimates		= new DSPCOMPLEX *[symbolsinFrame];
	for (i = 0; i < symbolsinFrame; i ++)
	   pilotEstimates [i] = new DSPCOMPLEX [carriersinSymbol];

//	precompute 2-D-Wiener filter-matrix w.r.t. power boost
//	Reference: Peter Hoeher, Stefan Kaiser, Patrick Robertson:
//	"Two-Dimensional Pilot-Symbol-Aided Channel Estimation By
//	Wiener Filtering",
//	ISIT 1997, Ulm, Germany, June 29 - July 4
//	PHI	= auto-covariance-matrix
//	THETA	= cross-covariance-vector
//
//	This code is a rewrite of the diorama Matlab code as well
//	as the C translation of the diorama Matlab code by Ties Bos.
	f_cut_t = 0.0675 / windowsinFrame;
	f_cut_k = 1.75 * (float) Tg_of (Mode) / (float) Tu_of (Mode);
//
//	first: create the vector of gain reference cells for the
//	whole frame(s) of this mode
//	Now the handling, a large loop for each of the symbols
//	in the  range 0 .. windowsinFrame
//	Note that there is a periodicity such that the pilot structure
//	can be described in just a few "windows"
	for (window = 0; window < windowsinFrame; window ++) {
	   int16_t	cells_for_this_window;
	   int16_t	k_index1, k_index2, carrier;
	   currentTrainers	= myTrainers [window];
//
//	build up the "trainers" as relative addresses of the pilots
	   cells_for_this_window
	                  = buildTrainers (window, myTrainers [window]);
//	   fprintf (stderr, "For window %d we have %d cells\n",
//	                  window, cells_for_this_window);
	   gain_cells_per_group [window] = cells_for_this_window;
	   W_symbol_blk [window]	= new double  *[carriersinSymbol];
	   for (i = 0; i < carriersinSymbol; i ++)
	      W_symbol_blk [window][i] = new double [cells_for_this_window];
//
	   PHI		= zeros<mat> (cells_for_this_window,
	                              cells_for_this_window);
	   THETA	= zeros<vec> (cells_for_this_window);
//	Calculating PHI for the current "window"
           for (k_index1 = 0;
                k_index1 < cells_for_this_window; k_index1 ++) {
              for (k_index2 = 0;
                   k_index2 < cells_for_this_window; k_index2 ++) {
	         int16_t k1_pos, k2_pos, t1_pos, t2_pos;
//
//	for k1 and k2 we need the position in the symbol it is in
                 k1_pos = indexFor (currentTrainers [k_index1]) %
                                                  carriersinSymbol + K_min;
                 t1_pos = indexFor (currentTrainers [k_index1]) /
                                                  carriersinSymbol;
                 k2_pos = indexFor (currentTrainers [k_index2]) %
                                                  carriersinSymbol + K_min;
                 t2_pos = indexFor (currentTrainers [k_index2]) /
                                                  carriersinSymbol;
                 PHI (k_index1, k_index2) = sinc ((k1_pos - k2_pos) * f_cut_k)
	                                  * sinc ((t1_pos - t2_pos) * f_cut_t);
              }
	   }
//	add the noise
	   for (i = 0; i < cells_for_this_window; i++) {
	      int16_t relSym, actualCarrier;
	      getRelAddress (currentTrainers [i], 
	                           &relSym, &actualCarrier);
	      DSPCOMPLEX v = getPilotValue (Mode, Spectrum,
	                                    relSym + window, actualCarrier);
	      float amp = real (v * conj (v));
	      PHI (i, i) += sigmaq_noise * 2.0 / amp;
	   }
	   PHI_INV = pinv (PHI);

//	Now for Theta's
	   for (carrier = K_min; carrier <= K_max; carrier++) {
	      int16_t j, k;
	      if (carrier == 0)
	         continue;
	// first a new THETA
	      for (k_index2 = 0;
	           k_index2 < cells_for_this_window; k_index2++) {
	         int16_t k1_pos, k2_pos, t1_pos, t2_pos;
	         t1_pos = symbols_to_delay;
	         k2_pos = indexFor (currentTrainers [k_index2]) %
	                                     carriersinSymbol + K_min;
	         t2_pos = indexFor (currentTrainers [k_index2]) /
	                                                carriersinSymbol;
//	THETA = cross covariance-vector
	         THETA [k_index2] = sinc ((carrier - k2_pos) * f_cut_k) *
	                            sinc ((t1_pos - t2_pos) * f_cut_t);
	      }

//      calc matrix product THETA * PHI_INV
	      for (j = 0; j < cells_for_this_window; j++) {
	         W_symbol_blk [window][indexFor (carrier)][j] = 0.0;
	         for (k = 0; k < cells_for_this_window; k++)
	            W_symbol_blk [window][indexFor (carrier)][j] +=
	                          THETA [k] * PHI_INV (k, j);
	      }
	   }			/* end k_index1-loop */
	}			/* end www-loop  pa0mbo 26-5-2007 */
//
//	The "windows" are ready now
	Estimators	= new estimate_1 *[symbolsinFrame];
	for (i = 0; i < symbolsinFrame; i ++)
	   Estimators [i] = new estimate_1 (refFrame, Mode, Spectrum, i);
	delta_time_offset	= 0;
	delta_time_offset_I	= 0;
	delta_time_offset_P	= 0;
	time_offset_fractional	= 0;
	delta_time_offset_integer	= 0;
	Ts			= Ts_of (Mode);
	Tu			= Tu_of (Mode);
	Tg			= Tg_of	(Mode);
	lH			= Kmax (Mode, Spectrum) -
	                          Kmin (Mode, Spectrum) + 1;
	Tgh			= (int)floor (Tg / 2 + 0.5);
	Tc			= (int) pow(2, ceil(log((double) lH) / log(2.0)));
	Tgs			= (int)floor ((float) Tg / (float)Tu * (float)Tc);
	Tgsh			= (int) floor((float) Tg / (float) Tu / 2.0 * (float) Tc);
	fftVector		= (DSPCOMPLEX *)fftwf_malloc (Tc * sizeof (DSPCOMPLEX));
	plan_1			= fftwf_plan_dft_1d (Tc,
	                                             (fftwf_complex *)fftVector,
	                                             (fftwf_complex *)fftVector,
	                                             FFTW_FORWARD,
	                                             FFTW_PATIENT);
	sinFilter		= new float [Tgs];
	for (i = 0; i < Tgs; i ++)
	   sinFilter [i] = (float) pow (sin ((float)(i + 1.0) /
	                                     (float)(Tgs + 1.0) * M_PI), 0.001);
}

		equalizer_1::~equalizer_1 (void) {
int16_t	i;
	for (i = 0; i < windowsinFrame; i ++) 
	   delete [] myTrainers [i];
	delete []	myTrainers;
	for (i = 0; i < symbolsinFrame; i ++)
	   delete [] pilotEstimates [i];
	delete [] pilotEstimates;
	for (i = 0; i < symbolsinFrame; i ++)
	   delete Estimators [i];
	delete Estimators;
}

int16_t		equalizer_1::buildTrainers (int16_t window, int16_t *v) {
int16_t symbol, carrier;
int16_t	myCount	= 0;

	for (symbol = window;
	     symbol < window + symbols_per_2D_window; symbol ++) {
	   for (carrier = K_min; carrier <= K_max; carrier ++) {
	      if (isPilotCell (Mode, symbol, carrier)) {
	         v [myCount] =
	               (carrier + (symbol - window) * carriersinSymbol);
	         myCount ++;
	      }
	   }
	}
	return myCount;
}

//	The pilot indices are grouped and made relative to
//	the origin of the window they are in
void	equalizer_1::getRelAddress	(int16_t	pilotIndex,
	                                 int16_t	*sym,
	                                 int16_t	*carrier) {
int16_t	lPilot		= pilotIndex - K_min;
	*sym		= lPilot / carriersinSymbol;
	*carrier	= (lPilot % carriersinSymbol) + K_min;
}
//
bool	equalizer_1::equalize (DSPCOMPLEX *testRow, int16_t newSymbol,
	                       theSignal **outFrame,
	                       int16_t	*offset_integer,
	                       float	*offset_fractional) {
int16_t	carrier;
int16_t	symbol_to_process;

//	First, we copy the incoming vector to the appropriate vector
//	in the testFrame. Next we compute the estimates for the
//	channels of the pilots.
//	This is the most simple approach: the channels of the pilots
//	are obtained by dividing the value observed by the pilot value.

	for (carrier = K_min; carrier <= K_max; carrier ++) {
	   if (carrier == 0)
	      continue;
	   testFrame [newSymbol][indexFor (carrier)] =
	                         testRow [indexFor (carrier)];
	   if (isPilotCell (Mode, newSymbol, carrier))
	      pilotEstimates [newSymbol][indexFor (carrier)] =
	                  testRow [indexFor (carrier)] /
	                    getPilotValue (Mode, Spectrum, newSymbol, carrier);
	}
//
//	For equalizing symbol X, we need the pilotvalues
//	from the symbols X - symbols_to_delay .. X + symbols_to_delay - 1
//
//	We added the symbol at loc newSymbol, so we can equalize
//	the symbol "symbols_to_delay - 1" back.
//	
	symbol_to_process = newSymbol - symbols_to_delay;
	if (symbol_to_process < 0)
	   symbol_to_process += symbolsinFrame;

	processSymbol (symbol_to_process, outFrame [symbol_to_process]);
//
//	Now for the time offset:

	float		delta_theta;
	float		max_delta_theta = Tgh;
	int16_t		max_delta_time_offset_integer = 3;
	float		*h_absq = (float *)alloca (2 * Tc * sizeof (float));
	float		*h_absq_filtered = (float *)alloca (2 * Tc * sizeof (float));
	int16_t		i;

	for (carrier = K_min; carrier <= K_max; carrier ++)
	   fftVector [carrier - K_min] =
	               refFrame [symbol_to_process][indexFor (carrier)];

	for (i = K_max - K_min + 1; i < Tc; i ++)
	   fftVector [i] = DSPCOMPLEX (0, 0);

	fftwf_execute (plan_1);

	for (i = 0; i < Tc; i ++) {
	   h_absq [i]		= real (fftVector [i] * conj (fftVector [i]));
	   h_absq [Tc + i]	= real (fftVector [i] * conj (fftVector [i]));
	}

	drmfilter1 (h_absq, h_absq_filtered, sinFilter, 2 * Tc, Tgs);

//	now determine max and position */
	float	dummy = -1.0E30;
	delta_theta = 0.0;
	for (i = 0; i < 2 * Tc; i++) {
	   if (h_absq_filtered [i] > dummy) {
	      dummy = h_absq_filtered [i];
	      delta_theta = (float) i;
	   }
	}
	delta_theta =
	   (float) (((((Tc + Tgsh + 1 - (int) delta_theta +
	                (int) (Tc * 1.5)) % Tc) - Tc / 2) * Tu) /
	           (float) Tc);

	delta_theta = std::max (std::min (max_delta_theta, delta_theta),
	                        - max_delta_theta);

//	filter theta: P-I controller */
	float time_offset_ctrl = delta_theta - time_offset_fractional;
	delta_time_offset_I += kI_timing_controller * time_offset_ctrl;
	delta_time_offset_P =
	             kP_large_timing_controller * time_offset_ctrl +
	          threshold_timing_small_large *
	            (kP_small_timing_controller - kP_large_timing_controller) *
              tanhf(time_offset_ctrl / threshold_timing_small_large);

	delta_time_offset =
	      delta_time_offset_P + delta_time_offset_I +
	                                          time_offset_fractional;
	delta_time_offset_integer = (int) floor(delta_time_offset + 0.5);

	delta_time_offset_integer =
	                  std::max (std::min ((int)delta_time_offset_integer,
	                                      3),
	                            - 3);
	time_offset_fractional =
              delta_time_offset - delta_time_offset_integer;

	*offset_integer		= delta_time_offset_integer;
	*offset_fractional	= time_offset_fractional;

//	If we have a frame full of output: return true
	return symbol_to_process == symbolsinFrame - 1;
}

bool	equalizer_1::equalize (DSPCOMPLEX *testRow, int16_t newSymbol,
	                       theSignal **outFrame) {
int16_t	carrier;
int16_t	symbol_to_process;

//	First, we copy the incoming vector to the appropriate vector
//	in the testFrame. Next we compute the estimates for the
//	channels of the pilots.
//	This is the most simple approach: the channels of the pilots
//	are obtained by dividing the value observed by the pilot value.

	for (carrier = K_min; carrier <= K_max; carrier ++) {
	   if (carrier == 0)
	      continue;
	   testFrame [newSymbol][indexFor (carrier)] =
	                         testRow [indexFor (carrier)];
	   if (isPilotCell (Mode, newSymbol, carrier))
	      pilotEstimates [newSymbol][indexFor (carrier)] =
	                  testRow [indexFor (carrier)] /
	                    getPilotValue (Mode, Spectrum, newSymbol, carrier);
	}
//
//	For equalizing symbol X, we need the pilotvalues
//	from the symbols X - symbols_to_delay .. X + symbols_to_delay - 1
//
//	We added the symbol at loc newSymbol, so we can equalize
//	the symbol "symbols_to_delay - 1" back.
//	
	symbol_to_process = newSymbol - symbols_to_delay;
	if (symbol_to_process < 0)
	   symbol_to_process += symbolsinFrame;

	processSymbol (symbol_to_process, outFrame [symbol_to_process]);
//	If we have a frame full of output: return true
	return symbol_to_process == symbolsinFrame - 1;
}

//
//	process symbol "symbol", 
void	equalizer_1::processSymbol (int16_t symbol,
	                            theSignal *outVector) {
int16_t	windowBase = symbol - symbols_to_delay + 1;

	if (windowBase < 0)
	   windowBase += symbolsinFrame;

//	ntwee will indicate the "model" of the window, we deal with
//	while windowBase indicates the REAL window, i.e. the
//	the first symbol of the window as appearing in the frame.
int16_t	ntwee		= windowBase % windowsinFrame;
int16_t	nrTrainers	= gain_cells_per_group [ntwee];
int16_t *currentTrainers	= myTrainers [ntwee];
DSPCOMPLEX pilotChannels [nrTrainers];
int16_t	trainer;
int16_t	carrier, k;

//	The trainers are over N subsequent symbols in the frame, starting
//	at windowBase.
//	we determine the REAL address in the frame by first computing the
//	relative address, and then adding the base of the window over
//	the frame we are looking at

	for (trainer = 0; trainer < nrTrainers; trainer ++) {
	   int16_t relSym, actualCarrier, actualSym;
	   getRelAddress (currentTrainers [trainer], 
	                  &relSym, &actualCarrier);
	   actualSym = (relSym + windowBase) % symbolsinFrame;
	   pilotChannels [trainer] =
	             pilotEstimates [actualSym][indexFor (actualCarrier)];
	}
//
//	As soon as we have the channelestimates for the pilots, we can apply
//	the Wiener filtering. As usual, we build up the channels
//	in the refFrame
//
	for (carrier = K_min; carrier <= K_max; carrier ++) {
	   double sum = 0;
	   if (carrier == 0)
	      continue;
	   refFrame [symbol] [indexFor (carrier)] = 0;
	   for (k = 0; k < nrTrainers;  k ++) {
	      refFrame [symbol] [indexFor (carrier)] +=
	                cmul (pilotChannels [k],
	                       W_symbol_blk [ntwee][indexFor (carrier)] [k]);
	      sum += W_symbol_blk [ntwee][indexFor (carrier)][k];
	   }
//	and normalize the value
	   refFrame [symbol][indexFor (carrier)] =
	                cdiv (refFrame [symbol][indexFor (carrier)], sum);
	}
//
//	The transfer function is now there, stored in the appropriate
//	entry in the refFrame, so let us equalize
	for (carrier = K_min; carrier <= K_max; carrier ++) {
	   DSPCOMPLEX temp	=
	                   refFrame [symbol] [indexFor (carrier)];
	   if (carrier == 0)
	      outVector [indexFor (0)]. signalValue = DSPCOMPLEX (0, 0);
	   else
	      outVector [indexFor (carrier)] . signalValue =
	             testFrame [symbol][indexFor (carrier)] / temp;
	   outVector [indexFor (carrier)]. rTrans =
	             real (temp * conj (temp));
	}
}

void	equalizer_1::drmfilter1 (float	*sigin,
	                        float	*out,
	                        float	*coef,
	                        int32_t	dataLen,
	                        int32_t	coefLen) {
int32_t i, j;

	for (i = 0; i < dataLen; i++) {
	   out [i] = 0.0;
	   for (j = 0; ((j <= i) && (j < coefLen)); j++)
	      out[i] += coef[j] * sigin [i - j];
	}
}

