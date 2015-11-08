#
/*
 *    Copyright (C) 2011, 2012, 2013
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

#ifndef	__SSTV_WORKER
#define	__SSTV_WORKER

#include	<QThread>
#include	<QImage>
#include	<QPoint>
#include	<stdint.h>
#include	<stdio.h>
#include	"swradio-constants.h"
#include	"ringbuffer.h"
#include	"oscillator.h"

// SSTV modes
enum {
  UNKNOWN=0,
  M1,    M2,   M3,    M4,
  S1,    S2,   SDX,
  R72,   R36,  R24,   R24BW, R12BW, R8BW,
  PD50,  PD90, PD120, PD160, PD180, PD240, PD290,
  W2120, W2180
};

// Color encodings
enum {
  GBR, RGB, YUV, BW
};

typedef struct ModeSpec {
	int16_t	visCode;
	char   *Name;
	char   *ShortName;
	double  SyncLen;
	double  PorchLen;
	double  SeparatorLen;
	double  PixelLen;
	double  LineLen;
	uint16_t ImgWidth;
	uint16_t ImgHeigth;
	uint8_t  YScale;
	uint8_t  ColorEnc;
} visDescriptor;

extern
visDescriptor *getVis	(int16_t);

class	sstvBuffer;
class	SNRcalc;

class	sstvWorker: public QThread {
Q_OBJECT
public:
		sstvWorker	(int32_t,
	                         RingBuffer<DSPCOMPLEX> *, QImage **);
		~sstvWorker	(void);
	void	stop		(void);
private:
	bool	getVisCode	(float *, uint8_t *);
	bool	makeVIS		(float *, float, uint8_t *);
	int16_t	fmDemodulateLuminance	(DSPCOMPLEX);
	void	receiveImage	(visDescriptor *);
	void	receiveGBRImage	(visDescriptor *);
	void	receiveRGBImage	(visDescriptor *);
	void	receiveYUVImage	(visDescriptor *);
	void	receiveBWImage	(visDescriptor *);
	void	run		(void);
	DSPCOMPLEX	getSample	(void);

	int32_t		theRate;
	Oscillator	*localOscillator;
	RingBuffer<DSPCOMPLEX> *theBuffer;
	QImage		**theImageP;
	sstvBuffer	*filterBuffer;
	SNRcalc		*computeSNR;
	uint8_t		State;
	int32_t		currentShift;
	float		*tones;
	bool		running;
	int32_t		oneBitSampleCount;
signals:
	void		displayImage	(void);
	void		displayMode	(QString);
	void		displayStage	(QString);
};

#endif


