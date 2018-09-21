/******************************************************************************
// FILE:    l3_adc.c
// TITLE:   NPC 3-Level converter controller ADC program
// DESCRIPTION:
//          ADC Program.
//
// 2009-10-24	V1.0
******************************************************************************/

#include "l3_global.h"

/*ȫ��һ�����
*****************************************************************************/


/*����
*****************************************************************************/
#define	ONE_HALF_Q16	98304//_IQ16(1.5)//
/*funName:		ADC_read()
* Inputs: 		NONE
* Outputs: 		NONE
* Description: 	��ȡ,ת������6��ADͨ��ֵRESULTx(��12λ��Ч��), ����Gain��Offset����, ���Ϊ_iq��ʽ
*				�Ӳ����������Y��ADC����ֵX�Ķ�Ӧ��ϵΪ: [-1.5V, 1.5V]-->[0xfff0, 0]
*				��: Y = 1.5-(X*3/65520) = 1.5 - (3/65520)*X = (98304)q16 - (3)q16*X
******************************************************************************/
void ADC_read(ADC_handle p)
{
    _iq16	dat1,dat2;
	_iq		tmp;

	/* Wait until ADC conversion is completed */
	while (AdcRegs.ADCST.bit.SEQ1_BSY == 1)  	// �ȴ�ADCת�����
	{};

// AD_CH_IA
	dat1 = AdcRegs.ADCRESULT0 & 0xfff0;					// 281x ADC����12λ��Ч
	dat1 = ONE_HALF_Q16-dat1*3;
	dat2 = AdcRegs.ADCRESULT1 & 0xfff0;
	dat2 = ONE_HALF_Q16-dat2*3;
	tmp	 = _IQ16toIQ( (dat1+dat2)>>1 );
	tmp -= p->qOffs[AD_CH_IA];							// ��ƫ����
	p->qOut[AD_CH_IA] = _IQmpy(tmp, p->qGain[AD_CH_IA]);// ���洦��
	
// AD_CH_IB
	dat1 = AdcRegs.ADCRESULT2 & 0xfff0;
	dat1 = ONE_HALF_Q16-dat1*3;
	dat2 = AdcRegs.ADCRESULT3 & 0xfff0;
	dat2 = ONE_HALF_Q16-dat2*3;
	tmp	 = _IQ16toIQ( (dat1+dat2)>>1 );
	tmp -= p->qOffs[AD_CH_IB];
	p->qOut[AD_CH_IB] = _IQmpy(tmp, p->qGain[AD_CH_IB]);

// AD_CH_IDC
	dat1 = AdcRegs.ADCRESULT4 & 0xfff0;
	dat1 = ONE_HALF_Q16-dat1*3;
	dat2 = AdcRegs.ADCRESULT5 & 0xfff0;
	dat2 = ONE_HALF_Q16-dat2*3;
	tmp	 = _IQ16toIQ( (dat1+dat2)>>1 );
	tmp -= p->qOffs[AD_CH_IDC];
	p->qOut[AD_CH_IDC] = _IQmpy(tmp, p->qGain[AD_CH_IDC]);
		
// AD_CH_INP
	dat1 = AdcRegs.ADCRESULT6 & 0xfff0;
	dat1 = ONE_HALF_Q16-dat1*3;
	dat2 = AdcRegs.ADCRESULT7 & 0xfff0;
	dat2 = ONE_HALF_Q16-dat2*3;
	tmp	 = _IQ16toIQ( (dat1+dat2)>>1 );
	tmp -= p->qOffs[AD_CH_INP];
	p->qOut[AD_CH_INP] = _IQmpy(tmp, p->qGain[AD_CH_INP]);
		
// AD_CH_UDC
	dat1 = AdcRegs.ADCRESULT8 & 0xfff0;
	dat1 = ONE_HALF_Q16-dat1*3;
	dat2 = AdcRegs.ADCRESULT9 & 0xfff0;
	dat2 = ONE_HALF_Q16-dat2*3;
	tmp	 = _IQ16toIQ( (dat1+dat2)>>1 );
	tmp -= p->qOffs[AD_CH_UDC];
	p->qOut[AD_CH_UDC] = _IQmpy(tmp, p->qGain[AD_CH_UDC]);
		
// AD_CH_UNP
	dat1 = AdcRegs.ADCRESULT10 & 0xfff0;
	dat1 = ONE_HALF_Q16-dat1*3;
	dat2 = AdcRegs.ADCRESULT11 & 0xfff0;
	dat2 = ONE_HALF_Q16-dat2*3;
	tmp	 = _IQ16toIQ( (dat1+dat2)>>1 );
	tmp -= p->qOffs[AD_CH_UNP];
	p->qOut[AD_CH_UNP] = _IQmpy(tmp, p->qGain[AD_CH_UNP]);
	
	AdcRegs.ADCTRL2.bit.RST_SEQ1 = 1;         // Reset SEQ1
}


/*funName:		ADC_offset()
* Inputs: 		NONE
* Outputs: 		NONE
* Description: 	����ADREF_HIGH��ADREF_LOWͨ��ֵУ��ADC��Gain��Offset
*				Gain 	= (xh-xl)/(yh-yl)
*				Offset	= yl*Gain-xl
******************************************************************************/
void ADC_offset(ADC_handle p)
{
	long 	dx,dy,yl,yh;
	_iq15	tt;
	
	yh = p->qOut[AD_CH_REF25];
	yl = p->qOut[AD_CH_REF05];
	dx = (long)AD_REF_HIGH - AD_REF_LOW;
	dy = yh-yl;
	tt = _IQ15div(dx,dy);
	p->qAll_gain = _IQ15mpy(yl,tt)-AD_REF_LOW;
	p->qAll_gain = tt>>2;							// IQ15toIQ13
}

