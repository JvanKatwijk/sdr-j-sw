#
/*
 *    Copyright (C) 2008, 2009, 2010
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Programming
 *
 *    This file is part of the SDR-J
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

#ifndef	__OLIVIA_DECODER
#define	__OLIVIA_DECODER

#include	<QObject>
#include	<QWidget>
#include	<QSettings>
#include	"ui_widget.h"
#include	"swradio-constants.h"
#include	"decoder-interface.h"
#include	"utilities.h"
#include	"pj_mfsk.h"

class oliviaDecoder: public decoderInterface, public Ui_Form {
Q_OBJECT
#if QT_VERSION >= 0x050000
Q_PLUGIN_METADATA (IID "oliviaDecoder")
#endif
Q_INTERFACES	(decoderInterface)
public:
	QWidget		*createPluginWindow	(int32_t,
	                                         QSettings *);
			~oliviaDecoder		(void);
	void		doDecode		(DSPCOMPLEX);
	bool		initforRate		(int32_t);
	int32_t		rateOut			(void);
	int16_t		detectorOffset		(void);
private slots:
	void		olivia_setTones		(const QString &);
	void		olivia_setBW		(const QString &);
	void		olivia_setSyncMargin	(int);
	void		olivia_setSyncIntegLen	(int);
	void		olivia_setSquelch	(const QString &);
	void		olivia_setSquelchvalue	(int);
	void		olivia_setReverse	(const QString &);
private:
	int32_t		_WorkingRate;
	QSettings	*ISettings;

	QFrame		*myFrame;
	void		setup_Device	(int32_t);
	void		restart		(void);
	int16_t		unescape	(int16_t);
	MFSK_Receiver<DSPFLOAT>	*Receiver;
	bool		escape;
	bool		reverse;
	int32_t		SampleRate;
	DSPFLOAT	*oliviaBuffer;
	int16_t		inp;
	int32_t		baseFrequency;
	int32_t		frequency;
	DSPFLOAT	snr;
	char		*msg1;
	char		*msg2;
	int16_t		Tones;
	int16_t		Bandwidth;
	int16_t		SyncMargin;
	int16_t		SyncIntegLen;
	DSPFLOAT	SyncThreshold;
	int16_t		bw;
	bool		SquelchOn;
	int16_t		squelchValue;
	QString		oliviaText;
	int32_t		lastFreq;

//	former signals, now implemented
	void		olivia_clrText		(void);
	void		olivia_addChar		(char);
	void		olivia_put_Status1	(char *);
	void		olivia_put_Status2	(char *);
};

#endif
