/*
        File:		SampleAppl.cp

        Contains:	Sample application using example classes.

        Written by:	Paul A. Bauersfeld
                                Jon A. Weiner

        Copyright:	� 1994 by The 3DO Company. All rights reserved.
                                This material constitutes confidential and
   proprietary information of the 3DO Company and shall not be used by any
   Person or for any purpose except as expressly authorized in writing by the
   3DO Company.

        Change History (most recent first):

        <1>	 10/28/93	pab		New today.

        To Do:
*/
#include "CApplication.h"
#include "CShape.h"
#include "CSound.h"
#include "Portfolio.h"
#include "Utils3DO.h"
#include "stdio.h"

#include "CPlusSwiHack.h"

class SampleAppl : public CApplication
{
public:
  SampleAppl (ScreenContext *pSC);
  ~SampleAppl (void);

  void Draw (void);

protected:
  void
  OnStart (void)
  {
  }
  void
  OnX (void)
  {
  }

  void
  OnUp (void)
  {
    fRect->Erase ();
    fRect->MoveUp (1);
    Draw ();
  }
  void
  OnDown (void)
  {
    fRect->Erase ();
    fRect->MoveDown (1);
    Draw ();
  }
  void
  OnLeft (void)
  {
    fRect->Erase ();
    fRect->MoveLeft (1);
    Draw ();
  }
  void
  OnRight (void)
  {
    fRect->Erase ();
    fRect->MoveRight (1);
    Draw ();
  }

  void
  OnA (void)
  {
    PutUpRect ();
  }
  void OnB (void);
  void OnC (void);

  void
  PutUpRect (void)
  {
    fRect->Erase ();
    fRect->MoveTo (125, 75);
    Draw ();
  }

private:
  CRect *fRect;
  ScreenContext *fScreenContext;
  CSound *fSound;
  CText *fText;
};

SampleAppl::SampleAppl (ScreenContext *pSC)
{
  Rect r1 = { 0, 0, 320, 240 };
  Rect r2 = { 50, 50, 100, 100 };
  CRect s1 (pSC, r1);

  fScreenContext = pSC;

  s1.Erase (); // Erase the screen

  fRect = new CRect (fScreenContext, r2, 0xcccc);
  // mpRect->SetFrameFill(0);

  fText = new CText (fScreenContext, "Press C button to quit!");

  fSound = new CSound ("$boot/UFOSound/Explosion.aiff", 0x20000);

  this->PutUpRect ();
}

SampleAppl::~SampleAppl (void)
{
  delete fText;
  delete fRect;
  delete fSound;
}

void
SampleAppl::Draw (void)
{
  fText->MoveTo (50, 210);
  fText->Draw ();
  fRect->Draw ();
}

void
SampleAppl::OnB (void)
{
  fRect->SetColor ((rand () & 0xffff));
  fRect->Draw ();

  fSound->Loop (1);
}

void
SampleAppl::OnC (void)
{
  this->Stop ();
}

int
main (void)
{
  ScreenContext sc;

  if (OpenGraphics (&sc, 1) < 0)
    {
      printf ("OpenGraphics failed\n");
      exit (2);
    }

  /* Make the screen we have created visible. */
  sc.sc_curScreen = 1;
  if (DisplayScreen (sc.sc_Screens[0], 0) < 0)
    {
      printf ("DisplayScreen() failed\n");
      exit (3);
    }

  SampleAppl sampleAppl (&sc);

  sampleAppl.Run ();

  ShutDown ();
}