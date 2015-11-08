
#include	"sstv-filters.h"

	 	sstvFilter:: sstvFilter	(int32_t freq,
	                                 int32_t rate, float threshold) {
	bandpass	= new BandPassIIR (5,
	                                   freq - 100,
	                                   freq + 100,
	                                   rate, S_BUTTERWORTH);
	this	-> threshold	= threshold;
}

	        sstvFilter::~sstvFilter	(void) {
	delete bandpass;
}

void	sstvFilter::init		(void) {
}

bool	sstvFilter::check		(DSPCOMPLEX v) {

	v	= bandpass -> Pass (v);
	return abs (v) > threshold;
}


