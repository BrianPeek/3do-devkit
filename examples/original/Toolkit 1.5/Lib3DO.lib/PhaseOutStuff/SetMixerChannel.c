/*****************************************************************************
 *	File:			SetMixerChannel.c
 *
 *	Contains:		Routine to set volume on a channel of the global
 *mixer.
 *
 *	Copyright:		(c) 1993-1994 The 3DO Company.  All Rights
 *Reserved.
 *
 *	History:
 *	07/12/94  Ian 	General library cleanup.
 *
 *	Implementation notes:
 *
 *	This routine is not thread-safe.
 ****************************************************************************/

#include "Debug3DO.h"
#include "Utils3DO.h"
#include "audio.h"

extern Item TheMixer;

Boolean
SetMixerChannel (int nChannel, int32 nLeftLevel, int32 nRightLevel)
{
  static int nLastChannel = -1;
  static Item iLastLeft = -1;
  static Item iLastRight = -1;

  static char LeftName[] = "LeftGain0";
  static char RightName[] = "RightGain0";

  if (nChannel != nLastChannel)
    {
      ReleaseKnob (iLastLeft);
      ReleaseKnob (iLastRight);
      nLastChannel = nChannel;

      LeftName[8] = nChannel + '0';
      RightName[9] = nChannel + '0';
      iLastLeft = GrabKnob (TheMixer, LeftName);
      iLastRight = GrabKnob (TheMixer, RightName);
      if (iLastLeft < 0 || iLastRight < 0)
        {
          DIAGNOSE (("Cannot attach balance knobs"));
          return FALSE;
        }
    }
#ifndef CardinalChanges
  TweakKnob (iLastLeft, nLeftLevel);
  TweakKnob (iLastRight, nRightLevel);
#else
  PutKnob (iLastLeft, nLeftLevel);
  PutKnob (iLastRight, nRightLevel);
#endif

  return TRUE;
}
