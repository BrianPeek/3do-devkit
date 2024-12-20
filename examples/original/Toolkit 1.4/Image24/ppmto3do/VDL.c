/*
        File:		VDL.c

        Contains:	This file contains routines to mess around with video
   display lists.

        Written by:	eric carlson

        Copyright:	� 1993 by The 3DO Company. All rights reserved.
                                This material constitutes confidential and
   proprietary information of the 3DO Company and shall not be used by any
   Person or for any purpose except as expressly authorized in writing by the
   3DO Company.

        Change History (most recent first):

                 <4>	 8/11/93	JAY		remove 2 filler words
   from DMA control word 'length' field because of change in SubmitVDL's
   verification process. check DMA control word of vlds loaded from disk and
   adjust if length field includes filler words <3>	 6/23/93	JAY
   intergrated ec's changes for Dragon (4B1) <2>	 3/18/93	JAY
   commented out tempLong local variable. It is not used <1>	 3/18/93
   JAY		first checked in
                                2/17/93		ec		Added VDL
   diddling routines. 2/15/93		ec		Add CorrectVDL for
   Magneto6
                                 2/5/93		ec		Change for VDL
   structure generated by PS export module
                                12/5/92		ec		New Today

        To Do:
*/

#undef KERNEL

#include "Portfolio.h"
#include "debug.h"

#include "ourVDL.h"

// some globals
short gCurrFieldNum = 0;

//  Convert an 8 bit value into a long with two indexes
long
Convert8 (int32 n)
{
  long x, y;

  if (n < 128)
    {
      x = n & 0x07;
      y = 31 - (n >> 3);
    }
  else
    {
      x = (n & 0x07) + 8;
      y = 30 - ((n & 0x7F) >> 3);
    }

  return (y << 16) | x;
}

//	Convert a 16 bit pixel to a 24 bit pixel
long
ConvertPixel (int32 pixel)
{
  return (Convert8 ((pixel >> 7) & 0xF8) << 10)
         | (Convert8 ((pixel >> 2) & 0xF8) << 5)
         | Convert8 ((pixel << 3) & 0xF8);
}

/*
 * Build a vdl DMA clut control word.
 */
int32
SetDMACtlWord (int32 clutColorWords, int32 scanLineCount, Boolean lastScanLine)
{
  // the standard clut control flags are:
  //		link to next color set is relative
  //		use "current frame buffer" address from this color set
  //		use "previous frame buffer" address from this color set

  DMAControlWord clutCtlWord;

  // zero all fields to begin with
  LONGWORD (clutCtlWord) = 0;

  clutCtlWord.useCurrentLineAddr = true;
  clutCtlWord.usePreviousLineAddr = true;
  clutCtlWord.numColorWords = (int)clutColorWords;
  clutCtlWord.scanLines = (int)scanLineCount;

  // if this isn't for the last scanline, next address is relative
  if (!lastScanLine)
    {
      clutCtlWord.relativeAddrNextCLUT = true;
      clutCtlWord.videoDMA = true;
    }

  return LONGWORD (clutCtlWord);
}

/*
 *	Build a VDL display control word.
 */
int32
SetDisplayCtlWord (Boolean Hinterp, Boolean Vinterp, Boolean clutBypass)
{
  VDLDisplayCtlWord displayControl;

  LONGWORD (displayControl) = 0;

  displayControl.controlWord = 1;
  displayControl.controlWordType = 1;
  if (Hinterp)
    {
      displayControl.horizontalInterpolation = 1;
      displayControl.windowHorizontalInterp = 1;
    }
  if (Vinterp)
    {
      displayControl.verticalInterpolation = 1;
      displayControl.windowVerticalInterp = 1;
    }

  displayControl.blueLSBSource = 2; // use bit 0 of frame-buffer data
  displayControl.HSource = 0;       // not from frame, but zero always
  displayControl.VSource = 0;       // not from frame, but zero always

  displayControl.windowBlueLSBSource = 2; // use bit 0 of frame-buffer data
  displayControl.windowHSource = 0;
  displayControl.windowVSource = 0;

  if (clutBypass)
    {
      displayControl.bypassCLUT = 1;
      displayControl.bypassLSBIsRandom = 1;
      // displayControl.windowMSBReplicate = 0;
    }

  return LONGWORD (displayControl);
}

