#include "types.h"

#include "audio.h"
#include "debug.h"
#include "filefunctions.h"
#include "folio.h"
#include "graphics.h"
#include "io.h"
#include "kernel.h"
#include "kernelnodes.h"
#include "list.h"
#include "mem.h"
#include "nodes.h"
#include "operamath.h"
#include "semaphore.h"
#include "stdlib.h"
#include "strings.h"
#include "task.h"

#include "Form3DO.h"
#include "Init3DO.h"
#include "KTextBox.h"
#include "Parse3DO.h"
#include "Utils3DO.h"

#define isKanji(c) (0x81 <= (c) && (c) <= 0x9F || 0xE0 <= (c) && (c) <= 0xFC)
#define isKanji2(c) (0x40 <= (c) && (c) <= 0xFC && (c) != 0x7F)
#define isKana(c) (0xA0 <= (c) && (c) <= 0xDF)
/*#define isANK(c)		( isprint(c) || isKana(c) )*/
#define isANK(c) ((0x20 <= (c) && (c) <= 0x7E) || isKana (c))

#define CR 0x0D         /* ���s */
#define LF 0x0A         /* ���A */
#define ESC 0x1B        /* �G�X�P�[�v */
#define TAB 0x09        /* �^�u */
#define SPACE 0x20      /* �X�y�[�X */
#define KANA_START 0xA0 /* ���p�J�^�J�i�X�^�[�g */
#define DEL 0x7F        /* �f���[�g���� */

#define O_VERTICAL 0x01 /* �c���� ON */
#define O_KINSOKU 0x02  /* �֑����� ON */

#define ATTR_STD 0x01    /* �W���̎��� */
#define ATTR_BOLD 0x02   /* �{�[���h��(����) */
#define ATTR_ITARIC 0x04 /* �C�^���b�N��(�Α�) */
#define ATTR_WHITE 0x08  /* ������ */
#define ATTR_SHADOW 0x10 /* �e�t�� */

#define XXX_UNDERLINE 0x00   /* ���� */
#define XXX_RUBI 0x00        /* ���r */
#define XXX_SUBSCRIPT 0x00   /* ���t�� */
#define XXX_SUPERSCRIPT 0x00 /* ��t�� */

#define TOP_KIN_ZEN "" /* �S�p�s���֑����� */
#define BTM_KIN_ZEN "" /* �S�p�s���֑����� */
#define TOP_KIN_HAN "" /* ���p�s���֑����� */
#define BTM_KIN_HAN "" /* ���p�s���֑����� */

#define FONT_GOTHIC 1  /* �����S�V�b�N�� */
#define FONT_MINCHOU 2 /* ���������� */

#define PLUTSIZE_4BIT_CODEDCEL 32 /* 4bit�Z����PLUT�̃T�C�Y */
#define PLUTSIZE_1BIT_CODEDCEL 4  /* 1bit�Z����PLUT�̃T�C�Y */

#define FOURBITS_PER_PIXEL 4 /* 4bit�Z����PLUT�̏ꍇ */
#define ONEBITS_PER_PIXEL 1  /* 1bit�Z����PLUT�̃T�C�Y */

#define CODE_TABLE_HAN 0x01   /* ���p�R�[�h�e�[�u�������݂��� */
#define CODE_TABLE_ZEN 0x02   /* �S�p�R�[�h�e�[�u�������݂��� */
#define MINMAX_TABLE_HAN 0x04 /* ���p�����͈̓e�[�u�������݂��� */
#define MINMAX_TABLE_ZEN 0x08 /* �S�p�����͈̓e�[�u�������݂��� */
#define WIDTH_TABLE_HAN 0x10  /* ���p�������e�[�u�������݂��� */
#define WIDTH_TABLE_ZEN 0x20  /* �S�p�������e�[�u�������݂��� */

typedef struct
{
  Int32 Value; /* �����R�[�h */
} KCharRec;

typedef struct
{
  Int32 chunk_ID; /* 'JFNT' Magic number to identify font data */
  Int32
      chunk_size; /* Size in bytes of chunk including chunk_size & chunk_ID */
  uint32 version; /* File format version identifier.  0 for now */

  uint32 headerLength; /* Size in bytes of KFontFileHeader */
  uint32 fontType;     /* 0 = bitmap */
  uint32 fontSubType;  /* Type face of the font */
  uint32 fontFlags;    /* 32-bits of font flags.  0 for now */

  uint32 fontCharWidth;  /* �����̍��� */
  uint32 fontCharHeight; /* �����̕� */
  uint32 bitsPerPixel;   /* �P��f������̃r�b�g�� */
  uint32 grayScaleBit;   /* �P��f������̃r�b�g���̂����K���p�r�b�g�� */
  uint32 cornerWeightBit;
  uint32 hmincode;        /* ���p�����ŏ��R�[�h */
  uint32 hmaxcode;        /* ���p�����ő�R�[�h */
  uint32 zmincode;        /* �S�p�����ŏ��R�[�h */
  uint32 zmaxcode;        /* �S�p�����ő�R�[�h */
  uint32 hcharNum;        /* ���p������ */
  uint32 zcharNum;        /* �S�p������ */
  uint32 offsetHcodetbl;  /* ���p�k�ރe�[�u���ւ̃I�t�Z�b�g */
  uint32 offsetZcodetbl;  /* �S�p�k�ރe�[�u���ւ̃I�t�Z�b�g */
  uint32 offsetHwidthtbl; /* ���p�������e�[�u���ւ̃I�t�Z�b�g */
  uint32 offsetZwidthtbl; /* �S�p�������e�[�u���ւ̃I�t�Z�b�g */
  uint32 offsetHbitmap;
  uint32 offsetZbitmap;
  uint32 offsetCopyright;    /* offset to '/0' terminated Copyright string */
  uint32 offsetTypeFaceName; /* offset to '/0' terminated ASCII string */
  uint32 revision;           /* revision number of actual font bitmap data */
  uint32 compression;
  uint32 charCode;
  uint32 reserved1;
  uint32 reserved2;
  uint32 reserved3;
  uint32 reserved4;
} KFontFileHeader;

typedef struct
{
  KFontFileHeader *header; /* �t�H���g�t�@�C���w�b�_ */
  char *hcodeTbl;          /* ���p�̕����R�[�h�e�[�u�� */
} KFontDesc;

///������������������������������������������������������������������������������������
///	GLOBALS
///������������������������������������������������������������������������������������

static uint8 MASK_TABLE[7] = { 0x01, 0x03, 0x07, 0x0F, 0x1F, 0x3F, 0x7F };

/*
        Make 16 gray scale level
*/
void
makeElementPLUT (frac16 *element, Int32 fgElement, Int32 bgElement)
{

  Int32 diffElement, i, delta32;
  frac16 delta, bgf16, fgf16;

  diffElement = fgElement - bgElement;
  bgf16 = Convert32_F16 (bgElement);
  fgf16 = Convert32_F16 (fgElement);

  if (diffElement > 0)
    {
      delta = DivSF16 (Convert32_F16 (diffElement), Convert32_F16 (15));
      delta32 = ConvertF16_32 (delta);
      *element++ = bgf16;
      *element++ = bgf16 + delta;
      *element++ = bgf16 + Convert32_F16 (delta32 * 2);
      *element++ = bgf16 + Convert32_F16 (delta32 * 3);
      *element++ = bgf16 + Convert32_F16 (delta32 * 4);
      *element++ = bgf16 + Convert32_F16 (delta32 * 5);
      *element++ = bgf16 + Convert32_F16 (delta32 * 6);
      *element++ = bgf16 + Convert32_F16 (delta32 * 7);
      *element++ = fgf16 - Convert32_F16 (delta32 * 7);
      *element++ = fgf16 - Convert32_F16 (delta32 * 6);
      *element++ = fgf16 - Convert32_F16 (delta32 * 5);
      *element++ = fgf16 - Convert32_F16 (delta32 * 4);
      *element++ = fgf16 - Convert32_F16 (delta32 * 3);
      *element++ = fgf16 - Convert32_F16 (delta32 * 2);
      *element++ = fgf16 - delta;
      *element = fgf16;
    }
  else if (diffElement < 0)
    {
      delta
          = DivSF16 (Convert32_F16 (((-1) * diffElement)), Convert32_F16 (15));
      delta32 = ConvertF16_32 (delta);
      *element++ = bgf16;
      *element++ = bgf16 - delta;
      *element++ = bgf16 - Convert32_F16 (delta32 * 2);
      *element++ = bgf16 - Convert32_F16 (delta32 * 3);
      *element++ = bgf16 - Convert32_F16 (delta32 * 4);
      *element++ = bgf16 - Convert32_F16 (delta32 * 5);
      *element++ = bgf16 - Convert32_F16 (delta32 * 6);
      *element++ = bgf16 - Convert32_F16 (delta32 * 7);
      *element++ = fgf16 + Convert32_F16 (delta32 * 7);
      *element++ = fgf16 + Convert32_F16 (delta32 * 6);
      *element++ = fgf16 + Convert32_F16 (delta32 * 5);
      *element++ = fgf16 + Convert32_F16 (delta32 * 4);
      *element++ = fgf16 + Convert32_F16 (delta32 * 3);
      *element++ = fgf16 + Convert32_F16 (delta32 * 2);
      *element++ = fgf16 + delta;
      *element = fgf16;
    }
  else if (diffElement == 0)
    {
      for (i = 0; i < 16; i++)
        {
          *element++ = fgf16;
        }
    }
}

/*
        Convert color to PLUT.
        16 gray scale level.
                and
         transparent attribute.
*/
Int32
KMakePLUT16 (Color fgColor, Color bgColor, uint8 opaqueFlag,
             uint16 **thePLUTPtr)
{
  Int32 i;
  uint16 newColor;
  Int32 fgRed, fgGreen, fgBlue, bgRed, bgGreen, bgBlue;
  uint8 newRed, newGreen, newBlue;
  /* add hidekazu */
  /* fgColor */
  frac16 red[16], green[16], blue[16];
  /* end hidekazu */

  *thePLUTPtr = (uint16 *)ALLOCMEM (PLUTSIZE_4BIT_CODEDCEL, MEMTYPE_CEL);
  if (*thePLUTPtr == NULL)
    {
      DIAGNOSTIC ("Cannot allocate storage for PLUT of 4Bit Cel.\n");
      return (ER_KTextBox_CannotMemAlloc);
    }

  fgRed = ((fgColor & 0x00FF0000) >> 19);
  fgGreen = ((fgColor & 0xFF00) >> 11);
  fgBlue = ((fgColor & 0xFF) >> 3);
  bgRed = ((bgColor & 0x00FF0000) >> 19);
  bgGreen = ((bgColor & 0xFF00) >> 11);
  bgBlue = ((bgColor & 0xFF) >> 3);

  makeElementPLUT (red, fgRed, bgRed);
  makeElementPLUT (green, fgGreen, bgGreen);
  makeElementPLUT (blue, fgBlue, bgBlue);

  /* (CHG) 93082 */
  if (fgColor == 0x00000000)
    {
      blue[15] = Convert32_F16 (2);
    }
  /* compensation */
  for (i = 0; i < 16; i++)
    {
      if (blue[i] < Convert32_F16 (2))
        blue[i] = Convert32_F16 (2);
    }

  /* set color to PLUT */
  for (i = 0; i < 16; i++)
    {
      newRed = (uint8)ConvertF16_32 (red[i]);
      newGreen = (uint8)ConvertF16_32 (green[i]);
      newBlue = (uint8)ConvertF16_32 (blue[i]);
      newColor = (newRed << 10) | (newGreen << 5) | newBlue;
      *(*thePLUTPtr + i) = newColor & 0x7FFE;
    }
  if (!(opaqueFlag & fg_Opaque))
    *(*thePLUTPtr + 15) = 0x0000;
  if (!(opaqueFlag & bg_Opaque))
    *(*thePLUTPtr + 0) = 0x0000;

  return (ER_KTextBox_NoError);
}

/*
        Convert color to PLUT.
*/
Int32
KMakePLUT2 (Color fgColor, Color bgColor, uint8 opaqueFlag,
            uint16 **thePLUTPtr)
{
  uint16 foreColor, backColor;
  uint8 red, green, blue, *byteptr;

  /* 1bit�Z����PLUT��ݒ肷�� */
  /* set foreground color */
  byteptr = (uint8 *)&fgColor;
  red = *(byteptr + 1);
  green = *(byteptr + 2);
  blue = *(byteptr + 3);
  foreColor
      = ((red & 0xf8) << 7) | ((green & 0xf8) << 2) | ((blue & 0xf8) >> 3);

  /* set background color */
  byteptr = (uint8 *)&bgColor;
  red = *(byteptr + 1);
  green = *(byteptr + 2);
  blue = *(byteptr + 3);
  backColor
      = ((red & 0xf8) << 7) | ((green & 0xf8) << 2) | ((blue & 0xf8) >> 3);

  *thePLUTPtr = (uint16 *)ALLOCMEM (PLUTSIZE_1BIT_CODEDCEL, MEMTYPE_CEL);
  if (*thePLUTPtr == NULL)
    {
      DIAGNOSTIC ("Cannot allocate storage for PLUT of 1Bit Cel.\n");
      return (ER_KTextBox_CannotMemAlloc);
    }
  *(*thePLUTPtr + 0) = backColor;
  *(*thePLUTPtr + 1) = foreColor;
  if (!(opaqueFlag & bg_Opaque))
    *(*thePLUTPtr + 0) = 0x0000;
  if (!(opaqueFlag & fg_Opaque))
    *(*thePLUTPtr + 1) = 0x0000;

  return (ER_KTextBox_NoError);
}