/*funName:		ADC_init()
* Inputs: 		NONE
* Outputs: 		NONE
* Description: 	��ʼ��ADC
******************************************************************************/
void ADC_init(ADC_handle p)
{
	DELAY_US(5000L);						// ADC��λǰ��ʱ8ms
	AdcRegs.ADCTRL1.bit.RESET		= 1;
	NOP();
	NOP();
	AdcRegs.ADCTRL1.bit.RESET		= 0;

	AdcRegs.ADCTRL1.bit.SUSMOD		= 3;	// 
	AdcRegs.ADCTRL1.bit.ACQ_PS		= 1;	// SOC������Ϊ1+1��ADCLK
	AdcRegs.ADCTRL1.bit.CPS			= 0;	// ADCLK = FCLK/1
	AdcRegs.ADCTRL1.bit.CONT_RUN	= 0;	// 0:������ת��;		1:�����Զ�ת��;
	AdcRegs.ADCTRL1.bit.SEQ_CASC	= 1;	// 0:2������8ͨ��SEQ; 	1:������16ͨ��SEQ;
   
   	// ��϶�Ͳο���·�ϵ�
	AdcRegs.ADCTRL3.bit.ADCBGRFDN	= 0x3;
	DELAY_US(8000L);						// ADC�ϵ�����ǰ��ʱ8ms
  	// ����϶�Ͳο���·֮���ADC����ģ���·�ϵ�
    AdcRegs.ADCTRL3.bit.ADCPWDN		= 1;	// Power up rest of ADC 
 	DELAY_US(20L);

   	// ADC�ں�ʱ�ӷ�Ƶ�� ADCLK�� HSPCLK/[ADCCLKPS[3:0]*(ADCTRL1[7]+1)]
	AdcRegs.ADCTRL3.bit.ADCCLKPS 	= 3;	// ADCCLK=150M/6=25MHz 
	AdcRegs.ADCTRL3.bit.SMODE_SEL	= 0;	// 0:˳�����ģʽ;		1:ͬʱ����ģʽ; (281x��A,B�����ɱ�)

	// ȷ������������, ��������β�����ͨ����
	AdcRegs.ADCMAXCONV.bit.MAX_CONV1= 12;	// 6ͨ��������2��
	AdcRegs.ADCCHSELSEQ1.bit.CONV00 = AD_CH_IA;
	AdcRegs.ADCCHSELSEQ1.bit.CONV01 = AD_CH_IA; 
	AdcRegs.ADCCHSELSEQ1.bit.CONV02 = AD_CH_IB;
	AdcRegs.ADCCHSELSEQ1.bit.CONV03 = AD_CH_IB;
	AdcRegs.ADCCHSELSEQ2.bit.CONV04 = AD_CH_IDC;
	AdcRegs.ADCCHSELSEQ2.bit.CONV05 = AD_CH_IDC;
	AdcRegs.ADCCHSELSEQ2.bit.CONV06 = AD_CH_INP;
	AdcRegs.ADCCHSELSEQ2.bit.CONV07 = AD_CH_INP;
	AdcRegs.ADCCHSELSEQ3.bit.CONV08 = AD_CH_UDC;
	AdcRegs.ADCCHSELSEQ3.bit.CONV09 = AD_CH_UDC;
	AdcRegs.ADCCHSELSEQ3.bit.CONV10 = AD_CH_UNP;
	AdcRegs.ADCCHSELSEQ3.bit.CONV11 = AD_CH_UNP;

	// �����Ƿ�ʹ��ϵͳ�¼�����AD
	AdcRegs.ADCTRL2.bit.RST_SEQ1	= 0;	// 0:�޶���;			1:������λSEQ
	AdcRegs.ADCTRL2.bit.SOC_SEQ1	= 0;	// 0:�����������;		1:�������ADC
	AdcRegs.ADCTRL2.bit.EVA_SOC_SEQ1= 1;	// 1:EVA����SEQ1/SEQ��Ч
	AdcRegs.ADCTRL2.bit.INT_ENA_SEQ1= 0;	// 1:SEQ�������ж���Ч
	AdcRegs.ADCTRL2.bit.INT_MOD_SEQ1= 0;	// 0:ÿ��SEQ���ж�;		1:����SEQ���ж�
	AdcRegs.ADCTRL2.bit.EXT_SOC_SEQ1= 0;	// 1:ADCSOC��������ADC��Ч
	AdcRegs.ADCTRL2.bit.RST_SEQ2	= 0;
	AdcRegs.ADCTRL2.bit.SOC_SEQ2	= 0;
	AdcRegs.ADCTRL2.bit.EVB_SOC_SEQ2= 0;
	AdcRegs.ADCTRL2.bit.INT_ENA_SEQ2= 0;
	AdcRegs.ADCTRL2.bit.INT_MOD_SEQ2= 0;
	AdcRegs.ADCTRL2.bit.EVB_SOC_SEQ2= 0;
		
	// ��ADC��־
	AdcRegs.ADCST.bit.INT_SEQ1_CLR	= 1;	// SEQ1�жϱ�־
	AdcRegs.ADCST.bit.INT_SEQ2_CLR	= 1;	// SEQ2�жϱ�־
}  

/*funName:		ADC_Start()
* Inputs: 		NONE
* Outputs: 		NONE
* Description: 	����ADC��SEQ
******************************************************************************/
void ADC_start(ADC_handle p)
{
	AdcRegs.ADCTRL2.bit.SOC_SEQ1 = 1;
}

//===========================================================================
// No more.
//===========================================================================
