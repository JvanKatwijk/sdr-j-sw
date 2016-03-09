#
//
//	The abstract interface for the input devices
//	In this version of the sw radio, any decoder being used is
//	loaded as a plugin
#ifndef	__RIG_INTERFACE
#define	__RIG_INTERFACE

#include	<QtPlugin>
#include	<QFrame>
#include	<QThread>
#include	<QObject>
#include	"swradio-constants.h"

QT_BEGIN_NAMESPACE
class		QSettings;
class 		QObject;
class		QWidget;
QT_END_NAMESPACE
//
//	Rig implementations should implement ALL of the
//	abstract functions of this interface
class	rigInterface : public QThread {
//class	rigInterface :public QObject {
Q_OBJECT
public:
virtual	QWidget	*createPluginWindow	(int32_t, QSettings *)	= 0;
virtual		~rigInterface		(void)		= 0;

virtual	int32_t	getRate			(void)		= 0;
virtual	void	setVFOFrequency		(int32_t)	= 0;
virtual	int32_t	getVFOFrequency		(void)		= 0;
virtual	bool	legalFrequency		(int32_t)	= 0;
virtual	int32_t	defaultFrequency	(void)		= 0;

virtual	bool	restartReader		(void)		= 0;
virtual	void	stopReader		(void)		= 0;
virtual	int32_t	Samples			(void)		= 0;
virtual	int32_t	getSamples		(DSPCOMPLEX *, int32_t, uint8_t) = 0;
virtual int16_t	bitDepth		(void)		= 0;
virtual void	exit			(void)		= 0;
virtual	bool	isOK			(void)		= 0;
signals:
//	The following signals originate from the Winrad Extio interface
	void	set_ExtFrequency	(int);
	void	set_ExtLO		(int);
	void	set_lockLO		(void);
	void	set_unlockLO		(void);
	void	set_stopHW		(void);
	void	set_startHW		(void);
	void	set_changeRate		(int);
//
//	and to signal data available
	void	samplesAvailable	(int);
};

QT_BEGIN_NAMESPACE
Q_DECLARE_INTERFACE(rigInterface, "sw radio/1.0")
QT_END_NAMESPACE
#endif