int32
OpenRomFile (KFont3DO *theFont, char *name)
{
  TagArg targs[3];
  static Item romfitem;
  static OpenFile *romfptr;
  uint32 romBlockSize = 0;

  romfitem = OpenDiskFile (name);
  romfptr = (OpenFile *)LookupItem (romfitem);
  theFont->romior = NULL;

  if (romfptr)
    {
      targs[0].ta_Tag = CREATEIOREQ_TAG_DEVICE;
      targs[0].ta_Arg = (void *)(romfptr->ofi.dev.n_Item);
      targs[1].ta_Tag = TAG_END;
      if ((theFont->romIOReqItem
           = CreateItem (MKNODEID (KERNELNODE, IOREQNODE), targs))
          < 0)
        {
          DIAGNOSTIC (("OpenRomFil:  Unable to create IOReq node (%ld)\n",
                       theFont->romIOReqItem));
          return -1;
        }
      romBlockSize = romfptr->ofi_File->fi_BlockSize;
      printf (" romBlockSize = %ld \n", romBlockSize);
      DIAGNOSTIC ((" ROM File Blocksize is %ld \n", romBlockSize));
    }
  theFont->logRomBlockSize = 0;
  while (((uint32)1 << theFont->logRomBlockSize) < romBlockSize)
    theFont->logRomBlockSize++;
  //	printf( " romBlockSize = %ld log = %ld \n", romBlockSize,
  //theFont->logRomBlockSize);
  theFont->romior = (IOReq *)LookupItem (theFont->romIOReqItem);
  memset (&theFont->romioInfo, 0, sizeof (theFont->romioInfo));
  theFont->romioInfo.ioi_Command = CMD_READ;
  theFont->romioInfo.ioi_Send.iob_Buffer = NULL;
  theFont->romioInfo.ioi_Send.iob_Len = 0;
  theFont->romioInfo.ioi_Offset = 0;
  return (0);
}

int32
ReadRom (KFont3DO *theFont, uint32 offset, char *buf, int32 len)
{
  int32 err;

  if (theFont->romIOReqItem == (Item)NULL)
    return -1;
  theFont->romioInfo.ioi_Offset = offset >> theFont->logRomBlockSize;
  theFont->romioInfo.ioi_Recv.iob_Buffer = buf;
  theFont->romioInfo.ioi_Recv.iob_Len = len;
  if (((err = DoIO (theFont->romIOReqItem, &theFont->romioInfo)) < 0)
      || (theFont->romior->io_Error != 0))
    {
      PrintfSysErr ((Item)theFont->romior->io_Error);
      DeleteItem (theFont->romIOReqItem);
      DIAGNOSTIC (
          ("ReadRom:  Error in DoIO offset = %ld ( %lx )\n", offset, offset));
      theFont->romIOReqItem = (Item)NULL;
      return -1;
    }
  return (len);
}

/*
        Read File T-Ver
*/

int
ReadFile_T (Stream *fs, Int32 size, long *buffer, long offset)
{
  long retval;
  char *cbuf;
  long ntoread, nleft;
  int retries = 4;

  retval = SeekDiskStream (fs, offset, SEEK_SET);
  nleft = size;
  cbuf = (char *)buffer;
  if (retval >= 0)
    while (nleft > 0)
      {
        ntoread = ((FS_CHUNKSIZE >= nleft) ? nleft : FS_CHUNKSIZE);
        retval = ReadDiskStream (fs, cbuf, ntoread);
        if (retval < 0)
          {
            retries--;
            if (retries == 0)
              break;
          }
        else
          {
            nleft -= ntoread;
            cbuf += ntoread;
            retries = 4;
          }
      }
  return (int)retval;
}

/*
        Read File E-Ver
*/

int
ReadFile_E (KFont3DO *theFont, Int32 size, long *buffer, long offset)
{
  if (theFont->fsRead)
    return ReadFile_T (theFont->font_fs, size, buffer, offset);
  else
    return ReadRom (theFont, offset, (char *)buffer, size);
}

/*
        Load FontDesc.
*/

