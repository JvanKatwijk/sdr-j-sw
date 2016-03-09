#
/*
 *    Copyright (C) 2011, 2012, 2013
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
 *	Control object for elektor507. This code was originally
 *	derived from
 *	the Hamlib code. However, in order to be able to address the
 *	i2c bus directly (needed for the preselector), which was
 *	not supported by hamlib, we rewrote the thing.
 *	The windows interface code is a rewrite of the code of
 *	Burkhardt's Pascal code, using the DLL windows interface,
 *	All relevant copyrights are fully acknowledged.
 */
#include	<stdlib.h>
#include	<string.h>
#include	"elektor.h"

QWidget	*elektor::createPluginWindow	(int32_t rate, QSettings *s) {
	(void)rate;
	this	-> elektorSettings	= s;
	this	-> myFrame	= new QFrame;
	setupUi	(myFrame);
	inputRate		= rateSelector -> currentText (). toInt ();
	myReader		= NULL;
	radioOK			= false;
	setupDevice ();

	if (radioOK) 
	   status -> setText (QString ("elektor ok"));
	else
	   status -> setText (QString ("not functioning"));
	
	if (elektorSettings != NULL) {
	   elektorSettings	-> beginGroup ("elektor0507");
	   int16_t k	= elektorSettings -> value ("elektor_Xtal",
	                                             0). toInt ();
	   Xcalibrator	-> setValue (k);
	   handleXcalibrator (k);
	   elektorSettings	-> endGroup ();
	}
	set_gainValue (gainSlider -> value ());
	myReader		= new paReader (inputRate, cardSelector);
	connect (cardSelector, SIGNAL (activated (int)),
	         this, SLOT (set_streamSelector (int)));
	connect (rateSelector, SIGNAL (activated (const QString &)),
	         this, SLOT (set_rateSelector (const QString &)));
	connect (gainSlider, SIGNAL (valueChanged (int)),
	         this, SLOT (set_gainValue (int)));
	connect (myReader, SIGNAL (samplesAvailable (int)),
	         this, SIGNAL (samplesAvailable (int)));
//
//	default:
	setAntenna (1);
	connect (elektor_f1, SIGNAL (clicked (void)),
	         this, SLOT (handle_f1 (void)));
	connect (elektor_f2, SIGNAL (clicked (void)),
	         this, SLOT (handle_f2 (void)));
	connect (elektor_f3, SIGNAL (clicked (void)),
	         this, SLOT (handle_f3 (void)));
	connect (elektor_f4, SIGNAL (clicked (void)),
	         this, SLOT (handle_f4 (void)));
	connect (elektor_f5, SIGNAL (clicked (void)),
	         this, SLOT (handle_f5 (void)));
	connect (elektor_f6, SIGNAL (clicked (void)),
	         this, SLOT (handle_f6 (void)));
	connect (elektor_f7, SIGNAL (clicked (void)),
	         this, SLOT (handle_f7 (void)));
	connect (elektor_f8, SIGNAL (clicked (void)),
	         this, SLOT (handle_f8 (void)));

	connect (preselectButton, SIGNAL (clicked (void)),
	         this, SLOT (handle_preselectButton (void)));

	connect (elektor_db0, SIGNAL (clicked (void)),
	         this, SLOT (handle_db0	(void)));
	connect (elektor_db10, SIGNAL (clicked (void)),
	         this, SLOT (handle_db10	(void)));
	connect (elektor_db20, SIGNAL (clicked (void)),
	         this, SLOT (handle_db20	(void)));
	
	connect (preselectSlider, SIGNAL (valueChanged (int)),
	         this, SLOT (handle_preselectSlider (int)));
	connect (Xcalibrator, SIGNAL (valueChanged (int)),
	         this, SLOT (handleXcalibrator	(int)));
	return myFrame;
}

	rigInterface::~rigInterface	(void) {
}

	elektor::~elektor	(void) {
	myReader	-> stopReader ();
	delete myReader;
	myReader	= NULL;
	if (elektorSettings != NULL) {
	   elektorSettings	-> beginGroup ("elektor0507");
	   elektorSettings	-> setValue ("elektor_Xtal", Xcalibrator -> value ());
	   elektorSettings	-> endGroup ();
	}

#ifdef __MINGW32__
	if (dll)
	   FreeLibrary (dll);
#else
	ftdi_deinit	(&ftdic);
#endif
	delete priv;
}

