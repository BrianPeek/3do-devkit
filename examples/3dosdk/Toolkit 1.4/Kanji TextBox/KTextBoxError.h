#ifndef _KTEXTBOXERROR_H
#define _KTEXTBOXERROR_H

#ifndef _OPERROR_H
#include "operror.h"
#endif

#define ER_KTextBox_NoError 		 0 /* ����I�� */
#define ER_KTextBox_BadParameter	-1 /* �p�����[�^���s���ł� */
#define ER_KTextBox_CannotMemAlloc	-2 /* ���������m�ۂł��܂��� */
#define ER_KTextBox_NotFound		-3 /* �T�[�`�Ώۂ�������܂��� */
#define ER_KTextBox_BadCharCode		-4 /* �����R�[�h���s���ł� */
#define ER_KTextBox_BadFontFile		-5 /* �t�H���g�t�@�C�����s���ł� */
#define ER_KTextBox_CannotOpenDS	-6 /* �f�B�X�N�X�g���[�����I�[�v���ł��܂��� */

#endif /* _KTEXTBOXERROR_H */
