#
/*
 *    Copyright (C) 2013
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    lazy Chair Computing
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
#ifndef	__SIMPLEBUF
#define	__SIMPLEBUF

#include	"swradio-constants.h"
#include	"ringbuffer.h"

class	drmDecoder;
//
//	For now we have a simple abstraction layer over a ringbuffer
//	that provides a suitable buffer.  It acts as a - more or
//	less regular - array, however, how the data gets in is kept
//	a secret. In a next version we eliminate the ringbuffer.
class	simpleBuf {
public:
			simpleBuf (RingBuffer<DSPCOMPLEX> *,
	                           int16_t, drmDecoder *);
			~simpleBuf (void);
	void		needed		(int32_t);
	void		shiftBuffer	(int16_t);
	uint16_t	bufSize;
	DSPCOMPLEX	*data;
	uint16_t	currentIndex;
protected:
	uint16_t	Contents	(void);
	uint16_t	firstFreeCell;
	RingBuffer<DSPCOMPLEX> * ringBuffer;
	drmDecoder		*master;
};

#endif

