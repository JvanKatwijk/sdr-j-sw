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

#ifndef __SDRPLAY_SERVER
#define	__SDRPLAY_SERVER

#include	<QObject>
#include	<QDialog>
#include	"swradio-constants.h"
#include	"ringbuffer.h"
#include	"ui_widget.h"
#include	<libusb-1.0/libusb.h>
#include	<QByteArray>
#include	<QHostAddress>
#include	<QtNetwork>
#include	<QTcpServer>
#include	<QTcpSocket>
#include	<QTimer>
class	sdrplayWorker;
class	sdrplayLoader;

class	sdrplayServer: public QDialog, private Ui_Form {
Q_OBJECT

public:
		sdrplayServer		(int32_t, QWidget *parent = NULL);
		~sdrplayServer		(void);
private:
	void	setVFOFrequency		(QByteArray);
	bool	legalFrequency		(int32_t);
	int32_t	defaultFrequency	(void);

	bool	restartReader		(void);
	void	stopReader		(void);
	int16_t	bitDepth		(void);
private slots:
	void	sendSamples		(int);
public slots:
	void	processCommand		(void);
	void	acceptConnection	(void);
	void	handleReset		(void);
	void	handleQuit		(void);
	void	checkConnection		(void);
private:
	void		setAttenuation	(QByteArray);
	void		setRate		(QByteArray);
	void		set_agcControl	(QByteArray);
	void		set_ppmControl	(QByteArray);
	bool		deviceOK;
	sdrplayLoader	*theLoader;
	sdrplayWorker	*theWorker;
	RingBuffer<DSPCOMPLEX>	*_I_Buffer;
	int32_t		inputRate;
	int32_t		outputRate;
	QFrame		*myFrame;
	bool		isValidRate	(int32_t);
	int32_t		getInputRate	(int32_t);
	int32_t		vfoFrequency;
	int32_t		vfoOffset;
	int16_t		currentGain;
	int16_t		ppm;
	QTcpServer	server;
	QTcpServer	streamer;
	QTcpSocket	*client;
	QTcpSocket	*streamerAddress;
	QTimer		watchTimer;
	bool		notConnected;
};
#endif

