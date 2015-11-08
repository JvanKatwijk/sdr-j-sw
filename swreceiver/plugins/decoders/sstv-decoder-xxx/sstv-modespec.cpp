#
/*
 *	Copyright (c) 2007-2013, Oona Räisänen (OH2EIQ [at] sral.fi)
 *	
 *	Permission to use, copy, modify, and/or distribute this software for any
 *	purpose with or without fee is hereby granted, provided that the above
 *	copyright notice and this permission notice appear in all copies.
 *	
 *	THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 *	WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 *	MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 *	ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 *	WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 *	ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 *	OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */
#include	"sstv-worker.h"
/*
 * Mode specifications
 *
 * Name          Long, human-readable name for the mode
 * ShortName     Abbreviation for the mode, used in filenames
 * SyncLen       Duration of synchronization pulse in seconds
 * PorchLen      Duration of sync porch pulse in seconds
 * SeparatorLen  Duration of channel separator pulse in seconds
 * PixelLen      Duration of one pixel in seconds
 * LineLen       Time in seconds from the beginning of a sync pulse to the beginning of the next one
 * ImgWidth      Pixels per scanline
 * ImgHeigth     Number of scanlines
 * YScale        Height of one scanline in pixels (1 or 2)
 * ColorEnc      Color format (GBR, RGB, YUV, BW)
 *
 *
 * Note that these timings do not fully describe the workings of the different modes.
 *
 * References: 
 *             
 *             JL Barber N7CXI (2000): "Proposal for SSTV Mode Specifications". Presented at the
 *             Dayton SSTV forum, 20 May 2000.
 *
 *             Dave Jones KB4YZ (1999): "SSTV modes - line timing".
 *             <http://www.tima.com/~djones/line.txt>
 */

