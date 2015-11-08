#
/*
 *    Copyright (C) 2014
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
#
#ifndef	__SSTV_DECODER
#define	__SSTV_DECODER
#include	<QWidget>
#include	<QScrollArea>
#include	<QFrame>
#include	<QImage>
#include	"ui_widget.h"
#include	"swradio-constants.h"
#include 	"decoder-interface.h"
#include	"ringbuffer.h"

class	QSettings;
class	sstvFilter;
class	BandPassIIR;
class	sstvWorker;
class	HilbertFilter;

class sstvDecoder :public decoderInterface, public Ui_Form {
Q_OBJECT
#if QT_VERSION >= 0x050000
Q_PLUGIN_METADATA (IID "sstvDecoder")
#endif
Q_INTERFACES (decoderInterface)

public:
	QWidget		*createPluginWindow	(int32_t, QSettings *);
			~sstvDecoder		(void);
	void		doDecode		(DSPCOMPLEX);
	bool		initforRate		(int32_t);
	int32_t		rateOut			(void);
	int16_t		detectorOffset		(void);
private:
	QScrollArea	*theScroller;
	QFrame		*myFrame;
	int32_t		theRate;
	BandPassIIR	*sstvBandFilter;
	HilbertFilter	*theFilter;
	sstvWorker	*theWorker;
	RingBuffer<DSPCOMPLEX> *theBuffer;
	QImage		*myImage;
	QImage		**theImageP;
	bool		mono;
private slots:
	void		handle_startButton	(void);
	void		setMono			(const QString &);
public slots:
	void		displayImage		(void);
	void		displayMode		(QString);
	void		displayStage		(QString);
	void		displaySNR		(float);
};
#endif

