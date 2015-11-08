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
#ifndef	__CONFIGURATOR
#define	__CONFIGURATOR

#include	<QWidget>
#include	<QDialog>
#include	<QObject>
#include	<stdio.h>
#include	<stdint.h>
#include	<QSettings>
#include	"ui_configurator.h"

class	configurator:public QDialog,
	             private Ui_Dialog {
Q_OBJECT
public:
		configurator (QSettings *sw,
	                      QWidget *parent = (QWidget *)0);
	       ~configurator	(void);
private:

	QSettings	*swreceiver;
	QString		deviceBase;
	QString		decoderBase;
private slots:
void	setDeviceDirectory	(void);
void	setDecoderDirectory	(void);
void	saveSettings		(void);
void	terminate		(void);
};

#endif

