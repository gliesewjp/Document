/******************************************************************************
// FILE:    l3_global.h
// TITLE:   NPC 3-Level converter controller global define file
// DESCRIPTION:
//          Global define.
//
// 2009-10-15	V1.0
******************************************************************************/

#ifndef __L3_GLOBAL_H__
#define __L3_GLOBAL_H__

// !���ú�׺��_q�ĺ궨��ʱӦ����_IQ()

// ���Գ���
/*###########################################################################*/
// ϵͳ���ñ���
//#define	GLOBAL_Q		20//24//	// ��IQmathLib.h���޸�
#define	FLASH_OR_RAM		0	 		// 1:FLASH����			0:RAM����
#define	NORMAL_OR_TEST		1	 		// 1:��ʽ����			0:���Գ���
#define TEST_PWM			0			// 1:PWM����ռ�ձ�		0:PWMΪIO����	(NORMAL_OR_TEST=0��Ч)
#define PWM_ON				1//0//		// 1:ʹ��PWM���		0:��ֹPWM���	(NORMAL_OR_TEST=1��Ч)
#define	L3_NPC_FB			0//1//		// 1: ʹ������ƽ��(Flexible Blance)����		0:��ʹ��
#define L3_MODE				L3_SV//L3_SP//
#define L3_SV_MODE			L3_SV_SELECTSR//L3_SV_SMALLVEC//L3_SV_NONPC//
#define L3_SP_MODE		   	L3_SP_SR//L3_SP_SA//L3_SP_HS//L3_SP_NONPC//L3_SP_HZ//
#define	UDCREF				_IQ(200)	// ֱ��ĸ�ߵ�ѹ����ֵ
#define	UNPREF				_IQ(150)//_IQ(40)//UDCREF>>1	// ֱ���е��ѹ����ֵ

#define	CHEAT_STATE			0			// 1:����Ϊflag.err=S_OK,�Ͳ�ִ��CheckFault()��
#define	CHEAT_START			0//1//		// 1:����Ϊ����START��
#define	CHEAT_NO_IDCOC		0			// 1:����ֱ�������
#define	DUAL_EV_MODE		1			// 1:ʹ��˫EV�������ƽPWMģʽ
#define	REC_ALLTHETIME		1//0//		// 1:ѭ����¼ 			0:���μ�¼
#define	SAMPLE_PWM			1//4		// ÿSAMPLE_PWM�ν���һ�β���
#define ADUDC_DEAL			1			// 1:����UDC,UNPͨ�����˲��㷨����

// ����ֵ
#define OPEN_REF_ID_q		0.6//0.4//0.2//0.7147//0.8//	// D���������������ֵ
#define OPEN_REF_IQ_q		0			// Q���������������ֵ

// ����ģʽ
#define CLOSE_CURRENT		0			// 1: �����ջ�ģʽ		0:������������idref,iqref
#define	L3_SV				10			// ����ƽSVPWM����
	#define	L3_SV_NONPC		11			// SV���е����
	#define	L3_SV_SMALLVEC	12			// SVСʸ������
	#define L3_SV_SELECTSR	13			// SV����ѡ����

#define	L3_SP				20			// ����ƽSPWM����
	#define	L3_SP_NONPC		21			// SP���е����
	#define	L3_SP_HZ		22			// SP�ͻ����(Hysteresis Zoom)����
	#define	L3_SP_HS		23			// SP�ͻ�ƫ��(Hysteresis Shift)����
	#define	L3_SP_SA		24			// SP�ز�����()����
	#define	L3_SP_SR		25			// SP�ز�ѡ���Ϳ���
// NPC��ʽ
#define NPC_HYS_MODE		NPC_HYS_DOUBLE//NPC_HYS_SINGLE//	// �ͻ���ʽ
#define NPC_HYS_SINGLE		1			// ���ͻ�
#define NPC_HYS_DOUBLE		2			// ˫�ͻ�

