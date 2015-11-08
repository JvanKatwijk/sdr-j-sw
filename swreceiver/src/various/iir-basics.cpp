#
/*
 *
 *    Copyright (C) 2008, 2009, 2010
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Programming
 *
 *    This file is part of the JFF SDR (JSDR).
 *    Many of the ideas as implemented in JSDR are derived from
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
 *
 */

#include	"iir-basics.h"
#ifdef	__MINGW32__
void	*alloca (size_t);
#else
#include	"alloca.h"
#endif
/*
 * 	apass is the (loss of) gain in the passband
 * 	astop is the attenuation in the passband
 * 	wpass is the frequency in the passband where apass applies
 * 	wstop is the frequency in the stopband where astop applies
 */
int16_t	guessOrderB (int16_t apass, int16_t astop,
	             DSPFLOAT wpass, DSPFLOAT wstop) {
DSPFLOAT	tmp;
DSPFLOAT nominator, denom;

	nominator	= (pow (10.0, -0.1 * astop) - 1);
	denom		= (pow (10.0, -0.1 * apass) - 1);
	nominator	= log (nominator / denom);
	tmp		= nominator / (2 * log ((DSPFLOAT)wstop / wpass)); 
/*
 *	we ensure here that the return value is even (and up)
 */
	return	(int)(tmp + 0.5);
}

int	guessOrderC (int apass, int astop, int wpass, int wstop) {
DSPFLOAT	tmp;
DSPFLOAT nominator, denom;

	nominator	= (pow (10.0, -0.1 * astop) - 1);
	denom		= (pow (10.0, -0.1 * apass) - 1);
	nominator	= sqrt (nominator / denom);
	tmp		= coshm1 (nominator) / coshm1 ((DSPFLOAT) wstop / wpass);
/*
 *	we ensure here that the return value is even (and up)
 */
	return	((int)(tmp + 0.5) + 1);
}

int	guessOrderIC (int apass, int astop, int wpass, int wstop) {
	return guessOrderC (apass, astop, wpass, wstop);
}

DSPFLOAT	warpDtoA (int fd, int fs) {
	return 2.0 * fs * tan ((2 * M_PI * fd) / (2 * fs));
}

int	warpAtoD (DSPFLOAT omega, int fs) {
        return (int)(2.0 * fs / tan (omega / (2 * fs)));
}

DSPFLOAT	newButterworth (element *biQuads,
	                int numofQuads, int order, int apass) {
element	*currentQuad;
int	i;
DSPFLOAT	Phim;
DSPFLOAT	sigma, omega;
DSPFLOAT	R;
DSPFLOAT	Eps	= sqrt (pow (10.0, -0.1 * apass) - 1);
	R = 1.0 / pow (Eps, 1.0 / order);
	if (order & 01) {
	   currentQuad	= &biQuads [0];
	   currentQuad	-> A0	= 0;
	   currentQuad	-> A1	= 0;
	   currentQuad	-> A2	= R;
	   currentQuad	-> B0	= 0;
	   currentQuad	-> B1	= 1;
	   currentQuad	-> B2	= R;
	   
	   for (i = 1; i < numofQuads; i ++) {
	      currentQuad = &biQuads [i];

	      Phim = (M_PI * (2 * (i - 1) + order + 1) / (2 * order));
	      sigma  = R * cos (Phim);
	      omega  = R * sin (Phim);

	      currentQuad -> A0 = 0;
	      currentQuad -> A1 = 0;
	      currentQuad -> A2 = (sigma * sigma + omega * omega);
	      currentQuad -> B0 = 1;
	      currentQuad -> B1 = -2 * sigma;
	      currentQuad -> B2 = (sigma * sigma + omega * omega);
	   }
	}
	else
	for (i = 0; i < numofQuads; i ++) {
	   currentQuad = &biQuads [i];

	   Phim = (M_PI * (2 * i + order + 1) / (2 * order));
	   sigma  = R * cos (Phim);
	   omega  = R * sin (Phim);

	   currentQuad -> A0 = 0;
	   currentQuad -> A1 = 0;
	   currentQuad -> A2 = (sigma * sigma + omega * omega);
	   currentQuad -> B0 = 1;
	   currentQuad -> B1 = -2 * sigma;
	   currentQuad -> B2 = (sigma * sigma + omega * omega);
	}
	return 1.0;
}

