#
//
//	For the different formats for input, we have
//	different readers, with one "mother" reader.
//	Note that the cardreader is quite different here
#ifndef	__VIRTUAL_READER
#define	__VIRTUAL_READER

#include	<stdint.h>
#include	<stdio.h>
#include	"ringbuffer.h"
#include	"swradio-constants.h"
#include	"simple-converter.h"

class	ExtioHandler;
//	The virtualReader is the mother of the readers.
//	The cardReader is slighty different, however
//	made fitting the framework
class	Reader_base {
protected:
RingBuffer<DSPCOMPLEX>	*theBuffer;
int32_t	blockSize;
public:
		Reader_base	(ExtioHandler *, int32_t);
virtual		~Reader_base	(void);
virtual void	restartReader	(int32_t s);
virtual void	stopReader	(void);
virtual void	processData	(float IQoffs, void *data, int cnt);
virtual	int16_t	bitDepth	(void);
virtual	void	setInputrate	(int32_t);
	int32_t	store_and_signal		(DSPCOMPLEX *, int32_t);
protected:
	ExtioHandler	*myMaster;
	int32_t		previousAmount;
	int32_t		base;
	int32_t		inputRate;
private:
	int32_t		outputRate;
	converter	*myConverter;
};

#endif

