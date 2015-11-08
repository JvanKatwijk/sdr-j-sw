#
//
//	The abstract interface for the decoders.
//	In this version of the sw radio, any decoder being used is
//	loaded as a plugin
#ifndef	__DECODER_INTERFACE
#define	__DECODER_INTERFACE

#include	<QtPlugin>
#include	<QFrame>
#include	"swradio-constants.h"

QT_BEGIN_NAMESPACE
class		QSettings;
class 		QObject;
class		QWidget;
QT_END_NAMESPACE
//
//	All decoders implement a pure abstract interface.
//
class	decoderInterface :public QObject {
Q_OBJECT
public:
	virtual QWidget		*createPluginWindow	(int32_t,
	                                                 QSettings *) = 0;
	virtual			~decoderInterface	(void) = 0;
	virtual	void		doDecode		(DSPCOMPLEX) = 0;
	virtual	bool		initforRate		(int32_t) = 0;
	virtual int32_t		rateOut			(void) = 0;
	virtual int16_t		detectorOffset		(void) = 0;
signals:
		void		setDetectorMarker (int);
	        void		outputSample	  (float, float);
};

QT_BEGIN_NAMESPACE
Q_DECLARE_INTERFACE(decoderInterface, "sw radio/1.0")
QT_END_NAMESPACE
#endif