DSPFLOAT	newChebyshev (element *biQuads,
	              int numofQuads, int order, int apass) {
int	i;
DSPFLOAT	sinhD, coshD;
DSPFLOAT	Phim;
DSPFLOAT	sigma, omega;
element	*currentQuad;
DSPFLOAT	D;
DSPFLOAT	Eps	= sqrt (pow (10.0, -0.1 * apass) - 1);

	D = sinhm1 (1.0 / Eps) / order;
	sinhD	= sinh (D);
	coshD	= cosh (D);
	if (order & 01) {
	   currentQuad	= &biQuads [0];
	   currentQuad	-> A0	= 0;
	   currentQuad	-> A1	= 0;
	   currentQuad	-> A2	= sinhD;
	   currentQuad	-> B0	= 0;
	   currentQuad	-> B1	= 1;
	   currentQuad	-> B2	= sinhD;
	   for (i = 1; i < numofQuads; i ++) {
	      currentQuad = &biQuads [i];

	      Phim = (M_PI * (2 * (i - 1) + 1) / (2 * order));
	      sigma  = - sinhD * sin (Phim);
	      omega  =   coshD * cos (Phim);

	      currentQuad -> A0 = 0;
	      currentQuad -> A1 = 0;
	      currentQuad -> A2 = (sigma * sigma + omega * omega);
	      currentQuad -> B0 = 1;
	      currentQuad -> B1 = -2 * sigma;
	      currentQuad -> B2 = (sigma * sigma + omega * omega);
	   }
	}
	else
	for (i = 0; i < numofQuads; i ++) {
	   currentQuad = &biQuads [i];

	   Phim = (M_PI * (2 * i + 1) / (2 * order));
	   sigma  = - sinhD * sin (Phim);
	   omega  =   coshD * cos (Phim);

	   currentQuad -> A0 = 0;
	   currentQuad -> A1 = 0;
	   currentQuad -> A2 = (sigma * sigma + omega * omega);
	   currentQuad -> B0 = 1;
	   currentQuad -> B1 = -2 * sigma;
	   currentQuad -> B2 = (sigma * sigma + omega * omega);
	}

	if ((order & 01) == 0)		// even
	   return pow (10.0, 0.05 * apass);
	else
	   return 1.0;
}

DSPFLOAT	newChebyshevI (element *biQuads,
	               int numofQuads, int order, int apass) {
int	i;
element	*currentQuad;
DSPFLOAT	Phim;
DSPFLOAT	sigma, omega, sigmaP, omegaP, sigmazm, omegazm;
DSPFLOAT	D;
double	A1, A2, B1, B2;
int	astop;
double	Eps;
double	sinhD, coshD;

	astop	= -30;		/* db		*/
	Eps	= 1.0 / sqrt (pow (10.0, -0.1 * astop) - 1);
	D = sinhm1 (1.0 / Eps) / order;

	sinhD	= sinh (D);
	coshD	= cosh (D);
	if (order & 01) {
	   currentQuad	= &biQuads [0];
	   currentQuad	-> A0	= 0;
	   currentQuad	-> A1	= 0;
	   currentQuad	-> A2	= 1.0 / sinhD;
	   currentQuad	-> B0	= 0;
	   currentQuad	-> B1	= 1;
	   currentQuad	-> B2	= 1.0 / sinhD;
	   for (i = 1; i < numofQuads; i ++) {
	      currentQuad = &biQuads [i];

	      Phim = (M_PI * (2 * (i - 1) + 1) / (2 * order));
	      sigmaP = -sinhD * sin (Phim);
	      omegaP =  coshD * cos (Phim);

	      sigma  = sigmaP / (sigmaP * sigmaP + omegaP * omegaP);
	      omega  = - omegaP / (sigmaP * sigmaP + omegaP * omegaP);

	      sigmazm = 0.0;
	      omegazm = 1.0 / cos (Phim);

	      A1	= -2 * sigmazm;
	      A2	= sigmazm * sigmazm + omegazm * omegazm;

	      B1	= -2 * sigma;
	      B2	= sigma * sigma + omega * omega;

	      currentQuad -> A0	= 1.0;
	      currentQuad -> B0	= A2 / B2;

	      currentQuad -> A1	= A1;
	      currentQuad -> B1	= B1 * A2 / B2;

	      currentQuad -> A2	= A2;
	      currentQuad -> B2	= B2 * A2 / B2;
	   }
	}
	else
	for (i = 0; i < numofQuads; i ++) {
	   currentQuad = &biQuads [i];

	   Phim = (M_PI * (2 * i + 1) / (2 * order));
	   sigmaP = -sinh (D) * sin (Phim);
	   omegaP =  cosh (D) * cos (Phim);

	   sigma  = sigmaP / (sigmaP * sigmaP + omegaP * omegaP);
	   omega  = - omegaP / (sigmaP * sigmaP + omegaP * omegaP);

	   sigmazm = 0.0;
	   omegazm = 1.0 / cos (Phim);

	   A1	= -2 * sigmazm;
	   A2	= sigmazm * sigmazm + omegazm * omegazm;

	   B1	= -2 * sigma;
	   B2	= sigma * sigma + omega * omega;

	   currentQuad -> A0	= 1.0;
	   currentQuad -> B0	= A2 / B2;

	   currentQuad -> A1	= A1;
	   currentQuad -> B1	= B1 * A2 / B2;

	   currentQuad -> A2	= A2;
	   currentQuad -> B2	= B2 * A2 / B2;
	}
	return 1.0;
}

