#
/*
 *    Copyright (C) 2013, 2014
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Computing
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
 *    along with JSDR; if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef __DRM_DECODER
#define	__DRM_DECODER
#

#include	"swradio-constants.h"
#include	"decoder-interface.h"
#include	"ui_widget.h"
#include	"ringbuffer.h"
#include	<QLabel>

class	frameProcessor;
class	IQDisplay;
class	localScope;
/*
 * DRM  -- receiver classs
 */
class drmDecoder: public decoderInterface, public Ui_Form {
Q_OBJECT
#if QT_VERSION >= 0x050000
Q_PLUGIN_METADATA (IID "drmDecoder")
#endif
Q_INTERFACES (decoderInterface)

public:
	QWidget		*createPluginWindow	(int32_t, QSettings *);
			~drmDecoder		(void);
	void		doDecode		(DSPCOMPLEX);
	bool		initforRate		(int);
	int32_t		rateOut			(void);
	int16_t		detectorOffset		(void);
//
//	For internal use:
	void		show			(DSPCOMPLEX);
	void		show_stationLabel	(const QString &);
	bool		haltSignal		(void);
	int16_t		getAudioChannel		(void);
	int16_t		getDataChannel		(void);
	bool		isSelectedDataChannel	(int16_t);
private:
	int32_t		theRate;
	QSettings	*drmSettings;

	QLabel		*pictureLabel;
	int16_t		currentS;
	QFrame		*myFrame;
	frameProcessor	*my_frameProcessor;
	RingBuffer<DSPCOMPLEX> *buffer;
	bool		running;
	bool		decimatorFlag;
	bool		validFlag;
	int16_t		Length;
	int16_t		bufferLength;
	void		showBlock		(DSPCOMPLEX *, uint8_t);
	IQDisplay	*myIQDisplay;
	localScope	*myScope;
	double		**Y_values;
public slots:
	void		show_frames	(int, int);
	void		show_audioMode	(QString);
	void		sampleOut	(float, float);
	void		newYvalue	(int, float);
	void		showEq		(int);
	void		show_coarseOffset	(float);
	void		show_fineOffset		(float);
	void		show_timeDelay		(float);
	void		show_timeOffset		(float);
	void		show_clockOffset	(float);
	void		show_channels		(int, int);
	void		show_angle		(float);
	void		faadSuccess		(bool);
	void		showMOT			(QByteArray, int);
private slots:
	void		showIQ	(float, float);
	void		executeTimeSync	(bool);
	void		executeFACSync	(bool);
	void		executeSDCSync	(bool);
	void		execute_showMode	(int);
	void		execute_showSpectrum	(int);
	void		showSNR		(float);
	void		showMessage	(QString);
//
	void		selectChannel_1	(void);
	void		selectChannel_2	(void);
	void		selectChannel_3	(void);
	void		selectChannel_4	(void);
};

typedef	struct distances {
	float	distance_to_0;
	float	distance_to_1;
} distance;


#endif


