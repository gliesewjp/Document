/******************************************************************************
// FILE:    _lpf.c
// TITLE:   Low pass filter program
// DESCRIPTION:
//		out(k)=kin0*in(k)+kin1*in(k-1)+kin2*in(k-2)
//			  -kout1*out(k-1)-kout2*out(k-2)]
// 2010-03-21	V1.0
******************************************************************************/

#include "_lpf.h"

/*��������
*****************************************************************************/
void LPF_calc(LPF_handle);

/*ȫ��һ�����
*****************************************************************************/


/*����
*****************************************************************************/
/*funName:		LPF_calc(LPF_handle)
* Inputs: 		NONE
* Outputs: 		NONE
* Description: 	��ͨ�˲�����
******************************************************************************/
void LPF_calc(LPF_handle p)
{
	_iq	qtemp;
	
	qtemp = _IQmpy(p->qIn[0],  p->qkin[0]);
	qtemp+= _IQmpy(p->qIn[1],  p->qkin[1]);
	qtemp+= _IQmpy(p->qIn[2],  p->qkin[2]);
	qtemp-= _IQmpy(p->qOut[1], p->qkout[1]);
	qtemp-= _IQmpy(p->qOut[2], p->qkout[2]);
	
	p->qOut[0] = qtemp;
}

