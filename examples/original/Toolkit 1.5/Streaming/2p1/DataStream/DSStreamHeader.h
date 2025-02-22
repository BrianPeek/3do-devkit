/*******************************************************************************************
 *	File:			DSStreamHeader.h
 *
 *	Contains:		definitions for stream header chunk
 *
 *	Written by:		Joe Buczek
 *
 *	Copyright � 1993 The 3DO Company. All Rights Reserved.
 *
 *	History:
 *  6/23/94			rdg	    fixed bug in SubsChunkHeader
 *definition... no trailing semicolon
 *  5/19/94			fyp	    Version 2.0  integrated lla and nmd's
 *changes for condition RELATIVE_BRANCHING 4/27/94			fyp
 *Version 2.0 Changed the definitions of DS_MSG_HEADER and SUBS_CHUNK_COMMON to
 *require semicolon when used in context.  (For readibility and compilation
 *using ObjectMaster). 1/20/94			rdg		make C++
 *compatible
 *	12/2/93			jb		Added 'numSubsMsgs' to stream
 *header; changed stream version to 2. 10/13/93		jb		New
 *today.
 *
 *******************************************************************************************/
#ifndef _DSSTREAMHEADER_H_
#define _DSSTREAMHEADER_H_

#include "types.h"

#ifndef __CC_NORCROFT
/* The following preamble is used at the top of each subscriber
 * chunk passed in from the streamer.
 */

#include "SubsChunkCommon.h"

#endif

/**********************/
/* Internal constants */
/**********************/

#define HEADER_CHUNK_TYPE (0x53484452) /* 'SHDR' */

#define DS_STREAM_VERSION 2 /* Stream header version number */

/********************************************************/
/* Each subscriber entry in the header consists of a	*/
/* data type for the subscriber and a delta priority at	*/
/* which the subscriber is instantiated.				*/
/********************************************************/
typedef struct DSHeaderSubs
{
  long subscriberType; /* data type for subscriber */
  long deltaPriority;  /* its delta priority */
} DSHeaderSubs, *DSHeaderSubsPtr;

/************************************/
/* Format of a stream header chunk	*/
/************************************/
#define DS_HDR_MAX_PRELOADINST 16
#define DS_HDR_MAX_SUBSCRIBER 16

typedef struct DSHeaderChunk
{
  SUBS_CHUNK_COMMON; /* from SubscriberUtils.h */

  long headerVersion; /* version of header data */

  long streamBlockSize;  /* size of stream buffers in this stream */
  long streamBuffers;    /* suggested number of stream buffers to use */
  long streamerDeltaPri; /* delta priority for streamer thread */
  long dataAcqDeltaPri;  /* delta priority for data acquisition thread */
  long numSubsMsgs;      /* number of subscriber messages to allocate */

  long audioClockChan;  /* logical channel number of audio clock channel */
  long enableAudioChan; /* mask of audio channels to enable */

  long preloadInstList[DS_HDR_MAX_PRELOADINST];
  /* NULL terminated preloaded instrument list */

  DSHeaderSubs subscriberList[DS_HDR_MAX_SUBSCRIBER];
  /* NULL terminated list of subscriber tags */

} DSHeaderChunk, *DSHeaderChunkPtr;

#endif /* _DSSTREAMHEADER_H_ */
