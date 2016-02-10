#
/*
 *    Copyright (C) 2015
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
/*
 *	MSC processor 
 */
#ifndef	__MSC_PROCESSOR
#define	__MSC_PROCESSOR

#include	"swradio-constants.h"
#include	"basics.h"

class	drmDecoder;
class	mscConfig;
class	deInterleaver;
class	mscHandler;
class	viterbi;
class	dataProcessor;

class	mscProcessor {
public:
		mscProcessor		(mscConfig *,
	                                 drmDecoder *,
	                                 viterbi *);
		~mscProcessor		(void);
	void	check_mscConfig		(mscConfig *);
	void	addtoMux		(int16_t, int32_t, theSignal);
	void	newFrame		(void);
	void	endofFrame		(void);
private:
	void		delete_Handlers	(void);
	void		create_Handlers	(mscConfig *);
	drmDecoder	*drmMaster;
	mscConfig	*msc;
	uint8_t		protLevelA;
	uint8_t		protLevelB;
	int16_t		numofStreams;
	uint8_t		QAMMode;
	uint8_t		mscMode;
	int16_t		muxsampleLength;
	theSignal	*muxsampleBuf;
	theSignal	*tempBuffer;
	int16_t		bufferP;
	deInterleaver	*my_deInterleaver;
	mscHandler	*my_mscHandler;
	dataProcessor	*my_dataProcessor;
	viterbi		*viterbiDecoder;
	int16_t		muxNo;
	int16_t		dataLength;
};

#endif


	