// NPC����
#define	NPC_DEAD_q			1						// �ͻ�������Χ V
#define	NPC_DEAD_IN_q		0.5						// �ͻ���������Χ V
#define	NPC_KB_q			0.1//0.5//0.05//		// ���ϵ��
#define	NPC_KB_IN_q			(NPC_KB_q*5)			// ���ϵ��(������)
#define	NPC_KS_q			0.1//0.05//0.2			// ƫ��ϵ��
#define	NPC_KS_IN_q			(NPC_KS_q/5)			// ƫ��ϵ��(������)
#define	NPC_LPF_FC			1//10//50//				// ���Կ��Ƶ�ͨ�˲�����ֹƵ�� Hz
#define	NPC_LPF_LBD_q		1						// ���Կ���ǰ������ϵ��
#define	NPC_KH_q			0.1//0.2//0.05			// ����ϵ��
#define	NPC_KH_IN_q			(NPC_KH_q/5)			// 
#define	NPC_KR_q			0.2//0.05//0.2			// ѡ�����ƶ�ϵ��
#define	NPC_KR_IN_q			(NPC_KR_q/5)			// 
#define	NPC_KM_SV			0.5//0.05//0.2			// SVѡ�����ƶ�ϵ��
#define	NPC_KM_SV_q			(NPC_KM_SV/5)			// 
#define NPC_Ksmall_SV		0.5	
#define	NPC_Ksmall_SV_q    (NPC_Ksmall_SV/5) 
#define SPM					1					
// 2�����ֵ�ͨ�˲�������
#if		NPC_LPF_FC==1
	#define	LPF_KIN0_q		0.00000982591682
	#define	LPF_KIN1_q		0.00001965183364
	#define	LPF_KIN2_q		LPF_KIN0_q
	#define	LPF_KOUT1_q		-1.99111429220165
	#define	LPF_KOUT2_q		0.99115359586894
#elif	NPC_LPF_FC==10
	#define	LPF_KIN0_q		0.00094469184384
	#define	LPF_KIN1_q		0.00188938368768
	#define	LPF_KIN2_q		LPF_KIN0_q
	#define	LPF_KOUT1_q		-1.91119706742607
	#define	LPF_KOUT2_q		0.91497583480143
#elif	NPC_LPF_FC==50
	#define	LPF_KIN0_q		0.02008336556421
	#define	LPF_KIN1_q		0.04016673112842
	#define	LPF_KIN2_q		LPF_KIN0_q
	#define	LPF_KOUT1_q		-1.56101807580072
	#define	LPF_KOUT2_q		0.64135153805756
#endif

// DSPϵͳ
// ʱ��Ƶ��
/*###########################################################################*/
#define	F_SYS				150//120//			// ϵͳʱ��Ƶ��	MHz
#define	F_PWM				1000//2000//400//200//500//10000//	// ����Ƶ��		Hz
#define	KM					20//50//10//100//32//		// ���Ʊ�
#define DTHETA				_IQ((float)1/KM)	// �����		����floatֵ��Ϊ0
#define	LED_DELAY			((int)F_PWM/4)		// ���е�����ʱ,��Ϊ1/2Hz

// ����Ƶ��		F_PWM = F_SYS/[(2^TPS)*T1PR*2]
#if		F_SYS==150
 #if	F_PWM==10000	// ����Ƶ��10kHz
	#define	T1_TPS			0			// EV T1CON 1��Ƶ
	#define T1PR_VAL		7500		// T1PR����ֵ
 #elif	F_PWM==2000		// ����Ƶ��2kHz
 	#define	T1_TPS			0			// EV T1CON 1��Ƶ
	#define T1PR_VAL		37500
 #elif	F_PWM==1000		// ����Ƶ��1kHz
 	#define	T1_TPS			1			// EV T1CON 2��Ƶ
	#define T1PR_VAL		37500
 #elif	F_PWM==500		// ����Ƶ��500Hz
 	#define	T1_TPS			2			// EV T1CON 4��Ƶ
	#define T1PR_VAL		37500
 #elif	F_PWM==400		// ����Ƶ��400Hz
 	#define	T1_TPS			2			// EV T1CON 4��Ƶ
	#define T1PR_VAL		46875
 #elif	F_PWM==200		// ����Ƶ��200Hz
 	#define	T1_TPS			3			// EV T1CON 8��Ƶ
	#define T1PR_VAL		46875
 #endif
