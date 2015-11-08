#
/*
 *    Copyright (C) 2011, 2012, 2013
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
 *    along with SDR-J; if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#ifndef __ELEKTOR507
#define	__ELEKTOR507

#include	<QWidget>
#include	<QFrame>
#include	<QComboBox>
#include	<QMessageBox>
#include	<QSettings>
#include	"ui_widget.h"
#include	"rig-interface.h"
#include	"pa-reader.h"
#include	"frequencytables.h"
#ifdef	__MINGW32__
#include	<windows.h>
#endif

#include	<libftdi1/ftdi.h>
#define	ANT_AUTO		0200
#define	ANT_EXT			0201
#define	ANT_BAND4		0202
#define	ANT_BAND5		0203
#define	ANT_BAND6		0204
#define	ANT_BAND7		0205
#define	ANT_TEST		0210
#define	ANT_ERROR		0211
#define	NO_ANT			0222

#define	OSCFREQ			10000	/* in Hz		*/
#define	XTAL_CAL		128
#define	FT_OutBuffer_MAX	1024

#define	CY_I2C_RAM_ADR		210
#define	CY_I2C_EEPROM_ADR	208
#define	CLKOE_REG		0x09
#define	DIV1_REG		0x0c
#define	DIV2_REG		0x47
#define	XTALCTL_REG		0x12
#define	CAPLOAD_REG		0x13
#define	PUMPCOUNTERS_REG	0x40
#define	CLKSRC_REG		0x44

#define	PCF8951_ADDRESS		0x90
#define DAC_ENABLE		0x40

#ifdef __MINGW32__
#define	ELEKTOR507_DLL	"FTD2XX.dll"

typedef enum {
	FT_OK				= 0,
	FT_INVALID_HANDLE		= 1,
	FT_DEVICE_NOT_FOUND		= 2,
	FT_DEVICE_NOT_OPENED		= 3,
	FT_IO_ERROR			= 4,
	FT_INSUFFICIENT_RESOURCES	= 5,
	FT_INVALID_PARAMETERS		= 6,
	FT_SUCCESS			= FT_OK,
	FT_INVALID_BAUDRATE		= 7,
	FT_DEVICE_NOT_OPENED_FOR_ERASE	= 8,
	FT_DEVICE_NOT_OPENED_FOR_WRITE	= 9,
	FT_FAILED_TO_WRITE_DEVICE	= 10,
	FT_EEPROM_READ_FAILED		= 11,
	FT_EEPROM_WRITE_FAILED		= 12,
	FT_EEPROM_ERASE_FAILED		= 13,
	FT_EEPROM_NOT_PRESENT		= 14,
	FT_EEPROM_NOT_PROGRAMMED	= 15,
	FT_INVALID_ARGS			= 16,
	FT_OTHER_ERROR			= 17,
} FT_Result;

typedef FT_Result (__stdcall *FNCFT_Open)(int Index, unsigned long *ftHandle);
typedef FT_Result (__stdcall *FNCFT_Close)(unsigned long ftHandle);
typedef FT_Result (__stdcall *FNCFT_SetBitMode)(unsigned long ftHandle,
	                                        unsigned char Mask,
	                                        unsigned char Enable);
typedef FT_Result (__stdcall *FNCFT_SetBaudRate)(unsigned long ftHandle,
	                                         unsigned long BaudRate);
typedef FT_Result (__stdcall *FNCFT_Write)(unsigned long ftHandle,
	                                   void *FTOutBuf,
	                                   unsigned long BufferSize,
	                                   int *ResultPtr);
#endif

struct priv_data {
	unsigned	xtal_cal;
	unsigned	osc_freq;
	unsigned	ant;
	int		voltage;
	int		P;
	int		Q;
	int		Div1N;
	uint8_t		FT_port;
	int		Buf_address;
	uint8_t		FT_OutBuffer [FT_OutBuffer_MAX];
};
/*
 */
class	elektor: public rigInterface, public Ui_Form {
Q_OBJECT
#if QT_VERSION >= 0x050000
Q_PLUGIN_METADATA (IID "elektor")
#endif
Q_INTERFACES (rigInterface)
public:
	QWidget		*createPluginWindow	(int32_t, QSettings *);
			~elektor		(void);
	int32_t		getRate			(void);
	void		setVFOFrequency		(int32_t);
	int32_t		getVFOFrequency		(void);
	bool		legalFrequency		(int32_t);
	int32_t		defaultFrequency	(void);

	bool		providesSamples		(void);
	bool		restartReader		(void);
	void		stopReader		(void);
	int32_t		Samples			(void);
	int32_t		getSamples		(DSPCOMPLEX *,
	                                         int32_t, uint8_t);
	int16_t		bitDepth		(void);
	void		exit			(void);
private:
	paReader	*myReader;
	int32_t		inputRate;
	QSettings	*elektorSettings;
	bool		radioOK;
	bool		automaticPreselect;
	void		setupDevice		(void);
	void		setLevel		(int16_t);
	void		setAntenna		(int16_t);
	void		setXtalCal		(int16_t);
	void		preSelect		(int64_t,
	                                         int16_t *, int16_t *);
	void		setVoltage		(int16_t);
	QFrame		*myFrame;
	frequencytables		*ElektorFrequencies;
	const	char	*preselection;
#ifdef	__MINGW32__
	HMODULE			dll;
	FNCFT_Open		FT_Open;
	FNCFT_Close		FT_Close;
	FNCFT_SetBitMode	FT_SetBitMode;
	FNCFT_SetBaudRate	FT_SetBaudRate;
	FNCFT_Write		FT_Write;
	unsigned long		ftHandle;
#endif
	struct ftdi_context	ftdic;
	void		cy_update_pll	(uint8_t);
	void		i2c_write_reg	(uint8_t, uint8_t, uint8_t);
	void		i2c_write_regs	(uint8_t,
	                                 int,
	                                 uint8_t, uint8_t, uint8_t, uint8_t);
	void		ftdi_I2C_Write_Byte (uint8_t);
	void		ftdi_I2C_Init	(void);
	void		ftdi_I2C_Start	(void);
	void		ftdi_I2C_Stop	(void);
	void		ftdi_SDA	(int8_t);
	void		ftdi_SCL	(int8_t);
	void		find_P_Q	(struct priv_data *, int64_t);
	void		find_P_Q_DIV1N	(struct priv_data *, int64_t);
	int		getPump		(int);

	bool		rig_is_working;
	priv_data	*priv;
	void		WriteData (void *, unsigned long);
	QComboBox	*streamSelector;
private slots:
	void		handle_f1	(void);
	void		handle_f2	(void);
	void		handle_f3	(void);
	void		handle_f4	(void);
	void		handle_f5	(void);
	void		handle_f6	(void);
	void		handle_f7	(void);
	void		handle_f8	(void);
	void		handle_preselectButton	(void);
	void		handle_db0	(void);
	void		handle_db10	(void);
	void		handle_db20	(void);
	void		handle_preselectSlider	(int);
	void		handleXcalibrator	(int);
	void		set_streamSelector	(int);
	void		set_rateSelector	(const QString &);
};
#endif