/*
 * Fill in a �simple� VDL - One CLUT for the screen with CLUTByPass Enabled
 */
void
InitSimpleVDL (SVDL *v, Bitmap *destBitmap)
{
  int32 scanLine, colorTableEntry;
  uint32 i;
  uint32 color;

  v->SVDLDMA
      = SetDMACtlWord (kClutColorWords - kFillerWords, kScreenHeight, false);
  v->SVDLCurBuf = ((uint32)destBitmap->bm_Buffer);
  v->SVDLPrevBuf = v->SVDLCurBuf;
  v->SVDLNextVDL = 0;
  v->SVDLDisplay = SetDisplayCtlWord (true, true, true);
  v->backgroundEntry = 0xE0000000;
  for (i = 0; i < kCLUTWords; i++)
    {
      color
          = (i * 0x41D) / 0x80; // This line matches the system default values
      color &= 0xFF;
      v->SVDLColors[i].index = (int)i;
      v->SVDLColors[i].select = (int)0;
      v->SVDLColors[i].flag = (int)0;
      v->SVDLColors[i].red = (int)color;
      v->SVDLColors[i].green = (int)color;
      v->SVDLColors[i].blue = (int)color;
    }
  v->SVDLFiller1 = VDL_NULLVDL;
  v->SVDLFiller1 = VDL_NULLVDL;
}

//********************************************************************************
//**
//** 							EXPERIMENTAL ROUTINES
//**							  USE WITH CAUTION!!
//**

enum
{
  nullAMYVideoControl = 0,
  videoIsPAL,
  select640Mode,
  bypassCLUT,
  slipStreamOverlay,
  forceTransparency,
  backgroundDetector,

  swapWindowPenHV,
  windowVSource,
  windowHSource,
  windowBlueLSBSource,
  windowVerticalInterp,
  windowHorizontalInterp,
  windowMSBReplicate,

  bypassLSBIsRandom,

  swapPenVH,
  VSource,
  HSource,
  blueLSBSource,
  verticalInterpolation,
  horizontalInterpolation,

  colorsOnly,

  noVerticalInterpLine
};

static char *fieldNames[] = { "nullAMYVideoControl",
                              "videoIsPAL",
                              "select640Mode",
                              "bypassCLUT",
                              "slipStreamOverlay",
                              "forceTransparency",
                              "backgroundDetector",

                              "swapWindowPenHV",
                              "windowVSource",
                              "windowHSource",
                              "windowBlueLSBSource",
                              "windowVerticalInterp",
                              "windowHorizontalInterp",
                              "windowMSBReplicate",

                              "bypassLSBIsRandom",

                              "swapPenVH",
                              "VSource",
                              "HSource",
                              "blueLSBSource",
                              "verticalInterpolation",
                              "horizontalInterpolation",

                              "colorsOnly",

                              "noVerticalInterpLine" };

/*
 * limit a number to a range
 */
short
LimitNum (short num, short min, short max)
{
  if (num > max)
    num = min;
  else if (num < min)
    num = max;

  return num;
}

/*
 * Change the VDL display control word 'focus' bit field.
 */
void
ShowAnotherField (VDLDisplayCtlWord dmaWord, Boolean nextField)
{
  if (nextField)
    gCurrFieldNum++;
  else
    gCurrFieldNum--;

  gCurrFieldNum
      = LimitNum (gCurrFieldNum, nullAMYVideoControl, noVerticalInterpLine);
  kprintf ("%s is %d,\t[%X]\n", fieldNames[gCurrFieldNum],
           GetDisplayCtlValue (dmaWord, gCurrFieldNum), LONGWORD (dmaWord));
}

