/*******************************************************************************************
 *	File:			StreamChunks.cp
 *
 *	Contains:		Classes for chunks found in stream files.
 *
 *	Written by:		3DO Software Attic
 *					Chris McFall
 *
 *	Copyright � 1994 The 3DO Company. All Rights Reserved.
 *
 *	History:
 *	9/16/94		crm		New today
 *
 *******************************************************************************************/

#include "Chunk.h"
#include "ChunkFileFormat.h"
#include "ChunkQueue.h"
#include "ChunkTypes.h"
#include "Form3DOFileFormat.h"
#include "SAnimFileFormat.h"
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

/******************
 * TSAnimCCBChunk
 ******************
 * GetCCB
 ******************
 * Return the CCB encapsulated in this chunk
 */

const CCB *
TSAnimCCBChunk::GetCCB () const

{
  SAnimCCBChunk *sanimCCBChunk;

  sanimCCBChunk = (SAnimCCBChunk *)this->GetRawChunk ();

  if (sanimCCBChunk == NULL)
    return NULL;
  else
    return &sanimCCBChunk->ccb;
}

/********************
 * TSAnimFrameChunk
 ********************
 * GetChunkList
 ********************
 *
 */

void
TSAnimFrameChunk::GetEmbeddedChunks (TChunkQueue &chunkList) const

{
  TChunk *chunk;
  ChunkHeader *rawChunk;
  SAnimFrameChunk *sanimFrame;
  uint32 size;

  sanimFrame = (SAnimFrameChunk *)this->GetRawChunk ();

  // Save the size of the frame chunk and get a pointer
  // to the first encapsulated chunk in the frame

  size = sanimFrame->size - sizeof (SAnimFrameChunk);
  rawChunk = (ChunkHeader *)((char *)sanimFrame + sizeof (SAnimFrameChunk));

  for (;;)
    {
      // Make a chunk object from the encapsulated raw chunk

      chunk = new TChunk (rawChunk);

      // Add the chunk to the end of the list

      if (chunk != NULL)
        chunkList.Add (chunk);

      // Detect last chunk inside frame chunk by noting when
      // sum of encapsulated chunks equals the size of the
      // frame chunk

      size -= rawChunk->size;
      if (size <= 0)
        break;

      // Move to the next chunk

      rawChunk = (ChunkHeader *)((char *)rawChunk + rawChunk->size);
    }
}

/******************
 * TSAnimCCBChunk
 ******************
 * GetPLUT
 ******************
 * Return the PLUT encapsulated in this chunk
 */

const uint32 *
TSAnimPLUTChunk::GetPLUT (int &count) const

{
  SAnimPLUTChunk *sanimPLUTChunk;

  sanimPLUTChunk = (SAnimPLUTChunk *)this->GetRawChunk ();
  if (sanimPLUTChunk == NULL)
    {
      count = 0;
      return NULL;
    }
  else
    {
      count = (int)sanimPLUTChunk->plutChunk.numEntries;
      return (uint32 *)sanimPLUTChunk->plutChunk.PLUT;
    }
}
