/*
        File:		UMemory.c

        Contains:	Implementation file for new memory unit that adds
   minimal intelligence to the memory allocation scheme available on 3DO, so
   that callers are not required to remember the size of a previously-allocated
   block in order to free it.

        Written by:	Anup K Murarka, ian, eric

        Copyright:	� 1993 by The 3DO Company. All rights reserved.
                                This material constitutes confidential and
   proprietary information of the 3DO Company and shall not be used by any
   Person or for any purpose except as expressly authorized in writing by the
   3DO Company.

        Change History (most recent first):

                 <4>	 9/28/93	ec		Added prefix 'Internal'
   to names of NewPtr/FreePtr/Malloc/Free routines to integrate the leaks code.
   NO CODE CHANGES NEED RESULT FROM THESE CHANGES - macros now changes old name
   to 'Internal...' names when compile-time flag CHECK_FOR_MEMORY_LEAKS is 0.
   If the flag is true, macros change names to redirect through routines in
   Leaks.c which allocate some additional memory which make it easier to track
   down memory leaks.
                 <3>	  9/6/93	JAY		fix the fix. The
   masking acutally makes checking for gPageSizeTable being NULL. What is
   important was making sure the cur table pointer is not nil.
                 <2>	 8/25/93	JAY		fix a bug in
   VRAMTableSearch. The routine needs to consider the case of gPageSizeTable
   being NULL. <1>	 8/20/93	JAY		first checked in

                 <9>	 7/27/93	ian 	Added special handling for
   page-aligned VRAM. Added simple detection for writing beyond the boundaries
   of an allocated block. <8+>	 5/17/93	akm 	Adding rudimentary
   memory leak support <8>	 5/13/93	akm 	Changed abort() to
   Debug(); <7>	 5/13/93	akm 	Added eric*s changes from JPIDemo
   integration
                 <6>	 5/13/93	akm 	Fixed header problems for old
   mem check routines
                 <5>	 5/13/93	akm 	Split shared memory routines
   into seperate file. Added Phil*s AvailMem code. <4>	 5/11/93	akm
   Changed sharedmemory structures to work around compiler bug
                 <3>	  5/7/93	akm 	Added new implementations for
   shared memory routines <2>	  5/3/93	akm 	Added new wrappers for
   ALLOCMEM and FREEMEM. Added new getptrsize routine.
                 <1>	  5/3/93	AKM 	First Writing

        To Do:
*/

/*******************************************************************************************
 *
 *	Implementation notes:
 *
 *		The goals here are to provide good performance without using a
 *lot of code or extra memory, and to properly handle page-aligned VRAM blocks.
 *		Basic operation involves allocating four extra bytes of memory on
 *each allocation request, storing the size of the block at the head of the
 *		block, then returning ptr+4 to the caller.	Unfortunately,
 *this doesn't work for page-aligned VRAM allocations (eg, frame buffers) where
 *we can't stash our length code in the allocated block.
 *
 *		To handle the VRAM issue, we maintain a special list of
 *page-aligned VRAM allocations in a dynamically-allocated array.  It is easy
 *to detect such an allocation (from the memtype flags), but trickier to detect
 *when such a block is being freed.  To avoid searching the special list for
 *every free operation, we use a mask to quickly eliminate blocks which can't
 *possibly be VRAM page-aligned blocks.  Any pointer being freed is masked, and
 *if any low-order bits are on, it can't be a VRAM page-size aligned block, so
 *we quickly fall into the normal free logic.  If all the low-order bits are
 *off, it *might* be a page-aligned allocation that was tracked via the special
 *list instead of an in-block marker, so we first search the special list.  If
 *the block isn't found there, the normal free logic is used instead.  Since we
 *add 4 to any normal-block pointer, it will be a one-in-a-bajillion shot that
 *a non-page-aligned block will make it through the masking, so performance on
 *a free is pretty good for normal blocks.  And since page-aligned block
 *allocations/frees are not something a program normally does a lot of, the
 *simple linear search of the special list should provide "good enough"
 *performance.
 *
 *		To keep code size down, don't have any provisions for dynamically
 *expanding the list of page-aligned VRAM allocations.	We auto-init ourselves
 *to a default of 64 list entries, which should be way more than enough for
 *most programs.  If a program does need more, it can call InitMalloc() and
 *pass a new value for the table size; this must be done before any VRAM
 *page-aligned allocations are done.
 *
 *		The mask for detecting a page-aligned address initially starts
 *out as 0xFFFFFFFF. This causes no addresses to be detected as page-aligned.
 *If a page-aligned VRAM allocation is made, the mask gets set to a proper
 *value (based on the VRAM page size) and from that point on, detection of
 *page-aligned addresses is possible.	So, the initial value of 0xFFFFFFFF
 *serves the dual purpose of not trying to detect page-aligned addresses until
 *there's a need to do so, and also to serve as a flag for whether the special
 *address table and its related groodah have been allocated and initialized
 *yet.
 *******************************************************************************************/