Int32
KLoadFontDesc (char *name, KFontDesc *fontDesc, KFont3DO *theFont,
               Boolean fullRead, Boolean fsRead)
{
  char fileName[64];
  uint32 dataSize, tmpSize;
  char *dataPtr;
  Int32 error;
  Int32 fseekOffset;
  Int32 i;
  Int32 pSize;

  /***********************************************************************\
   *	�t�H���g�t�@�C���̃w�b�_�������[�h����	                    *
   \***********************************************************************/
  strcpy (fileName, (char *)name);

  /* KFontFileHeader��ǂݍ��� */
  dataSize = sizeof (KFontFileHeader);
  /* KFontFileHeader ��ǂݍ��ނ��߂̃��������m�ۂ��� */
  dataPtr = (char *)ALLOCMEM (dataSize, MEMTYPE_CEL);
  if (dataPtr == NULL)
    {
      DIAGNOSTIC ("Cannot allocate storage for font file KFontFileHeader.\n");
      return (ER_KTextBox_CannotMemAlloc);
    }
  fseekOffset = 0;
  theFont->fsRead = fsRead;

  /* �t�@�C���I�[�v�� */

  if (theFont->fsRead)
    {
      theFont->font_fs = OpenDiskStream (fileName, 0);
      if (theFont->font_fs == NULL)
        {
          FREEMEM (dataPtr, dataSize);
          DIAGNOSTIC ("Cannot open disk stream.\n");
          return (ER_KTextBox_CannotOpenDS);
        }
    }
  else
    {
      if (OpenRomFile (theFont, fileName) < 0)
        {
          FREEMEM (dataPtr, dataSize);
          DIAGNOSTIC ("Cannot open disk stream.\n");
          return (ER_KTextBox_CannotOpenDS);
        }
    }

  /* We are about to read the font file header */
  error = ReadFile_E (theFont, (int)dataSize, (Int32 *)dataPtr, fseekOffset);
  if (error < 0)
    {
      if (theFont->fsRead)
        CloseDiskStream (theFont->font_fs);
      FREEMEM (dataPtr, dataSize);
      DIAGNOSTIC ("Error reading font file KFontFileHeader.\n");
      return error;
    }

  fontDesc->header = (KFontFileHeader *)dataPtr;
  fseekOffset
      += sizeof (KFontFileHeader); /* MAC�̃t�H���g�t�@�C���̃w�b�_�̑傫��*/

  printf ("\n\n");
  printf ("chunk_ID   = '%c%c%c%c'\n",
          (fontDesc->header->chunk_ID & 0xff000000) >> 24,
          (fontDesc->header->chunk_ID & 0x00ff0000) >> 16,
          (fontDesc->header->chunk_ID & 0x0000ff00) >> 8,
          (fontDesc->header->chunk_ID & 0x000000ff) >> 0);

  printf ("chunk_size = %ld\n", fontDesc->header->chunk_size);
  printf ("version    = %ld\n\n", fontDesc->header->version);

  printf ("headerLength = %ld\n", fontDesc->header->headerLength);
  printf ("fontType     = %ld\n", fontDesc->header->fontType);
  printf ("fontSubType  = %d\n", fontDesc->header->fontSubType);
  printf ("fontFlags    = %ld\n\n", fontDesc->header->fontFlags);

  printf ("fontCharWidth   = %ld\n", fontDesc->header->fontCharWidth);
  printf ("fontCharHeight  = %ld\n", fontDesc->header->fontCharHeight);
  printf ("bitsPerPixel    = %ld\n", fontDesc->header->bitsPerPixel);
  printf ("grayScaleBit    = %ld\n", fontDesc->header->grayScaleBit);
  printf ("cornerWeightBit = %ld\n\n", fontDesc->header->cornerWeightBit);

  printf ("hmincode = %ld\n", fontDesc->header->hmincode);
  printf ("hmaxcode = %ld\n", fontDesc->header->hmaxcode);
  printf ("zmincode = %ld\n", fontDesc->header->zmincode);
  printf ("zmaxcode = %ld\n", fontDesc->header->zmaxcode);
  printf ("hcharNum = %ld\n", fontDesc->header->hcharNum);
  printf ("zcharNum = %ld\n\n", fontDesc->header->zcharNum);

  printf ("offsetHcodetbl     = %lx\n", fontDesc->header->offsetHcodetbl);
  printf ("offsetZcodetbl     = %lx\n", fontDesc->header->offsetZcodetbl);
  printf ("offsetHwidthtbl    = %lx\n", fontDesc->header->offsetHwidthtbl);
  printf ("offsetZwidthtbl    = %lx\n", fontDesc->header->offsetZwidthtbl);
  printf ("offsetHbitmap      = %lx\n", fontDesc->header->offsetHbitmap);
  printf ("offsetZbitmap      = %lx\n", fontDesc->header->offsetZbitmap);
  printf ("offsetCopyright    = %lx\n", fontDesc->header->offsetCopyright);
  printf ("offsetTypeFaceName = %lx\n\n",
          fontDesc->header->offsetTypeFaceName);

  printf ("revision     = %lx\n", fontDesc->header->revision);
  printf ("compression  = %lx\n", fontDesc->header->compression);
  printf ("charCode     = %lx\n", fontDesc->header->charCode);

  /***********************************************************************
   *	Setup none full read *
   ***********************************************************************/

  theFont->font_FullRead = fullRead;
  theFont->font_HFSeekOffset = fontDesc->header->offsetHbitmap;
  theFont->font_ZFSeekOffset = fontDesc->header->offsetZbitmap;

  /***********************************************************************
   *	�t�H���g�t�@�C���̃w�b�_�����`�F�b�N���� *
   ***********************************************************************/

  if ((fontDesc->header->fontCharWidth) == 0)
    {
      if (theFont->fsRead)
        CloseDiskStream (theFont->font_fs);
      FREEMEM (fontDesc->header, sizeof (KFontFileHeader));
      DIAGNOSTIC ("Error fontCharWidth is 0.\n");
      return (ER_KTextBox_BadFontFile);
    }

  if (fontDesc->header->fontCharHeight == 0)
    {
      if (theFont->fsRead)
        CloseDiskStream (theFont->font_fs);
      FREEMEM (fontDesc->header, sizeof (KFontFileHeader));
      DIAGNOSTIC ("Error fontCharHeight is 0.\n");
      return (ER_KTextBox_BadFontFile);
    }

  if (fontDesc->header->bitsPerPixel == 0)
    {
      if (theFont->fsRead)
        CloseDiskStream (theFont->font_fs);
      FREEMEM (fontDesc->header, sizeof (KFontFileHeader));
      DIAGNOSTIC ("Error bitsPerPixel is 0.\n");
      return (ER_KTextBox_BadFontFile);
    }

  if (fontDesc->header->grayScaleBit == 0)
    {
      if (theFont->fsRead)
        CloseDiskStream (theFont->font_fs);
      FREEMEM (fontDesc->header, sizeof (KFontFileHeader));
      DIAGNOSTIC ("Error grayScaleBit is 0.\n");
      return (ER_KTextBox_BadFontFile);
    }

  if ((fontDesc->header->hcharNum == 0) && (fontDesc->header->zcharNum == 0))
    {
      if (theFont->fsRead)
        CloseDiskStream (theFont->font_fs);
      FREEMEM (fontDesc->header, sizeof (KFontFileHeader));
      DIAGNOSTIC ("Error hcharNum and zcharNum is 0.\n");
      return (ER_KTextBox_BadFontFile);
    }

  if (((fontDesc->header->hmincode == 0) || (fontDesc->header->hmaxcode == 0))
      && ((fontDesc->header->zmincode == 0)
          || (fontDesc->header->zmaxcode == 0)))
    {
      if (theFont->fsRead)
        CloseDiskStream (theFont->font_fs);
      FREEMEM (fontDesc->header, sizeof (KFontFileHeader));
      DIAGNOSTIC ("Error minmaxcode is 0.\n");
      return (ER_KTextBox_BadFontFile);
    }
  /*
      if( (fontDesc->header->offsetHcodetbl < 0) ||
     (fontDesc->header->offsetZcodetbl < 0)){ DIAGNOSTIC("Error codetbl is less
     than zero.\n"); return (ER_KTextBox_BadFontFile);
      }

      if( (fontDesc->header->offsetHwidthtbl < 0) ||
     (fontDesc->header->offsetZwidthtbl < 0)){ DIAGNOSTIC("Error widthtbl is
     less than zero.\n"); return (ER_KTextBox_BadFontFile);
      }
  */
  if (fontDesc->header->hcharNum > 0)
    {
      /***********************************************************************\
       *	���p�̃R�[�h�e�[�u���ƕ������e�[�u�������[�h���� *
       \***********************************************************************/
      if (fontDesc->header->offsetHcodetbl != 0)
        { // <HPP> for our sake this is ignored
          /* ���p�̃R�[�h�e�[�u���p�̃��������m�ۂ���(�e���|����) */
          dataSize = fontDesc->header->hcharNum;
          tmpSize = dataSize;
          dataPtr = (char *)ALLOCMEM (dataSize, MEMTYPE_CEL);
          if (dataPtr == NULL)
            {
              if (theFont->fsRead)
                CloseDiskStream (theFont->font_fs);
              FREEMEM (fontDesc->header, sizeof (KFontFileHeader));
              DIAGNOSTIC ("Cannot allocate storage for font file hcodeTbl.\n");
              return (ER_KTextBox_CannotMemAlloc);
            }
          fseekOffset = fontDesc->header->offsetHcodetbl;
          /* ���p�̃R�[�h�e�[�u�� ��ǂݍ��� */
          error = ReadFile_E (theFont, (int)dataSize, (Int32 *)dataPtr,
                              fseekOffset);
          if (error < 0)
            {
              if (theFont->fsRead)
                CloseDiskStream (theFont->font_fs);
              FREEMEM (fontDesc->header, sizeof (KFontFileHeader));
              DIAGNOSTIC ("Error reading font file hcodeTbl.\n");
              return error;
            }
          fontDesc->hcodeTbl = (char *)dataPtr;

          /* ���p�̃R�[�h�e�[�u���p�̃��������m�ۂ��� */
          dataSize = fontDesc->header->hcharNum * sizeof (uint16);
          dataPtr = (char *)ALLOCMEM (dataSize, MEMTYPE_CEL);
          if (dataPtr == NULL)
            {
              if (theFont->fsRead)
                CloseDiskStream (theFont->font_fs);
              FREEMEM (fontDesc->hcodeTbl, tmpSize);
              FREEMEM (fontDesc->header, sizeof (KFontFileHeader));
              DIAGNOSTIC ("Cannot allocate storage for font file "
                          "font_Hinfo.codeTbl.\n");
              return (ER_KTextBox_CannotMemAlloc);
            }
          theFont->font_Hinfo.codeTbl = (uint16 *)dataPtr;
          /* KFont3DO�\\���̂ɔ��p�R�[�h�e�[�u�� ���Z�b�g���� */
          for (i = 0; i < fontDesc->header->hcharNum; i++)
            {
              theFont->font_Hinfo.codeTbl[i] = (uint16)fontDesc->hcodeTbl[i];
            }
          FREEMEM (fontDesc->hcodeTbl, tmpSize);
          fseekOffset += tmpSize;
        }

      if (fontDesc->header->offsetHwidthtbl != 0)
        { // <HPP> for our sake this is ignored
          /* ���p�̕������e�[�u���p�̃��������m�ۂ��� */
          dataSize = fontDesc->header->hcharNum;
          tmpSize = dataSize;
          dataPtr = (char *)ALLOCMEM (dataSize, MEMTYPE_CEL);
          if (dataPtr == NULL)
            {
              if (fontDesc->header->offsetHcodetbl != 0)
                {
                  FREEMEM (theFont->font_Hinfo.codeTbl,
                           fontDesc->header->hcharNum * sizeof (uint16));
                }
              if (theFont->fsRead)
                CloseDiskStream (theFont->font_fs);
              FREEMEM (fontDesc->header, sizeof (KFontFileHeader));
              DIAGNOSTIC (
                  "Cannot allocate storage for font file hwidthTbl.\n");
              return (ER_KTextBox_CannotMemAlloc);
            }
          fseekOffset = fontDesc->header->offsetHwidthtbl;
          /* ���p�̕������e�[�u�� ��ǂݍ��� */
          error = ReadFile_E (theFont, (int)dataSize, (Int32 *)dataPtr,
                              fseekOffset);
          if (error < 0)
            {
              if (fontDesc->header->offsetHcodetbl != 0)
                {
                  FREEMEM (theFont->font_Hinfo.codeTbl,
                           fontDesc->header->hcharNum * sizeof (uint16));
                }
              if (theFont->fsRead)
                CloseDiskStream (theFont->font_fs);
              FREEMEM (dataPtr, tmpSize);
              FREEMEM (fontDesc->header, sizeof (KFontFileHeader));
              DIAGNOSTIC ("Error reading font file hwidthTbl.\n");
              return error;
            }
          /* KFont3DO�\\���̂ɕ������e�[�u�� ���Z�b�g���� */
          theFont->font_Hinfo.widthTbl = (uint8 *)dataPtr;
          fseekOffset += dataSize;
        }
    }

  if (fontDesc->header->zcharNum > 0)
    {
      /***********************************************************************\
       *	�S�p�̃R�[�h�e�[�u���ƕ������e�[�u�������[�h���� *
       \***********************************************************************/
      if (fontDesc->header->offsetZcodetbl != 0)
        { // <HPP> for our sake this is ignored
          /* �S�p�̃R�[�h�e�[�u���p�̃��������m�ۂ���(�e���|����) */
          dataSize = fontDesc->header->zcharNum * sizeof (uint16);
          dataPtr = (char *)ALLOCMEM (dataSize, MEMTYPE_CEL);
          if (dataPtr == NULL)
            {
              if (fontDesc->header->hcharNum > 0)
                {
                  if (fontDesc->header->offsetHcodetbl != 0)
                    {
                      FREEMEM (theFont->font_Hinfo.codeTbl,
                               fontDesc->header->hcharNum * sizeof (uint16));
                    }
                  if (fontDesc->header->offsetHwidthtbl != 0)
                    {
                      FREEMEM (theFont->font_Hinfo.widthTbl,
                               fontDesc->header->hcharNum);
                    }
                }
              if (theFont->fsRead)
                CloseDiskStream (theFont->font_fs);
              FREEMEM (fontDesc->header, sizeof (KFontFileHeader));
              DIAGNOSTIC ("Cannot allocate storage for font file zcodeTbl.\n");
              return (ER_KTextBox_CannotMemAlloc);
            }
          fseekOffset = fontDesc->header->offsetZcodetbl;
          /* �S�p�̃R�[�h�e�[�u�� ��ǂݍ��� */
          error = ReadFile_E (theFont, (int)dataSize, (Int32 *)dataPtr,
                              fseekOffset);
          if (error < 0)
            {
              if (fontDesc->header->hcharNum > 0)
                {
                  if (fontDesc->header->offsetHcodetbl != 0)
                    {
                      FREEMEM (theFont->font_Hinfo.codeTbl,
                               fontDesc->header->hcharNum * sizeof (uint16));
                    }
                  if (fontDesc->header->offsetHwidthtbl != 0)
                    {
                      FREEMEM (theFont->font_Hinfo.widthTbl,
                               fontDesc->header->hcharNum);
                    }
                }
              if (theFont->fsRead)
                CloseDiskStream (theFont->font_fs);
              FREEMEM (fontDesc->header, sizeof (KFontFileHeader));
              FREEMEM (dataPtr, dataSize);
              DIAGNOSTIC ("Error reading font file zcodeTbl.\n");
              return error;
            }
          /* KFont3DO�\\���̂ɑS�p�R�[�h�e�[�u�� ���Z�b�g���� */
          theFont->font_Zinfo.codeTbl = (uint16 *)dataPtr;

          fseekOffset += dataSize;
        }

      if (fontDesc->header->offsetZwidthtbl != 0)
        { // <HPP> for our sake this is ignored
          /* �S�p�̕������e�[�u���p�̃��������m�ۂ��� */
          dataSize = fontDesc->header->zcharNum;
          dataPtr = (char *)ALLOCMEM (dataSize, MEMTYPE_CEL);
          if (dataPtr == NULL)
            {
              if (fontDesc->header->hcharNum > 0)
                {
                  if (fontDesc->header->offsetHcodetbl != 0)
                    {
                      FREEMEM (theFont->font_Hinfo.codeTbl,
                               fontDesc->header->hcharNum * sizeof (uint16));
                    }
                  if (fontDesc->header->offsetHwidthtbl != 0)
                    {
                      FREEMEM (theFont->font_Hinfo.widthTbl,
                               fontDesc->header->hcharNum);
                    }
                }
              if (fontDesc->header->offsetZcodetbl != 0)
                {
                  FREEMEM (theFont->font_Zinfo.codeTbl,
                           fontDesc->header->zcharNum * sizeof (uint16));
                }
              if (theFont->fsRead)
                CloseDiskStream (theFont->font_fs);
              FREEMEM (fontDesc->header, sizeof (KFontFileHeader));
              DIAGNOSTIC (
                  "Cannot allocate storage for font file zwidthTbl.\n");
              return (ER_KTextBox_CannotMemAlloc);
            }
          fseekOffset = fontDesc->header->offsetZwidthtbl;
          /* �S�p�̕������e�[�u�� ��ǂݍ��� */
          error = ReadFile_E (theFont, (int)dataSize, (Int32 *)dataPtr,
                              fseekOffset);
          if (error < 0)
            {
              if (fontDesc->header->hcharNum > 0)
                {
                  if (fontDesc->header->offsetHcodetbl != 0)
                    {
                      FREEMEM (theFont->font_Hinfo.codeTbl,
                               fontDesc->header->hcharNum * sizeof (uint16));
                    }
                  if (fontDesc->header->offsetHwidthtbl != 0)
                    {
                      FREEMEM (theFont->font_Hinfo.widthTbl,
                               fontDesc->header->hcharNum);
                    }
                }
              if (fontDesc->header->offsetZcodetbl != 0)
                {
                  FREEMEM (theFont->font_Zinfo.codeTbl,
                           fontDesc->header->zcharNum * sizeof (uint16));
                }
              if (theFont->fsRead)
                CloseDiskStream (theFont->font_fs);
              FREEMEM (fontDesc->header, sizeof (KFontFileHeader));
              DIAGNOSTIC ("Error reading font file hwidthTbl.\n");
              return error;
            }
          /* KFont3DO�\\���̂ɕ������e�[�u�� ���Z�b�g���� */
          theFont->font_Zinfo.widthTbl = (uint8 *)dataPtr;
          fseekOffset += dataSize;
        }
    }

  if (fontDesc->header->hcharNum > 0)
    {
      /* ���p�̃t�H���g�f�[�^�p�̃��������m�ۂ��� */
      pSize = fontDesc->header->fontCharWidth
              * fontDesc->header->fontCharHeight
              * fontDesc->header->bitsPerPixel / 2;
      theFont->font_Hinfo.oneCharSize
          = (pSize + (((pSize + 31) / 32) * 32 - pSize)) / 8;

      if (theFont->font_FullRead)
        { // <HPP 03-08-94>
          dataSize
              = fontDesc->header->hcharNum * theFont->font_Hinfo.oneCharSize;
          dataPtr = (char *)ALLOCMEM (dataSize, MEMTYPE_CEL);
          if (dataPtr == NULL)
            {
              if (fontDesc->header->offsetHcodetbl != 0)
                {
                  FREEMEM (theFont->font_Hinfo.codeTbl,
                           fontDesc->header->hcharNum * sizeof (uint16));
                }
              if (fontDesc->header->offsetHwidthtbl != 0)
                {
                  FREEMEM (theFont->font_Hinfo.widthTbl,
                           fontDesc->header->hcharNum);
                }
              if (fontDesc->header->zcharNum > 0)
                {
                  if (fontDesc->header->offsetZcodetbl != 0)
                    {
                      FREEMEM (theFont->font_Zinfo.codeTbl,
                               fontDesc->header->zcharNum * sizeof (uint16));
                    }
                  if (fontDesc->header->offsetZwidthtbl != 0)
                    {
                      FREEMEM (theFont->font_Zinfo.widthTbl,
                               fontDesc->header->zcharNum);
                    }
                }
              if (theFont->fsRead)
                CloseDiskStream (theFont->font_fs);
              FREEMEM (fontDesc->header, sizeof (KFontFileHeader));
              DIAGNOSTIC (
                  "Cannot allocate storage for font file hcharData.\n");
              return (ER_KTextBox_CannotMemAlloc);
            }

          /* ���p�̃t�H���g�f�[�^ ��ǂݍ��� */
          error = ReadFile_E (theFont, (int)dataSize, (Int32 *)dataPtr,
                              fseekOffset);
          if (error < 0)
            {
              if (fontDesc->header->offsetHcodetbl != 0)
                {
                  FREEMEM (theFont->font_Hinfo.codeTbl,
                           fontDesc->header->hcharNum * sizeof (uint16));
                }
              if (fontDesc->header->offsetHwidthtbl != 0)
                {
                  FREEMEM (theFont->font_Hinfo.widthTbl,
                           fontDesc->header->hcharNum);
                }
              if (fontDesc->header->zcharNum > 0)
                {
                  if (fontDesc->header->offsetZcodetbl != 0)
                    {
                      FREEMEM (theFont->font_Zinfo.codeTbl,
                               fontDesc->header->zcharNum * sizeof (uint16));
                    }
                  if (fontDesc->header->offsetZwidthtbl != 0)
                    {
                      FREEMEM (theFont->font_Zinfo.widthTbl,
                               fontDesc->header->zcharNum);
                    }
                }
              if (theFont->fsRead)
                CloseDiskStream (theFont->font_fs);
              FREEMEM (fontDesc->header, sizeof (KFontFileHeader));
              DIAGNOSTIC ("Error reading font file hcharData.\n");
              return error;
            }

          theFont->font_Hinfo.charData = (char *)dataPtr;
          fseekOffset += dataSize;
        }
    }

  if (fontDesc->header->zcharNum > 0)
    {
      /* �S�p�̃t�H���g�f�[�^�p�̃��������m�ۂ��� */
      pSize = fontDesc->header->fontCharHeight
              * fontDesc->header->fontCharWidth
              * fontDesc->header->bitsPerPixel;
      theFont->font_Zinfo.oneCharSize
          = (pSize + (((pSize + 31) / 32) * 32 - pSize)) / 8;

      if (theFont->font_FullRead)
        { // <HPP 03-08-94>

          dataSize
              = fontDesc->header->zcharNum * theFont->font_Zinfo.oneCharSize;
          dataPtr = (char *)ALLOCMEM (dataSize, MEMTYPE_CEL);
          if (dataPtr == NULL)
            {
              if (fontDesc->header->hcharNum > 0)
                {
                  if (fontDesc->header->offsetHcodetbl != 0)
                    {
                      FREEMEM (theFont->font_Hinfo.codeTbl,
                               fontDesc->header->hcharNum * sizeof (uint16));
                    }
                  if (fontDesc->header->offsetHwidthtbl != 0)
                    {
                      FREEMEM (theFont->font_Hinfo.widthTbl,
                               fontDesc->header->hcharNum);
                    }
                  FREEMEM (theFont->font_Hinfo.charData,
                           fontDesc->header->hcharNum
                               * theFont->font_Hinfo.oneCharSize);
                }
              if (fontDesc->header->offsetZcodetbl != 0)
                {
                  FREEMEM (theFont->font_Zinfo.codeTbl,
                           fontDesc->header->zcharNum * sizeof (uint16));
                }
              if (fontDesc->header->offsetZwidthtbl != 0)
                {
                  FREEMEM (theFont->font_Zinfo.widthTbl,
                           fontDesc->header->zcharNum);
                }
              if (theFont->fsRead)
                CloseDiskStream (theFont->font_fs);
              FREEMEM (fontDesc->header, sizeof (KFontFileHeader));
              DIAGNOSTIC (
                  "Cannot allocate storage for font file zcharData.\n");
              return (ER_KTextBox_CannotMemAlloc);
            }

          /* �S�p�̃t�H���g�f�[�^ ��ǂݍ��� */
          error = ReadFile_E (theFont, (int)dataSize, (Int32 *)dataPtr,
                              fseekOffset);
          if (error < 0)
            {
              if (fontDesc->header->hcharNum > 0)
                {
                  if (fontDesc->header->offsetHcodetbl != 0)
                    {
                      FREEMEM (theFont->font_Hinfo.codeTbl,
                               fontDesc->header->hcharNum * sizeof (uint16));
                    }
                  if (fontDesc->header->offsetHwidthtbl != 0)
                    {
                      FREEMEM (theFont->font_Hinfo.widthTbl,
                               fontDesc->header->hcharNum);
                    }
                  FREEMEM (theFont->font_Hinfo.charData,
                           fontDesc->header->hcharNum
                               * theFont->font_Hinfo.oneCharSize);
                }
              if (fontDesc->header->offsetZcodetbl != 0)
                {
                  FREEMEM (theFont->font_Zinfo.codeTbl,
                           fontDesc->header->zcharNum * sizeof (uint16));
                }
              if (fontDesc->header->offsetZwidthtbl != 0)
                {
                  FREEMEM (theFont->font_Zinfo.widthTbl,
                           fontDesc->header->zcharNum);
                }
              if (theFont->fsRead)
                CloseDiskStream (theFont->font_fs);
              FREEMEM (theFont->font_Zinfo.charData,
                       fontDesc->header->zcharNum
                           * theFont->font_Zinfo.oneCharSize);
              FREEMEM (fontDesc->header, sizeof (KFontFileHeader));
              DIAGNOSTIC ("Error reading font file zcharData.\n");
              return error;
            }

          theFont->font_Zinfo.charData = (char *)dataPtr;
          if (theFont->fsRead)
            CloseDiskStream (theFont->font_fs);
        }
    }
  return (ER_KTextBox_NoError);
}

