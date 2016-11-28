#
/*
 *    Copyright (C) 2011, 2012, 2013
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Programming
 *
 *    This file is part of the SDR-J (JSDR).
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
 *
 */

#ifndef __THROB_DECODER
#define	__THROB_DECODER

#define	QT_STATIC_CONST

#include	<QFrame>
#include	<QString>
#include	"ui_widget.h"
#include	"swradio-constants.h"
#include	"decoder-interface.h"
#include	"utilities.h"

class	QSettings;
class	throb;
class	outConverter;

class throbDecoder: public decoderInterface, public Ui_Form {
Q_OBJECT
Q_PLUGIN_METADATA (IID "throbDecoder")
Q_INTERFACES (decoderInterface)

public:
	QWidget		*createPluginWindow	(int32_t, QSettings *);
			~throbDecoder		(void);
	void		doDecode		(DSPCOMPLEX);
	bool		initforRate		(int32_t);
	int32_t		rateOut			(void);
	int16_t		detectorOffset		(void);
private:
	int32_t		theRate;
	QSettings	*throbSettings;
	QFrame		*myFrame;
	throb		*my_Throb;
	QString		throbText;
	uint8_t		Modefor			(QString);
	uint8_t		throbMode;
	bool		throbReverse;
	bool		afcSwitch;
private slots:
	void		setReverse	(void);
	void		selectMode	(QString);
	void		set_Afc		(void);
	void		setThreshold	(int);
public slots:
	void		showChar	(int16_t);
	void		clrText		(void);
	void		setMarker	(float);
	void		showMetrics	(double);
	void		showS2N		(double);
};
#endif

