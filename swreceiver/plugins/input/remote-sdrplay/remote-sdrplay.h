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
 *
 */

#ifndef	__REMOTE_SDRPLAY
#define	__REMOTE_SDRPLAY
#include	<QtNetwork>
#include	<QSettings>
#include	<QLabel>
#include	<QMessageBox>
#include	<QLineEdit>
#include	<QHostAddress>
#include	<QByteArray>
#include	<QTcpSocket>
#include	<QTimer>
#include	<QComboBox>
#include	"swradio-constants.h"
#include	"rig-interface.h"
#include	"ringbuffer.h"
#include	"ui_widget.h"

class	remote: public rigInterface, public Ui_Form {
Q_OBJECT
#if QT_VERSION >= 0x050000
Q_PLUGIN_METADATA (IID "remote")
#endif
Q_INTERFACES (rigInterface)
public:
	QWidget		*createPluginWindow (int32_t, QSettings *);
			~remote		(void);
	int32_t		getRate		(void);
	bool		legalFrequency	(int32_t);
	int32_t		defaultFrequency	(void);
	void		setVFOFrequency	(int32_t);
	int32_t		getVFOFrequency	(void);
	bool		restartReader	(void);
	void		stopReader	(void);
	int32_t		getSamples	(DSPCOMPLEX *V,
	                                 int32_t size, uint8_t Mode);
	int32_t		Samples		(void);
	int16_t		bitDepth	(void);
	void		exit		(void);
	bool		isOK		(void);
	void		sendStart	(void);
	void		sendStop	(void);
	void		sendVFO		(int32_t);
	void		sendRate	(int32_t);
	void		sendPpm		(int32_t);
	void		sendAgc		(uint8_t);
private	slots:
	void		sendGain	(int);
	void		setOffset	(int);
	void		readData	(void);
	void		readControl	(void);
	void		toBuffer	(QByteArray);
	void		setConnection	(void);
	void		wantConnect	(void);
	void		setDisconnect	(void);
	void		setRate		(const QString &);
	void		set_ppmControl	(int);
	void		set_agcControl	(int);
private:
	bool		isvalidRate	(int32_t);
	QSettings	*remoteSettings;
	QFrame		*theFrame;
	int32_t		vfoOffset;
	int32_t		theRate;
	int32_t		vfoFrequency;
	int32_t		ppm;
	uint8_t		agcMode;
	RingBuffer<DSPCOMPLEX>	*_I_Buffer;
	bool		connected;
	int16_t		theGain;
	QHostAddress	serverAddress;
	QTcpSocket	toServer;
	QTcpSocket	streamer;
	qint64		basePort;
};

#endif

