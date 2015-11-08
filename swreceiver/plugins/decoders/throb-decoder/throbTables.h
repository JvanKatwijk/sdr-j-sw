#
/*
 *    Copyright (C) 2010, 2011, 2012
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Programming
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
 *
 */
#ifndef	THROB_TABLES
#define	THROB_TABLES

#include	<stdint.h>
//
//	The tables are from:
// throb.cxx  --  throb modem
//
//	Dave Freese
// Copyright (C) 2006-2010
//		Dave Freese, W1HKJ
// ThrobX additions by Joe Veldhuis, KD8ATU
//
// This file is part of SDR-J. 
//
// throb static declarations
//=====================================================================
static
int16_t	throbTonePairs[45][2] = {
	{5, 5},			/* idle... no print */
	{4, 5},			/* A */
	{1, 2},			/* B */
	{1, 3},			/* C */
	{1, 4},			/* D */
	{4, 6},			/* SHIFT (was E) */
	{1, 5},			/* F */
	{1, 6},			/* G */
	{1, 7},			/* H */
	{3, 7},			/* I */
	{1, 8},			/* J */
	{2, 3},			/* K */
	{2, 4},			/* L */
	{2, 8},			/* M */
	{2, 5},			/* N */
	{5, 6},			/* O */
	{2, 6},			/* P */
	{2, 9},			/* Q */
	{3, 4},			/* R */
	{3, 5},			/* S */
	{1, 9},			/* T */
	{3, 6},			/* U */
	{8, 9},			/* V */
	{3, 8},			/* W */
	{3, 3},			/* X */
	{2, 2},			/* Y */
	{1, 1},			/* Z */
	{3, 9},			/* 1 */
	{4, 7},			/* 2 */
	{4, 8},			/* 3 */
	{4, 9},			/* 4 */
	{5, 7},			/* 5 */
	{5, 8},			/* 6 */
	{5, 9},			/* 7 */
	{6, 7},			/* 8 */
	{6, 8},			/* 9 */
	{6, 9},			/* 0 */
	{7, 8},			/* , */
	{7, 9},			/* . */
	{8, 8},			/* ' */
	{7, 7},			/* / */
	{6, 6},			/* ) */
	{4, 4},			/* ( */
	{9, 9},			/* E */
	{2, 7}			/* space */
};

static
int16_t	ThrobXTonePairs[55][2] = {
        {6, 11},                /* idle (initially) */
        {1, 6},                 /* space (initially) */
        {2, 6},                 /* A */
        {2, 5},                 /* B */
        {2, 7},                 /* C */
        {2, 8},                 /* D */
        {5, 6},                 /* E */
        {2, 9},                 /* F */
        {2, 10},                /* G */
        {4, 8},                 /* H */
        {4, 6},                 /* I */
        {2, 11},                /* J */
        {3, 4},                 /* K */
        {3, 5},                 /* L */
        {3, 6},                 /* M */
        {6, 9},                 /* N */
        {6, 10},                /* O */
        {3, 7},                 /* P */
        {3, 8},                 /* Q */
        {3, 9},                 /* R */
        {6, 8},                 /* S */
        {6, 7},                 /* T */
        {3, 10},                /* U */
        {3, 11},                /* V */
        {4, 5},                 /* W */
        {4, 7},                 /* X */
        {4, 9},                 /* Y */
        {4, 10},                /* Z */
        {1, 2},                 /* 1 */
        {1, 3},                 /* 2 */
        {1, 4},                 /* 3 */
        {1, 5},                 /* 4 */
        {1, 7},                 /* 5 */
        {1, 8},                 /* 6 */
        {1, 9},                 /* 7 */
        {1, 10},                /* 8 */
        {2, 3},                 /* 9 */
        {2, 4},                 /* 0 */
        {4, 11},                /* , */
        {5, 7},                 /* . */
        {5, 8},                 /* ' */
        {5, 9},                 /* / */
        {5, 10},                /* ) */
        {5, 11},                /* ( */
        {7, 8},                 /* # */
        {7, 9},                 /* " */
        {7, 10},                /* + */
        {7, 11},                /* - */
        {8, 9},                 /* ; */
        {8, 10},                /* : */
        {8, 11},                /* ? */
        {9, 10},                /* ! */
        {9, 11},                /* @ */
        {10, 11},               /* = */
	{1, 11}			/* cr */ //FIXME: !!COMPLETELY NONSTANDARD!!
};

static
uint8_t	ThrobCharSet[45] = {
	'\0',			/* idle */
	'A',
	'B',
	'C',
	'D',
	'\0',			/* shift */
	'F',
	'G',
	'H',
	'I',
	'J',
	'K',
	'L',
	'M',
	'N',
	'O',
	'P',
	'Q',
	'R',
	'S',
	'T',
	'U',
	'V',
	'W',
	'X',
	'Y',
	'Z',
	'1',
	'2',
	'3',
	'4',
	'5',
	'6',
	'7',
	'8',
	'9',
	'0',
	',',
	'.',
	'\'',
	'/',
	')',
	'(',
	'E',
	' '
};

static
uint8_t	ThrobXCharSet[55] = {
        '\0',                   /* idle (initially) */
        ' ',                    /* space (initially) */
        'A',
        'B',
        'C',
        'D',
        'E',
        'F',
        'G',
        'H',
        'I',
        'J',
        'K',
        'L',
        'M',
        'N',
        'O',
        'P',
        'Q',
        'R',
        'S',
        'T',
        'U',
        'V',
        'W',
        'X',
        'Y',
        'Z',
        '1',
        '2',
        '3',
        '4',
        '5',
        '6',
        '7',
        '8',
        '9',
        '0',
        ',',
        '.',
        '\'',
        '/',
        ')',
        '(',
        '#',
        '"',
        '+',
        '-',
        ';',
        ':',
        '?',
        '!',
        '@',
        '=',
	'\n'
};

static
DSPFLOAT ThrobToneFreqsNarrow [9] = {-32, -24, -16,  -8,  0,  8, 16, 24, 32};
static
DSPFLOAT ThrobToneFreqsWid[9] = {-64, -48, -32, -16,  0, 16, 32, 48, 64};
static
DSPFLOAT ThrobXToneFreqsNarrow [11] = {-39.0625, -31.25, -23.4375, -15.625, -7.8125, 0, 7.8125, 15.625, 23.4375, 31.25, 39.0625};
static
DSPFLOAT ThrobXToneFreqsWid[11] = {-78.125, -62.5, -46.875, -31.25, -15.625, 0, 15.625, 31.25, 46.875, 62.5, 78.125};

#endif

