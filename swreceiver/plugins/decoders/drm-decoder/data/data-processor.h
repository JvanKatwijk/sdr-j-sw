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
#ifndef	__DATA_PROCESSOR
#define	__DATA_PROCESSOR

#include	<QObject>
#include	"swradio-constants.h"
#include	<cstring>

class	drmDecoder;
class	mscConfig;
class	DRM_aacDecoder;
class	messageProcessor;
class	LowPassFIR;

typedef	struct frame {
	int16_t length, startPos;
	uint8_t	aac_crc;
	uint8_t audio [512];
} audioFrame; 
	
class	dataProcessor: public QObject {
Q_OBJECT
public:
		dataProcessor	(mscConfig *, drmDecoder *);
		~dataProcessor	(void);
	void	process		(uint8_t *, int16_t);
enum	{
	S_AAC, S_CELPT, S_HVXC
};

private:
	mscConfig	*msc;
	drmDecoder	*drmMaster;
	void	process_audio	(uint8_t *, int16_t,
	                         int16_t, int16_t, int16_t, int16_t);
	void	process_data	(uint8_t *, int16_t,
	                         int16_t, int16_t, int16_t, int16_t);
	void	process_celp	(uint8_t *, int16_t,
	                         int16_t, int16_t, int16_t, int16_t);
	void	process_hvxc	(uint8_t *, int16_t,
	                         int16_t, int16_t, int16_t, int16_t);
	void	process_aac	(uint8_t *, int16_t, int16_t,
	                         int16_t, int16_t, int16_t);
	void	handle_uep_audio	(uint8_t *, int16_t,
	                         int16_t, int16_t, int16_t, int16_t);
	void	handle_eep_audio	(uint8_t *, int16_t, int16_t, int16_t);
	void	handle_uep_data	(uint8_t *, int16_t,
	                         int16_t, int16_t, int16_t, int16_t);
	void	handle_eep_data	(uint8_t *, int16_t, int16_t, int16_t);
	void	firstSegment	(uint8_t *, int16_t, int8_t);
	void	lastSegment	(uint8_t *, int16_t, int8_t);
	void	addSegment	(uint8_t *, int16_t, int8_t);
	void	writeOut	(int16_t *, int16_t, int32_t);
	void	toOutput	(float *, int16_t);
	void	playOut		(int16_t);
	DRM_aacDecoder	*my_aacDecoder;
	messageProcessor	*my_messageProcessor;
	int16_t		numFrames;
	LowPassFIR	*upFilter_12000;
	LowPassFIR	*upFilter_24000;
	uint8_t		aac_isInit;
	uint8_t		prev_audioSamplingRate;
	uint8_t		prevSBR_flag;
	uint8_t		prev_audioMode;
signals:
	void		show_audioMode	(QString);
	void		putSample	(float, float);
	void		faadSuccess	(bool);
};

#endif

