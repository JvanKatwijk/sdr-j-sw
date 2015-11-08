#
/*
 *    Copyright (C) 2014
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair programming
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

#ifndef __SDRPLAY
#define	__SDRPLAY

#include	<QObject>
#include	<QFrame>
#include	"swradio-constants.h"
#include	"rig-interface.h"
#include	"ringbuffer.h"
#include	"ui_widget.h"
#include	<libusb-1.0/libusb.h>

class	QSettings;
class	sdrplayWorker;
class	sdrplayLoader;

class	sdrplay: public rigInterface, public Ui_Form {
Q_OBJECT
#if QT_VERSION >= 0x050000
Q_PLUGIN_METADATA(IID "sdrplay")
#endif
Q_INTERFACES (rigInterface)

public:
	QWidget	*createPluginWindow	(int32_t, QSettings *);
		~sdrplay		(void);
	int32_t	getRate			(void);
	void	setVFOFrequency		(int32_t);
	int32_t	getVFOFrequency		(void);
	bool	legalFrequency		(int32_t);
	int32_t	defaultFrequency	(void);

	bool	restartReader		(void);
	void	stopReader		(void);
	int32_t	getSamples		(DSPCOMPLEX *, int32_t, uint8_t);
	int32_t	Samples			(void);
	int16_t	bitDepth		(void);
	void	exit			(void);
private	slots:
	void	setExternalGain		(int);
	void	setCorrection		(int);
	void	setKhzOffset		(int);
	void	set_rateSelector	(const QString &);
private:
	QSettings	*sdrplaySettings;
	bool		deviceOK;
	sdrplayLoader	*theLoader;
	sdrplayWorker	*theWorker;
	RingBuffer<DSPCOMPLEX>	*_I_Buffer;
	int32_t		inputRate;
	int32_t		outputRate;
	QFrame		*myFrame;
	bool		isValidRate	(int32_t);
	int32_t		getBandwidth	(int32_t);
	int32_t		getInputRate	(int32_t);
	int32_t		vfoFrequency;
	int32_t		vfoOffset;
	int16_t		currentGain;
};
#endif

