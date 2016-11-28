#
//
//	The abstract interface for the input devices
//	In this version of the sw radio, any decoder being used is
//	loaded as a plugin
#include	"rig-interface.h"

int32_t	rigInterface::getRate			(void) { return 96000;}
void	rigInterface::setVFOFrequency		(int32_t) {}
int32_t	rigInterface::getVFOFrequency		(void) { return 14700000;}
bool	rigInterface::legalFrequency		(int32_t) { return true; }
int32_t	rigInterface::defaultFrequency	(void) { return 200000000; }

bool	rigInterface::restartReader		(void) {return false;}
void	rigInterface::stopReader		(void) {}
int32_t	rigInterface::Samples			(void) {return 0;}
int32_t	rigInterface::getSamples		(DSPCOMPLEX *, int32_t, uint8_t) { return 0;}
int16_t	rigInterface::bitDepth		(void) {return 24; }
void	rigInterface::exit			(void) {}
bool	rigInterface::isOK			(void) {return false;}