/*
 * Change the value of the VDL display control word 'focus' bit field.
 */
void
SetCtlField (VDLDisplayCtlWord *dmaWord, Boolean increase)
{
  short currValue = GetDisplayCtlValue (*dmaWord, gCurrFieldNum);

  if (increase)
    currValue++;
  else
    currValue--;

  *dmaWord = SetDisplayCtlValue (*dmaWord, currValue, gCurrFieldNum);
  kprintf ("%s set to %d,\t[%X]\n", fieldNames[gCurrFieldNum],
           GetDisplayCtlValue (*dmaWord, gCurrFieldNum), LONGWORD (*dmaWord));
}

/*
 * Read the value of the VDL display control word 'focus' bit field.
 */
short
GetDisplayCtlValue (VDLDisplayCtlWord dmaWord, short fieldNum)
{
  switch (fieldNum)
    {
    case nullAMYVideoControl: //
      return dmaWord.nullAMYVideoControl;
      break;

    case videoIsPAL: // [   27] PAL video
      return dmaWord.videoIsPAL;
      break;

    case select640Mode: // [   26] select 640 mode
      return dmaWord.select640Mode;
      break;

    case bypassCLUT: // [   25] bypass CLUT if pen MSB set
      return dmaWord.bypassCLUT;
      break;

    case slipStreamOverlay: // [   24] source of overlay is SlipStream
      return dmaWord.slipStreamOverlay;
      break;

    case forceTransparency: // [   23]
      return dmaWord.forceTransparency;
      break;

    case backgroundDetector: // [   22] enable background detector
      return dmaWord.backgroundDetector;
      break;

    case swapWindowPenHV: // [   21] swap H and V bits in pen numbers in window
      return dmaWord.swapWindowPenHV;
      break;

    case windowVSource: // [20-19] selects source for V sub-positions in window
                        // pixels
      return dmaWord.windowVSource;
      break;

    case windowHSource: // [18-17] selects source for H sub-position in window
                        // pixels
      return dmaWord.windowHSource;
      break;

    case windowBlueLSBSource: // [16-15] selects source for window blue pen
                              // number's LSB
      return dmaWord.windowBlueLSBSource;
      break;

    case windowVerticalInterp: // [   14] enable vertical interpolation in
                               // window
      return dmaWord.windowVerticalInterp;
      break;

    case windowHorizontalInterp: // [   13] enable horizontal interpolation in
                                 // window
      return dmaWord.windowHorizontalInterp;
      break;

    case windowMSBReplicate: // [   11]
      return dmaWord.windowMSBReplicate;
      break;

    case bypassLSBIsRandom: // [   12] enable random number for low-bits of
                            // CLUT bypass
      return dmaWord.bypassLSBIsRandom;
      break;

    case swapPenVH: // [   10] swap H and V bits in pen number
      return dmaWord.swapPenVH;
      break;

    case VSource: // [ 9- 8] selects source of vertical sub-position
      return dmaWord.VSource;
      break;

    case HSource: // [ 7- 6] selects source of horizontal sub-position
      return dmaWord.HSource;
      break;

    case blueLSBSource: // [ 5- 4] selects source of blue pen number's LSB
      return dmaWord.blueLSBSource;
      break;

    case verticalInterpolation: // [    3] enable vertical interpolation
      return dmaWord.verticalInterpolation;
      break;

    case horizontalInterpolation: // [    2] enable horizontal interpolation
      return dmaWord.horizontalInterpolation;
      break;

    case colorsOnly: // [    1] reserved; set to zero
      return dmaWord.colorsOnly;
      break;

    case noVerticalInterpLine: // [    0] suppress vertical interpolation for
                               // this line only
      return dmaWord.noVerticalInterpLine;
      break;
    }

  return 0;
}

/*
 * Cycle the value of the VDL display control word 'focus' bit field up or
 * down.
 */
