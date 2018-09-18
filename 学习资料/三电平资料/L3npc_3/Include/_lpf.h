/******************************************************************************
// FILE:    _lpf.h
// TITLE:   Low pass filter program's include .h
// DESCRIPTION:
//          .
//
// 2010-03-21	V1.0
******************************************************************************/

#ifndef __LPF_H__
#define __LPF_H__

#include "IQmathLib.h"

/*�ṹ�嶨��
*****************************************************************************/
/* lpf�ͽṹ�� */
/*****************************************************************************/
typedef struct {
	_iq	qIn[3];						// Input buffer
	_iq	qOut[3];					// Output buffer
	_iq	qkin[3];					// Input coefficint
	_iq	qkout[3];					// Output coefficint

	void (*calc)();
}LPF;

/* LPF�ͽṹ���ʼ��ֵ */
#define LPF_DEFAULTS {\
	{_IQ(0),_IQ(0),_IQ(0)},\
	{_IQ(0),_IQ(0),_IQ(0)},\
	{_IQ(1),_IQ(1),_IQ(1)},\
	{_IQ(1),_IQ(1),_IQ(1)},\
	(void (*)(Uint32))LPF_calc \
	}
typedef LPF *LPF_handle;

/*������������
*****************************************************************************/
extern void LPF_calc(LPF_handle);		// LPF calculation


#endif