int32_t	elektor::getRate	(void) {
	return inputRate;
}

void	elektor::setupDevice	(void) {
int	res;
	preselection		= "./FreqFile";
	priv			= new priv_data;
	priv	-> xtal_cal	= XTAL_CAL;
	priv	-> osc_freq	= OSCFREQ;
	priv	-> ant		= ANT_AUTO;
//	DIV1N set to safe default
	priv	-> Div1N	= 8;
	priv	-> P		= 8;
	priv	-> Q		= 2;
	ElektorFrequencies	= new frequencytables (preselection);
#ifdef __MINGW32__
	dll		= LoadLibraryA (ELEKTOR507_DLL);
	if (!dll) {
	   status -> setText (QString ("no library"));
	   return;
	}
	status -> setText (QString ("library loaded"));
/*
 * 	Get process addresses
 */
	FT_Open	=
	         (FNCFT_Open) GetProcAddress (dll, "FT_Open");
	if (FT_Open == NULL) {
	   status	-> setText (QString ("FT_Open ?"));
	   fprintf (stderr, "Cannot find FT_Open\n");
	   return;
	}
	FT_Close =
	         (FNCFT_Close) GetProcAddress (dll, "FT_Close");
	if (FT_Close == NULL) {
	   status -> setText (QString ("FT_Close ?"));
	   return;
	}
	FT_SetBitMode =
	         (FNCFT_SetBitMode) GetProcAddress (dll, "FT_SetBitMode");
	if (FT_SetBitMode == NULL) {
	   status	-> setText (QString ("FT_SetBitMode"));
	   return;
	}
	FT_SetBaudRate =
	         (FNCFT_SetBaudRate) GetProcAddress (dll, "FT_SetBaudRate");
	if (FT_SetBaudRate == NULL) {
	   status	-> setText (QString ("FT_SetBaudRate"));
	   return;
	}
	FT_Write =
	         (FNCFT_Write) GetProcAddress (dll, "FT_Write");
	if (FT_Write == NULL) {
	   status	-> setText (QString ("FT_Write"));
	   return;
	}

	fprintf (stderr, "Functions from  %s are loaded\n", ELEKTOR507_DLL);
	if (FT_Open (0, &ftHandle) != 0) {
	   status ->  setText (QString ("no device"));
	   return;
	}
	FT_Close (ftHandle);
#else		// Linux apparently
	fprintf (stderr, "We gaan ftdi_init in\n");
	if (ftdi_init (&ftdic) < 0) {
	   fprintf (stderr, "ftdi_init problem\n");
	   status	-> setText (QString ("ftdi_init"));
	   return;
	}

	res = ftdi_usb_open (&ftdic, 0x0403, 0x6001);
	if (res < 0) {
	   return;
	}

	if (ftdic. type == TYPE_R) {
	   unsigned int chipid;
	   fprintf (stderr, "ftdi_read_chipid: %d\n", 
	                 ftdi_read_chipid (&ftdic, &chipid));
	   fprintf (stderr, "FTDI chipid: %X\n", chipid);
	}

	ftdi_usb_close (&ftdic);
#endif
	priv	-> FT_port	= 0x03;
	automaticPreselect	= false;
	radioOK	= true;
}

