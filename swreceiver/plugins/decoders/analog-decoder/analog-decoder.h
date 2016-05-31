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
 */
#include	<QStringList>
#include	<QPushButton>
#include	<QSlider>
#include	<QVBoxLayout>
#include	<QFrame>
#include 	"decoder-interface.h"
#include	"ui_widget.h"
#include	"fir-filters.h"

class	QSettings;
class	pllC;
class	squelch;
class	HilbertFilter;
class	adaptiveFilter;
class	decoderBase;

class analogDecoder :public decoderInterface, public Ui_Form {
Q_OBJECT
#if QT_VERSION >= 0x050000
Q_PLUGIN_METADATA (IID "analogDecoder")
#endif
Q_INTERFACES (decoderInterface)

public:
	QWidget		*createPluginWindow	(int32_t,
	                                         QSettings *);
			~analogDecoder		(void);
	void		doDecode		(DSPCOMPLEX);
	bool		initforRate		(int);
	int32_t		rateOut			(void);
	int16_t		detectorOffset		(void);
private:
	QFrame		*myFrame;

	enum Decoder {
	   AM_DECODER =		0121,
	   FM_DECODER =		0122,
	   USB_DECODER =	0123,
	   LSB_DECODER =	0124,
	   ISB_DECODER =	0125,
	   COHERENT =		0126
	};

private slots:
	void	set_decoder		(const QString &);
	void	set_adaptiveFilter	(void);
	void	set_adaptiveFiltersize	(int);	
	void	set_squelch		(int);
private:
	void	setup_analogDecoder		(int32_t);
	void	showDetectorType		(void);
	int32_t		theRate;
	QSettings	*analogSettings;
	DSPFLOAT	analog_IF;
	uint16_t	CycleCount;
	decoderBase	*ourDecoder;
	adaptiveFilter	*adaptive;
	HilbertFilter	*SSB_Filter;
	int16_t		adaptiveFiltersize;
	int16_t		old_adaptiveFiltersize;
	bool		adaptiveFiltering;
	int16_t		detectorType;
	int16_t		analog_Volume;
	LowPassFIR	*lowpass;
	pllC		*the_pll;
	DSPFLOAT	currLock;
	bool		squelchON;
	squelch		*mySquelch;
//
//	signals are derived from the baseclass
//signals:
//	void	setDetectorMarker	(int);
};