/*
        Load Font.
*/

Int32
KLoadFont (char *name, KFont3DO *theFont, Boolean fullRead)
{

  KFontDesc theFontDesc;
  Int32 ret;

  /***********************************************************************\
   *	Font3DO�\\���̂�����������			                     *
   \***********************************************************************/

  theFont->font_Gpp = 0;
  theFont->font_Bpp = 0;

  theFont->font_Hinfo.nChars = 0;
  theFont->font_Hinfo.minCode = 0;
  theFont->font_Hinfo.maxCode = 0;
  theFont->font_Hinfo.charHeight = 0;
  theFont->font_Hinfo.charWidth = 0;
  theFont->font_Hinfo.oneCharSize = 0;
  theFont->font_Hinfo.codeTbl = (uint16 *)0;
  theFont->font_Hinfo.widthTbl = (uint8 *)0;
  theFont->font_Hinfo.charData = (char *)0;

  theFont->font_Zinfo.nChars = 0;
  theFont->font_Zinfo.minCode = 0;
  theFont->font_Zinfo.maxCode = 0;
  theFont->font_Zinfo.charHeight = 0;
  theFont->font_Zinfo.charWidth = 0;
  theFont->font_Zinfo.oneCharSize = 0;
  theFont->font_Zinfo.codeTbl = (uint16 *)0;
  theFont->font_Zinfo.widthTbl = (uint8 *)0;
  theFont->font_Zinfo.charData = (char *)0;

  theFont->maxCharSize = 0;

  /***********************************************************************\
   *	KFontDesc�\\���̂փf�[�^�����[�h����		                     *
   \***********************************************************************/

  ret = KLoadFontDesc (name, &theFontDesc, theFont, fullRead, true);
  if (ret < 0)
    return (ret);

  /***********************************************************************\
   *	�c��̃f�[�^���Z�b�g����			                    *
   \***********************************************************************/

  theFont->font_Gpp = (uint8)theFontDesc.header->grayScaleBit;
  theFont->font_Bpp = (uint8)theFontDesc.header->bitsPerPixel;
  if (theFontDesc.header->hcharNum > 0)
    {
      theFont->font_Hinfo.nChars = (uint16)theFontDesc.header->hcharNum;
      theFont->font_Hinfo.minCode = (uint16)theFontDesc.header->hmincode;
      theFont->font_Hinfo.maxCode = (uint16)theFontDesc.header->hmaxcode;
      theFont->font_Hinfo.charHeight
          = (uint8)theFontDesc.header->fontCharHeight;
      theFont->font_Hinfo.charWidth
          = (uint8)theFontDesc.header->fontCharWidth / 2;
      if (theFont->font_Hinfo.oneCharSize > theFont->maxCharSize)
        theFont->maxCharSize = theFont->font_Hinfo.oneCharSize;
    }

  if (theFontDesc.header->zcharNum > 0)
    {
      theFont->font_Zinfo.nChars = (uint16)theFontDesc.header->zcharNum;
      theFont->font_Zinfo.minCode = (uint16)theFontDesc.header->zmincode;
      theFont->font_Zinfo.maxCode = (uint16)theFontDesc.header->zmaxcode;
      theFont->font_Zinfo.charHeight
          = (uint8)theFontDesc.header->fontCharHeight;
      theFont->font_Zinfo.charWidth = (uint8)theFontDesc.header->fontCharWidth;
      if (theFont->font_Zinfo.oneCharSize > theFont->maxCharSize)
        theFont->maxCharSize = theFont->font_Zinfo.oneCharSize;
    }

  return (ER_KTextBox_NoError);
}

/*
        Convert color to PLUT.
*/
Int32
KFreeFont (KFont3DO *theFont)
{

  /* ���p�̃R�[�h�e�[�u���ƕ��e�[�u���ƃs�N�Z���f�[�^����������� */
  if (theFont->font_Hinfo.codeTbl != NULL)
    {
      FREEMEM (theFont->font_Hinfo.codeTbl,
               (Int32)theFont->font_Hinfo.nChars * sizeof (uint16));
      theFont->font_Hinfo.codeTbl = (uint16 *)0;
    }
  if (theFont->font_Hinfo.widthTbl != NULL)
    {
      FREEMEM (theFont->font_Hinfo.widthTbl,
               (Int32)theFont->font_Hinfo.nChars * sizeof (uint16));
      theFont->font_Hinfo.widthTbl = (uint8 *)0;
    }
  FREEMEM (theFont->font_Hinfo.charData,
           theFont->font_Hinfo.nChars * theFont->font_Hinfo.oneCharSize);
  theFont->font_Hinfo.charData = (char *)0;

  /* �S�p�̃R�[�h�e�[�u���ƕ��e�[�u���ƃs�N�Z���f�[�^����������� */
  if (theFont->font_Zinfo.codeTbl != NULL)
    {
      FREEMEM (theFont->font_Zinfo.codeTbl,
               (Int32)theFont->font_Zinfo.nChars * sizeof (uint16));
      theFont->font_Zinfo.codeTbl = (uint16 *)0;
    }
  if (theFont->font_Zinfo.widthTbl != NULL)
    {
      FREEMEM (theFont->font_Zinfo.widthTbl,
               (Int32)theFont->font_Zinfo.nChars * sizeof (uint16));
      theFont->font_Zinfo.widthTbl = (uint8 *)0;
    }
  FREEMEM (theFont->font_Zinfo.charData,
           theFont->font_Zinfo.nChars * theFont->font_Zinfo.oneCharSize);
  theFont->font_Zinfo.charData = (char *)0;

  return (ER_KTextBox_NoError);
}

Int32
KFreeCel (CCB *theCCB)
{
  Int32 boxRowBytes;
  Int32 boxHeight;
  Int32 pixelDepth;

  if (theCCB == NULL)
    {
      DIAGNOSTIC ("theCCB is NULL.\n");
      return (ER_KTextBox_BadParameter);
    }

  /* �Z���̐[�������߂�*/
  pixelDepth = theCCB->ccb_PRE0 & 0x07;
  /* PLUT����L���郁�������������B */
  if (pixelDepth == PRE0_BPP_4)
    {
      /* �S�r�b�g�Z���̏ꍇ */
      FREEMEM (theCCB->ccb_PLUTPtr, PLUTSIZE_4BIT_CODEDCEL);
      boxRowBytes = ((((theCCB->ccb_Width + 1) / 2) + 3) / 4) * 4;
    }
  else if (pixelDepth == PRE0_BPP_1)
    {
      /* �P�r�b�g�Z���̏ꍇ */
      FREEMEM (theCCB->ccb_PLUTPtr, PLUTSIZE_1BIT_CODEDCEL);
      boxRowBytes = ((((theCCB->ccb_Width + 7) / 8) + 3) / 4) * 4;
    }
  else
    {
      /* �\\�����Ă��Ȃ��Z�����^����ꂽ */
      DIAGNOSTIC ("Bad cel type.\n");
      return (ER_KTextBox_BadParameter);
    }

  if (boxRowBytes < 8)
    boxRowBytes = 8;
  boxHeight = theCCB->ccb_Height;
  /* �s�N�Z���f�[�^����L���郁�������������B */
#if RED_HARDWARE
  FREEMEM (theCCB->ccb_CelData, boxHeight * boxRowBytes);
#else
  FREEMEM (theCCB->ccb_SourcePtr, boxHeight * boxRowBytes);
#endif
  /* CCB����L���郁�������������B */
  FREEMEM (theCCB, sizeof (CCB));

  /* �������̉���ɐ��� */
  return (ER_KTextBox_NoError);
}

Int32
KConvertText (uint8 *src, KCharRec **dst, Int32 len)
{
  Int32 dataSize;
  Int32 i;

  if (*src == '\0')
    {
      DIAGNOSTIC ("Error: Target text is NULL.\n");
      return (ER_KTextBox_BadParameter);
    }
  if (len <= 0)
    {
      DIAGNOSTIC ("Error: Convert len is less than zero.\n");
      return (ER_KTextBox_BadParameter);
    }

  dataSize = sizeof (KCharRec) * len;
  *dst = (KCharRec *)ALLOCMEM (dataSize, MEMTYPE_CEL);
  if (*dst == NULL)
    {
      DIAGNOSTIC ("Cannot allocate storage for convert text.\n");
      return (ER_KTextBox_CannotMemAlloc);
    }

  for (i = 0; i < len; i++)
    {
      if (isKanji (*src))
        {
          /* �V�t�gJIS�R�[�h�̑�P�o�C�g�ł��� */
          (*dst + i)->Value = (*src++ << 8);
          if (isKanji2 (*src))
            {
              /* �V�t�gJIS�R�[�h�̑�Q�o�C�g�ł��� */
              (*dst + i)->Value += *src++;
            }
          else
            {
#if DEBUG
              kprintf ("ERROR:the character(%x) is not a shift jis code.\n",
                       *(Int16 **)(src - 1));
#endif
              return i;
            }
        }
      else if (isANK (*src) || (*src == LF) || (*src == CR))
        {
          /* ASCII�R�[�h�ł��� */
          (*dst + i)->Value = *src++;
        }
      else
        {
#if DEBUG
          kprintf ("ERROR:the character(%x) is not a shift jis code.\n", *src);
#endif
          return i;
        }
    }

  return i;
}

Int32
KTextLength (uint8 *theText)
{
  Int32 n = 0;

  if (*theText == '\0')
    {
      DIAGNOSTIC ("Error: Terget Text is NULL.\n");
      return (0);
    }

  for (; *theText != '\0'; theText++)
    {
      if (isKanji (*theText))
        {
          /* �V�t�gJIS�R�[�h�̑�P�o�C�g�ł��� */
          theText++;
          if (isKanji2 (*theText))
            {
              /* �V�t�gJIS�R�[�h�̑�Q�o�C�g�ł��� */
              n++;
            }
          else
            {
#if DEBUG
              kprintf ("ERROR:the character(%x) is not a shift jis code.\n",
                       *(uint16 *)(--theText));
#endif
              return n;
            }
        }
      else if (isANK (*theText) || (*theText == LF) || (*theText == CR))
        {
          /* ASCII�R�[�h�ł��� */
          n++;
        }
      else
        {
#if DEBUG
          kprintf ("ERROR:the character(%x) is not a shift jis code.\n",
                   *theText);
#endif
          return n;
        }
    }

  return n;
}

