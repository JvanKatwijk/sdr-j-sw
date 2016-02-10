#
/*
 *    Copyright (C) 2013
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Computing
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
#ifndef	__SYNCER
#define	__SYNCER
#include	"swradio-constants.h"
#include	"ringbuffer.h"
#include	"simplebuf.h"

class		drmDecoder;

struct	modeinfo {
	int16_t		mode_indx;
	float		time_offset;
	float		sample_rate_offset;
	float		freq_offset_fract;
};

typedef	struct modeinfo smodeInfo;


class	Syncer : public simpleBuf {
public:
			Syncer 		(RingBuffer<DSPCOMPLEX> *,
	                                 drmDecoder *, int32_t);	
			~Syncer 	(void);
	void		getMode		(smodeInfo *);
private:

	int32_t		nSamples;
	DSPCOMPLEX	*correlations;
	DSPCOMPLEX	*summedCorrelations;
	float		*squares;
	float		*summedSquares;
	float		EPSILON;
};
#endif

