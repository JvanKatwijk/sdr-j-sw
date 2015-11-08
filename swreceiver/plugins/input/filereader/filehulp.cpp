#
/*
 *    Copyright (C) 2010, 2011, 2012
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair programming
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
 */

#include	<QFrame>
#include	<samplerate.h>
#include	<sys/time.h>
#include	<time.h>
#include	"swradio-constants.h"
#include	"filehulp.h"
#include	"utilities.h"

#define	__BUFFERSIZE	32768 * 8

static inline
int64_t		getMyTime	(void) {
struct timeval	tv;

	gettimeofday (&tv, NULL);
	return ((int64_t)tv. tv_sec * 1000000 + (int64_t)tv. tv_usec);
}

	fileHulp::fileHulp	(QString f, int32_t rate) {
SF_INFO	*sf_info;

	fileName		= f;
	readerOK		= false;
	this	-> rate		= rate;
	_I_Buffer		= new RingBuffer<float>(__BUFFERSIZE);

	sf_info			= (SF_INFO *)alloca (sizeof (SF_INFO));
	sf_info	-> format	= 0;
	readerOK		= false;
	filePointer		= sf_open (fileName. toLatin1 (). data (),
	                                   SFM_READ, sf_info);
	if (filePointer == NULL) {
	   fprintf (stderr, "file %s no legitimate sound file\n",
	                    f. toLatin1 (). data ());
	   return;
	}

	samplesinFile	= sf_info	-> frames;
	sampleRate	= sf_info	-> samplerate;
	numofChannels	= sf_info	-> channels;
	readerOK	= true;
	readerPausing	= true;
	resetRequest	= false;
	
	currPos		= 0;
	fprintf (stderr, "file %s geopend\n", f. toLatin1 (). data ());
	start ();
}


	fileHulp::~fileHulp (void) {
	ExitCondition = true;
	if (readerOK) {
	   while (isRunning ())
	      usleep (100);
	   sf_close (filePointer);
	}
	delete _I_Buffer;
}

bool	fileHulp::restartReader	(void) {
	if (readerOK)
	   readerPausing = false;
	fprintf (stderr, "filereader restarted\n");
	return readerOK;
}

void	fileHulp::stopReader	(void) {
	if (readerOK)
	   readerPausing = true;
}

int32_t	fileHulp::Samples	(void) {
	if (ExitCondition)
	   return 0;
	return _I_Buffer -> GetRingBufferReadAvailable () / 2;
}

int32_t	fileHulp::getSamples	(DSPCOMPLEX *V, int32_t n, uint8_t Mode) {
float	*buf = (float *)alloca (2 * n * sizeof (float));
int32_t	i;

	while (!ExitCondition &&
	       (_I_Buffer -> GetRingBufferReadAvailable () < (uint32_t)(2 * n))) {
	   usleep (100);
	}

	if (ExitCondition)
	   return 0;

	 _I_Buffer	-> getDataFromBuffer (buf, 2 * n);
	for (i = 0; i < n; i ++) {
	   switch (Mode) {
	      default:
	      case IandQ:
	         V [i] = DSPCOMPLEX (buf [2 * i], buf [2 * i + 1]);
	         break;

	      case QandI:
	         V [i] = DSPCOMPLEX (buf [2 * i + 1], buf [2 * i]);
	         break;

	      case I_Only:
	         V [i]= DSPCOMPLEX (buf [2 * i], 0.0);
	         break;

	      case Q_Only:
	         V [i]	= DSPCOMPLEX (buf [2 * i + 1], 0.0);
	         break;
	   }
	}

	return n;
}

/*	length is number of floats that we read.
 *	In case of mono, we add zeros to the right channel
 *	and therefore only read length / 2 floats
 *	Notice that sf_readf_xxx reads frames (samples * numofchannels)
 */
int32_t	fileHulp::readBuffer (float *data, int32_t length) {
int32_t	n, i;
float	*tempBuffer;
static	int cnt	= 0;

	if (numofChannels == 2) {
	   n = sf_readf_float (filePointer, data, length / 2);
	}
	else {
//	apparently numofChannels == 1
	   tempBuffer	= (float *)alloca (length / 2 * sizeof (float));
	   n		= sf_readf_float (filePointer, tempBuffer, length / 2);
	   for (i = 0; i < n; i ++) {
	      data [2 * i] = tempBuffer [i];
	      data [2 * i + 1] = 0;
	   }
	}

	if (n < length / 2) {
	   sf_seek (filePointer, 0, SEEK_SET);
	   fprintf (stderr, "eof reached, starting again\n");
	}
	if (++cnt > 5) {
	   currPos		= sf_seek (filePointer, 0, SEEK_CUR);
	   emit set_progressBar (currPos * 100 / samplesinFile);
	   cnt = 0;
	}
	return	2 * n;
}