Int32
KTextLength2 (uint8 *theText, Int32 nbytes)
{
  Int32 n = 0;

  if (*theText == '\0')
    {
      DIAGNOSTIC ("Error: Terget Text is NULL.\n");
      return (0);
    }
  if (nbytes > strlen (theText))
    {
      DIAGNOSTIC ("Error: nbytes is too large.\n");
      return (0);
    }

  for (; nbytes; theText++, nbytes--)
    {
      if (isKanji (*theText))
        {
          /* �V�t�gJIS�R�[�h�̑�P�o�C�g�ł��� */
          theText++;
          nbytes--;
          if (isKanji2 (*theText))
            {
              /* �V�t�gJIS�R�[�h�̑�Q�o�C�g�ł��� */
              n++;
            }
          else
            {
#if DEBUG
              kprintf ("ERROR:the character(%x) is not a shift jis code.\n",
                       *(uint16 *)(--theText));
#endif
              return n;
            }
        }
      else if (isANK (*theText) || (*theText == LF) || (*theText == CR))
        {
          /* ASCII�R�[�h�ł��� */
          n++;
        }
      else
        {
#if DEBUG
          kprintf ("ERROR:the character(%x) is not a shift jis code.\n",
                   *theText);
#endif
          return n;
        }
    }

  return n;
}

Int32
KsjisToiCode (Int32 c)
{
  Int32 high;
  Int32 low;
  Int32 iCode = 0;

  c &= 0xffff;
  high = ((c & 0xFF00) >> 8);
  low = (c & 0x00FF);
  if (!(SJIS_KANJI_HIGH (high) && SJIS_KANJI_LOW (low)))
    {
#if DEBUG
      kprintf ("ERROR:the character(%x) is not a shift jis code.\n", c);
#endif
      return (ER_KTextBox_BadCharCode);
    }

  if (!SJIS_KANJI_LEVEL2 (c))
    {
      if ((0x8140 <= c) && (c <= 0x84fc))
        {
          if (low <= 0x7e)
            {
              iCode = (high - 0x81) * 188 + low - 64; // - 0x40
            }
          else if (0x80 <= low)
            {
              iCode = (high - 0x81) * 188 + low - 65; // -0x7f + 62
            }
        }
      else if ((0x889f <= c) && (c <= 0x88fc))
        {
          iCode = c - 34223; // - 0x889e + 751
        }
      else if ((0x8940 <= c) && (c <= 0x987e))
        {
          if (low <= 0x7e)
            {
              iCode = (high - 0x89) * 188 + low + 782; // -0x3f + 845
            }
          else if (0x80 <= low)
            {
              iCode = (high - 0x89) * 188 + low + 781; // -0x7f + 845 + 63;
            }
        }
    }
  else
    {
      if ((0x989f <= c) && (c <= 0x98fc))
        {
          iCode = NUM_KANJI_LEVEL1 + low - 159; // -1 - 0x9e;
        }
      else if ((0x9940 <= c) && (c <= 0x9ffc))
        {
          if (low <= 0x7e)
            {
              iCode = (high - 0x99) * 188 + low
                      + 3790; // + NUM_KANJI_LEVEL1 -1 +94 - 0x3f;
            }
          else if (0x80 <= low)
            {
              iCode = (high - 0x99) * 188 + low
                      + 3789; // + NUM_KANJI_LEVEL1 -1 +94 - 0x7f + 63;
            }
        }
      else if ((0xe040 <= c) && (c <= 0xeafc))
        {
          if (low <= 0x7e)
            {
              iCode = (high - 0xe0) * 188 + low
                      + 5106; // + NUM_KANJI_LEVEL1-1 +1410 - 0x3f;
            }
          else if (0x80 <= low)
            {
              iCode = (high - 0xe0) * 188 + low
                      + 5105; // + NUM_KANJI_LEVEL1 -1 +1410 - 0x7f + 63;
            }
        }
    }

  return (iCode);
}

Int32
KankToiCode (Int32 c)
{
  if (!isANK (c))
    {
#if DEBUG
      kprintf ("ERROR:the character(%x) is not a ANK code.\n", c);
#endif
      return (ER_KTextBox_BadCharCode);
    }

  if ((0x20 <= (c) && (c) <= 0x7E))
    {
      return (c - SPACE);
    }
  else if (isKana (c))
    {
      return (c - KANA_START + (DEL - SPACE));
    }
  return (ER_KTextBox_BadCharCode);
}

CCB *
KCreate4BitCodedCel (uint32 numRows, uint32 width, Int32 rowBytes)
{
  CCB *newCCB;
  Int32 scanlinesizeinwords;
  char *celData;
  Int32 celDataSize;

  newCCB = (CCB *)ALLOCMEM (sizeof (CCB), MEMTYPE_CEL);
  if (newCCB == NULL)
    {
      DIAGNOSTIC ("Cannot allocate storage for font ccb\n");
      return NULL;
    }
  /* Fill in the values */
  memset (newCCB, 0, sizeof (CCB));
  newCCB->ccb_HDX = (1 << 20);
  newCCB->ccb_VDY = (1 << 16);
  newCCB->ccb_Width = width;
  newCCB->ccb_Height = numRows;
  newCCB->ccb_Flags = CCB_LAST | CCB_NPABS | CCB_SPABS | CCB_PPABS | CCB_LDSIZE
                      | CCB_LDPRS | CCB_LDPPMP | CCB_CCBPRE | CCB_YOXY
                      | CCB_LDPLUT | CCB_ACW | CCB_ACCW;
#if RED_HARDWARE
  newCCB->ccb_PPMPC = 0x1F001F00;
#else
  newCCB->ccb_PIXC = 0x1F001F00;
#endif
  newCCB->ccb_PLUTPtr = NULL;
  newCCB->ccb_PRE0 = ((numRows - 1) << PRE0_VCNT_SHIFT) | PRE0_BPP_4;
  scanlinesizeinwords = ((rowBytes + 3) / 4) - 2;
  if (scanlinesizeinwords < 0)
    scanlinesizeinwords = 0;
  newCCB->ccb_PRE1 = (scanlinesizeinwords << PRE1_WOFFSET8_SHIFT)
                     | ((width - 1) << PRE1_TLHPCNT_SHIFT);
  celDataSize = rowBytes * numRows;
  celData = (char *)ALLOCMEM (celDataSize, MEMTYPE_CEL);
  if (celData == NULL)
    {
      FREEMEM (newCCB, sizeof (CCB));
      DIAGNOSTIC ("Cannot allocate storage for font ccb\n");
      return NULL;
    }
  memset (celData, 0, celDataSize);
#if RED_HARDWARE
  newCCB->ccb_CelData = (CelData *)celData;
#else
  newCCB->ccb_SourcePtr = (CelData *)celData;
#endif

  return newCCB;
}

CCB *
KCreate1BitCodedCel (uint32 numRows, uint32 width, Int32 rowBytes)
{
  CCB *newCCB;
  Int32 scanlinesizeinwords;
  char *celData;
  Int32 celDataSize;

  newCCB = (CCB *)ALLOCMEM (sizeof (CCB), MEMTYPE_CEL);
  if (newCCB == NULL)
    {
      DIAGNOSTIC ("Cannot allocate storage for font ccb\n");
      return NULL;
    }
  /* Fill in the values */
  memset (newCCB, 0, sizeof (CCB));
  newCCB->ccb_HDX = (1 << 20);
  newCCB->ccb_VDY = (1 << 16);
  newCCB->ccb_Width = width;
  newCCB->ccb_Height = numRows;
  newCCB->ccb_Flags = CCB_LAST | CCB_NPABS | CCB_SPABS | CCB_PPABS | CCB_LDSIZE
                      | CCB_LDPRS | CCB_LDPPMP | CCB_CCBPRE | CCB_YOXY
                      | CCB_LDPLUT | CCB_ACW | CCB_ACCW;
#if RED_HARDWARE
  newCCB->ccb_PPMPC = 0x1F001F00;
#else
  newCCB->ccb_PIXC = 0x1F001F00;
#endif
  newCCB->ccb_PRE0 = ((numRows - 1) << PRE0_VCNT_SHIFT) | PRE0_BPP_1;
  scanlinesizeinwords = ((rowBytes + 3) / 4) - 2;
  if (scanlinesizeinwords < 0)
    scanlinesizeinwords = 0;
  newCCB->ccb_PRE1 = (scanlinesizeinwords << PRE1_WOFFSET8_SHIFT)
                     | ((width - 1) << PRE1_TLHPCNT_SHIFT);
  celDataSize = rowBytes * numRows;
  celData = (char *)ALLOCMEM (celDataSize, MEMTYPE_CEL);
  if (celData == NULL)
    {
      FREEMEM (newCCB, sizeof (CCB));
      DIAGNOSTIC ("Cannot allocate storage for font ccb\n");
      return NULL;
    }
  memset (celData, 0, celDataSize);
#if RED_HARDWARE
  newCCB->ccb_CelData = (CelData *)celData;
#else
  newCCB->ccb_SourcePtr = (CelData *)celData;
#endif

  return newCCB;
}

Int32
KBinarySearch (KFontRec *thisFont, uint16 charVal, uint16 n)
{
  Int32 mid, left, right;

  left = 0;
  right = ((Int32)n) - 1;

  if (thisFont->codeTbl[left] == charVal)
    return left;
  while (left < right)
    {
      mid = (left + right) / 2;
      if (thisFont->codeTbl[mid] < charVal)
        {
          left = mid + 1;
        }
      else
        {
          right = mid;
        }
    }
  if (thisFont->codeTbl[left] == charVal)
    return left;
  return (ER_KTextBox_NotFound);
}

Int32
KGetCharIndex (KFont3DO *theFont, uint16 charVal)
{
  Int32 i = 0;
  Int32 min, max;

  /* �t�H���g�t�@�C�����k�ރt�H���g�̏ꍇ */
  if (isANK (charVal))
    {
      if (theFont->font_Hinfo.codeTbl == NULL)
        {
          /* min��max��ank�ł��邱�Ƃ��ۏ؂���Ă��� */
          min = KankToiCode (theFont->font_Hinfo.minCode);
          max = KankToiCode (theFont->font_Hinfo.maxCode);
          i = KankToiCode (charVal);
          if (i < 0)
            {
#if DEBUG
              kprintf ("ERROR:the character(%x) is not a ank code.\n",
                       charVal);
#endif
              return 0;
            }
          else if (min <= i && i <= max)
            {
              return (i - min);
            }
          else
            {
#if DEBUG
              kprintf (
                  "ERROR:the character(%x) is not a part of this fontfile.\n",
                  charVal);
#endif
              return 0;
            }
        }
      else
        {
          i = KBinarySearch (&theFont->font_Hinfo, charVal,
                             theFont->font_Hinfo.nChars);
          if (i < 0)
            {
#if DEBUG
              kprintf (
                  "ERROR:the character(%x) is not a part of this fontfile.\n",
                  charVal);
#endif
              return 0;
            }
          return i;
        }
    }
  else
    {
      if (theFont->font_Zinfo.codeTbl == NULL)
        {
          /* min��max��sjis�ł��邱�Ƃ��ۏ؂���Ă��� */
          min = KsjisToiCode (theFont->font_Zinfo.minCode);
          max = KsjisToiCode (theFont->font_Zinfo.maxCode);
          i = KsjisToiCode (charVal);
          if (i < 0)
            {
#if DEBUG
              kprintf ("ERROR:the character(%x) is not a sjis code.\n",
                       charVal);
#endif
              return 0;
            }
          else if (min <= i && i <= max)
            {
              return (i - min);
            }
          else
            {
#if DEBUG
              kprintf (
                  "ERROR:the character(%x) is not a part of this fontfile.\n",
                  charVal);
#endif
              return 0;
            }
        }
      else
        {
          i = KBinarySearch (&theFont->font_Zinfo, charVal,
                             theFont->font_Zinfo.nChars);
          if (i < 0)
            {
#if DEBUG
              kprintf (
                  "ERROR:the character(%x) is not a part of this fontfile.\n",
                  charVal);
#endif
              return 0;
            }
          return i;
        }
    }
}

Int32
KGetCharWidth (KFont3DO *theFont, uint16 charVal)
{
  Int32 i;

  if (isANK (charVal))
    {
      if (theFont->font_Hinfo.nChars <= 0)
        {
#if DEBUG
          kprintf ("ERROR:the character(%x) is not a part of this fontfile.\n",
                   charVal);
#endif
          return 0;
        }
      if (theFont->font_Hinfo.widthTbl == NULL)
        return theFont->font_Hinfo.charWidth;
      i = KGetCharIndex (theFont, charVal);
      return (Int32)theFont->font_Hinfo.widthTbl[i];
    }
  else
    {
      if (theFont->font_Zinfo.nChars <= 0)
        {
#if DEBUG
          kprintf ("ERROR:the character(%x) is not a part of this fontfile.\n",
                   charVal);
#endif
          return 0;
        }
      if (theFont->font_Zinfo.widthTbl == NULL)
        return theFont->font_Zinfo.charWidth;
      i = KGetCharIndex (theFont, charVal);
      return (Int32)theFont->font_Zinfo.widthTbl[i];
    }
}

Int32
KGetCharHeight (KFont3DO *theFont, uint16 charVal)
{
  if (isANK (charVal))
    {
      if (theFont->font_Hinfo.nChars <= 0)
        {
#if DEBUG
          kprintf ("ERROR:the character(%x) is not a part of this fontfile.\n",
                   charVal);
#endif
          return 0;
        }
      return (Int32)theFont->font_Hinfo.charHeight;
    }
  else
    {
      if (theFont->font_Zinfo.nChars <= 0)
        {
#if DEBUG
          kprintf ("ERROR:the character(%x) is not a part of this fontfile.\n",
                   charVal);
#endif
          return 0;
        }
      return (Int32)theFont->font_Zinfo.charHeight;
    }
}

