#
/*
 *    Copyright (C) 2008, 2009, 2010
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
#ifndef __CARDREADER__
#define	__CARDREADER__

#include	<QWidget>
#include	<QFrame>
#include	<QSettings>
#include	<QComboBox>
#include	<QMutex>
#include	"swradio-constants.h"
#include	"rig-interface.h"
#include	"ui_widget.h"
/*
 */
class	paReader;
class	cardReader: public rigInterface, public Ui_Form {
Q_OBJECT
#if QT_VERSION >= 0x050000
Q_PLUGIN_METADATA (IID "cardReader")
#endif
Q_INTERFACES (rigInterface)
public:
	bool createPluginWindow	(int32_t, QFrame *, QSettings *);
		~cardReader		(void);
	int32_t	getRate			(void)	Q_DECL_OVERRIDE;
	void	setVFOFrequency		(int32_t)	Q_DECL_OVERRIDE;
	int32_t	getVFOFrequency		(void)	Q_DECL_OVERRIDE;
	bool	legalFrequency		(int32_t)	Q_DECL_OVERRIDE;
	int32_t	defaultFrequency	(void)	Q_DECL_OVERRIDE;
	bool	restartReader		(void)	Q_DECL_OVERRIDE;
	void	stopReader		(void)	Q_DECL_OVERRIDE;
	int32_t	Samples			(void)	Q_DECL_OVERRIDE;
	int32_t	getSamples		(DSPCOMPLEX *, int32_t, uint8_t)	Q_DECL_OVERRIDE;
	int16_t	bitDepth		(void)	Q_DECL_OVERRIDE;
	void	exit			(void)	Q_DECL_OVERRIDE;
	bool	isOK			(void)	Q_DECL_OVERRIDE;
private:
	QFrame		*myFrame;
	int32_t		myFrequency;
	int32_t		inputRate;
	paReader	*myReader;
	QMutex		readerOwner;
	int16_t		gainValue;
private slots:
	void		set_streamSelector	(int);
	void		set_rateSelector	(const QString &);
	void		set_gainValue		(int);
};
#endif

