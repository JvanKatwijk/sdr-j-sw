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
 *	This is a rewrite of the code for
 *	Library for
 *	MSDR command line manager (experimental)
 *	control a PMSDR hardware on Linux
 *	Modified to comply to firmware version 2.1.x
 *
 *	Copyright (C) 2008,2009,2010  Andrea Montefusco IW0HDV,
 *	Martin Pernter IW3AUT
 *	his program is free software: you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation, either version 3 of the License, or
 *	(at your option) any later version.
 *
 *	This program is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *	GNU General Public License for more details.
 *
 *	You should have received a copy of the GNU General Public License
 *	along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef	__PMSDR
#define	__PMSDR

#include	<QtGlobal>
#include	<QFrame>
#include	<QMutex>
#include	"swradio-constants.h"
#include	"rig-interface.h"
#include	"pmsdr_comm.h"
#include	"ui_widget.h"

class	QSettings;
class	paReader;
class	si570Handler;
//	Implementing the rig interface
// Global CONSTANTS
//-----------------------------------------------------------------------------

//
const static QsdBiasFilterIfGain qsdBfigDefault = {
    512,  // bias at half scale
    0,    // mute off
    0,    // no filter
    1     // 20 dB gain
};

class	pmsdr: public rigInterface, public Ui_Form {
Q_OBJECT
Q_PLUGIN_METADATA (IID "pmsdr-2")
Q_INTERFACES (rigInterface)

public:
	bool		createPluginWindow	(int32_t,
	                                         QFrame *,  QSettings *);
			~pmsdr			(void);
	int32_t		getRate			(void);
	void		setVFOFrequency		(int32_t);
	int32_t		getVFOFrequency		(void);
	bool		legalFrequency		(int32_t);
	int32_t		defaultFrequency	(void);

	bool		restartReader		(void);
	void		stopReader		(void);
	int32_t		Samples			(void);
	int32_t		getSamples		(DSPCOMPLEX *,
	                                         int32_t, uint8_t);
	int16_t		bitDepth		(void);
	void		exit			(void);
	bool		isOK			(void);
private slots:
	void		setMute			(void);
//	void		setGain			(int);
	void		set_gainValue		(int);
	void		setBias			(int);
	void		set_autoButton		(void);
	void		set_button_1		(void);
	void		set_button_2		(void);
	void		set_button_3		(void);
	void		set_button_4		(void);
	void		set_button_5		(void);
	void		set_offset_KHz		(int);
	void		set_Streamselector	(int);
	void		set_rateSelector	(const QString &);
private:
	int16_t		gainValue;
	int32_t		inputRate;
	QSettings	*pmsdrSettings;
	si570Handler	*mySI570;
	void		setFilter		(int16_t);
	void		setFilterAutomatic	(int32_t);
	bool		automaticFiltering;
	void		displayRange		(int16_t);
	RadioInterface	*myRadio;
	paReader	*myReader;
	QMutex		readerOwner;
	void		setup_device	(void);
	int32_t		vfoOffset;
	QFrame		*myFrame;
	QsdBiasFilterIfGain qsdBfig;
	RadioInterface	*myRadioInterface;
	bool		radioOK;
	pmsdr_comm	*pmsdrDevice;
	int16_t		usbFirmwareSub;
	int16_t		usbFirmwareMajor;
	int16_t		usbFirmwareMinor;
	int32_t		LOfreq;
	int32_t		defaultFreq;
	bool		thirdHarmonic;
	uint8_t		fLcd;
	
	int16_t		currentFilter;
//	uint8_t		postdivider;
//	uint8_t		si570_reg[8];
//	si570data	si570pmsdr;
//
//	bool		ReadandConvertRegisters	(int8_t);
//	void		calc_si570registers	(uint8_t *, si570data *);
//	uint8_t		SetBits			(uint8_t, uint8_t, uint8_t);
//	void		ComputeandSetSi570	(double);
//	void		convertFloattoHex	(si570data *, uint16_t *);
//	bool		computeDividers		(double, uint8_t *, uint8_t *);
//	bool		setFrequencySmallChange	(si570data *,
//	                                         double, uint16_t *);
//	bool		setFrequencyLargeChange	(si570data *,
//	                                         double, uint16_t *);
	bool		store_hwlo		(int32_t);
	bool		read_hwlo		(int32_t *);

};

#endif