Int32
KGetLineChars (KFont3DO *theFont, KTextBox *theTextBox, Int32 boxWidth,
               KCharRec **lineStart, Int32 *textLeft, Int32 *lineHeight,
               Int32 *lineTextWidth)
{
  Int32 lineChars;
  Int32 wordChars; /* �P���[�h�̕����� */
  Int32 wordWidth; /* ���[�h�� */
  Int32 charWidth;
  bool roomLeft; /* ���[�h������󂫂����邩�ǂ��� */
  bool startofline;
  bool firstWord;
  Int32 widthLeft;   /* �`��\\�Ȏc��̕�*/
  KCharRec *curChar; /* ���ݏ������̕��� */

  firstWord = TRUE;
  startofline = TRUE;
  roomLeft = TRUE;
  lineChars = 0;
  wordChars = 0;
  wordWidth = 0;
  *lineTextWidth = 0;
  widthLeft = boxWidth;
  curChar = *lineStart;

  /* ���[�h���b�v�́A3DOFont�p�����̂ݗL��*/
  /*    if (theFont->font_Flags & FONT_ASCII) {
          *lineHeight = theFont->fontHinfo.charHeight;
          while ( (*textLeft > 0) && roomLeft && (curChar->Value != '\n') ) {
              wordWidth = 0;
              wordChars = 0;
              numendingword = 0;
              while ( (*textLeft > 0) && (curChar->Value == ' ') &&
     (curChar->Value != '\n') ) { if (!startofline) { wordWidth +=
     tx_get_char_width(theFont,' ',fontAscii); wordChars++; numendingword++;
                  }
                  else (*lineStart)++;
                  curChar++;
                  (*textLeft)--;
              }
              startofline = FALSE;
              if ( curChar->Value != '\n' ) {
                  while ( (*textLeft > 0) && ((wordWidth <= widthLeft) ||
     firstWord) && (curChar->Value != ' ') && (curChar->Value != '\n') ) {
                      (*textLeft)--;
                      wordChars++;
                      wordWidth +=
     tx_get_char_width(theFont,curChar->Value,fontAscii); curChar++;
                      numendingword++;
                  }
                  if ( (wordWidth > widthLeft) && (!firstWord) ) {
                      roomLeft = FALSE;
                      (*textLeft)+=numendingword;
                  }
                  else {
                      *lineTextWidth += wordWidth;
                      widthLeft -= wordWidth;
                      if (widthLeft < 0) {
                          roomLeft = FALSE;
                          widthLeft = 0;
                      }
                      lineChars += wordChars;
                  }
              }
              firstWord = FALSE;
          }
      }
      else {
  */
  if (theFont->font_Hinfo.nChars > 0)
    { /*������̐擪�ɉ��s�R�[�h�������Ă����ꍇ��lineHeight��ݒ肷��B*/
      *lineHeight = (Int32)theFont->font_Hinfo.charHeight;
    }
  else if (theFont->font_Zinfo.nChars > 0)
    {
      *lineHeight = (Int32)theFont->font_Zinfo.charHeight;
    }
  while ((*textLeft > 0) && (widthLeft > 0) && (curChar->Value != CR)
         && (curChar->Value != LF))
    {
      charWidth = KGetCharWidth (theFont, curChar->Value);
      *lineHeight = KGetCharHeight (theFont, curChar->Value);
      if (charWidth > widthLeft)
        {
          widthLeft = 0;
        }
      else
        {
          (*textLeft)--;
          lineChars++;
          curChar++;
          *lineTextWidth += (charWidth + theTextBox->charPitch);
          widthLeft -= (charWidth + theTextBox->charPitch);
        }
    }
  /*    }*/
  return lineChars;
}

Int32
KDrawChar4BitCel (KFont3DO *theFont, uint16 charVal, Int32 x, Int32 y,
                  Int32 celByteWidth, CCB *fourBitCel, char *dataPtr)
{

  Int32 curX, curY;
  Int32 i, j, k;
  Int32 xDiff, yDiff;
  Int32 startj, endj, startk, endk;
  Int32 charWidth, charHeight;
  Int32 bitStart;
  char *pCelData, *pImageData, imageByte;
  bool skipChar;
  Int32 pixelDepth;
  uint8 lineData[128];
  Int32 loop;
  Int32 fseekOffset, oneCharSize;
  int32 error;

  if (theFont == NULL)
    {
      DIAGNOSTIC ("theFont is NULL.\n");
      return x;
    }

  if (fourBitCel == NULL)
    {
      DIAGNOSTIC ("fourBitCel is NULL.\n");
      return x;
    }

  /* 4Bit�Z���ȊO�̓G���[ */
  pixelDepth = fourBitCel->ccb_PRE0 & 0x07;
  if (pixelDepth != PRE0_BPP_4)
    {
      DIAGNOSTIC ("ERROR:This cel's pixel depth isn't PRE0_BPP_4.\n");
      return x;
    }

  charWidth = KGetCharWidth (theFont, charVal);
  if (charWidth == 0)
    return x;
  charHeight = KGetCharHeight (theFont, charVal);
  if (charHeight == 0)
    return x;
  skipChar = false;
  startk = 0;
  //    endk = (charWidth + (((charWidth + 7)/8)*8 - charWidth))/ 2;
  //	endk = charWidth/ 2;
  startj = 0;
  endj = charHeight;

  /* �t�H���g�̑傫�����Z�����傫�� */
  if ((charWidth > fourBitCel->ccb_Width)
      || (charHeight > fourBitCel->ccb_Height))
    {
      DIAGNOSTIC ("ERROR:charWidth or charHeight is greater than cel size.");
      return x;
    }

  curX = x;
  curY = y - charHeight;

#if RED_HARDWARE
  xDiff = curX - ConvertF16_32 (fourBitCel->ccb_X);
  yDiff = curY - ConvertF16_32 (fourBitCel->ccb_Y);
#else
  xDiff = curX - ConvertF16_32 (fourBitCel->ccb_XPos);
  yDiff = curY - ConvertF16_32 (fourBitCel->ccb_YPos);
#endif

  if (yDiff < 0)
    {
      /* �����n�߂���[����͂ݏo��ꍇ */
      if (yDiff + charHeight <= 0)
        skipChar = true;
      else
        startj = -yDiff;
      yDiff = 0;
    }
  else if (yDiff + charHeight > fourBitCel->ccb_Height)
    {
      /* �����I��肪���[����͂ݏo��ꍇ */
      if (yDiff >= fourBitCel->ccb_Height)
        skipChar = true;
      else
        endj = fourBitCel->ccb_Height - yDiff;
    }

  if (xDiff < 0)
    {
      /* �����n�߂����[����͂ݏo��ꍇ */
      if (xDiff + charWidth <= 0)
        skipChar = true;
      else
        startk = -xDiff / 2;
      xDiff = 0;
    }
  else if (xDiff + charWidth > fourBitCel->ccb_Width)
    {
      /* �����I��肪�E�[����͂ݏo��ꍇ */
      if (xDiff >= fourBitCel->ccb_Width)
        skipChar = true;
      else
        endk = (fourBitCel->ccb_Width - xDiff) / 2;
    }

  if (!skipChar)
    {
      bitStart = xDiff & 0x01;
#if RED_HARDWARE
      /* point to right position in celdata */
      pCelData = (char *)fourBitCel->ccb_CelData
                 + ((xDiff / 2) + (celByteWidth * yDiff));
#else
      /* point to right position in celdata */
      pCelData = (char *)fourBitCel->ccb_SourcePtr
                 + ((xDiff / 2) + (celByteWidth * yDiff));
#endif

      i = KGetCharIndex (theFont, charVal);

      if (!theFont->font_FullRead)
        { // <HPP & EL 03-08-94> if memory not loaded fully
          if (isANK (charVal))
            {
              oneCharSize = theFont->font_Hinfo.oneCharSize;
              fseekOffset = theFont->font_HFSeekOffset;
            }
          else
            {
              oneCharSize = theFont->font_Zinfo.oneCharSize;
              fseekOffset = theFont->font_ZFSeekOffset;
            }
          error = ReadFile_E (theFont, (int)oneCharSize, (Int32 *)dataPtr,
                              fseekOffset + i * oneCharSize);
          if (error < 0)
            printf ("ReadFile_E dataSize %d, fseekOffset %d\n",
                    (int)oneCharSize, (int)fseekOffset + i * oneCharSize);
          pImageData = dataPtr;
        }
      else
        {
          if (isANK (charVal))
            pImageData = theFont->font_Hinfo.charData
                         + i * theFont->font_Hinfo.oneCharSize;
          else
            pImageData = theFont->font_Zinfo.charData
                         + i * theFont->font_Zinfo.oneCharSize;
        }

      /*
      #if DEBUG
                      kprintf("pCelData(address) = %x ", pCelData);
                      kprintf("pImageData(address) = %x ", pImageData);
      #endif
      */
      if (charWidth & 0x01)
        { /* ����������̏ꍇ */
          loop = charWidth / 2 + 1;
          for (j = 0; j < endj; j++)
            { /* �Z���̉��[����͂ݏo����O�̃s�N�Z���s�܂ŕ`�悷��*/
              if (j < startj)
                { /* �Z���̏�[����͂ݏo���s�N�Z���s�́A�`�悵�Ȃ�*/
                  if (j & 0x01)
                    pImageData += (loop - 1);
                  else
                    pImageData += loop;
                  continue;
                }
              for (i = 0; i < loop; i++)
                {
                  lineData[i] = *pImageData++;
                }
              if (j & 0x01)
                {
                  lineData[0] &= 0x0F;
                  pImageData += (loop - 1);
                }
              else
                {
                  lineData[i - 1] &= 0xF0;
                  pImageData += (loop - 1);
                }
              /*
              #if DEBUG
                          for(i=0;i<loop;i++)
                              kprintf("%x ", lineData[i]);
              #endif
              */
              endk = loop;
              for (k = startk; k < endk; k++)
                {
                  imageByte = lineData[k];
                  *(pCelData + (k - startk)) |= (imageByte >> (bitStart * 4));
                  *(pCelData + (k - startk) + 1) |= imageByte
                                                    << (8 - (bitStart * 4));
                }
              pCelData += celByteWidth;
            }
        }
      else
        { /* �������������̏ꍇ */
          loop = charWidth / 2;
          for (j = 0; j < endj; j++)
            { /* �Z���̉��[����͂ݏo����O�̃s�N�Z���s�܂ŕ`�悷��*/
              if (j < startj)
                { /* �Z���̏�[����͂ݏo���s�N�Z���s�́A�`�悵�Ȃ�*/
                  pImageData += loop;
                  continue;
                }
              for (i = 0; i < loop; i++)
                {
                  lineData[i] = *pImageData++;
                }

              /*
              #if DEBUG
                                      for(i=0;i<loop;i++)
                                              kprintf("%x ", lineData[i]);
                                              kprintf("\n");
              #endif
              */
              endk = loop;
              for (k = startk; k < endk; k++)
                {
                  imageByte = lineData[k];
                  *(pCelData + (k - startk)) |= (imageByte >> (bitStart * 4));
                  *(pCelData + (k - startk) + 1) |= imageByte
                                                    << (8 - (bitStart * 4));
                  /*
                  #if DEBUG
                                                  kprintf("%x ",
                  *(pCelData+k)); #endif
                  */
                }
              pCelData += celByteWidth;
              /*
              #if DEBUG
                                      kprintf("\n");
              #endif
              */
            }

          /*
          #if DEBUG
                                  kprintf("===========\n");
          #endif
          */
        }
      return x + charWidth;
    }
  return x;
}