#include "UMemory.h"
#include "semaphore.h"
#include "types.h"

#include "Debug3DO.h"

#define IS_ALIGNED_ALLOCATION(typebits)                                       \
  (((typebits)                                                                \
    & (MEMTYPE_VRAM | MEMTYPE_STARTPAGE)                                      \
          == (MEMTYPE_VRAM | MEMTYPE_STARTPAGE)))

#define IS_ALIGNED_ADDRESS(addr) ((((uint32)addr) & gPageAddressMask) == 0)

typedef struct VRAMPageSize
{
  void *addr;
  uint32 size;
} VRAMPageSize;

static uint32 gPageAddressMask = 0xFFFFFFFF;
static uint32 gPageSizeTableCount = 64;
static Item gPageSizeTableSemaphore;
static VRAMPageSize *gPageSizeTable = NULL;

/*------------------------------------------------------------------------------
 * routine to search for a block in the VRAM page-aligned size table
 *----------------------------------------------------------------------------*/

static VRAMPageSize *
VRAMTableSearch (void *addr)
{
  VRAMPageSize *cur;
  int i;

  if (gPageSizeTable == NULL)
    return NULL;

  for (i = 0, cur = gPageSizeTable; i < gPageSizeTableCount; ++i, ++cur)
    {
      if (cur != nil)
        if (cur->addr == addr)
          return cur;
    }
  return NULL;
}

/*------------------------------------------------------------------------------
 * routine to allocate a page-aligned VRAM block & store its size in the table
 *----------------------------------------------------------------------------*/

static void *
AllocVRAMPages (uint32 size, uint32 typebits)
{
  void *newBlock;
  VRAMPageSize *tableEntry;

  if (gPageAddressMask == 0xFFFFFFFF)
    {
      gPageSizeTableSemaphore = CreateSemaphore ("UMemory Table", 1);
      if (gPageSizeTableSemaphore < 0)
        {
          DIAGNOSE (("Cannot CreateSemaphore() for page-aligned VRAM tracking "
                     "table\n"));
          return NULL;
        }
      gPageSizeTable = (VRAMPageSize *)ALLOCMEM (
          gPageSizeTableCount * sizeof (VRAMPageSize),
          MEMTYPE_ANY | MEMTYPE_FILL | 0x00);
      if (gPageSizeTable == NULL)
        {
          DeleteItem (gPageSizeTableSemaphore);
          DIAGNOSE (("Cannot create page-aligned VRAM tracking table\n"));
          return NULL;
        }
      gPageAddressMask = GetPageSize (MEMTYPE_VRAM) - 1;
    }

  LockItem (gPageSizeTableSemaphore, 1); // lock the table
  tableEntry = VRAMTableSearch (NULL);   // find an empty slot
  if (tableEntry == NULL)                // if out of slots, punt
    {
      DIAGNOSE (("Page-aligned VRAM tracking table is full, increase via "
                 "InitMalloc()\n"));
      return NULL;
    }

  newBlock = ALLOCMEM (size, typebits);

  tableEntry->addr = newBlock; // if newBlock is NULL, adding it to
  tableEntry->size = size;     // the table anyway is innocuous.

  UnlockItem (gPageSizeTableSemaphore); // unlock the table
  return newBlock;                      // and return the block ptr
}

/*------------------------------------------------------------------------------
 * routine to free a page-aligned VRAM block if it's in the table.
 *	(if it isn't in the table it was a false detection of a potential
 *	VRAM block at a higher level, and this routine returns false.
 *	false detections should almost never happen though.)
 *----------------------------------------------------------------------------*/

static Boolean
FreeVRAMPages (void *block)
{
  VRAMPageSize *tableEntry;
  Boolean rv;

  LockItem (gPageSizeTableSemaphore, 1);
  tableEntry = VRAMTableSearch (block);
  if (tableEntry == NULL)
    {
      rv = FALSE;
    }
  else
    {
      FREEMEM (block, tableEntry->size);
      tableEntry->addr = NULL;
      rv = TRUE;
    }
  UnlockItem (gPageSizeTableSemaphore);
  return rv;
}