#elif	F_SYS==120
 #if	F_PWM==10000	// ����Ƶ��10kHz
	#define	T1_TPS			0			// EV T1CON��Ƶϵ��
	#define T1PR_VAL		6000		// T1PR����ֵ
 #elif	F_PWM==2000		// ����Ƶ��2kHz
 	#define	T1_TPS			0			// EV T1CON��Ƶϵ��
	#define T1PR_VAL		30000
 #elif	F_PWM==1000		// ����Ƶ��1kHz
 	#define	T1_TPS			1			// EV T1CON��Ƶϵ��
	#define T1PR_VAL		30000
 #elif	F_PWM==500		// ����Ƶ��500Hz
 	#define	T1_TPS			2			// EV T1CON��Ƶϵ��
	#define T1PR_VAL		30000
 #elif	F_PWM==400		// ����Ƶ��400Hz
 	#define	T1_TPS			2			// EV T1CON��Ƶϵ��
	#define T1PR_VAL		37500
 #elif	F_PWM==200		// ����Ƶ��200Hz
 	#define	T1_TPS			3			// EV T1CON��Ƶϵ��
	#define T1PR_VAL		37500
 #endif
#endif

// ����ʱ��		DBtime = T_HSPCLK*(DBT*(2^DBTPS))	(DBTPS<=5)
#define	DEADTIME		   	32//16//		//��λ0.1us
#if		F_SYS==150
 #if	DEADTIME==32
	#define	VDBT			0x0f
	#define	VDBTPS			5
 #elif	DEADTIME==16
	#define	VDBT			0x0f
	#define	VDBTPS			4
 #endif
#elif	F_SYS==120
 #if	DEADTIME==32
	#define	VDBT			0x0c
	#define	VDBTPS			5
 #elif	DEADTIME==16
	#define	VDBT			0x0c
	#define	VDBTPS			4
 #endif
#endif

// ���㳣��
/*###########################################################################*/
#define SQRT3				1.732050808	// sqrt(3)
#define SQRT_3				0.577350269	// 1/sqrt(3)
#define ONE_Q16				65535

#define CINI				5//10		// ��������ֵ
#define CMAX				100			// ���������ֵ
#define CPDPINT				5			// PDPINT�ж��������

#define	PI					3.1415926
#define	LOW_ACTIVE			1			// 1:����Ч

// ADC��س���
/*###########################################################################*/
// ADʱ�䳣��
#if		F_SYS==120
 #define CPU_CLOCK_SPEED	8.3333L		// for a 120MHz CPU clock speed
#elif	F_SYS==150
 #define CPU_CLOCK_SPEED	6.6667L		// for a 150MHz CPU clock speed
#endif
#define DELAY_US(A)			DSP28x_usDelay(((((long double) A * 1000.0L) / (long double)CPU_CLOCK_SPEED) - 9.0L) / 5.0L)
// DSP28x_usDelay()	��DSP281x_usDelay.asm�ж���õĻ����ʱ����, ������RAM�����в���ȷ����us��ʱ
// DELAY_US(A)		����usΪ��λ����ʱ����
// CPU_CLOCK_SPEED	��һ��ϵͳʱ���������ж���ns

#define	WATCH_POINTS		200			// �۲���ʱ��������������
#define AD_CH_NUM			6			// AD�źŸ���
// ADͨ����
#define AD_CH_IA			0			// IAʹ��ADCINA0
#define AD_CH_IB			1
#define AD_CH_IDC			2
#define AD_CH_INP			3
#define AD_CH_UDC			4
#define AD_CH_UNP			5
#define AD_CH_REF25			6		   // REF2.5Vʹ��ADCINA6
#define AD_CH_REF05			7		   // REF0.5Vʹ��ADCINA7
//ADͨ����ƫ		AD1 = AD-offset;
#define AD_OFF_IA			_IQ(-0.069)//0
#define AD_OFF_IB			_IQ(-0.056)//0
#define AD_OFF_IDC			0//_IQ(-0.08)
#define AD_OFF_INP			0//_IQ(-0.0)
#define AD_OFF_UDC		   _IQ(-0.0655)//	_IQ(-0.0643)//_IQ(-0.0650)//_IQ(-0.056)//0
#define AD_OFF_UNP			_IQ(-0.0170)//0
//ADͨ��ϵ��K	( ���������ֵV2 * K = ����·һ��ֵV1 )
// ����������: 12A/24mA, Rs=200��, �� V1/V2=(12/24m)/Rs=2.5, [-1.5,1.5]*2.5A-[0,3]V-[_IQ(-1.5),_IQ(1.5)]
#define AD_K_IA				_IQ(2.5)			// ������������Ϊ��
#define AD_K_IB				AD_K_IA
#define AD_K_IDC			-AD_K_IA			// ֱ��������Ϊ��, ��Ӳ������෴
#define AD_K_INP			-AD_K_IA
// ��ѹ������: 1000/2500,R1=30k��,Rs=50,�� V1/V2=R1/(Rs*2.5)=240, [-1.5,1.5]*240-[0,3]V-[_IQ(-1.5),_IQ(1.5)]
#define AD_K_UDC			_IQ(235.3)//_IQ(240)//
#define AD_K_UNP			_IQ(242.6)//AD_K_UDC//