void	elektor::setVFOFrequency (int32_t freq) {
int32_t	final_freq;
int32_t	Mux;
int16_t	band, voltage;

	if (!radioOK)
	   return;

	if (priv -> ant	== ANT_AUTO) {
	   if (freq < KHz (2000))
	      Mux = 1;
	   else
	      Mux = 2;

	   priv	-> FT_port &= 0x63;	//0, 1 = I2C, 2,3,4 Mux, 5,6 Att
	   priv -> FT_port |= Mux << 2;
	}

	find_P_Q_DIV1N (priv, (int)freq);
	final_freq = getVFOFrequency ();
	fprintf (stderr,
	     "freq = %d, final = %d,  delta = %d, Div1 = %d, P = %d, Q = %d\n",
	                  (int)freq, (int)final_freq,
	                  (int)(final_freq - freq),
	                  (int)priv -> Div1N, (int)priv -> P, (int)priv -> Q);

	(void)cy_update_pll (CY_I2C_RAM_ADR);

	if (automaticPreselect)
	   if (ElektorFrequencies -> findTuningData (freq, &band, &voltage)) {
	      (void)i2c_write_reg (PCF8951_ADDRESS, DAC_ENABLE, voltage);
	      elektor_voltageDisplay	-> display (voltage);
	      preselectSlider		-> setValue (voltage);
	      setAntenna  (band + 3);
	   }
}

int32_t	elektor::getVFOFrequency (void) {
double	VCO;

	if (!radioOK) 
	   return Khz (14070);

	VCO = ((double)priv -> osc_freq * Khz (1)) / priv -> Q  * priv -> P;
	return (VCO / priv -> Div1N) / 4;
}

bool	elektor::legalFrequency (int32_t f) {
	return Khz (40) < f && f < Khz (30000);
}

int32_t	elektor::defaultFrequency	(void) {
	return Khz (14070);
}

void	elektor::setLevel (int16_t val) {
int16_t	att;

	if (!radioOK)
	   return;

	switch (val) {
	   case 0:	att = 0;	break;
	   case 10:	att = 1;	break;
	   case 20:	att = 2;	break;
	   default:
	      return;
	}

	priv	-> FT_port &= 0x1f;
	priv	-> FT_port |= (att & 0x3) << 5;
	WriteData (&priv -> FT_port, 1);
}

void	elektor::setAntenna (int16_t ant) {
int16_t	Mux;
QString	s;

	if (!radioOK)
	   return;

	switch (ant) {
	   case 1:	Mux = 0;
	                s	= QString ("ant ext");
	                priv -> ant = ANT_EXT;	break;
	   case 2:	
	   case 3:	Mux = 0; 
	                s	= QString ("ant auto");
	                priv -> ant = ANT_AUTO;  break; /* auto		*/
	   case 4:	Mux = 3;
	                s	= QString ("ant band 4");
	                priv -> ant = ANT_BAND4; break;	/* 	*/
	   case 5:	Mux = 4;
	                s	= QString ("ant band 5");
	                priv -> ant = ANT_BAND5; break;
	   case 6:	Mux = 5;
	                s	= QString ("ant band 6");
	                priv -> ant = ANT_BAND6; break;
	   case 7:	Mux = 6;
	                s	= QString ("ant band 7");
	                priv -> ant = ANT_BAND7; break;	/* */
	   case 8:	Mux = 7;
	                s	= QString ("ant test");
	                priv -> ant = ANT_TEST;  break; /* test		*/
	   default:	Mux = 0;
	                priv -> ant = NO_ANT;
	}

	priv	-> FT_port &= 0x63;
	priv	-> FT_port |= Mux << 2;
	i2c_write_reg (CY_I2C_RAM_ADR,
	                      CLKOE_REG, 
	                      0x20 | (priv -> ant == ANT_TEST ? 0x04 : 0));
	elektor_bandLabel	-> setText (s);
}

void	elektor::setXtalCal (int16_t n) {
	priv -> xtal_cal = XTAL_CAL + n;
	cy_update_pll (CY_I2C_RAM_ADR);
}

