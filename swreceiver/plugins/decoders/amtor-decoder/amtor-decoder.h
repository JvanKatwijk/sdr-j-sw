#
/*
 *    Copyright (C) 2010, 2011, 2012, 2013
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
 */

#ifndef __AMTOR_DECODER
#define	__AMTOR_DECODER
#
#include	<QFrame>
#include	<QSettings>
#include	"ui_widget.h"
#include	"swradio-constants.h"
#include	"decoder-interface.h"

class	Oscillator;
class	average;
class	LowPassFIR;

#define	AMTOR_DECODERQUEUE	10
typedef struct char_map {
	char	letter;
	char	digit;
	int	key;
} CharMap;

class amtorDecoder: public decoderInterface, public Ui_Form {
Q_OBJECT
Q_PLUGIN_METADATA (IID "amtorDecoder")
Q_INTERFACES (decoderInterface)

public:
	QWidget		*createPluginWindow	(int32_t,
	                                         QSettings *);
			~amtorDecoder		(void);
	void		doDecode		(DSPCOMPLEX);
	bool		initforRate		(int32_t);
	int32_t		rateOut			(void);
	int16_t		detectorOffset		(void);
private slots:
	void	set_amtorAfcon		(const QString &);
	void	set_amtorReverse	(const QString &);
	void	set_amtorFecError	(const QString &);
	void	set_amtorMessage	(const QString &);
private:
	int32_t		theRate;
	QSettings	*amtorSettings;

	QString		amtorTextstring;
	void		amtorText		(uint8_t);
	void		setup_amtorDecoder	(int32_t);
	QFrame		*myFrame;
	void		amtor_setAfcon		(bool);
	void		amtor_setReverse	(bool);
	void		amtor_setFecError	(bool);
	void		amtor_setMessage	(bool);

	void		addbitfragment		(int8_t);
	DSPFLOAT	correctIF		(DSPFLOAT);
	bool		validate		(uint16_t, int8_t);
	void		addfullBit		(int8_t);
	void		decoder			(int);
	void		initMessage		();
	void		addBytetoMessage	(int);
	void		flushBuffer		(void);
	void		HandleChar		(int16_t);

	Oscillator	*LocalOscillator;

	int16_t		CycleCount;
	float		amtor_IF;
	average		*amfilter;
	int16_t		amtor_decimator;

	LowPassFIR	*LPM_Filter;
	bool		amtor_afcon;
	bool		amtor_reversed;
	float		amtor_strength;
	float		amtor_shift;
	float		amtor_baudrate;
	uint16_t 	amtor_bitlen;
	uint16_t	amtor_phaseacc;
	float		amtor_bitclk;
	uint8_t		fragment_cnt;
	int8_t		amtor_oldfragment;
	uint16_t	amtor_shiftreg;
	int16_t		amtor_bitCount;
	int16_t		amtor_Lettershift;
	DSPCOMPLEX	amtor_oldz;
	int16_t		amtor_state;
	int16_t		amtor_queue [AMTOR_DECODERQUEUE];
	int16_t		amtor_qp;
	bool		amtor_fecerror;
	bool		showAlways;
	int16_t		messageState;

	enum messageMode {
	   NOTHING,
	   STATE_Z,
	   STATE_ZC,
	   STATE_ZCZ,
	   STATE_ZCZC,
	   STATE_N,
	   STATE_NN,
	   STATE_NNN,
	   STATE_NNNN
	};


//	former signals
	void		amtor_showStrength	(float);
	void		amtor_showCorrection	(float);
	void		amtor_clrText		();
	void		amtor_addText		(char);
};

#endif