// ADУ��ֵ
#define AD_REF_HIGH			22159  //22072	   // (2.511/3*4095*2^4)^0x8000=()Q15signed	[^0x8000=-2^15]
#define AD_REF_LOW			-21805//-21827	   // (0.501/3*4095*2^4)^0x8000=()Q15signed

// ����ͬ������
#define DTHETA_DEFAULT_q	(1/400)		// Ĭ�ϲ���Ƕ�
#define STEP_DEFAULT		25000		// Ĭ��2��CAP֮����С���?

// PI����
// PI_U
#define PI_KP_U				500//50//512//256//150//			// ��ѹ��PI����ϵ��		Q9
#define PI_KI_U		   		50//600//1500//10//0//				// ��ѹ��PI����ϵ��		Q13
#define PI_KD_U				0									// ��ѹ��PI΢��ϵ��		Q13
#define PI_MAX_U			1229//(30A)	 1638//					// ��ѹ��PI�޷�����		Q11
#define PI_MIN_U			-PI_MAX_U							// ��ѹ��PI�޷�����		Q11
#define PI_UI_MAX_U			PI_MAX_U//2047//1843//1024//		// ��ѹ��PI�������޷�����	Q11
#define PI_UI_MIN_U			-PI_UI_MAX_U						// ��ѹ��PI�������޷�����	Q11
// PI_I
#define PI_KP_I		   		1300//500//300//5000//				// ������PI�ȵ�		Q9
#define PI_KI_I		   		200//50//30//100//400//				// ������PI����ϵ��		Q13
#define PI_KD_I				0									// ������PI΢��ϵ��		Q13
#define PI_MAX_I			2047//1843//1500//1770//			// ������PI�޷�����		Q11
#define PI_MIN_I			-PI_MAX_I							// ������PI�޷�����		Q11
#define PI_UI_MAX_I			1843//2047//1024//PI_MAX_I//		// ������PI�������޷�����	Q11
#define PI_UI_MIN_I			-PI_UI_MAX_I						// ������PI�������޷�����	Q11


// L3Cϵͳ
/*###########################################################################*/
// IO
#define IO_REG				(volatile unsigned int *)0x70E4   /* I/O port B data & dir reg */
#define IN_IDCOC			8			//PB.8
#define IN_START			9			//PB.9
#define IN_STOP				10			//PB.10
#define IN_RESET			11			//PB.11

#define OUT_IDCOC			12			//PB.12
#define OUT_START			13			//PB.13
#define OUT_STOP			14			//PB.14
#define OUT_RESET			15			//PB.15

#define IN_BIT_START		GpioDataRegs.GPBDAT.bit.GPIOB9
#define IN_BIT_STOP			GpioDataRegs.GPBDAT.bit.GPIOB10
#define IN_BIT_NpcOn		GpioDataRegs.GPBDAT.bit.GPIOB8
#define IN_BIT_NpcOff		GpioDataRegs.GPBDAT.bit.GPIOB11
#define IN_BIT_IDCOC		GpioDataRegs.GPBDAT.bit.GPIOB8
#define IN_BIT_RESET		GpioDataRegs.GPBDAT.bit.GPIOB11

#define LED_BIT_START		GpioDataRegs.GPBDAT.bit.GPIOB13
#define LED_BIT_STOP		GpioDataRegs.GPBDAT.bit.GPIOB14
#define LED_BIT_NpcOn		GpioDataRegs.GPBDAT.bit.GPIOB12
#define LED_BIT_NpcOff		GpioDataRegs.GPBDAT.bit.GPIOB15
#define LED_BIT_IDCOC		GpioDataRegs.GPBDAT.bit.GPIOB12
#define LED_BIT_RESET		GpioDataRegs.GPBDAT.bit.GPIOB15

