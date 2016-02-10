
#ifndef	__GETTIMEOFFSET
#define	__GETTIMEOFFSET
#include	"swradio-constants.h"
#include	<fftw3.h>

class	simpleBuf;

class	timeOffset {
public:
		timeOffset	(simpleBuf *, uint8_t, uint8_t);
		~timeOffset	(void);
	void	setFractionalInit	(float);
	float	getFractionalInit	(void);
	void	getTimeOffset	(int16_t *, float *);
private:
	void		drmfilter1 (float *,
	                            float *, float *, int32_t, int32_t);
	uint8_t		Mode;
	uint8_t		Spectrum;
	simpleBuf	*buffer;
	float		delta_time_offset_I;
	float		delta_time_offset_P;
	float		delta_time_offset;
	int16_t		delta_time_offset_integer;
	float		time_offset_fractional;

const	float EPSILON = 1.0E-10;
const	float max_theta = 10.0;
  //  float max_time_offset_ctrl = 10.0;
  //  float max_delta_time_offset = 2.0;
const	float kP_small_timing_controller = 0.01;
const	float kP_large_timing_controller = 0.01;
const	float threshold_timing_small_large = 2.0;
const	float kI_timing_controller = 0.00005;
};

#endif