void	elektor::cy_update_pll (uint8_t IICaddress) {
int32_t	P0, R40, R41, R42;
uint8_t Div1N;
uint8_t Clk3_src;
int	Pump;

	Pump	= getPump (priv -> P);
	P0	= priv -> P & 0x01;
	R40	= (((priv -> P >> 1) - 4) >> 8) | (Pump << 2) | 0xc0;
	R41	= (((priv -> P >> 1) - 4) & 0xff);
	R42	= (priv -> Q - 2) | (P0 << 7);

	i2c_write_regs (IICaddress, 3,
	                      PUMPCOUNTERS_REG, R40, R41, R42);
	switch (priv -> Div1N) {
	   case 2:		/* fixed /2 divider option	*/
	      Clk3_src = 0x80;
	      Div1N = 8;
	      break;

	   case 3:		/* fixed /3 divider option	*/
	      Clk3_src = 0xc0;
	      Div1N = 6;
	      break;

	   default:
	      Div1N = priv -> Div1N;
	      Clk3_src = 0x40;
	}

	i2c_write_reg (IICaddress, DIV1_REG, Div1N);
/* set 2 low bits of CLKSRC for clock5, DIV1CLK is already set	*/
	i2c_write_reg (IICaddress,
	                     CLKSRC_REG + 2, Clk3_src | 0x07);
	i2c_write_reg (CY_I2C_RAM_ADR, XTALCTL_REG, 0x32);
	i2c_write_reg (IICaddress, 
	                     CAPLOAD_REG, priv -> xtal_cal);
}

void	elektor::i2c_write_reg (uint8_t IICaddress,
	                        uint8_t reg_addr, uint8_t val) {
	i2c_write_regs (IICaddress, 1, reg_addr, val, 0, 0);
}

void	elektor::i2c_write_regs (uint8_t IICaddress, int reg_count,
	                         uint8_t reg_addr, uint8_t reg_val1,
	                         uint8_t reg_val2, uint8_t reg_val3) {

	priv	-> Buf_address = 0;
	ftdi_I2C_Init ();
	ftdi_I2C_Start ();
	ftdi_I2C_Write_Byte (IICaddress);
	ftdi_I2C_Write_Byte (reg_addr);

	if (reg_count >= 1) ftdi_I2C_Write_Byte (reg_val1);
	if (reg_count >= 2) ftdi_I2C_Write_Byte (reg_val2);
	if (reg_count >= 3) ftdi_I2C_Write_Byte (reg_val3);
	ftdi_I2C_Stop ();
// usleep (10000);
 	return WriteData (priv -> FT_OutBuffer, priv -> Buf_address);
}

void	elektor::ftdi_I2C_Write_Byte (uint8_t c) {
int16_t	i;

	for (i = 7; i >= 0; i --) {
	   ftdi_SDA (c & (1 << i));		/* SDA value	*/
	   ftdi_SCL (1);
	   ftdi_SCL (0);
	}

	ftdi_SDA (1); ftdi_SCL (1); ftdi_SCL (0);
}

void	elektor::ftdi_I2C_Init (void) {
	ftdi_SCL (1); ftdi_SDA (1);
}

void	elektor::ftdi_I2C_Start (void) {
	ftdi_SDA (0); ftdi_SCL (0);
}

void	elektor::ftdi_I2C_Stop (void) {
	ftdi_SCL (0); ftdi_SDA (0); ftdi_SCL (1); ftdi_SDA (1);
}

void	elektor::ftdi_SDA (int8_t d) {
	if (priv -> Buf_address >= FT_OutBuffer_MAX)
	   return;

	if (d == 0)
	   priv -> FT_port &= ~0x01;
	else
	   priv -> FT_port |= 0x01;

	priv -> FT_OutBuffer [priv -> Buf_address ++] = priv -> FT_port;
}

void	elektor::ftdi_SCL (int8_t d) {
	if (priv -> Buf_address >= FT_OutBuffer_MAX)
	   return;

	if (d == 0)
	   priv -> FT_port &= ~0x02;
	else
	   priv -> FT_port |= 0x02;

	priv -> FT_OutBuffer [priv -> Buf_address ++] = priv -> FT_port;
}