Int32
KDrawChar1BitCel (KFont3DO *theFont, uint16 charVal, Int32 x, Int32 y,
                  Int32 celByteWidth, CCB *oneBitCel, char *dataPtr)
{

  Int32 curX, curY;
  Int32 i, j, l, m;
  Int32 w;
  Int32 xDiff, yDiff;
  Int32 startj, endj;
  Int32 charWidth, charHeight;
  Int32 bitStart;
  char *pCelData, *pImageData, imageRow;
  bool skipChar;
  Int32 pixelDepth;
  uint8 lineData[32];
  Int32 xbit, ybit, zbit;
  Int32 leftBit;
  Int32 fseekOffset, oneCharSize;
  int32 error;

  if (theFont == NULL)
    {
      DIAGNOSTIC ("theFont is NULL.\n");
      return x;
    }

  if (oneBitCel == NULL)
    {
      DIAGNOSTIC ("oneBitCel is NULL.\n");
      return x;
    }

  /* 1Bit�Z���ȊO�̓G���[ */
  pixelDepth = oneBitCel->ccb_PRE0 & 0x07;
  if (pixelDepth != PRE0_BPP_1)
    {
      DIAGNOSTIC ("ERROR:This cel's pixel depth isn't PRE0_BPP_1.\n");
      return x;
    }

  charWidth = KGetCharWidth (theFont, charVal);
  if (charWidth == 0)
    return x;
  charHeight = KGetCharHeight (theFont, charVal);
  if (charHeight == 0)
    return x;
  skipChar = false;
  /*    startk = 0;*/
  /*    endk = (charWidth + ((charWidth + 7)/8)*8 - charWidth))/ 2;*/
  startj = 0;
  endj = charHeight;

  /* �t�H���g�̑傫�����Z�����傫�� */
  if ((charWidth > oneBitCel->ccb_Width)
      || (charHeight > oneBitCel->ccb_Height))
    {
      DIAGNOSTIC ("ERROR:charWidth or charHeight is greater than cel size.");
      return x;
    }

  curX = x;
  curY = y - charHeight;

#if RED_HARDWARE
  xDiff = curX - ConvertF16_32 (oneBitCel->ccb_X);
  yDiff = curY - ConvertF16_32 (oneBitCel->ccb_Y);
#else
  xDiff = curX - ConvertF16_32 (oneBitCel->ccb_XPos);
  yDiff = curY - ConvertF16_32 (oneBitCel->ccb_YPos);
#endif

  if (yDiff < 0)
    {
      /* �����n�߂���[����͂ݏo��ꍇ */
      if (yDiff + charHeight <= 0)
        skipChar = true;
      else
        startj = -yDiff;
      yDiff = 0;
    }
  else if (yDiff + charHeight > oneBitCel->ccb_Height)
    {
      /* �����I��肪���[����͂ݏo��ꍇ */
      if (yDiff >= oneBitCel->ccb_Height)
        skipChar = true;
      else
        endj = oneBitCel->ccb_Height - yDiff;
    }

  if ((xDiff + charWidth < 0) || (xDiff > (oneBitCel->ccb_Width)))
    skipChar = true;
  if (!skipChar)
    {

#if RED_HARDWARE
      /* point to right position in celdata */
      pCelData = (char *)oneBitCel->ccb_CelData + (xDiff / 8)
                 + (celByteWidth * yDiff);
#else
      /* point to right position in celdata */
      pCelData = (char *)oneBitCel->ccb_SourcePtr + (xDiff / 8)
                 + (celByteWidth * yDiff);
#endif
      i = KGetCharIndex (theFont, charVal);

      if (!theFont->font_FullRead)
        { // <HPP & EL 03-08-94> if memory not loaded fully
          if (isANK (charVal))
            {
              oneCharSize = theFont->font_Hinfo.oneCharSize;
              fseekOffset = theFont->font_HFSeekOffset;
            }
          else
            {
              oneCharSize = theFont->font_Zinfo.oneCharSize;
              fseekOffset = theFont->font_ZFSeekOffset;
            }
          error = ReadFile_E (theFont, (int)oneCharSize, (Int32 *)dataPtr,
                              fseekOffset + i * oneCharSize);
          if (error < 0)
            printf ("ReadFile_E dataSize %d, fseekOffset %d\n",
                    (int)oneCharSize, (int)fseekOffset + i * oneCharSize);
          pImageData = (uint8 *)dataPtr;
        }
      else
        {
          if (isANK (charVal))
            pImageData = (uint8 *)theFont->font_Hinfo.charData
                         + i * theFont->font_Hinfo.oneCharSize;
          else
            pImageData = (uint8 *)theFont->font_Zinfo.charData
                         + i * theFont->font_Zinfo.oneCharSize;
        }

      /*
      #if DEBUG
              for(i=0;i<9;i++)
                  kprintf("%x ", pImageData[i]);
                  kprintf("\n");
      #endif
      */
      xbit = 0; /*8bit��MSB���琔����xbit�ڂ��w��*/
      ybit = 8 - xbit;
      zbit = 0;
      leftBit = charWidth;
      l = m = 0;
      /*	pImageData = &lineData[0];*/

      if (xDiff < 0)
        { /* Draw partial character */
          bitStart = (-xDiff) & 0x07;
          for (j = 0; j < endj; j++)
            { /* �Z���̉��[����͂ݏo����O�̃s�N�Z���s�܂ŕ`�悷��*/
              while (leftBit >= 8)
                {
                  lineData[l] = pImageData[m++] << xbit;
                  lineData[l++] += (pImageData[m] >> ybit);
                  leftBit -= 8;
                }
              if (leftBit > ybit)
                {
                  lineData[l] = pImageData[m++] << xbit;
                  zbit = leftBit - ybit;
                  lineData[l] += ((pImageData[m] >> zbit) << (zbit - ybit));
                  xbit = zbit;
                }
              else if (leftBit > 0)
                {
                  lineData[l] = (pImageData[m] << xbit) >> (8 - leftBit);
                  xbit += leftBit;
                }
              ybit = 8 - xbit;
              if (j < startj)
                { /* �Z���̏�[����͂ݏo���s�N�Z���s�́A�`�悵�Ȃ�*/
                  l = 0;
                  continue;
                }
              imageRow = lineData[0];
              *pCelData |= imageRow << (8 - bitStart);
              for (i = 1, w = charWidth - 8; w > 0; i++, w -= 8)
                {
                  imageRow = lineData[i];
                  *(pCelData) |= imageRow >> bitStart;
                  *(pCelData + i) |= imageRow << (8 - bitStart);
                }
              pCelData += celByteWidth;
              l = 0;
              leftBit = charWidth;
            }
        }
      else if ((xDiff + charWidth) > (oneBitCel->ccb_Width))
        { /* Draw partial character */
          bitStart = (charWidth + xDiff) - (oneBitCel->ccb_Width);
          for (j = 0; j < endj; j++)
            { /* �Z���̉��[����͂ݏo����O�̃s�N�Z���s�܂ŕ`�悷��*/
              while (leftBit >= 8)
                {
                  lineData[l] = pImageData[m++] << xbit;
                  lineData[l++] += (pImageData[m] >> ybit);
                  leftBit -= 8;
                }
              if (leftBit > ybit)
                {
                  lineData[l] = pImageData[m++] << xbit;
                  zbit = leftBit - ybit;
                  lineData[l] += ((pImageData[m] >> zbit) << (zbit - ybit));
                  xbit = zbit;
                }
              else if (leftBit > 0)
                {
                  lineData[l] = (pImageData[m] << xbit) >> (8 - leftBit);
                  xbit += leftBit;
                }
              ybit = 8 - xbit;
              if (j < startj)
                { /* �Z���̏�[����͂ݏo���s�N�Z���s�́A�`�悵�Ȃ�*/
                  l = 0;
                  continue;
                }
              imageRow = lineData[0];
              *pCelData |= (imageRow >> bitStart);
              for (i = 1, w = charWidth - 8; w > 0; i++, w -= 8)
                {
                  imageRow = lineData[i];
                  *(pCelData) |= imageRow << (8 - bitStart);
                  *(pCelData + i) |= imageRow >> bitStart;
                }
              pCelData += celByteWidth;
              l = 0;
              leftBit = charWidth;
            }
        }
      else
        { /* Draw whole character */
          bitStart = xDiff & 0x07;
          for (j = 0; j < endj; j++)
            { /* �Z���̉��[����͂ݏo����O�̃s�N�Z���s�܂ŕ`�悷��*/
              if (leftBit >= 8)
                {
                  while (leftBit >= 8)
                    {
                      if (xbit > 0)
                        {
                          lineData[l] = pImageData[m++] << xbit;
                          leftBit -= ybit;
                          lineData[l++] += pImageData[m] >> ybit;
                          leftBit -= xbit;
                        }
                      else
                        {
                          lineData[l++] = pImageData[m++];
                          leftBit -= 8;
                        }
                    }
                  if (leftBit > 0)
                    {
                      if (leftBit >= ybit)
                        {
                          lineData[l] = pImageData[m++] << xbit;
                          leftBit -= ybit;
                          if (leftBit > 0)
                            lineData[l]
                                += ((pImageData[m] >> (8 - leftBit)) << ybit);
                          xbit = leftBit;
                        }
                      else
                        {
                          zbit = 8 - leftBit;
                          lineData[l] = (pImageData[m] >> zbit) << zbit;
                          xbit = leftBit;
                        }
                    }
                }
              else
                {
                  if (xbit == 0)
                    {
                      lineData[l] = pImageData[m] >> (8 - leftBit);
                      xbit = leftBit;
                    }
                  else if (leftBit > ybit)
                    {
                      lineData[l] = (pImageData[m++] << (leftBit - ybit))
                                    & MASK_TABLE[leftBit - 0];
                      leftBit -= ybit;
                      if (leftBit > 0)
                        lineData[l] += (pImageData[m] >> (8 - leftBit));
                      xbit = leftBit;
                    }
                  else if (leftBit == ybit)
                    {
                      lineData[l] = pImageData[m++] & MASK_TABLE[leftBit - 0];
                      xbit = 0;
                    }
                  else
                    {
                      zbit = 8 - leftBit;
                      lineData[l] = (pImageData[m] >> zbit) << zbit;
                      xbit = leftBit;
                    }
                }

              ybit = 8 - xbit;

              if (j < startj)
                { /* �Z���̏�[����͂ݏo���s�N�Z���s�́A�`�悵�Ȃ�*/
                  l = 0;
                  for (i = 0; i < l; i++)
                    lineData[i] = 0;
                  continue;
                }
              /*
              #if DEBUG
                          kprintf("%d ", lineData[0]);
              #endif
              */
              imageRow = lineData[0];
              *pCelData |= (imageRow >> bitStart);
              *(pCelData + 1) |= imageRow << (8 - bitStart);
              for (i = 1, w = charWidth - 8; w > 0; i++, w -= 8)
                {
                  imageRow = lineData[i];
                  *(pCelData + i) |= (imageRow >> bitStart);
                  *(pCelData + i + 1) |= imageRow << (8 - bitStart);
                }
              pCelData += celByteWidth;
              for (i = 0; i < l; i++)
                lineData[i] = 0;
              l = 0;
              leftBit = charWidth;
            }
        }
      return x + charWidth;
    }
  return x;
}

void
KDrawTextH (KFont3DO *theFont, KCharRec *theLine, Int32 lineChars, Int32 x,
            Int32 y, Int32 charPitch, Int32 celByteWidth, CCB *theCCB)
{

  Int32 i;
  Int32 retVal;
  uint16 c;
  char *dataPtr;
  int32 dataSize;

  if (!theFont->font_FullRead)
    { // <HPP&EL 03-09-94>
      dataSize = theFont->maxCharSize;
      dataPtr = (char *)ALLOCMEM (dataSize, MEMTYPE_CEL);
    }

  for (i = 0; i < lineChars; i++)
    {

      c = theLine[i].Value;
      if (theFont->font_Bpp == FOURBITS_PER_PIXEL)
        retVal = KDrawChar4BitCel (theFont, c, x, y, celByteWidth, theCCB,
                                   dataPtr);
      else
        retVal = KDrawChar1BitCel (theFont, c, x, y, celByteWidth, theCCB,
                                   dataPtr);
      if (x != retVal)
        x = (retVal + charPitch);
    }

  if (!theFont->font_FullRead && dataPtr) // <HPP&EL 03-09-94>
    FREEMEM (dataPtr, dataSize);
}

void
KDrawTextV (KFont3DO *theFont, KCharRec *theLine, Int32 lineChars, Int32 x,
            Int32 y, Int32 charPitch, Int32 celByteWidth, CCB *theCCB)
{
  Int32 i;
  Int32 retVal;
  uint16 c;
  char *dataPtr;
  int32 dataSize;

  if (!theFont->font_FullRead)
    { // <HPP&EL 03-09-94>
      dataSize = theFont->maxCharSize;
      dataPtr = (char *)ALLOCMEM (dataSize, MEMTYPE_CEL);
    }

  for (i = 0; i < lineChars; i++)
    {

      c = theLine[i].Value;
      if (theFont->font_Bpp == FOURBITS_PER_PIXEL)
        retVal = KDrawChar4BitCel (theFont, c, x, y, celByteWidth, theCCB,
                                   dataPtr);
      else
        retVal = KDrawChar1BitCel (theFont, c, x, y, celByteWidth, theCCB,
                                   dataPtr);
      if (x != retVal)
        y += (KGetCharHeight (theFont, c) + charPitch);
    }

  if (!theFont->font_FullRead && dataPtr) // <HPP&EL 03-09-94>
    FREEMEM (dataPtr, dataSize);
}

void
KDrawTextA (KFont3DO *theFont, KTextBox *theTextBox, Int32 boxWidth,
            KCharRec *lineStart, Int32 lineChars, Int32 celByteWidth,
            Int32 lineTextWidth, Int32 curY, CCB *theCCB)
{
  /*
    For the rest of the text alignments (left, center, and right), we just
    move the pen to the right place and draw the text with DrawText.
    */
  Int32 left, right, x;
  Int32 charPitch;

  if (theTextBox->otherFlags & O_VERTICAL)
    {
      ;
    }
  else
    {
      left = ConvertF16_32 (theTextBox->wrapRect.rectf16_XLeft);
      right = ConvertF16_32 (theTextBox->wrapRect.rectf16_XRight);
      charPitch = theTextBox->charPitch;

      switch (theTextBox->align)
        {
        case justLeft:
          x = left;
          break;
        case justRight:
          x = right - lineTextWidth;
          break;
        case justCenter:
          x = left + ((boxWidth - lineTextWidth) / 2);
          break;
        case justJustify:
          /* ���݃W���X�e�B�t�@�C�͎����ł��Ă��Ȃ��B*/
          x = left;
          break;
        default:
          x = left;
          break;
        }

      KDrawTextH (theFont, lineStart, lineChars, x, curY, charPitch,
                  celByteWidth, theCCB);
    }
}