double	newNormalized (element *Quads,
	              int numofQuads, int order, int apass, int fType) {
/*
 */

	if (order == 0)	/* never trust a caller		*/
	   order = 6;

	if (fType == S_CHEBYSHEV) 
	   return newChebyshev (Quads, numofQuads, order, apass);
	else
	if (fType == S_BUTTERWORTH) 
	   return newButterworth (Quads, numofQuads, order, apass);
	else
	if (fType == S_INV_CHEBYSHEV)
	   return newChebyshevI (Quads, numofQuads, order, apass);
	else
	   return newButterworth (Quads, numofQuads, order, apass);
}
/*
 *	omega indicates radials
 */
double	unnormalizeLP (element *Transfer, double omega, int numofbiQuads) {
element	*CurrentQuad;
int	i;

	for (i = 0; i < numofbiQuads; i ++) {
	   CurrentQuad = &Transfer [i];

//	   CurrentQuad -> A0 = CurrentQuad -> A0;
//	   CurrentQuad -> B0 = CurrentQuad -> B0;

	   CurrentQuad -> A1 = CurrentQuad -> A1 * omega;
	   CurrentQuad -> B1 = CurrentQuad -> B1 * omega;

	   CurrentQuad -> A2 = CurrentQuad -> A2 * omega * omega;
	   CurrentQuad -> B2 = CurrentQuad -> B2 * omega * omega;
	}

	return 1.0;
}
/*
 *	Unnormalize for a High Pass filter
 */
double	unnormalizeHP (element *Transfer, double omega, int numofbiQuads) {
double A0, A1, A2, B0, B1, B2;
element	*CurrentQuad;
int	i;
double	gain	= 1.0;

	for (i = 0; i < numofbiQuads; i ++) {
	   CurrentQuad = &Transfer [i];

	   A0		= CurrentQuad -> A0;
	   A1		= CurrentQuad -> A1;
	   A2		= CurrentQuad -> A2;
	   B0		= CurrentQuad -> B0;
	   B1		= CurrentQuad -> B1;
	   B2		= CurrentQuad -> B2;

	   CurrentQuad -> A0 = 1.0;
	   CurrentQuad -> B0 = 1.0;

	   CurrentQuad -> A1 = (A1 / A2) * omega;
	   CurrentQuad -> B1 = (B1 / B2) * omega;

	   CurrentQuad -> A2 = (A0 / A2) * omega * omega;
	   CurrentQuad -> B2 = (B0 / B2) * omega * omega;

	   gain	*= A2 /  B2;
	}

	return gain;
}

/*
 *	Unnormalize for the BandPass
 */