void	elektor::find_P_Q (struct priv_data *priv, int64_t freq) {
double	Min, VCO;
int	p, q, q_max;
int32_t	freqK	= freq /= Khz (1);
	Min = priv -> osc_freq;
/*
 * 	Q: 2 .. 129
 * 	P: 8 .. 2055, best 16 .. 1023 because of pump
 * 	For stable operation:
 * 	REF /Qtotal must not fall below 250 kHz
 * 	(P * (REF / Qtotal) must not be above 400 Mhz or below 100 Mhz
 */
	q_max = priv -> osc_freq / 250;
	for (q = q_max; q >= 10; q --) 
	   for (p = 500; p <= 2000; p ++) {
	      VCO = ((double) priv -> osc_freq / q) * p;
	      if (fabs (4 * freqK - VCO / priv -> Div1N) < Min) {
	         Min = fabs (4 * freqK - VCO / priv -> Div1N);
	         priv -> Q = q;
	         priv -> P = p;
	      }
	   }

	VCO = ((double)priv -> osc_freq / priv -> Q) * priv -> P;
	if (VCO < 100000 || VCO > 400000)
	   fprintf (stderr, " Unstable for %f, %d %d %d\n",
	            VCO, priv -> P, priv -> Q, priv -> osc_freq);
}

#define	vco_min	100e3
#define	vco_max	500e3

void	elektor::find_P_Q_DIV1N (struct priv_data *priv, int64_t f) {
double	Min, VCO, freq4;
int	p, q, div1n, q_max;
int	div1n_min, div1n_max;

	Min	= priv -> osc_freq;
	freq4	= f * 4 / Khz (1);
/*
 * 	Q: 2 .. 129
 * 	P: 8 .. 2055, best 16 .. 1023 (because of pump)
 * 	For stable operations
 * 	REF / Qtotal must not fall below 250 kHz
 * 	P * (REF / Qtotal) must not be above 400 MHz or below 100 Mhz
 */
	q_max = priv -> osc_freq / 250;
	div1n_min	= vco_min / freq4;
	if (div1n_min < 2) 
	   div1n_min = 2;
	if (div1n_min > 127)
	   div1n_min = 127;

	div1n_max  	= vco_max / freq4;
	if (div1n_max > 127)
	   div1n_max = 127;
	if (div1n_max < 2)
	   div1n_max = 2;

	for (div1n = div1n_min; div1n < div1n_max; div1n++) {
// P / Qtotal	= FREQ4 * DIV1N / REF
// (Q * int (r) + frac (r) * Q) / Q`
	   for (q = q_max; q >= 2; q --) {
	      p = q * freq4 * div1n / priv -> osc_freq;
	      if (p < 16 || p > 1023)
	         continue;

	      VCO = ((double) priv -> osc_freq / q) * p;
	      if (VCO < vco_min || VCO > vco_max)
	         continue;

	      if (fabs (freq4 - VCO / div1n) < Min) {
	         Min = fabs (freq4 - VCO / div1n);
	         priv -> Div1N	= div1n;
	         priv -> P	= p;
	         priv -> Q	= q;
	      }
	   }
	}

	VCO = ((double) priv -> osc_freq / priv -> Q) * priv -> P;
	if (VCO < vco_min || VCO > 400e3)
	   fprintf (stderr, "Unstable parameters for %f\n", VCO);
}
	
int	elektor::getPump (int p) {
	if (p < 45)
	   return 0;
	if (p < 480)
	   return 1;
	if (p < 640)
	   return 2;
	if (p < 800)
	   return 3;
	return 4;
}
/*
 * 	The WriteData (to the FT232) code is sufficiently different
 * 	in the windows DLL setting from the Linux setting to justify
 * 	two occurrences of the definition
 */
#ifdef __MINGW32__
void	elektor::WriteData (void *FTOutBuf, unsigned long BufferSize) {
int	ret;
int	result;

	if (!radioOK)
	   return;

	ret = FT_Open (0, &ftHandle);
	if (ret != FT_OK) {
	   fprintf (stderr, "FT_Open, return code = %d\n", ret); 
	   return;
	}
	status -> setText (QString ("FT_Open OK"));
	ret = FT_SetBitMode (ftHandle, 0xff, 1);
	if (ret != FT_OK) 
	   return;

	ret = FT_SetBaudRate (ftHandle, 38400);
	if (ret != FT_OK) 
	   return;

	ret = FT_Write (ftHandle, FTOutBuf, BufferSize, &result);
	if (ret != FT_OK) 
	   return;

	ret = FT_Close (ftHandle);
	if (ret != FT_OK)
	   return;
}
#else
void	elektor::WriteData (void *FT_OutBuf, unsigned long BufferSize) {
int	ret;

	if (!radioOK)
	   return;

	ret = ftdi_usb_open (&ftdic, 0x0403, 0x6001);
	if (ret < 0 && ret != -5) {
	   fprintf (stderr, "unable to open ftdi %d (%s)\n",
	                    ret, ftdi_get_error_string (&ftdic));
	   return;
	}

	ftdi_set_bitmode (&ftdic, 0xFF, BITMODE_BITBANG);
	ftdi_set_baudrate (&ftdic, 38400 / 4);

 	ftdi_write_data (&ftdic,
	                       (unsigned char *)FT_OutBuf,
	                       BufferSize);
	ftdi_usb_close	(&ftdic);
}
#endif
//
////////////////////////////////////////////////////////////////////
//

