#
/*
 *    Copyright (C) 2013
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Programming
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

#ifndef	__REFERENCE_FRAME
#define	__REFERENCE_FRAME

#include	"swradio-constants.h"

bool		isTimeCell	(uint8_t, int16_t, int16_t);
bool		isPilotCell	(uint8_t, int16_t, int16_t);
bool		isBoostCell	(uint8_t, uint8_t, int16_t);
bool		isFreqCell	(uint8_t, int16_t, int16_t);
DSPCOMPLEX	getFreqRef	(uint8_t, int16_t, int16_t);
DSPCOMPLEX	getTimeRef	(uint8_t, int16_t, int16_t);
DSPCOMPLEX	getGainRef	(uint8_t, int16_t, int16_t);
DSPCOMPLEX	getPilotValue	(uint8_t, uint8_t, int16_t, int16_t);
DSPFLOAT	timeCorrelate	(DSPCOMPLEX *, uint8_t, uint8_t);
float		init_gain_ref_cells (int16_t *cells_k,
	                             DSPCOMPLEX *cells_v, int16_t *cnt);
#endif