Int32
KWrapAndDraw (KFont3DO *theFont, KTextBox *theTextBox, Int32 boxWidth,
              KCharRec *lineStart, KCharRec *textEnd, Int32 lineLeft,
              Int32 boxRowBytes, Int32 top, Int32 bottom, CCB *theCCB)
{
  Int32 lineCount;  /* Number of lines we've drawn */
  Int32 lineChars;  /* Number of bytes in one line */
  Int32 lineHeight; /* Calculated line height */
  Int32 curY;       /* Current vertical pen location */
  Int32 lineTextWidth;

  /* �����o���ʒuY���W�������� */
  curY = top;
  lineCount = 0;

  /*
    This is the main wrap-and-draw loop.  I bet you never thought wrapping
    text could be so easy...
    Keep figuring out lines until out of text or run out of vertical space
   */

  if (theTextBox->otherFlags & O_KINSOKU)
    {
      /*�֑�������ON�̏ꍇ*/
    }
  else
    {
      do
        {
          /* Figure out how many words we can write on this line */
          lineChars = KGetLineChars (theFont, theTextBox, boxWidth, &lineStart,
                                     &lineLeft, &lineHeight, &lineTextWidth);
          curY += lineHeight;
          if (curY > bottom)
            {
              if (lineCount == 0)
                {
                  /* ��s�ڂ̏����o���ʒu���͂ݏo��ꍇ�̓G���[*/
                  DIAGNOSTIC ("The font height size is greater than the "
                              "wrapRect height.\n");
                  return (ER_KTextBox_BadParameter);
                }
              else
                {
                  return (ER_KTextBox_NoError); /* ��s�ڂ̏ꍇ�̓m�b�g�G���[*/
                }
            }

          KDrawTextA (theFont, theTextBox, boxWidth, lineStart, lineChars,
                      boxRowBytes, lineTextWidth, curY, theCCB);

          /*
            Now we advance our vertical position down by the height of one
            line, advance lineStart by the number of bytes we just drew,
            calculate a new textLeft, and increment our line count.
            */

          curY += theTextBox->linePitch;
          lineStart += lineChars;
          /* Make sure to skip over the newline characters that were
           * encountered */
          if (lineStart->Value == CR || lineStart->Value == LF)
            {
              lineStart++;
              lineLeft--;
            }
          lineCount++;
        }
      while ((lineStart < textEnd) && (curY <= bottom));
    }

  return (ER_KTextBox_NoError);
}

Int32
KTextBoxMain (KFont3DO *theFont, KTextBox *theTextBox, uint8 *theText,
              Int32 textLen, CCB **userCCB)
{
  Int32 cel_left, cel_right, cel_top, cel_bottom;
  Int32 left, right, top, bottom;
  Int32 cel_boxWidth, boxWidth;        /* Width of the wrapBox */
  Int32 cel_boxHeight, boxHeight;      /* Height of the wrapBox */
  Int32 cel_boxRowBytes;               /* RowBytes of the wrapBox */
  Int32 textLeft;                      /* Pointer to remaining bytes of text */
  KCharRec *lineStart, *origLineStart; /* Pointer to beginning of a line */
  KCharRec *textEnd;                   /* Pointer to the end of input text */
  CCB *theCCB;
  Int32 retValue;

  /* �w�肳�ꂽ�������̃`�F�b�N������*/
  /* Do some idiot testing */
  if (theFont == NULL)
    {
      DIAGNOSTIC ("theFont is NULL.\n");
      return (ER_KTextBox_BadParameter);
    }
  if (*theText == '\0')
    {
      DIAGNOSTIC ("theText is NULL.\n");
      return (ER_KTextBox_BadParameter);
    }
  if (textLen == 0)
    {
      DIAGNOSTIC ("textLen is 0.\n");
      return (ER_KTextBox_BadParameter);
    }
  if (theTextBox == NULL)
    {
      DIAGNOSTIC ("theTextBox is NULL.\n");
      return (ER_KTextBox_BadParameter);
    }
  if (userCCB == NULL)
    {
      DIAGNOSTIC ("userCCB is NULL.\n");
      return (ER_KTextBox_BadParameter);
    }
  cel_left = ConvertF16_32 (theTextBox->celRect.rectf16_XLeft);
  cel_right = ConvertF16_32 (theTextBox->celRect.rectf16_XRight);
  cel_top = ConvertF16_32 (theTextBox->celRect.rectf16_YTop);
  cel_bottom = ConvertF16_32 (theTextBox->celRect.rectf16_YBottom);

  left = ConvertF16_32 (theTextBox->wrapRect.rectf16_XLeft);
  right = ConvertF16_32 (theTextBox->wrapRect.rectf16_XRight);
  top = ConvertF16_32 (theTextBox->wrapRect.rectf16_YTop);
  bottom = ConvertF16_32 (theTextBox->wrapRect.rectf16_YBottom);
  /*
     if ( (cel_left < 0 ) || (cel_right > 320 ) || (cel_top < 0 ) ||
     (cel_bottom > 240 ) ){ DIAGNOSTIC("celRect is too large.\n"); return
     (ER_KTextBox_BadParameter);
     }
  */
  if ((left < cel_left) || (right > cel_right) || (top < cel_top)
      || (bottom > cel_bottom))
    {
      DIAGNOSTIC ("wrapRect is too large.\n");
      return (ER_KTextBox_BadParameter);
    }

  cel_boxWidth = cel_right - cel_left;
  cel_boxHeight = cel_bottom - cel_top;
  if (cel_boxWidth <= 0)
    {
      DIAGNOSTIC ("celRect is too small.\n");
      return (ER_KTextBox_BadParameter);
    }
  if (cel_boxHeight <= 0)
    {
      DIAGNOSTIC ("celRect is too small.\n");
      return (ER_KTextBox_BadParameter);
    }

  boxWidth = right - left;
  boxHeight = bottom - top;
  if (boxWidth <= 0)
    {
      DIAGNOSTIC ("wrapRect is too small.\n");
      return (ER_KTextBox_BadParameter);
    }
  if (boxHeight <= 0)
    {
      DIAGNOSTIC ("wrapRect is too small.\n");
      return (ER_KTextBox_BadParameter);
    }

  /* �w�肳�ꂽ�����������e�L�X�g�f�[�^�̃t�H�[�}�b�g���R���o�[�g����B*/
  retValue = KConvertText (theText, (KCharRec **)&(lineStart), textLen);
  if (retValue < 0)
    {
      return (retValue);
    }
  else if (retValue < textLen)
    {
      FREEMEM (lineStart, retValue * sizeof (KCharRec));
      return (ER_KTextBox_BadCharCode);
    }
  origLineStart = lineStart;
  /* �Ō�̕����̎��̃A�h���X */
  textEnd = lineStart + textLen;
  /* �܂��`�悵�Ă��Ȃ��c��̕����� */
  textLeft = textLen;

  if (theFont->font_Bpp == ONEBITS_PER_PIXEL)
    {
      /*	1bit�Z���̃o�C�g�������߂�	*/
      cel_boxRowBytes = ((((cel_boxWidth + 7) / 8) + 3) / 4) * 4;
      if (cel_boxRowBytes < 8)
        cel_boxRowBytes = 8;
      /*  user requested that we allocate the memory for the CCB */

      if (*userCCB == NULL)
        theCCB = KCreate1BitCodedCel (cel_boxHeight, cel_boxWidth,
                                      cel_boxRowBytes);
      else
        {
          /* user requested that we use the CCB passed into the routine */
          theCCB = *userCCB;
#if RED_HARDWARE
          if (theCCB->ccb_CelData == NULL)
            {
              DIAGNOSTIC ("theCCB->ccb_CelData is NULL.\n");
              retValue = ER_KTextBox_BadParameter;
              goto ERROR;
            }
          memset (theCCB->ccb_CelData, 0, cel_boxHeight * cel_boxRowBytes);
#else
          if (theCCB->ccb_SourcePtr == NULL)
            {
              DIAGNOSTIC ("theCCB->ccb_SourcePtr is NULL.\n");
              retValue = ER_KTextBox_BadParameter;
              goto ERROR;
            }
          memset (theCCB->ccb_SourcePtr, 0, cel_boxHeight * cel_boxRowBytes);
#endif
        }
      if (theCCB == NULL)
        {
          retValue = ER_KTextBox_CannotMemAlloc;
          goto ERROR;
        }
#if RED_HARDWARE
      theCCB->ccb_X = theTextBox->celRect.rectf16_XLeft;
      theCCB->ccb_Y = theTextBox->celRect.rectf16_YTop;
#else
      theCCB->ccb_XPos = theTextBox->celRect.rectf16_XLeft;
      theCCB->ccb_YPos = theTextBox->celRect.rectf16_YTop;
#endif
      retValue = KMakePLUT2 (theTextBox->fgColor, theTextBox->bgColor,
                             theTextBox->opaqueFlag,
                             (uint16 **)&(theCCB->ccb_PLUTPtr));
      if (retValue < 0)
        goto ERROR;
    }
  else if (theFont->font_Bpp == FOURBITS_PER_PIXEL)
    {
      /*	4bit�Z���̃o�C�g�������߂�	*/
      cel_boxRowBytes = ((((cel_boxWidth + 1) / 2) + 3) / 4) * 4;
      if (cel_boxRowBytes < 8)
        cel_boxRowBytes = 8;
      /* user requested that we allocate the memory for the CCB */
      if (*userCCB == NULL)
        theCCB = KCreate4BitCodedCel (cel_boxHeight, cel_boxWidth,
                                      cel_boxRowBytes);
      else
        {
          /* user requested that we use the CCB passed into the routine */
          theCCB = *userCCB;
#if RED_HARDWARE
          if (theCCB->ccb_CelData == NULL)
            {
              DIAGNOSTIC ("theCCB->ccb_CelData is NULL.\n");
              retValue = ER_KTextBox_BadParameter;
              goto ERROR;
            }
          memset (theCCB->ccb_CelData, 0, cel_boxHeight * cel_boxRowBytes);
#else
          if (theCCB->ccb_SourcePtr == NULL)
            {
              DIAGNOSTIC ("theCCB->ccb_SourcePtr is NULL.\n");
              retValue = ER_KTextBox_BadParameter;
              goto ERROR;
            }
          memset (theCCB->ccb_SourcePtr, 0, cel_boxHeight * cel_boxRowBytes);
#endif
        }
      if (theCCB == NULL)
        {
          retValue = ER_KTextBox_CannotMemAlloc;
          goto ERROR;
        }
#if RED_HARDWARE
      theCCB->ccb_X = theTextBox->celRect.rectf16_XLeft;
      theCCB->ccb_Y = theTextBox->celRect.rectf16_YTop;
#else
      theCCB->ccb_XPos = theTextBox->celRect.rectf16_XLeft;
      theCCB->ccb_YPos = theTextBox->celRect.rectf16_YTop;
#endif
      /* 4bit�Z����PLUT��ݒ肷�� */
      retValue = KMakePLUT16 (theTextBox->fgColor, theTextBox->bgColor,
                              theTextBox->opaqueFlag,
                              (uint16 **)&(theCCB->ccb_PLUTPtr));
      if (retValue < 0)
        goto ERROR;
    }

  retValue = KWrapAndDraw (theFont, theTextBox, boxWidth, lineStart, textEnd,
                           textLeft, cel_boxRowBytes, top, bottom, theCCB);
  if (retValue < 0)
    goto ERROR;

  FREEMEM (origLineStart, sizeof (KCharRec) * textLen);
  *userCCB = theCCB;
  return (ER_KTextBox_NoError);
ERROR:
  FREEMEM (origLineStart, sizeof (KCharRec) * textLen);
  return (retValue);
}

CCB *
KTextBox_A (KFont3DO *theFont, uint8 *theText, Int32 textLen, Rectf16 *wrapBox,
            TextAlign *theAlign, Color fgColor, Color bgColor, CCB *userCCB)
{
  KTextBox theTextBox;

  if (wrapBox == NULL)
    {
      DIAGNOSTIC ("wrapBox is NULL.\n");
      return (0);
    }
  if (theAlign == NULL)
    {
      DIAGNOSTIC ("theAlign is NULL.\n");
      return (0);
    }

  theTextBox.celRect.rectf16_XLeft = wrapBox->rectf16_XLeft;
  theTextBox.celRect.rectf16_XRight = wrapBox->rectf16_XRight;
  theTextBox.celRect.rectf16_YTop = wrapBox->rectf16_YTop;
  theTextBox.celRect.rectf16_YBottom = wrapBox->rectf16_YBottom;
  theTextBox.wrapRect.rectf16_XLeft = wrapBox->rectf16_XLeft;
  theTextBox.wrapRect.rectf16_XRight = wrapBox->rectf16_XRight;
  theTextBox.wrapRect.rectf16_YTop = wrapBox->rectf16_YTop;
  theTextBox.wrapRect.rectf16_YBottom = wrapBox->rectf16_YBottom;
  theTextBox.align = theAlign->align;
  theTextBox.charPitch = theAlign->charPitch;
  theTextBox.linePitch = theAlign->linePitch;
  theTextBox.fgColor = fgColor;
  theTextBox.bgColor = bgColor;
  theTextBox.opaqueFlag = (fg_Opaque | bg_Opaque);
  theTextBox.otherFlags = 0;
  theTextBox.reserved = 0;

  KTextBoxMain (theFont, &theTextBox, theText, KTextLength2 (theText, textLen),
                &userCCB);

  return userCCB;
}

Int32
KTextBox_B (KFont3DO *theFont, KTextBox *theTextBox, uint8 *theText,
            Int32 textLen, CCB **userCCB)
{
  return (KTextBoxMain (theFont, theTextBox, theText,
                        KTextLength2 (theText, textLen), userCCB));
}