/*------------------------------------------------------------------------------
 * (optional) init routine which application can use to request more than
 *	the default number of entries in the VRAM page-aligned allocation
 *table.
 *----------------------------------------------------------------------------*/

int32
InitMalloc (uint32 numVRAMTableEntries)
{
  if (numVRAMTableEntries == 0)
    {
      DIAGNOSE (("InitMalloc() requires non-zero paramter\n"));
      return -1;
    }

  if (gPageSizeTable == NULL)
    {
      gPageSizeTableCount = numVRAMTableEntries;
      return 0;
    }
  else
    {
      DIAGNOSE (("InitMalloc() failed; some page-aligned VRAM allocations "
                 "have already been done\n"));
      return -1;
    }
}

/*------------------------------------------------------------------------------
 * routine to allocate a block of ram of any type.
 *	this routine is also known as 'InternalNewPtr', via a macro in
 *umemory.h.
 *----------------------------------------------------------------------------*/

void *
InternalMalloc (uint32 size, uint32 typebits)
{
  uint32 *newBlock;

#if DEBUG
  if (size == 0)
    {
      DIAGNOSE (("Warning: Should not allocate block of size 0"));
      return NULL;
    }
#endif

  if (IS_ALIGNED_ALLOCATION (typebits))
    {
      newBlock = (uint32 *)AllocVRAMPages (size, typebits);
    }
  else
    {
      newBlock = (uint32 *)ALLOCMEM (size + 4, typebits);
      if (newBlock != NULL)
        {
          *newBlock++ = size; // hide size for later retrieval on normal blocks
        }
      else
        {
          DIAGNOSE (("Not enough memory for Malloc(%ld, 0x%08lX)\n", size,
                     typebits));
        }
    }

  return (void *)newBlock;
}

/*------------------------------------------------------------------------------
 * routine to free a previously-allocated block of ram of any type.
 *	this routine is also known as 'InternalFreePtr' via a macro in
 *umemory.h.
 *----------------------------------------------------------------------------*/

void *
InternalFree (void *ptr)
{
  uint32 *oldblock;

  if (ptr == NULL)
    {
      DIAGNOSE (("Attempt to Free(NULL)\n"));
      return NULL;
    }

#if DEBUG
  if ((uint32)ptr & 0x00000003)
    {
      DIAGNOSE (("Free(%p); pointer is not word aligned\n", ptr));
      return NULL;
    }
#endif

  if (IS_ALIGNED_ADDRESS (ptr))
    { // if it looks like a page-aligned VRAM
      if (FreeVRAMPages (ptr))
        {              // address, try to free it as such; if
          return NULL; // that works, we can return now.
        }
    }

  oldblock = ((uint32 *)ptr) - 1; // retrieve size hidden by Malloc

  FREEMEM ((void *)oldblock, *oldblock + 4);

  return NULL; // just a nice thing for easy assignment back to variables
}

/*------------------------------------------------------------------------------
 * routine to return the size of a previously-allocated block of ram.
 *	this routine is also known as 'InternalGetPtrSize' via a macro in
 *umemory.h.
 *----------------------------------------------------------------------------*/

uint32
InternalMemBlockSize (void *ptr)
{
  VRAMPageSize *tableEntry;

  if (ptr == NULL)
    {
      DIAGNOSE (("Attempt to get MemBlockSize(NULL)\n"));
      return 0;
    }

#if DEBUG
  if ((uint32)ptr & 0x00000003)
    {
      DIAGNOSE (("MemBlockSize(%p) pointer is not word aligned", ptr));
      return 0;
    }
#endif

  if (IS_ALIGNED_ADDRESS (ptr))
    {
      if (NULL != (tableEntry = VRAMTableSearch (ptr)))
        {
          return tableEntry->size;
        }
    }

  return *(((uint32 *)ptr) - 1);
}

/*------------------------------------------------------------------------------
 * routines to provide historical continuity...
 *	each of these just invokes the new functions by the old name; when
 *	compiled without debugging, the compiler turns each of these into a
 *single branch instruction to one of the above routines (ie, it's fairly
 *efficient).
 *----------------------------------------------------------------------------*/

void *
InternalNewPtr (uint32 size, uint32 memtype)
{
  return InternalMalloc (size, memtype);
}

void *
InternalFreePtr (void *blk)
{
  return InternalFree (blk);
}

uint32
InternalGetPtrSize (void *ptr)
{
  return InternalMemBlockSize (ptr);
}