bool	elektor::restartReader		(void) {
bool	b;
	b = myReader	-> restartReader ();
	return b;
}

void	elektor::stopReader		(void) {
	myReader	-> stopReader ();
}

void	elektor::exit			(void) {
	stopReader	();
}

bool	elektor::isOK			(void) {
	return true;
}

int32_t	elektor::Samples		(void) {
int32_t	n;
	n = myReader	-> Samples ();
	return n;
}

int32_t	elektor::getSamples		(DSPCOMPLEX *b, int32_t a, uint8_t m) {
int32_t i, n;
DSPCOMPLEX	temp [m];

	n = myReader -> getSamples (temp, a, m);
	for (i = 0; i < n; i ++) 
	   b [i] = cmul (temp [i], float (gainValue) / 100.0);
	return n;
}

void	elektor::set_streamSelector (int idx) {
	if (!myReader	-> set_StreamSelector (idx))
	   QMessageBox::warning (myFrame, tr ("sdr"),
	                               tr ("Selecting  input stream failed\n"));
}

void	elektor::set_rateSelector (const QString &s) {
int32_t	newRate	= s. toInt ();
int16_t	i;
	if (newRate == inputRate)
	   return;

	myReader	-> stopReader ();
	for (i = 0; i < cardSelector -> count (); i ++)
	   cardSelector -> removeItem (cardSelector -> currentIndex ()); 
	delete myReader;
	
	inputRate	= newRate;
	myReader	= new paReader (inputRate, cardSelector);
	emit set_changeRate (newRate);
}


///////////////////////////////////////////////////////////////////////
void	elektor::handle_f1	(void) {
	setAntenna	(1);
}

void	elektor::handle_f2	(void) {
	setAntenna	(2);
}

void	elektor::handle_f3	(void) {
	setAntenna	(3);
}

void	elektor::handle_f4	(void) {
	setAntenna	(4);
}

void	elektor::handle_f5	(void) {
	setAntenna	(5);
}

void	elektor::handle_f6	(void) {
	setAntenna	(6);
}

void	elektor::handle_f7	(void) {
	setAntenna	(7);
}

void	elektor::handle_f8	(void) {
	setAntenna	(8);
}

void	elektor::handle_preselectButton	(void) {
	automaticPreselect = !automaticPreselect;
	if (automaticPreselect)
	   preselectButton	-> setText (QString ("manual"));
	else
	   preselectButton	-> setText (QString ("automatic"));
}

void	elektor::handle_db0	(void) {
	setLevel	(0);
}

void	elektor::handle_db10	(void) {
	setLevel	(10);
}

void	elektor::handle_db20	(void) {
	setLevel	(20);
}

void	elektor::handle_preselectSlider	(int voltage) {
	(void)i2c_write_reg (PCF8951_ADDRESS, DAC_ENABLE, voltage);
	elektor_voltageDisplay	-> display (voltage);
}

void	elektor::handleXcalibrator	(int s) {
	setXtalCal (s);
	elektor_XtalDisplay	-> display (s);
}

void	elektor::set_gainValue		(int n) {
	gainValue	= n;
	gainDisplay	-> display (n);
}

int16_t	elektor::bitDepth		(void) {
	return 24;
}

#if QT_VERSION < 0x050000
QT_BEGIN_NAMESPACE
Q_EXPORT_PLUGIN2(device_elektor, elektor)
QT_END_NAMESPACE
#endif

