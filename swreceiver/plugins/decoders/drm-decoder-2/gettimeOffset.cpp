#
#include	"gettimeOffset.h"
#include	"simplebuf.h"
#include	"basics.h"

	timeOffset::timeOffset (simpleBuf *buffer,
	                        uint8_t Mode, uint8_t Spectrum) {
int16_t	i;

	this	-> Mode		= Mode;
	this	-> Spectrum	= Spectrum;
	this	-> buffer	= buffer;
	delta_time_offset	= 0;
	delta_time_offset_I	= 0;
	delta_time_offset_P	= 0;
	time_offset_fractional	= 0;
	delta_time_offset_integer	= 0;
}

	timeOffset::~timeOffset	(void) {
}

void	timeOffset::setFractionalInit	(float t) {
	time_offset_fractional	= t;
	delta_time_offset_I	= 0;
}

float	timeOffset::getFractionalInit	(void) {
	return time_offset_fractional;
}

//	Peek into the buffer and estimate the timeoffset, i.e.
//	the amount of samples - full samples and fractional -
//	that we are delayed.
//void	timeOffset::getTimeOffset (int16_t *offset_integer,
//	                           float   *fractional) {
//DSPCOMPLEX	*rs	= buffer	-> data;
//int32_t		index	= buffer	-> currentIndex;
//int32_t		bufMask	= buffer	-> bufSize - 1;
//
//DSPCOMPLEX	temp1	= DSPCOMPLEX (0, 0);
//DSPCOMPLEX	temp2	= DSPCOMPLEX (0, 0);
//
//int16_t		Ts	= Ts_of (Mode);
//int16_t		Tu	= Tu_of (Mode);
//int16_t		Tg	= Tg_of (Mode);
//int16_t	i;
//
//	for (i = 0; i < Tg / 4; i ++) {
//	   temp1 += rs [(index + i) & bufMask] *
//	                 conj (rs [(index + Tu + i) & bufMask]);
//	   temp2 += rs [(index + Tg - Tg / 4 + i) & bufMask] *
//	                 conj (rs [(index + Ts - Tg / 4 + i) & bufMask]);
//	}
//	
//	float	delta_theta = arg (temp1 * conj (temp2)) / (Tg / 2);
//	delta_theta =  std::min (
//	                            std::max (delta_theta, -max_theta ),
//	                            max_theta);   // limit value
////
//	delta_time_offset	= 0.9 * delta_time_offset + 0.1 * delta_theta;
//	delta_time_offset_integer = floor (delta_time_offset + 0.5);
//	time_offset_fractional = delta_time_offset -
//	                                       delta_time_offset_integer;   
//	delta_time_offset_integer =
//	             std::min (std::max ((int)delta_time_offset_integer, -1), 1);
//	*offset_integer	= delta_time_offset_integer;
//	*fractional	= time_offset_fractional;
//}


void	timeOffset::getTimeOffset (int16_t *offset_integer,
	                           float   *fractional) {
DSPCOMPLEX	*rs	= buffer	-> data;
int32_t		index	= buffer	-> currentIndex;
int32_t		bufMask	= buffer	-> bufSize - 1;
int16_t		Ts	= Ts_of (Mode);
int16_t		Tu	= Tu_of (Mode);
int16_t		Tg	= Tg_of (Mode);
DSPCOMPLEX	temp1 	= DSPCOMPLEX (0, 0);
DSPCOMPLEX	temp2 	= DSPCOMPLEX (0, 0);
DSPCOMPLEX	temp3 	= DSPCOMPLEX (0, 0);
float		temp4	= 0;
DSPCOMPLEX	temp5 	= DSPCOMPLEX (0, 0);
DSPCOMPLEX	temp6 	= DSPCOMPLEX (0, 0);
DSPCOMPLEX	temp7 	= DSPCOMPLEX (0, 0);
DSPCOMPLEX	temp8 	= DSPCOMPLEX (0, 0);
DSPCOMPLEX	temp9 	= DSPCOMPLEX (0, 0);
DSPCOMPLEX	temp10 	= DSPCOMPLEX (0, 0);
int16_t	i;

	   for (i = 0; i < Tg + 2; i++) {
	      temp1 += rs [(index + i) & bufMask] *
	                       conj (rs [(index + Tu + i) & bufMask]);
	      temp2 += rs [(index + i) & bufMask] *
	                       conj (rs [(index + i) & bufMask]);
	      temp3 += rs [(index + Tu + i) & bufMask] *
	                       conj (rs [(index + Tu + i) & bufMask]);
	   }

	   temp4 = EPSILON + 0.5 * (abs (temp2) + abs (temp3));

//	get time offset measurement signal: theta
//	   in__Tu_plus_1_to_Tu_plus_5 = in((Tu+1):(Tu+5))';
//	   in__1_to_5 = in(1:5);
	   for (i = 0; i < 5; i ++) {
	      temp5	+= rs [(index + i) & bufMask] *
	                      conj (rs [(index + Tu + i) & bufMask]);
	      temp6	+= rs [(index + i) & bufMask] *
	                        conj (rs [(index + i) & bufMask]);
	      temp7	+= rs [(index + Tu + i) & bufMask] *
	                        conj (rs [(index + Tu + i) & bufMask]);
	   }

	   DSPCOMPLEX	theta_plus = abs (temp1 - temp5 -
	                                   cmul (- temp6 - temp7, 0.5));

//	   in__Tg_minus_2_to_Tg_plus_2 = in((Tg-2):(Tg+2));
//	   int_Tg_plus_Tu_minus_2_to_Tg_plus_Tu_plus_2 = in((Tg+Tu-2):(Tg+Tu+2))';

	   for (i = 0; i < 5; i ++) {
	      temp8	+= rs [(index + Tg - 3 + i) & bufMask] *
	                      conj (rs [(index + Ts - 3 + i) & bufMask]);
	      temp9	+= rs [(index + Tg - 3 + i) & bufMask] *
	                        conj (rs [(index + Tg - 3 + i) & bufMask]);
	      temp10	+= rs [(index + Ts - 3 + i) & bufMask] *
	                        conj (rs [(index + Ts - 3 + i) & bufMask]);
	   }

	   DSPCOMPLEX	theta_minus = abs (temp1 - temp8 -
	                                   cmul (- temp9 - temp10, 0.5));
//	estimated delay in samples:
	   float	delta_theta = real (theta_plus - theta_minus ) * Tg / 2 / temp4;
	   delta_theta =  std::min (
	                            std::max (delta_theta, -max_theta ),
	                            max_theta);   // limit value

	   delta_time_offset	= 0.9 * delta_time_offset + 0.1 * delta_theta;
	   delta_time_offset_integer = floor (delta_time_offset + 0.5);
	   time_offset_fractional = delta_time_offset -
	                                       delta_time_offset_integer;   
	   delta_time_offset_integer =
	             std::min (std::max ((int)delta_time_offset_integer, -1), 1);
//	only +/- one symbol 
	   *offset_integer	= delta_time_offset_integer;
	   *fractional		= time_offset_fractional;
}