#define LED_BIT_TWI_IDCOC	GpioDataRegs.GPBTOGGLE.bit.GPIOB12
#define LED_BIT_TWI_START	GpioDataRegs.GPBTOGGLE.bit.GPIOB13
#define LED_BIT_TWI_STOP	GpioDataRegs.GPBTOGGLE.bit.GPIOB14
#define LED_BIT_TWI_RESET	GpioDataRegs.GPBTOGGLE.bit.GPIOB15
#define LED_ON				0
#define LED_OFF				1


// KEY STATE
#define	KEY_ON			0						// ���¼�
#define	KEY_OFF			1						// δ����

// PWM ACTION STATE
#ifdef	LOW_ACTIVE
 #define FORCE_ACTIVE	0						// ����ǿ����Ч
 #define FREE_UP_ACTIVE	1						// �����Ϲ���Ч
 #define FREE_LO_ACTIVE	2						// �����¹���Ч
 #define FORCE_INACTIVE	3						// ����ǿ����Ч
#else
 #define FORCE_ACTIVE	3						// ����ǿ����Ч
 #define FREE_UP_ACTIVE	2						// �����Ϲ���Ч
 #define FREE_LO_ACTIVE	1						// �����¹���Ч
 #define FORCE_INACTIVE	0						// ����ǿ����Ч
#endif

// CAN STATE
#define	S_OK			0x0000					// ����״̬
#define SOK_HOLDON		0x0000					// ����״̬
#define SOK_CHARGING	0x3333					// ���״̬
#define	SOK_RUN			0xCCCC					// ����״̬

#define	S_ERROR			0xFFFF					// ����״̬
#define	SE_UACLV		0xC0C0					// ����Ƿѹ
#define	SE_UACOV		0xA0A0					// ������ѹ
#define	SE_IDCOC		0x9898					// ĸ�߹���
#define	SE_UDCOV		0x8484					// ĸ�߹�ѹ
#define	SE_PDPINT		0x8282					// IGBT����
#define	SE_OT			0x8181					// IGBT����
#define	S_ERROR_HALT	0xFFFF					// ����״̬��ֹͣ 20091028

// �꺯��
/*###########################################################################*/
#define SETBIT(DMA,BITID) 		*DMA|=1<<BITID						// ��*DMA�ĵ�BITIDλ��1
#define CLRBIT(DMA,BITID) 		*DMA&=(0xffff-(1<<BITID))			// ��*DMA�ĵ�BITIDλ��0
#define	TESTBIT0(DMA,BITID)		((*DMA&(1<<BITID))==0)				// ����*DMA�ĵ�BITIDλΪ0?
#define	TESTBIT1(DMA,BITID)		((*DMA|(0xffff-(1<<BITID)))==0xffff)// ����*DMA���BITIDλΪ1?
//#define SETFLAG(FLAG,BITID) 	FLAG|=1<<BITID						// ��*DMA�ĵ�BITIDλ��1
//#define CLRFLAG(FLAG,BITID) 	FLAG&=(0xffff-(1<<BITID))			// ��*DMA�ĵ�BITIDλ��0
//#define	FLAGBIT0(FLAG,BITID)	((FLAG&(1<<BITID))==0)				// ����FLAG�ĵ�BITIDλ0?
//#define	FLAGBIT1(FLAG,BITID)	((FLAG|(0xffff-(1<<BITID)))==0xffff)// ����FLAG�ĵ�BITIDλΪ1?
#define	NOP()					asm("	nop				")			// ��ָ��


// Include files
/*###########################################################################*/
#include "DSP281x_Device.h"

#include "dmc_ipark.h"
#include "dmc_pid_reg3.h"
#include "dmc_svgen_dq.h"

#if	CLOSE_CURRENT						// 0:��������(����idref, iqref)	1:�����ջ�ģʽ
#include "dmc_clarke.h"
#include "dmc_iclarke.h"
#include "dmc_park.h"
#endif

#include "l3_adc.h"
#include "l3_ints.h"
#include "_lpf.h"
#include "TLsvgen_dq.h"
#include "l3_main.h"


#endif
