#
/*
 *    Copyright (C) 2008, 2009, 2010
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
 */

#ifndef __FILEREADER
#define	__FILEREADER

#include	"rig-interface.h"
#include	<QWidget>
#include	<QFrame>
#include	<QString>
#include	"ui_widget.h"

class	QLabel;
class	QSettings;
class	fileHulp;
/*
 */
class	fileReader: public rigInterface, public Ui_Form {
Q_OBJECT
#if QT_VERSION >= 0x050000
Q_PLUGIN_METADATA (IID "fileReader")
#endif
Q_INTERFACES (rigInterface)
public:
	QWidget	*createPluginWindow	(int32_t, QSettings *);
		~fileReader		(void);
	int32_t	getRate			(void);
	void	setVFOFrequency		(int32_t);
	int32_t	getVFOFrequency		(void);
	bool	legalFrequency		(int32_t);
	int32_t	defaultFrequency	(void);

	bool	restartReader		(void);
	void	stopReader		(void);
	int32_t	Samples			(void);
	int32_t	getSamples		(DSPCOMPLEX *, int32_t, uint8_t);
	int16_t	bitDepth		(void);
	void	exit			(void);
	bool	isOK			(void);
protected:
	void		setup_Device	(int32_t);
	QFrame		*myFrame;
	fileHulp	*myReader;
	QLabel		*indicator;
	QLabel		*fileDisplay;
	int32_t		lastFrequency;
	int32_t		rate;
private slots:
	void		reset		(void);
	void		handle_progressBar (int);
	void		set_progressBar	(int);
};
#endif

