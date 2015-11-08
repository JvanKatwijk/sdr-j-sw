#
/*
 *     Copyright (C) 2014: Jan van Katwijk & Johan Maas PA3GSB
 *
 *     Header file for the plugin for the SDR-T9R radio.
 *
 */

#ifndef	__SDRT9R
#define	__SDRT9R
#include	<QtNetwork>
#include	<QSettings>
#include	<QLabel>
#include	<QMessageBox>
#include	<QLineEdit>
#include	<QUdpSocket>
#include	<QHostAddress>
#include	<QByteArray>
#include	<QTimer>
#include	"swradio-constants.h"
#include	"rig-interface.h"
#include	"ringbuffer.h"
#include	"ui_widget.h"

class	sdrt9r: public rigInterface, public Ui_Form {
Q_OBJECT
Q_INTERFACES (rigInterface)
public:
	QWidget		*createPluginWindow (int32_t, QSettings *);
			~sdrt9r		(void);
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

private	slots:
	void		setOffset	(int);
	void		readData	(void);
	void		toBuffer	(QByteArray);
	void		wantConnect	(void);
	void		setConnection	(void);
	void		setRate		(const QString &);
    void        broadcastDatagram(void);
    void        readBroadcastDatagram(void);
    void        sendControlDatagram(void);
private:
	QSettings	*sdrt9rSettings;
	QFrame		*theFrame;
	int32_t		vfoOffset;
	int32_t		theRate;
	double		Normalizer;
	bool		dataAllowed;
	int32_t		vfoFrequency;
	RingBuffer<int32_t>	*_I_Buffer;
	QUdpSocket	*theSocket;
    QUdpSocket  *udpSocketBroadcasrTx;
    QUdpSocket  *udpSocketBroadcastRx;
	QLineEdit	*theLineEditor;
	bool		connected;
    QTimer      *timer;
    bool        sdrFound;
};

#endif