double	unnormalizeBP (element  *temp, element *Transfer,
	                   double Wo, double BW, int numofbiQuads) {
complex A, B, C, D, E;
int	i;

	for (i = 0; i < numofbiQuads; i ++) {
/*
 *	for numerators with a zero A0 and A1 term
 */
	   if (temp [i]. A0 == 0.0) {
	      Transfer [2 * i]. A0 = 0.0;
	      Transfer [2 * i]. A1 = sqrt (temp [i]. A2) * BW;
	      Transfer [2 * i]. A2 = 0.0;
	      Transfer [2 * i + 1]. A0 = 0.0;
	      Transfer [2 * i + 1]. A1 = sqrt (temp [i]. A2) * BW;
	      Transfer [2 * i + 1]. A2 = 0.0;
	   }
	   else {
	    // make coefficients into complex numbers
	      A = complex (temp [i]. A0, 0.0);
	      B = complex (temp [i]. A1, 0.0);
	      C = complex (temp [i]. A2, 0.0);
	      cQuadratic (A, B, C, &D, &E);
	    // make required substitutions, then factor again
	      A = complex (1.0, 0.0);
	      B = cmul (-D, BW);
	      C = complex (Wo * Wo, 0.0);
	      cQuadratic (A, B, C, &D, &E);
	    // final values for coefficients
	      Transfer [2 * i]. A0 = 1.0;
	      Transfer [2 * i]. A1 = -2.0 * real (D);
	      Transfer [2 * i]. A2 = real (D * conj (D));
	      Transfer [2 * i + 1]. A0 = 1.0;
	      Transfer [2 * i + 1]. A1 = -2.0 * real (E);
	      Transfer [2 * i + 1]. A2 = real (E * conj (E));
	   }
	/*
	 *  now for the denominator (which always has A0 # 0)
	 */
	   A = complex (temp [i]. B0, 0.0);
	   B = complex (temp [i]. B1, 0.0);
	   C = complex (temp [i]. B2, 0.0);
	   cQuadratic (A, B, C, &D, &E);
	 // make required substitutions, then factor again
	   A = complex (1.0, 0.0);
	   B = cmul (-D, BW);
	   C = complex (Wo * Wo, 0);
	   cQuadratic (A, B, C, &D, &E);
	 // final values for coefficients
	   Transfer [2 * i]. B0 = 1.0;
	   Transfer [2 * i]. B1 = -2.0 * real (D);
	   Transfer [2 * i]. B2 = real (D * conj (D));
	   Transfer [2 * i + 1]. B0 = 1.0;
	   Transfer [2 * i + 1]. B1 = -2.0 * real (E);
	   Transfer [2 * i + 1]. B2 = real (E * conj (E));
	}

	return 1.0;
}

double	Bilineair (element *Transfer, int fs, int NumofQuad) {
element	*CurrentQuad;
double	f2, f4;	     /* do not make them int's, larger fs will overflow	*/
double	N0, N1, N2;
double	D0, D1, D2;
int	i;
double	gain	= 1.0;

	f2	=  2 * fs;
	f4	= f2 * f2;
	for (i = 0; i < NumofQuad; i ++) {
	   CurrentQuad = &Transfer [i];

	   N0 = CurrentQuad -> A0 * f4 + CurrentQuad -> A1 * f2 +
	                                 CurrentQuad -> A2;
	   N1 = 2 * (CurrentQuad -> A2 - CurrentQuad -> A0 * f4);
	   N2 = CurrentQuad -> A0 * f4 - CurrentQuad -> A1 * f2 +
	                                 CurrentQuad -> A2;

	   D0 = CurrentQuad -> B0 * f4 + CurrentQuad -> B1 * f2 + 
	                                 CurrentQuad -> B2;
	   D1 = 2 * (CurrentQuad -> B2 - CurrentQuad -> B0 * f4);
	   D2 = CurrentQuad -> B0 * f4 - CurrentQuad -> B1 * f2 +
	                                 CurrentQuad -> B2;

	   CurrentQuad	-> A0	= 1.0;
	   CurrentQuad	-> A1	= N1 / N0;
	   CurrentQuad	-> A2	= N2 / N0;
	   CurrentQuad	-> B0	= 1.0;
	   CurrentQuad	-> B1	= D1 / D0;
	   CurrentQuad	-> B2	= D2 / D0;
	   gain			*= (N0 / D0);
	}

	return gain;
}
/*
 *	Just an old square root formula, now in the complex domain
 */
void	cQuadratic (complex A, complex B, complex C,
	                       complex *D, complex *E) {
complex	temp	= sqrt (B * B - cmul (A * C, 4.0));

	*D	= ( -B + temp) / cmul (A, 2.0);
	*E	= ( -B - temp) / cmul (A, 2.0);
}

double	sinhm1 (double x) {
	return log (x + sqrt (x * x + 1));
}

double	coshm1 (double x) {
	return log (x + sqrt (x * x - 1));
}

