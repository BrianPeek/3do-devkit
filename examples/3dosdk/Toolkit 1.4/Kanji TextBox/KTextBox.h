#ifndef _KTEXTBOX_H
#define _KTEXTBOX_H
/*
#ifndef _CTYPE_H
#include "ctype.h"
#endif
*/
#ifndef _KTEXTBOXERROR_H
#include "KTextBoxError.h"
#endif

#define justLeft 0    /* ������ */
#define justRight 1   /* �������� */
#define justCenter 2  /* �E���� */
#define justJustify 3 /* �ϓ����� */

#define SJIS_KANJI_HIGH(c)                                                    \
  (((0x81 <= (c)) && ((c) <= 0x9f)) || ((0xe0 <= (c)) && ((c) <= 0xfc)))
#define SJIS_KANJI_LOW(c)                                                     \
  (((0x40 <= (c)) && ((c) <= 0x7e)) || ((0x80 <= (c)) && ((c) <= 0xfc)))
#define SJIS_KANJI_LEVEL2(c) ((0x989f <= (c)) && ((c) <= 0xeafc))
#define NUM_KANJI_LEVEL1 3760

#define fg_Opaque 0x01 /* �����F�ɕs����������ݒ� */
#define bg_Opaque 0x02 /* �w�i�F�ɕs����������ݒ� */

typedef struct
{
  short align;     /* ���񂹁A�����A�E��*/
  Int32 charPitch; /* ���ԁi�h�b�g�P�ʁj*/
  Int32 linePitch; /* �s�ԁi�h�b�g�P�ʁj*/
} TextAlign;

typedef struct
{
  Rectf16 celRect;  /* �Z���̑傫�����w�� */
  Rectf16 wrapRect; /* �e�L�X�g�`��̈���w�� */
  uint16 align;     /* �e�L�X�g�̑������̎w�� */
  Int32 charPitch;  /* ���Ԃ��w�� */
  Int32 linePitch;  /* �s�Ԃ��w�� */
  Color fgColor;    /* �����F���w�� */
  Color bgColor;    /* �w�i�F���w�� */
  uint8 opaqueFlag; /* �s�����������w�� */
  uint8 otherFlags; /* �\\�� */
  Int32 reserved;   /* �\\�� */
} KTextBox;

typedef struct
{
  uint16 nChars;      /* �܂܂�镶���R�[�h�̐�*/
  uint16 minCode;     /* �ŏ��̕����R�[�h*/
  uint16 maxCode;     /* �ő�̕����R�[�h*/
  uint8 charHeight;   /* �����̍���(pixels) */
  uint8 charWidth;    /* �����̕�(pixels) */
  uint32 oneCharSize; /* �P�����̃s�N�Z���f�[�^�̃o�C�g��(pad byte���܂�) */
  uint16 *codeTbl;    /* �����R�[�h�e�[�u���ւ̃|�C���^ */
  uint8 *widthTbl;    /* �σs�b�`���̕������e�[�u���ւ̃|�C���^ */
  char *charData;     /* �s�N�Z���f�[�^�̐擪�|�C���^*/
} KFontRec;           /* 6 x 4 = 24 bytes */

typedef struct
{
  uint8 font_Gpp;           /* Gray scale bits per pixel*/
  uint8 font_Bpp;           /* Bits per pixel */
  KFontRec font_Hinfo;      /* ���p�����pFontRec�\\���� */
  KFontRec font_Zinfo;      /* �S�p�����pFontRec�\\���� */
  Stream *font_fs;          /* font file stream */
  Boolean font_FullRead;    /* <HPP 03-08-94> if true read whole font image */
  uint32 font_HFSeekOffset; /* <HPP&EL 03-09-94> */
  uint32 font_ZFSeekOffset; /* <HPP&EL 03-09-94>	*/
  Boolean fsRead;
  uint32 maxCharSize; /* maximum char size */
  IOInfo romioInfo;
  Item romIOReqItem;
  IOReq *romior;
  uint32 logRomBlockSize;

} KFont3DO; /* 4 + 24 + 24 = 52 bytes */

/****************************************************************************/
extern Int32 KLoadFont (char *name, KFont3DO *theFont, Boolean fullRead);

extern Int32 KFreeFont (KFont3DO *theFont);

extern CCB *KTextBox_A (KFont3DO *theFont, uint8 *theText, Int32 textLen,
                        Rectf16 *wrapBox, TextAlign *theAlign, Color fgColor,
                        Color bgColor, CCB *userCCB);

extern Int32 KTextBox_B (KFont3DO *theFont, KTextBox *theTextBox,
                         uint8 *theText, Int32 textLen, CCB **userCCB);

extern Int32 KTextLength (uint8 *theText);

extern Int32 KFreeCel (CCB *theCCB);

extern Int32 KGetCharWidth (KFont3DO *theFont, uint16 charVal);

extern Int32 KGetCharHeight (KFont3DO *theFont, uint16 charVal);

#endif /* _KTEXTBOX_H */
