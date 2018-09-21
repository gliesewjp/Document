/******************************************************************************
// FILE:    l3_calc.c
// TITLE:   NPC 3-Level converter controller calculation programs
// DESCRIPTION:
//          Calculation Programs.
//
// 2009-11-01	V1.0
******************************************************************************/

#include "l3_global.h"

/*��������
*****************************************************************************/

/*ȫ��һ�����
*****************************************************************************/
int16	iPdpint=0;					// ��PDPINT�жϼ�����



/*����
*****************************************************************************/
/*funName:		Eva_T1UF_isr()
* Inputs: 		NONE
* Outputs: 		NONE
* Description: 	EVA T1�����жϷ������
******************************************************************************/
void Eva_T1UF_isr(void)
{
	EvaRegs.EVAIMRA.bit.T1UFINT = 1;
    EvaRegs.EVAIFRA.bit.T1UFINT = 1;
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP2;
}