int16_t	fileHulp::setFileat	(int16_t f) {
int32_t	currPos = sf_seek (filePointer, f * samplesinFile / 100, SEEK_SET);
	return currPos * 100 / samplesinFile;
}

void	fileHulp::reset	(void) {
	resetRequest	= true;
}

/*
 *	for processing the file-io we start a separate thread
 *	In this thread, we also apply samplerate conversions
 *	- if any
 *	Note we want buffers of app the size of BUFSIZE to
 *	get out from here (which is not the same as setting
 *	the buffersize for reading files to BUFSIZE
 */
void	fileHulp::run (void) {
int32_t	t, i;
float	*bi;
int32_t	bufferSize;
int32_t	period;
int64_t	nextStop;

	if (!readerOK)
	   return;

//	We emulate the sending baudrate by building in some
//	delays. The period is 100000 / (baudrate / 10) usecs
	period		= 100000;
	ExitCondition	= false;
	if (rate == sampleRate) {	
	   bufferSize	= 2 * rate / 10;
	   bi		= new float [bufferSize];
	   nextStop	= getMyTime ();
	   while (!ExitCondition) {
	      if (readerPausing) {	// just a polling type of wait
	         usleep (1000);
	         nextStop = getMyTime ();
	         continue;
	      }
	      if (resetRequest) {
	         sf_seek (filePointer, 0, SEEK_SET);
	         resetRequest = false;
	      }
	    
	      while (_I_Buffer -> WriteSpace () < bufferSize + 10) {
	         if (ExitCondition)
	            break;
	         usleep (100);
	      }

	      nextStop += period;
	      t = readBuffer (bi, bufferSize);
	      if (t <= 0) {
	         for (i = 0; i < bufferSize; i ++)
	             bi [i] = 0;
	         t = bufferSize;
	      }
	       _I_Buffer -> putDataIntoBuffer (bi, t);
	      if (_I_Buffer -> GetRingBufferReadAvailable () > rate / 10)
	         emit samplesAvailable (rate / 10);
	      if (nextStop - getMyTime () > 0)
	         usleep (nextStop - getMyTime ());
	   }
	}
	else {		// conversion needed
	   int32_t	outputLimit	= 4 * rate / 10;
	   double	ratio	= (double)rate / sampleRate;
//
//	inputbuffer size:
	   bufferSize		= ceil (outputLimit / ratio);
	   bi			= new float [bufferSize];
	   int		error;
//	outputbuffer size
	   float	*bo	=
	                    (float *)alloca (outputLimit  * sizeof (float));
	   SRC_STATE	*converter	= src_new (SRC_SINC_BEST_QUALITY, 2, &error);
	   SRC_DATA	*src_data	= new SRC_DATA;
	   if (converter == 0) {
	      fprintf (stderr, "error creating a converter %d\n", error);
	      return;
	   }

	   fprintf (stderr,
	               "Starting converter with ratio %f (in %d, out %d)\n",
	                                       ratio, sampleRate, rate);
	   fprintf (stderr, 
	                "conversie in = %d, conversie out = %d\n", 
	                          bufferSize, outputLimit);
	   src_data -> data_in		= bi;
	   src_data -> data_out		= bo;
	   src_data -> src_ratio	= ratio;
	   src_data -> end_of_input	= 0;
	   nextStop			= getMyTime ();
	   while (!ExitCondition) {
	      int res;
	      if (readerPausing) {
	         msleep (1);
	         nextStop	= getMyTime ();
	         continue;
	      }
	      if (resetRequest) {
	         sf_seek (filePointer, 0, SEEK_SET);
	         resetRequest = false;
	      }
	      while (_I_Buffer -> WriteSpace () < outputLimit + 10) {
	         if (ExitCondition)
	            break;
	         usleep (100);
	      }

	      nextStop	+= period;
	      t = readBuffer (bi, bufferSize);
	      if (t <= 0) {
	         for (i = 0; i < bufferSize; i ++)
	            bi [i] = 0;
	         t = bufferSize;
	      }
	      src_data -> input_frames	= t / 2;
	      src_data -> output_frames	= outputLimit / 2;
// we are reading continously, after eof we just start all over again,
// therefore end_of_input flag is not altered
	      res	= src_process	(converter, src_data);
	      if (res != 0) {
	         fprintf (stderr, "error %s\n", src_strerror (t));
	      }
	      else {
	         int numofOutputs = src_data -> output_frames_gen * 2;
	         _I_Buffer -> putDataIntoBuffer (src_data -> data_out,
                                                 numofOutputs);
	      }
	      if (_I_Buffer -> GetRingBufferReadAvailable () > rate / 10)
	         emit samplesAvailable (rate / 10);
	                                      
	      if (nextStop - getMyTime () > 0)
	         usleep (nextStop - getMyTime ());
	   }

	   delete	bi;
	   src_delete (converter);
	   delete src_data;
	}
	fprintf (stderr, "taak voor replay eindigt hier\n");
}


