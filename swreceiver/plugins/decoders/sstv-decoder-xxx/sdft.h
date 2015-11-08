#
#ifndef	__SDFT
#define	__SDFT
#include	"swradio-constants.h"
#include	<math.h>
#include	<stdint.h>
#include	<cstdlib>
#include	<cstring>

class	sliding_fft {
private:
	DSPCOMPLEX	*inBuffer;
	DSPCOMPLEX	*freqs;
	DSPCOMPLEX	*coeffs;
	DSPCOMPLEX	*icoeffs;
	int32_t		length;
	DSPCOMPLEX 	oldest_data, newest_data;
	int16_t		idx;
public:
		sliding_fft (int32_t length) {
int32_t	i;
double	a;

	this -> length	= length;
	inBuffer	= new DSPCOMPLEX [length];
	coeffs		= new DSPCOMPLEX [length];
	icoeffs		= new DSPCOMPLEX [length];
	freqs		= new DSPCOMPLEX [length + 1];
//
//	e ^ -j * 2 * M_PI * i / length
	memset (inBuffer, 0, length * sizeof (DSPCOMPLEX));
	for (i = 0; i < length; i++) {
	   a = -2.0 * M_PI * i  / double (length);
	   coeffs [i] = DSPCOMPLEX (cos (a), sin(a));
	}

	for (i = 0; i < length; ++i) {
	   a = 2.0 * M_PI * i  / double (length);
           icoeffs [i] = DSPCOMPLEX (cos(a), sin(a));
	}

	oldest_data = newest_data = DSPCOMPLEX (0, 0);
	idx = 0;
}

		~sliding_fft (void) {
	delete []	inBuffer;
	delete []	coeffs;
	delete []	icoeffs;
	delete []	freqs;
}

// simulating adding data to circular buffer
void	add_data (DSPCOMPLEX v) {
	oldest_data = inBuffer [idx];
	newest_data = inBuffer [idx] = v;
	sdft ();
	if (++ idx >= length)
	   idx = 0;
}

void	clear	(void) {
	oldest_data = newest_data = DSPCOMPLEX (0, 0);
	idx	= 0;
}

// sliding dft
void	sdft (void) {
DSPCOMPLEX delta = newest_data - oldest_data;
int32_t	ci = 0;
int32_t	i;

	for (i = 0; i < this -> length; i++) {
	   freqs [i] += delta * coeffs [ci];
	   if ((ci += idx) >= length)
	      ci -= length;
	}
}

// sliding inverse dft
void	isdft (void) {
DSPCOMPLEX	delta = newest_data - oldest_data;
int32_t	ci = 0;
int32_t	i;

	for (int i = 0; i < this -> length; ++i) {
	   freqs [i] += delta * icoeffs [ci];
	   if ((ci += idx) >= length)
	      ci -= length;
	}
}


DSPCOMPLEX	*getSpectrum	(void) {
	return freqs;
}

};

#endif