VDLDisplayCtlWord
SetDisplayCtlValue (VDLDisplayCtlWord dmaWord, short newValue, short fieldNum)
{
  //	int32	tempLong = LONGWORD(dmaWord);

  switch (fieldNum)
    {
    case nullAMYVideoControl: //
      dmaWord.nullAMYVideoControl = LimitNum (newValue, 0, 1);
      break;

    case videoIsPAL: // [   27] PAL video
      dmaWord.videoIsPAL = LimitNum (newValue, 0, 1);
      break;

    case select640Mode: // [   26] select 640 mode
      dmaWord.select640Mode = LimitNum (newValue, 0, 1);
      break;

    case bypassCLUT: // [   25] bypass CLUT if pen MSB set
      dmaWord.bypassCLUT = LimitNum (newValue, 0, 1);
      break;

    case slipStreamOverlay: // [   24] source of overlay is SlipStream
      dmaWord.slipStreamOverlay = LimitNum (newValue, 0, 1);
      break;

    case forceTransparency: // [   23]
      dmaWord.forceTransparency = LimitNum (newValue, 0, 1);
      break;

    case backgroundDetector: // [   22] enable background detector
      dmaWord.backgroundDetector = LimitNum (newValue, 0, 1);
      break;

    case swapWindowPenHV: // [   21] swap H and V bits in pen numbers in window
      dmaWord.swapWindowPenHV = LimitNum (newValue, 0, 1);
      break;

    case windowVSource: // [20-19] selects source for V sub-positions in window
                        // pixels
      dmaWord.windowVSource = LimitNum (newValue, 0, 3);
      break;

    case windowHSource: // [18-17] selects source for H sub-position in window
                        // pixels
      dmaWord.windowHSource = LimitNum (newValue, 0, 3);
      break;

    case windowBlueLSBSource: // [16-15] selects source for window blue pen
                              // number's LSB
      dmaWord.windowBlueLSBSource = LimitNum (newValue, 0, 3);
      break;

    case windowVerticalInterp: // [   14] enable vertical interpolation in
                               // window
      dmaWord.windowVerticalInterp = LimitNum (newValue, 0, 1);
      break;

    case windowHorizontalInterp: // [   13] enable horizontal interpolation in
                                 // window
      dmaWord.windowHorizontalInterp = LimitNum (newValue, 0, 1);
      break;

    case windowMSBReplicate: // [   11]
      dmaWord.windowMSBReplicate = LimitNum (newValue, 0, 1);
      break;

    case bypassLSBIsRandom: // [   12] enable random number for low-bits of
                            // CLUT bypass
      dmaWord.bypassLSBIsRandom = LimitNum (newValue, 0, 1);
      break;

    case swapPenVH: // [   10] swap H and V bits in pen number
      dmaWord.swapPenVH = LimitNum (newValue, 0, 1);
      break;

    case VSource: // [ 9- 8] selects source of vertical sub-position
      dmaWord.VSource = LimitNum (newValue, 0, 3);
      break;

    case HSource: // [ 7- 6] selects source of horizontal sub-position
      dmaWord.HSource = LimitNum (newValue, 0, 3);
      break;

    case blueLSBSource: // [ 5- 4] selects source of blue pen number's LSB
      dmaWord.blueLSBSource = LimitNum (newValue, 0, 3);
      break;

    case verticalInterpolation: // [    3] enable vertical interpolation
      dmaWord.verticalInterpolation = LimitNum (newValue, 0, 1);
      break;

    case horizontalInterpolation: // [    2] enable horizontal interpolation
      dmaWord.horizontalInterpolation = LimitNum (newValue, 0, 1);
      break;

    case colorsOnly: // [    1] reserved; set to zero
      dmaWord.colorsOnly = LimitNum (newValue, 0, 1);
      break;

    case noVerticalInterpLine: // [    0] suppress vertical interpolation for
                               // this line only
      dmaWord.noVerticalInterpLine = LimitNum (newValue, 0, 1);
      break;
    }

  return dmaWord;
}
