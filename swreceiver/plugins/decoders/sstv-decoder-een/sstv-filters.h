#
#ifndef	__SSTV_FILTERS
#define	__SSTV_FILTERS

#include	"iir-filters.h"


class	sstvFilter {
public:
	 		sstvFilter	(int32_t, int32_t, float);
	        	~sstvFilter	(void);
	void		init		(void);
	bool		check		(DSPCOMPLEX);
private:
	BandPassIIR	*bandpass;
	float		threshold;
};

#endif

