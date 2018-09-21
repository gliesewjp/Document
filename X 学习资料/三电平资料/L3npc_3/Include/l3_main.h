/******************************************************************************
// FILE:    l3_main.h
// TITLE:   NPC 3-Level converter controller main program's include .h
// DESCRIPTION:
//          .
//
// 2009-10-15	V1.0
******************************************************************************/

#ifndef __L3_MAIN_H__
#define __L3_MAIN_H__


/*�ṹ�嶨��
*****************************************************************************/
/* ����״̬�� */
typedef struct{						//	bits	description
	Uint16	start		:1;			//	0
	Uint16	stop		:1;			//	1
	Uint16	reset		:1;			//	2
	Uint16	err			:1;			//	3
	Uint16	err_idcoc	:1;			//	4
	Uint16	err_udcov	:1;			//	5
	Uint16	err_1		:1;			//	6
	Uint16	err_2		:1;			//	7
	Uint16	NpcOn		:1;			//	8
}RUNFLAG_BIT;

typedef union{
   Uint16		all;
   RUNFLAG_BIT	bit;
}RUNFLAG;


/* L3C�ͽṹ�� */
typedef struct{
//״̬��
	RUNFLAG	flag;					//���б�־
	RUNFLAG	flagBak;				//��ʷ���б�־
//�Ƕ�
	_iq		qTheta;					//ͬ���Ƕ�
	_iq		qDTheta;				//����Ƕ�
	Uint16	uStep;					//����
//	_iq 	iqSina;
//	_iq 	iqCosa;
//������
	int16	iFaultC;				//�ܹ��ϼ�����
	int16	iStartC;				//Start����������
	int16	iStopC;					//Stop����������
	int16	iResetC;				//Reset����������
	int16	iNpcOnC;				//����NPC����������
	int16	iNpcOffC;				//ֹͣNPC����������
	int16	iErrTwinkC;				//Err���Ƽ�����
	//3������
	int16	iPdpintC;				//Pdpint������
	int16	iUdcovC;				//Udc��ѹ������
	int16	iIdcocC;				//Idc����������
//һ�����
	Uint16 uCap;					//Captureֵ
	_iq qAD[6];						//AD����ֵ IA,IB,IDC,INP,UDC,UNP
	_iq qIdR;						//d���������
	_iq qIqR;						//q���������
	_iq qVa,qVb,qVc;				//���������ѹָ��ֵ
	_iq qTa,qTb,qTc;				//��������ʱ��
	_iq	qUnpe;						//ֱ���е��ѹ��ֵ	=Unp-Unpref

}L3C;

/* L3C�ͽṹ���ʼ��ֵ */
#define L3C_DEFAULTS 	{\
			0xffff*KEY_OFF, 0xffff*KEY_OFF, \
			0, _IQ(DTHETA_DEFAULT_q), STEP_DEFAULT, \
			CINI, CINI, CINI, CINI, CINI, CINI, 1000,\
			CPDPINT, CINI, CINI, \
			0,{_IQ(0), _IQ(0), _IQ(0), _IQ(0), _IQ(0), _IQ(0)}, \
			_IQ(OPEN_REF_ID_q), _IQ(OPEN_REF_IQ_q), \
			_IQ(0), _IQ(0), _IQ(0), _IQ(0), _IQ(0), _IQ(0), _IQ(0)}


/* NPC�ͽṹ�� */
typedef struct{
//ֱ����ƽ�ͻ����(HZDCL)����
	_iq	qKb1;					//�µ�ƽ���ϵ��
	_iq	qKb2;					//�ϵ�ƽ���ϵ��
//ֱ����ƽ�ͻ�ƫ��(HSDCL)����
	_iq	qKs;						//ƫ��ϵ��
	_iq qKh;					//�����ƶ�ϵ��
	_iq qKr;					//�����ƶ�ϵ��
	_iq Km_sv;
//ֱ����ƽ����ƽ��(FBDCL)����
	_iq	qlbd;					//ǰ����������ϵ��
	/*	out(k) = ki0*in(k)+kin1*in(k-1)+kin2*in(k-2)
		 	  - [kout1*out(k-1)+kout2*out(k-2)]			*/
	_iq	qLpfKin0;				//��ͨ�˲������� 0��ϵ��
	_iq	qLpfKin1;				//��ͨ�˲�������-1��ϵ��
	_iq	qLpfKin2;				//��ͨ�˲�������-2��ϵ��
	_iq	qLpfKout1;				//��ͨ�˲������-1��ϵ��
	_iq	qLpfKout2;				//��ͨ�˲������-2��ϵ��
}NPC;

/* NPC�ͽṹ���ʼ��ֵ */
#define NPC_DEFAULTS 	{\
			1,1,\
			0,\
			0,\
			0,\
			0,\
			0.2,\
			_IQ(LPF_KIN0_q), _IQ(LPF_KIN1_q), _IQ(LPF_KIN2_q),\
			_IQ(LPF_KOUT1_q), _IQ(LPF_KOUT2_q)}


/*������������
*****************************************************************************/
extern L3C		l3c;				// L3C�ͽṹ��ʼ��ֵ
extern NPC		npc;
extern ADC		ad;
extern PIDREG3	pi_id;
extern PIDREG3	pi_iq;
extern PIDREG3	pi_vdc;
extern IPARK	ipark;
extern SVGENDQ	sv;
extern LPF		lpfUnpe;
extern LPF		lpfUdc;
//extern TLSVGENDQ  tlsv;
extern long		GlobalQ;

#if	CLOSE_CURRENT						// 0:��������(����idref, iqref)	1:�����ջ�ģʽ
extern CLARKE	clarke;
extern PARK		park;
#endif

/*������������
*****************************************************************************/
extern void IDEL();					// �������״̬



#if	FLASH_OR_RAM==1
extern Uint16 ramfuncs_loadstart;
extern Uint16 ramfuncs_loadend;
extern Uint16 ramfuncs_runstart;
#endif


#endif
