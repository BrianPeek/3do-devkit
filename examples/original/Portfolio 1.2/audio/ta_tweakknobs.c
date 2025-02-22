/***************************************************************
**
** Tweak the knobs available on an instrument - ta_tweakknobs.c
** By:  Phil Burk
**
** Copyright (c) 1992, 3DO Company.
** This program is proprietary and confidential.
**
***************************************************************/

/*
** 921116 PLB Modified for explicit mixer connect.
** 921118 PLB Added ChangeDirectory("/remote") for filesystem.
** 921202 PLB Converted to LoadInstrument and GrabKnob.
** 921203 PLB Use /remote/ofx
** 930315 PLB Conforms to new API
*/

#include "debug.h"
#include "filefunctions.h"
#include "operror.h"
#include "stdio.h"
#include "types.h"

/* Include this when using the Audio Folio */
#include "audio.h"

#define PRT(x)                                                                \
  {                                                                           \
    printf x;                                                                 \
  }
#define ERR(x) PRT (x)
#define DBUG(x) PRT (x)

/* Macro to simplify error checking. */
#define CHECKRESULT(val, name)                                                \
  if (val < 0)                                                                \
    {                                                                         \
      Result = val;                                                           \
      ERR (("Failure in %s: $%x\n", name, val));                              \
      PrintfSysErr (Result);                                                  \
      goto cleanup;                                                           \
    }

#define MAX_KNOBS 32

Item MixerIns = -1;
Item LeftGainKnob = -1;
Item RightGainKnob = -1;
int32 SetupMixer (void);

int
main (int argc, char *argv[])
{
  Item TestIns = 0;
  Item NoiseIns = 0;
  Item NewDir;
  int32 i, j, Val, OldVal;
  int32 NumKnobs, Result = -1;
  char *Name[MAX_KNOBS];
  Item TestKnob[MAX_KNOBS];
  int32 Min[MAX_KNOBS], Max[MAX_KNOBS], Default[MAX_KNOBS], err;
  TagArg Tags[] = {
    { AF_TAG_MIN, 0 },
    { AF_TAG_DEFAULT, 0 },
    { AF_TAG_MAX, 0 },
    { TAG_END, 0 },
  };

  PRT (("ta_tweakknobs --> mixer\n"));

  NewDir = ChangeDirectory ("/remote/dsp");
  if (NewDir > 0)
    {
      PRT (("Directory changed to /remote/dsp for Mac based file-system.\n"));
    }
  else
    {
      ERR (("Directory /remote/dsp not found! Err = $%x\n", NewDir));
    }

  /* Initialize audio, return if error. */
  if (OpenAudioFolio ())
    {
      ERR (("Audio Folio could not be opened!\n"));
      return (-1);
    }

  if (SetupMixer ())
    return -1;

  /* Load instrument definition/template */
  if (argc < 2)
    {
      PRT (("Usage:   ta_tweaknobs <filename.dsp>\n"));
      goto cleanup;
    }

  TestIns = LoadInstrument (argv[1], 0, 100);
  CHECKRESULT (TestIns, "LoadInstrument");
  NoiseIns = LoadInstrument ("noise.dsp", 0, 100);
  CHECKRESULT (TestIns, "LoadInstrument noise");

  PRT (("Connect Instruments, Noise -> Input\n"));
  Result = ConnectInstruments (NoiseIns, "Output", TestIns, "Input");
  if (Result < 0)
    PRT (("%s has no Input\n", argv[1]));

  /* Connect to Mixer */
  DBUG (("Connect Instrument -> Mixer\n"));
  Result = ConnectInstruments (TestIns, "Output", MixerIns, "Input0");
  /*	CHECKRESULT(Result,"ConnectInstruments"); */

  /* Play a note using StartInstrument and default Freq and Amplitude */
  Result = StartInstrument (TestIns, NULL);

  NumKnobs = GetNumKnobs (TestIns);
  PRT (("%d knobs.\n", NumKnobs));

  /* Attach all available knobs */
  for (i = 0; i < NumKnobs; i++)
    {
      Name[i] = GetKnobName (TestIns, i);
      if (Name[i] != NULL)
        {
          /* Attach knob so we can see what's there. */
          TestKnob[i] = GrabKnob (TestIns, Name[i]);
          CHECKRESULT (TestKnob[i], "GrabKnob");

          /* Get attributes of knob. */
          err = GetAudioItemInfo (TestKnob[i], Tags);
          CHECKRESULT (err, "GetKnobInfo");
          /* Now Pull Values from TagList */
          Min[i] = (int32)Tags[0].ta_Arg;
          Default[i] = (int32)Tags[1].ta_Arg;
          Max[i] = (int32)Tags[2].ta_Arg;
        }
    }

#define NUMSTEPS 1000
  /* Slide the value of each knob up with other knobs at default. */
  for (i = 0; i < NumKnobs; i++)
    {
      /* Set defaults. */
      for (j = 0; j < NumKnobs; j++)
        {
          TweakKnob (TestKnob[j], Default[j]);
        }
      /* Slide selected knobs values from Min to Max */
      PRT (("Tweak Knob = %s\n", Name[i]));
      PRT (("   $%x, $%x, $%x\n", Min[i], Default[i], Max[i]));

      OldVal = 123456789;
      for (j = 0; j < NUMSTEPS; j++)
        {
          Val = Min[i] + (((Max[i] - Min[i]) * j) / NUMSTEPS);
          if (Val != OldVal)
            {
              TweakKnob (TestKnob[i], Val);
              OldVal = Val;
            }
          SleepAudioTicks (1);
        }
    }

  StopInstrument (TestIns, NULL);

  /* The Audio Folio is immune to passing NULL values as Items. */
  for (i = 0; i < NumKnobs; i++)
    {
      ReleaseKnob (TestKnob[i]);
    }
  Result = UnloadInstrument (TestIns);
  CHECKRESULT (Result, "UnloadInstrument");
  Result = UnloadInstrument (NoiseIns);
  CHECKRESULT (Result, "UnloadInstrument");
  Result = UnloadInstrument (MixerIns);
  CHECKRESULT (Result, "UnloadInstrument");

  ReleaseKnob (LeftGainKnob);
  ReleaseKnob (RightGainKnob);
cleanup:
  CloseAudioFolio ();
  return ((int)Result);
}

/*********************************************************************/
int32
SetupMixer ()
{
  int32 Result = 0;

  MixerIns = LoadInstrument ("mixer4x2.dsp", 0, 0);
  CHECKRESULT (MixerIns, "LoadInstrument");

  /* Attach the Left and Right gain knobs. */
  LeftGainKnob = GrabKnob (MixerIns, "LeftGain0");
  CHECKRESULT (LeftGainKnob, "GrabKnob");
  RightGainKnob = GrabKnob (MixerIns, "RightGain0");
  CHECKRESULT (RightGainKnob, "GrabKnob");

  /* Set Mixer Levels */
  TweakKnob (LeftGainKnob, 0x2000);
  TweakKnob (RightGainKnob, 0x2000);
  /* Mixer must be started */
  Result = StartInstrument (MixerIns, NULL);
  return Result;

cleanup:
  ReleaseKnob (LeftGainKnob);
  ReleaseKnob (RightGainKnob);
  UnloadInstrument (MixerIns);
  return Result;
}
