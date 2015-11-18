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

#ifndef __DABSTICK_SERVER
#define	__DABSTICK_SERVER

#include	<QObject>
#include	<QThread>
#include	<QDialog>
#include	"swradio-constants.h"
#include	"ringbuffer.h"
#include	<libusb-1.0/libusb.h>
#include	<QByteArray>
#include	<QHostAddress>
#include	<QtNetwork>
#include	<QTcpServer>
#include	<QTcpSocket>
#include	<QTimer>

class		QSettings;
class		dll_driver;
class		dabstickServer;
class		DecimatingFIR;
class		dongleSelect;
//
//	create typedefs for the library functions
typedef	struct rtlsdr_dev rtlsdr_dev_t;

typedef	void (*rtlsdr_read_async_cb_t) (uint8_t *buf, uint32_t len, void *ctx);
typedef	 int (*  pfnrtlsdr_open )(rtlsdr_dev_t **, uint32_t);
typedef	int (*  pfnrtlsdr_close) (rtlsdr_dev_t *);
typedef	int (*  pfnrtlsdr_set_center_freq) (rtlsdr_dev_t *, uint32_t);
typedef uint32_t (*  pfnrtlsdr_get_center_freq) (rtlsdr_dev_t *);
typedef	int (*  pfnrtlsdr_get_tuner_gains) (rtlsdr_dev_t *, int *);
typedef	int (*  pfnrtlsdr_set_tuner_gain_mode) (rtlsdr_dev_t *, int);
typedef	int (*  pfnrtlsdr_set_sample_rate) (rtlsdr_dev_t *, uint32_t);
typedef	int (*  pfnrtlsdr_get_sample_rate) (rtlsdr_dev_t *);
typedef	int (*  pfnrtlsdr_set_agc_mode) (rtlsdr_dev_t *, int);
typedef	int (*  pfnrtlsdr_set_tuner_gain) (rtlsdr_dev_t *, int);
typedef	int (*  pfnrtlsdr_get_tuner_gain) (rtlsdr_dev_t *);
typedef int (*  pfnrtlsdr_reset_buffer) (rtlsdr_dev_t *);
typedef	int (*  pfnrtlsdr_read_async) (rtlsdr_dev_t *,
	                               rtlsdr_read_async_cb_t,
	                               void *,
	                               uint32_t,
	                               uint32_t);
typedef int (*  pfnrtlsdr_cancel_async) (rtlsdr_dev_t *);
typedef int (*  pfnrtlsdr_set_direct_sampling) (rtlsdr_dev_t *, int);
typedef uint32_t (*  pfnrtlsdr_get_device_count) (void);
typedef	int (* pfnrtlsdr_set_freq_correction)(rtlsdr_dev_t *, int);
typedef	char *(* pfnrtlsdr_get_device_name)(int);

class	dll_driver : public QThread {
Q_OBJECT
public:
		dll_driver (dabstickServer *d);
		~dll_driver (void);
//	For the callback, we do need some environment 
//	This is the user-side call back function
static
void	RTLSDRCallBack (unsigned char *buf, uint32_t len, void *ctx);
private:
virtual
void	run (void);
	dabstickServer	*theStick;
signals:
	void	samplesAvailable (void);
};
//
/////////////////////////////////////////////////////////////////////

#include	"ui_widget.h"
class	dabstickServer: public QDialog, private Ui_Form {
Q_OBJECT

public:
		dabstickServer		(QSettings *, QWidget *parent = NULL);
		~dabstickServer		(void);
//	I_Buffer needs to be visible for use within the callback
	RingBuffer<uint8_t>	*_I_Buffer;
	pfnrtlsdr_read_async rtlsdr_read_async;
	struct rtlsdr_dev	*device;
	int32_t		inputRate;
	dll_driver	*workerHandle;
public slots:
	void	sendSamples		(void);
private:
	QSettings	*dabserverSettings;
	dongleSelect	*dongleSelector;
	QSettings	*dabSettings;
	void		setupDevice		(int32_t, int32_t);
	void		setVFOFrequency		(QByteArray);
	bool		legalFrequency		(int32_t);
	int32_t		defaultFrequency	(void);

	bool		restartReader		(void);
	void	stopReader		(void);

	HINSTANCE	Handle;
	int32_t		deviceCount;
	bool		libraryLoaded;
private slots:
public slots:
	void	processCommand		(void);
	void	acceptConnection	(void);
	void	handleReset		(void);
	void	handleQuit		(void);
	void	checkConnection		(void);
private:
	void		setAttenuation	(QByteArray);
	void		setAgc		(QByteArray);
	void		setRate		(QByteArray);
	bool		deviceOK;
	int32_t		outputRate;
	int16_t		ratio;
	QFrame		*myFrame;
	bool		isValidRate	(int32_t);
	int32_t		getInputRate	(int32_t);
	int32_t		vfoFrequency;
	int32_t		vfoOffset;
	int16_t		currentGain;
	QTcpServer	server;
	QTcpServer	streamer;
	QTcpSocket	*client;
	QTcpSocket	*streamerAddress;
	QTimer		watchTimer;
	bool		notConnected;

	int		*gains;
	int16_t		gainsCount;
	DecimatingFIR	*d_filter;
//	here we need to load functions from the dll
	bool		load_rtlFunctions	(void);
	pfnrtlsdr_open	rtlsdr_open;
	pfnrtlsdr_close	rtlsdr_close;
	pfnrtlsdr_set_center_freq rtlsdr_set_center_freq;
	pfnrtlsdr_get_center_freq rtlsdr_get_center_freq;
	pfnrtlsdr_get_tuner_gains rtlsdr_get_tuner_gains;
	pfnrtlsdr_set_tuner_gain_mode rtlsdr_set_tuner_gain_mode;
	pfnrtlsdr_set_sample_rate rtlsdr_set_sample_rate;
	pfnrtlsdr_get_sample_rate rtlsdr_get_sample_rate;
	pfnrtlsdr_set_agc_mode rtlsdr_set_agc_mode;
	pfnrtlsdr_set_tuner_gain rtlsdr_set_tuner_gain;
	pfnrtlsdr_get_tuner_gain rtlsdr_get_tuner_gain;
	pfnrtlsdr_reset_buffer rtlsdr_reset_buffer;
	pfnrtlsdr_cancel_async rtlsdr_cancel_async;
	pfnrtlsdr_set_direct_sampling	rtlsdr_set_direct_sampling;
	pfnrtlsdr_get_device_count rtlsdr_get_device_count;
	pfnrtlsdr_set_freq_correction rtlsdr_set_freq_correction;
	pfnrtlsdr_get_device_name rtlsdr_get_device_name;
private slots:
	void	setCorrection	(int);
	void	setOffset	(int);
};
#endif

