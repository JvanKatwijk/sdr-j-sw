#
//
//	For the different formats for input, we have
//	different readers, with one "mother" reader.
//	Note that the cardreader is quite different here
#ifndef	__COMMON_READERS
#define	__COMMON_READERS

class	ExtioHandler;

#include	"reader-base.h"

class	reader_16: public Reader_base {
public:
	reader_16	(ExtioHandler *, int32_t, int32_t);
	~reader_16	(void);
void	processData	(float IQoffs, void *data, int cnt);
int16_t bitDepth	(void);
};

class	reader_24: public Reader_base {
public:
	reader_24	(ExtioHandler *, int32_t, int32_t);
	~reader_24	(void);
void	processData	(float IQoffs, void *data, int cnt);
int16_t bitDepth	(void);
};

class	reader_32: public Reader_base {
public:
	reader_32	(ExtioHandler *, int32_t, int32_t);
	~reader_32	(void);
void	processData	(float IQoffs, void *data, int cnt);
int16_t	bitDepth	(void);
};


class	reader_float: public Reader_base {
public:
	reader_float	(ExtioHandler *, int32_t);
	~reader_float	(void);
void	processData	(float IQoffs, void *data, int cnt);
int16_t	bitDepth	(void);
};

#endif

