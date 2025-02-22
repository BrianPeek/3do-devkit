/*******************************************************************************************
 *	File:			StreamAnalyzer.cp
 *
 *	Contains:		Object that handles chunk sequence analysis on a
 *generic stream file.
 *
 *	Written by:		3DO Software Attic
 *					Chris McFall
 *
 *	Copyright � 1994 The 3DO Company. All Rights Reserved.
 *
 *	History:
 *	9/25/94		crm		New today
 *
 *******************************************************************************************/

#include "Chunk.h"
#include "ChunkFileFormat.h"
#include "ChunkQueue.h"
#include "ChunkSequence.h"
#include "ChunkSequenceHandler.h"
#include "ChunkTypes.h"
#include "Form3DOChunks.h"
#include "Form3DOFileFormat.h"
#include "MultiPartFileName.h"
#include "Options.h"
#include "StreamAnalyzer.h"
#include "StreamChunks.h"
#include "TypeID.h"
#include <files.h>
#include <iostream.h>
#include <memory.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <types.h>

/*****************************/
/*  PUBLIC MEMBER FUNCTIONS  */
/*****************************/

/*******************
 * TStreamAnalyzer
 *******************
 * Null constructor
 */

TStreamAnalyzer::TStreamAnalyzer ()

{
  // Initialize accumulators

  this->controlBytes = 0;
  this->fillerBytes = 0;
  this->dataBytes = 0;

  // Build control sequence

  this->controlSequence.Append (CHUNK_CONTROL, TTypeID::WILDCARD);

  // Build filler sequence

  this->fillerSequence.Append (CHUNK_FILL, TTypeID::WILDCARD);

  // Build data sequence

  this->dataSequence.Append (TTypeID::WILDCARD, TTypeID::WILDCARD);

  // Register handler functions for these sequences

  this->RegisterSequence (
      &this->fillerSequence,
      (TChunkSequenceFunction)&TStreamAnalyzer::HandleFillerSequence);
  this->RegisterSequence (
      &this->controlSequence,
      (TChunkSequenceFunction)&TStreamAnalyzer::HandleControlSequence);
  this->RegisterSequence (
      &this->dataSequence,
      (TChunkSequenceFunction)&TStreamAnalyzer::HandleDataSequence);
}

/********************
 * ~TStreamAnalyzer
 ********************
 * Destructor
 */

TStreamAnalyzer::~TStreamAnalyzer () { this->OnEnd (); }

/******************************/
/*  PRIVATE MEMBER FUNCTIONS  */
/******************************/

/*************************
 * TStreamAnalyzer
 *************************
 * HandleControlSequence
 *************************
 *
 */

void
TStreamAnalyzer::HandleControlSequence (const TChunkQueue &chunkList)

{
  TChunkQueueIterator iter (&chunkList); // iterator over chunk list

  controlBytes += iter.GetNext ()->GetSize ();
}

/************************
 * TStreamAnalyzer
 ************************
 * HandleFillerSequence
 ************************
 *
 */

void
TStreamAnalyzer::HandleFillerSequence (const TChunkQueue &chunkList)

{
  TChunkQueueIterator iter (&chunkList); // iterator over chunk list

  fillerBytes += iter.GetNext ()->GetSize ();
}

/**********************
 * TStreamAnalyzer
 **********************
 * HandleDataSequence
 **********************
 *
 */

void
TStreamAnalyzer::HandleDataSequence (const TChunkQueue &chunkList)

{
  TChunkQueueIterator iter (&chunkList); // iterator over chunk list

  dataBytes += iter.GetNext ()->GetSize ();
}

/*******************
 * TStreamAnalyzer
 *******************
 * OnEnd
 *******************
 * Perform end-of-run processing.
 * Print stream analysis report to the
 * standard output.
 */

void
TStreamAnalyzer::OnEnd ()

{
  double percent;
  uint32 totalBytes;

  totalBytes = this->fillerBytes + this->dataBytes + this->controlBytes;

  cout.width (7);
  cout.setf (ios::fixed, ios::floatfield);
  cout.precision (2);

  cout << "\n\nstream size     = " << totalBytes << "\n";

  percent = (double)this->controlBytes / (double)totalBytes;
  cout << "control bytes   = " << this->controlBytes << "  (" << percent
       << ")\n";

  percent = (double)this->fillerBytes / (double)totalBytes;
  cout << "filler bytes    = " << this->fillerBytes << "  (" << percent
       << ")\n";

  percent = (double)this->dataBytes / (double)totalBytes;
  cout << "data bytes      = " << this->dataBytes << "  (" << percent << ")\n";
}