static
visDescriptor theModes [] = {
{  // N7CXI, 2000
	.visCode	= 0x2C,
	.Name         = "Martin M1",
	.ShortName    = "M1",
	.SyncLen      = 4.862e-3,
	.PorchLen     = 0.572e-3,
	.SeparatorLen = 0.572e-3,
	.PixelLen     = 0.4576e-3,
	.LineLen      = 446.446e-3,
	.ImgWidth     = 320,
	.ImgHeigth    = 256,
	.YScale       = 1,
	.ColorEnc     = GBR
},

{  // N7CXI, 2000
	.visCode	= 0x28,
	.Name         = "Martin M2",
	.ShortName    = "M2",
	.SyncLen      = 4.862e-3,
	.PorchLen     = 0.572e-3,
	.SeparatorLen = 0.572e-3,
	.PixelLen     = 0.2288e-3,
	.LineLen      = 226.7986e-3,
	.ImgWidth     = 320,
	.ImgHeigth    = 256,
	.YScale       = 1,
	.ColorEnc     = GBR
},

{   // KB4YZ, 1999
	.visCode	= -1,	// do not know
	.Name         = "Martin M3",
	.ShortName    = "M3",
	.SyncLen      = 4.862e-3,
	.PorchLen     = 0.572e-3,
	.SeparatorLen = 0.572e-3,
	.PixelLen     = 0.2288e-3,
	.LineLen      = 446.446e-3,
	.ImgWidth     = 320,
	.ImgHeigth    = 128,
	.YScale       = 2,
	.ColorEnc     = GBR
},

{   // KB4YZ, 1999
	.visCode	= -1,	// do not know
	.Name         = "Martin M4",
	.ShortName    = "M4",
	.SyncLen      = 4.862e-3,
	.PorchLen     = 0.572e-3,
	.SeparatorLen = 0.572e-3,
	.PixelLen     = 0.2288e-3,
	.LineLen      = 226.7986e-3,
	.ImgWidth     = 320,
	.ImgHeigth    = 128,
	.YScale       = 2,
	.ColorEnc     = GBR
},

{  // N7CXI, 2000
	.visCode	= 0x3C,
	.Name 		= "Scottie S1",
	.ShortName    = "S1",
	.SyncLen      = 9e-3,
	.PorchLen     = 1.5e-3,
	.SeparatorLen = 1.5e-3,
	.PixelLen     = 0.4320e-3,
	.LineLen      = 428.22e-3,
//	.LineLen      = 428.38e-3,
	.ImgWidth     = 320,
	.ImgHeigth    = 256,
	.YScale       = 1,
	.ColorEnc     = GBR
},

{  // N7CXI, 2000
	.visCode	= 0x38,
	.Name         = "Scottie S2",
	.ShortName    = "S2",
	.SyncLen      = 9e-3,
	.PorchLen     = 1.5e-3,
	.SeparatorLen = 1.5e-3,
	.PixelLen     = 0.2752e-3,
	.LineLen      = 277.692e-3,
	.ImgWidth     = 320,
	.ImgHeigth    = 256,
	.YScale       = 1,
	.ColorEnc     = GBR
},

{  // N7CXI, 2000
	.visCode	= 0x4C,
	.Name         = "Scottie DX",
	.ShortName    = "SDX",
	.SyncLen      = 9e-3,
	.PorchLen     = 1.5e-3,
	.SeparatorLen = 1.5e-3,
	.PixelLen     = 1.08053e-3,
	.LineLen      = 1050.3e-3,
	.ImgWidth     = 320,
	.ImgHeigth    = 256,
	.YScale       = 1,
	.ColorEnc     = GBR
},

{  // N7CXI, 2000
	.visCode	= 0x0C,
	.Name         = "Robot 72",
	.ShortName    = "R72",
	.SyncLen      = 9e-3,
	.PorchLen     = 3e-3,
	.SeparatorLen = 4.7e-3,
	.PixelLen     = 0.2875e-3,
	.LineLen      = 300e-3,
	.ImgWidth     = 320,
	.ImgHeigth    = 240,
	.YScale       = 1,
	.ColorEnc     = YUV
},

{  // N7CXI, 2000
	.visCode	= 0x08,
	.Name         = "Robot 36",
	.ShortName    = "R36",
	.SyncLen      = 9e-3,
	.PorchLen     = 3e-3,
	.SeparatorLen = 6e-3,
	.PixelLen     = 0.1375e-3,
	.LineLen      = 150e-3,
	.ImgWidth     = 320,
	.ImgHeigth    = 240,
	.YScale       = 1,
	.ColorEnc     = YUV
},

{  // N7CXI, 2000
	.visCode	= -1,		// do not know
	.Name         = "Robot 24",
	.ShortName    = "R24",
	.SyncLen      = 9e-3,
	.PorchLen     = 3e-3,
	.SeparatorLen = 6e-3,
	.PixelLen     = 0.1375e-3,
	.LineLen      = 150e-3,
	.ImgWidth     = 320,
	.ImgHeigth    = 240,
	.YScale       = 1,
	.ColorEnc     = YUV
},

{  // N7CXI, 2000
	.visCode	= -1,
	.Name         = "Robot 24 B/W",
	.ShortName    = "R24BW",
	.SyncLen      = 7e-3,
	.PorchLen     = 0e-3,
	.SeparatorLen = 0e-3,
	.PixelLen     = 0.291e-3,
	.LineLen      = 100e-3,
	.ImgWidth     = 320,
	.ImgHeigth    = 240,
	.YScale       = 1,
	.ColorEnc     = BW
},

{  // N7CXI, 2000
	.visCode	= -1,
	.Name         = "Robot 12 B/W",
	.ShortName    = "R12BW",
	.SyncLen      = 7e-3,
	.PorchLen     = 0e-3,
	.SeparatorLen = 0e-3,
	.PixelLen     = 0.291e-3,
	.LineLen      = 100e-3,
	.ImgWidth     = 320,
	.ImgHeigth    = 120,
	.YScale       = 2,
	.ColorEnc     = BW
},

{  // N7CXI, 2000
	.visCode	= -1,
	.Name         = "Robot 8 B/W",
	.ShortName    = "R8BW",
	.SyncLen      = 7e-3,
	.PorchLen     = 0e-3,
	.SeparatorLen = 0e-3,
	.PixelLen     = 0.188e-3,
	.LineLen      = 67e-3,
	.ImgWidth     = 320,
	.ImgHeigth    = 120,
	.YScale       = 2,
	.ColorEnc     = BW
},
  
{ // KB4YZ, 1999
	.visCode	= -1,
	.Name         = "Wraase SC-2 120",
	.ShortName    = "W2120",
	.SyncLen      = 5.5225e-3,
	.PorchLen     = 0.5e-3,
	.SeparatorLen = 0e-3,
	.PixelLen     = 0.489039081e-3,
	.LineLen      = 475.530018e-3,
	.ImgWidth     = 320,
	.ImgHeigth    = 256,
	.YScale       = 1,
	.ColorEnc     = RGB
},

{  // N7CXI, 2000
	.visCode	= 0x37,
	.Name         = "Wraase SC-2 180",
	.ShortName    = "W2180",
	.SyncLen      = 5.5225e-3,
	.PorchLen     = 0.5e-3,
	.SeparatorLen = 0e-3,
	.PixelLen     = 0.734532e-3,
	.LineLen      = 711.0225e-3,
	.ImgWidth     = 320,
	.ImgHeigth    = 256,
	.YScale       = 1,
	.ColorEnc     = RGB
},

{  // N7CXI, 2000
	.visCode	= -1,
	.Name         = "PD-50",
	.ShortName    = "PD50",
	.SyncLen      = 20e-3,
	.PorchLen     = 2.08e-3,
	.SeparatorLen = 0e-3,
	.PixelLen     = 0.286e-3,
	.LineLen      = 388.16e-3,
	.ImgWidth     = 320,
	.ImgHeigth    = 256,
	.YScale       = 1,
	.ColorEnc     = YUV
},

{  // N7CXI, 2000
	.visCode	= -1,
	.Name         = "PD-90",
	.ShortName    = "PD90",
	.SyncLen      = 20e-3,
	.PorchLen     = 2.08e-3,
	.SeparatorLen = 0e-3,
	.PixelLen     = 0.532e-3,
	.LineLen      = 703.04e-3,
	.ImgWidth     = 320,
	.ImgHeigth    = 256,
	.YScale       = 1,
	.ColorEnc     = YUV
},

{  // N7CXI, 2000
	.visCode	= -1,
	.Name         = "PD-120",
	.ShortName    = "PD120",
	.SyncLen      = 20e-3,
	.PorchLen     = 2.08e-3,
	.SeparatorLen = 0e-3,
	.PixelLen     = 0.19e-3,
	.LineLen      = 508.48e-3,
	.ImgWidth     = 640,
	.ImgHeigth    = 496,
	.YScale       = 1,
	.ColorEnc     = YUV
},

{  // N7CXI, 2000
	.visCode	= -1,
	.Name         = "PD-160",
	.ShortName    = "PD160",
	.SyncLen      = 20e-3,
	.PorchLen     = 2.08e-3,
	.SeparatorLen = 0e-3,
	.PixelLen     = 0.382e-3,
	.LineLen      = 804.416e-3,
	.ImgWidth     = 512,
	.ImgHeigth    = 400,
	.YScale       = 1,
	.ColorEnc     = YUV
},

{  // N7CXI, 2000
	.visCode	= -1,
	.Name         = "PD-180",
	.ShortName    = "PD180",
	.SyncLen      = 20e-3,
	.PorchLen     = 2.08e-3,
	.SeparatorLen = 0e-3,
	.PixelLen     = 0.286e-3,
	.LineLen      = 754.24e-3,
	.ImgWidth     = 640,
	.ImgHeigth    = 496,
	.YScale       = 1,
	.ColorEnc     = YUV
},

{  // N7CXI, 2000
	.visCode	= -1,
	.Name         = "PD-240",
	.ShortName    = "PD240",
	.SyncLen      = 20e-3,
	.PorchLen     = 2.08e-3,
	.SeparatorLen = 0e-3,
	.PixelLen     = 0.382e-3,
	.LineLen      = 1000e-3,
	.ImgWidth     = 640,
	.ImgHeigth    = 496,
	.YScale       = 1,
	.ColorEnc     = YUV
},

{  // N7CXI, 2000
	.visCode	= -1,
	.Name         = "PD-290",
	.ShortName    = "PD290",
	.SyncLen      = 20e-3,
	.PorchLen     = 2.08e-3,
	.SeparatorLen = 0e-3,
	.PixelLen     = 0.286e-3,
	.LineLen      = 937.28e-3,
	.ImgWidth     = 800,
	.ImgHeigth    = 616,
	.YScale       = 1,
	.ColorEnc     = YUV
}
};

visDescriptor *getVis (int16_t visCode) {
int16_t	i;

	for (i = 0; i < sizeof (theModes) / sizeof (visDescriptor); i ++)
	   if (theModes [i]. visCode == visCode)
	      return &theModes [i];
	return NULL;
}

