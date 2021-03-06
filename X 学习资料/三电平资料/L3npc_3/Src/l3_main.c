/******************************************************************************
// FILE:    l3_main.c
// TITLE:   NPC 3-Level converter controller main program
// DESCRIPTION:
//          Main Program.
//
// 2009-10-15	V1.0
******************************************************************************/

#include "l3_global.h"

/*声明函数
*****************************************************************************/
void InitSysCtrl();				// 初始化系统控制寄存器
void InitPieCtrl();				// 初始化外设中断控制、标志寄存器
void InitPieVectTable();		// 初始化中断向量表
void InitGpio();				// 初始化IO
void InitPeripherals();			// 初始化外设
void InitEv();					// 初始化EV
void InitL3C();					// 初始化L3C

void CheckNormal();				// 判断运行状态,正常则开中断
void CheckFault();				// 判断故障输入信号
void IDEL();					// 进入待机状态

void test_io_ad(void);			// IO,PWM,AD等基本功能调试
void delayx(Uint16);			// 执行n个单位的小延时

/*全局一般变量
*****************************************************************************/
int32  GlobalQ = GLOBAL_Q;		// 定义此变量可配合IQmath.gel直接Watch IQ型变量

/*全局结构体变量
*****************************************************************************/
#pragma DATA_SECTION(l3c, "L3CRegsFile");
L3C		l3c		= L3C_DEFAULTS;			// L3C型结构初始化值
NPC		npc		= NPC_DEFAULTS;			// NPC型结构初始化值
ADC		ad		= ADC_DEFAULTS;
PIDREG3	pi_id	= PIDREG3_DEFAULTS;
PIDREG3	pi_iq	= PIDREG3_DEFAULTS;
PIDREG3	pi_vdc	= PIDREG3_DEFAULTS;
IPARK	ipark	= IPARK_DEFAULTS;
SVGENDQ	sv		= SVGENDQ_DEFAULTS;
LPF		lpfUdc	= LPF_DEFAULTS;
LPF		lpfUnp	= LPF_DEFAULTS;			// AD采样处理用
LPF		lpfUnpe	= LPF_DEFAULTS;			// FBDCL用
TLSVGENDQ tlsv  = TLSVGENDQ_DEFAULTS; 

#if	CLOSE_CURRENT						// 0:电流开环(给定idref, iqref)	1:电流闭环模式
CLARKE	clarke	= CLARKE_DEFAULTS;
PARK	park	= PARK_DEFAULTS;
//ICLARKE	iclarke	= ICLARKE_DEFAULTS;
#endif

/*主函数
*****************************************************************************/
/*funName:		main()
* Inputs: 		NONE
* Outputs: 		NONE
* Description: 	主函数
******************************************************************************/
void main(void)
{
	InitSysCtrl();				// 初始化系统控制寄存器
	InitGpio();					// 初始化IO
	DINT;						// 关闭CPU中断
	InitPieCtrl();				// 初始化外设中断控制、标志寄存器
	IER = 0x0000;				// 禁用CPU中断, 清CPU中断标志
	IFR = 0x0000;
	InitPieVectTable();			// 初始化中断向量表
	InitPeripherals();			// 初始化外设
	InitL3C();					// 初始化L3C

#if	FLASH_OR_RAM==1				// 初始化Flash
    memcpy(	&ramfuncs_runstart,
			&ramfuncs_loadstart,
			&ramfuncs_loadend - &ramfuncs_loadstart);
	InitFlash();
#endif

// 关联中断程序与中断向量表
	EALLOW;
	PieVectTable.T1UFINT = &Eva_T1UF_isr;
	PieVectTable.PDPINTA = &Eva_PDPINT_isr;
	EDIS;

// 初始化中断
	EvaRegs.EVAIMRA.bit.PDPINTA		= 1;
    EvaRegs.EVAIFRA.bit.PDPINTA		= 1;
	PieCtrlRegs.PIEIER1.bit.INTx1	= 1;	// 使能PDPINTA中断

 	EvaRegs.EVAIMRA.bit.T1UFINT		= 1;
    EvaRegs.EVAIFRA.bit.T1UFINT		= 1;
	PieCtrlRegs.PIEIER2.bit.INTx6	= 1;	// 使能T1UF中断

//	EvaRegs.EVAIMRA.bit.CAP1INT		= 1;
//	EvaRegs.EVAIFRA.bit.CAP1INT		= 1;
//	PieCtrlRegs.PIEIER2.bit.INTx1	= 1;	// 使能CAP1中断

	IER |= M_INT1;							// 使能CPU INT1(PDPINTA)
	IER |= M_INT2;							// 使能CPU INT2(T1UFINT)
	IFR = 0;								// 清除所有CPU中断标志
		
// User specific code:
#if CHEAT_START
	l3c.flag.bit.start = KEY_ON;				// 自动给一次START按键
#endif 

#if	NORMAL_OR_TEST
	for( ; ;){
	#if CHEAT_STATE								// CHEAT_STATE==1:总认为apf.uState==S_OK
		l3c.flag.bit.err = KEY_OFF;
	#else
		if(l3c.flag.bit.start)					//打开开关管后开始检测错误
			CheckFault();
	#endif
		CheckNormal();							// 无故障则判断运行状态
	}
#else
 #if	TEST_PWM
    EALLOW;
	EvaRegs.T1CON.bit.TENABLE	= 1;	// Enable T1
	EvaRegs.COMCONA.bit.FCOMPOE	= 1;	// Full compare output: enable
	EDIS;
	for(;;){
//		EvaRegs.COMCONA.bit.FCOMPOE = 1;
	}
 #else
    EALLOW;
	GpioMuxRegs.GPAMUX.all &= 0xFFC0;	// 配置pwm1~6引脚为IO口
	GpioMuxRegs.GPBMUX.all &= 0xFFC0;	// 配置pwm7~12引脚为IO口
 	GpioMuxRegs.GPADIR.all |= 0x003F;	// 输出
	GpioMuxRegs.GPBDIR.all  = 0xF03F;	// B0~5输出,B12~15输出
	EDIS;
	for(;;){
    	test_io_ad();
	}
 #endif

#endif

}


/*一般函数
*****************************************************************************/
/*funName:		InitGpio()
* Inputs: 		NONE
* Outputs: 		NONE
* Description: 	初始化IO
******************************************************************************/
void InitGpio(void)
{
	// IO配置位重启默认为0
	EALLOW;
// GPA
	GpioMuxRegs.GPAMUX.bit.PWM1_GPIOA0		= 1;
	GpioMuxRegs.GPAMUX.bit.PWM2_GPIOA1		= 1;
	GpioMuxRegs.GPAMUX.bit.PWM3_GPIOA2		= 1;
	GpioMuxRegs.GPAMUX.bit.PWM4_GPIOA3		= 1;
	GpioMuxRegs.GPAMUX.bit.PWM5_GPIOA4		= 1;
	GpioMuxRegs.GPAMUX.bit.PWM6_GPIOA5		= 1;	
	GpioMuxRegs.GPAMUX.bit.CAP1Q1_GPIOA8	= 1;
	GpioMuxRegs.GPAMUX.bit.CAP2Q2_GPIOA9	= 1;

// GPB
	GpioMuxRegs.GPBMUX.bit.PWM7_GPIOB0		= 1;
	GpioMuxRegs.GPBMUX.bit.PWM8_GPIOB1		= 1;
	GpioMuxRegs.GPBMUX.bit.PWM9_GPIOB2		= 1;
	GpioMuxRegs.GPBMUX.bit.PWM10_GPIOB3		= 1;
	GpioMuxRegs.GPBMUX.bit.PWM11_GPIOB4		= 1;
	GpioMuxRegs.GPBMUX.bit.PWM12_GPIOB5		= 1;
	
	GpioMuxRegs.GPBDIR.bit.GPIOB8			= 0;	// In0
	GpioMuxRegs.GPBDIR.bit.GPIOB9			= 0;	// In1
	GpioMuxRegs.GPBDIR.bit.GPIOB10			= 0;	// In2
	GpioMuxRegs.GPBDIR.bit.GPIOB11			= 0;	// In3
	GpioMuxRegs.GPBDIR.bit.GPIOB12			= 1;	// Out0
	GpioMuxRegs.GPBDIR.bit.GPIOB13			= 1;	// Out1
	GpioMuxRegs.GPBDIR.bit.GPIOB14			= 1;	// Out2
	GpioMuxRegs.GPBDIR.bit.GPIOB15			= 1;	// Out3

// GPD
	GpioMuxRegs.GPDMUX.bit.T1CTRIP_PDPA_GPIOD0	= 1;	// PDPAINT
	GpioMuxRegs.GPDQUAL.all	= 0xff;						// (0xff*2)个SYSCLKOUT时长判输入有效

// GPE
//	GpioMuxRegs.GPEDIR.bit.GPIOE0	= 0;

// GPF
//	GpioMuxRegs.GPFMUX.bit.MCLKXA_GPIOF8	= 0;

//	GpioMuxRegs.GPFDIR.bit.GPIOF14 = 0;

// GPG_MUX
//	GpioMuxRegs.GPGMUX.bit.SCIRXDB_GPIOG5 = 1;

	EDIS;
}

/*funName:		InitPeripherals()
* Inputs: 		NONE
* Outputs: 		NONE
* Description: 	初始化外设
******************************************************************************/
void InitPeripherals()
{
//	// Initialize CPU Timers To default State:
//	InitCpuTimers();

//	// Initialize McBSP Peripheral To default State:
//	InitMcbsp();

	// Initialize Event Manager Peripheral To default State:
	InitEv();

    // Initialize ADC Peripheral To default State:
    ad.init(&ad);

//	// Initialize eCAN Peripheral To default State:
//    InitECan();

//	// Initialize SPI Peripherals To default State:
//	InitSpi();

//	// Initialize SCI Peripherals To default State:
//	InitSci();
}

/*funName:		InitEv()
* Inputs: 		NONE
* Outputs: 		NONE
* Description: 	初始化EV
******************************************************************************/
void InitEv(void)
{
// EVA Control Register
//	EvaRegs.EXTCONA.bit.INDCOE	= 0;	// Compare output independent mode: disable

// Timer x Control Register
	EvaRegs.T1CON.bit.FREE		= 0;
	EvaRegs.T1CON.bit.SOFT		= 0;	// Stop immediately on emulation suspend
	EvaRegs.T1CON.bit.TMODE		= 1;	// Count Mode: continuous-Up/-Down
	EvaRegs.T1CON.bit.TPS		= T1_TPS;// TxCLK=HSPCLK/(2^TPS)=150MHz	HSPCLK=150MHz
	EvaRegs.T1CON.bit.TCLKS10	= 0;	// Clock source use: internal 
	EvaRegs.T1CON.bit.TCLD10	= 0;	// T1CMPR reload when: T1CNT=0
	EvaRegs.T1CON.bit.TECMPR	= 0;
	EvaRegs.T1CON.bit.TENABLE	= 0;	// Disable T1
//	EvaRegs.T2CON.bit.T2SWT1	= 0;	// T2 enabling use: own TENABLE bit
//	EvaRegs.T2CON.bit.SET1PR	= 0;	// T2PR using T1PR. (Reserved bit in T1CON)

// GP Timer Control Register
	EvaRegs.GPTCONA.bit.T1TOADC = 2;	// 周期中断启动ADC, 配合下溢中断执行主程序
//	EvaRegs.GPTCONA.bit.T2TOADC = 2;
//	EvaRegs.GPTCONA.bit.T2CTRIPE= 1;	// TxCTRIP enabled, TxCMPR output=HI-Z when TxCTRIP=low (active only when EXTCONA.0=1)
//	EvaRegs.GPTCONA.bit.T1CTRIPE= 1;
//	EvaRegs.GPTCONA.bit.TCMPOE	= 0;	// TxPWM_TxCMP are in HI-Z state
//	EvaRegs.GPTCONA.bit.T2CMPOE = 0;
//	EvaRegs.GPTCONA.bit.T1CMPOE = 0;
//	EvaRegs.GPTCONA.bit.T2PIN	= 0;	// Tx compare output polarity: Force low
//	EvaRegs.GPTCONA.bit.T1PIN	= 0;
	
// Compare Control Register
	EvaRegs.COMCONA.bit.CENABLE	= 1;	// Compare operation: Enabled
	EvaRegs.COMCONA.bit.CLD		= 0;	// CMPRx reload when: Underflow
	EvaRegs.COMCONA.bit.SVENABLE= 0;	// SVPWM mode: disable
	EvaRegs.COMCONA.bit.ACTRLD	= 0;	// ACTRx reload when: T1CNT=0
	EvaRegs.COMCONA.bit.FCOMPOE	= 0;	// Full compare output: disable
//	EvaRegs.COMCONA.bit.FCMP3OE = 1;	// CMPRx output: enable (active only when EXTCONA.0=0)
//	EvaRegs.COMCONA.bit.FCMP2OE = 1;
//	EvaRegs.COMCONA.bit.FCMP1OE = 1;
//	EvaRegs.COMCONA.bit.C3TRIPE	= 1;	// CxTRIP enabled, CMPRx output=HI-Z when CxTRIP=low (active only when EXTCONA.0=1)
//	EvaRegs.COMCONA.bit.C2TRIPE	= 1;
//	EvaRegs.COMCONA.bit.C1TRIPE	= 1;

// Compare Action Control Register
//	EvaRegs.ACTRA.bit.SVRDIR	= 0;	// SVPWM rotation direction: positive(CCW)
//	EvaRegs.ACTRA.bit.D			= 0;	// Basic space vector bits
	EvaRegs.ACTRA.all 			= 0x999;// CMPR1,3,5  active low; CMPR2, 4, 6 active high; 

// Dead-Band Timer Control Register
	EvaRegs.DBTCONA.bit.DBT		= VDBT;
	EvaRegs.DBTCONA.bit.DBTPS	= VDBTPS;	// DBtime = T_HSPCLK*(DBT*(2^DBTPS))	(DBTPS<=5)
	EvaRegs.DBTCONA.bit.EDBT1	= 1;		// Dead-band timer x enable
	EvaRegs.DBTCONA.bit.EDBT2	= 1;
	EvaRegs.DBTCONA.bit.EDBT3	= 1;
	
    EvaRegs.T1CNT	= 0x0000;
    EvaRegs.T1PR 	= T1PR_VAL;
//	EvaRegs.CMPR1	= T1PR_VAL>>1;
//	EvaRegs.CMPR2	= T1PR_VAL>>1;
//	EvaRegs.CMPR3	= T1PR_VAL>>1;

// IMR, IFR
	EvaRegs.EVAIMRA.bit.T1UFINT	= 0;
	EvaRegs.EVAIFRA.bit.T1UFINT	= 1;
	EvaRegs.EVAIMRA.bit.PDPINTA	= 0;
	EvaRegs.EVAIFRA.bit.PDPINTA	= 1;
	
#if	DUAL_EV_MODE
// EVB Control Register
// Timer x Control Register
	EvbRegs.T3CON.bit.FREE		= 0;
	EvbRegs.T3CON.bit.SOFT		= 0;	// Stop immediately on emulation suspend
	EvbRegs.T3CON.bit.TMODE		= 1;	// Count Mode: continuous-Up/-Down
	EvbRegs.T3CON.bit.TPS		= T1_TPS;// TxCLK=HSPCLK/(2^TPS)=150MHz	HSPCLK=150MHz
	EvbRegs.T3CON.bit.TCLKS10	= 0;	// Clock source use: internal 
	EvbRegs.T3CON.bit.TCLD10	= 0;	// T3CMPR reload when: T3CNT=0
	EvbRegs.T3CON.bit.TECMPR	= 0;
	EvbRegs.T3CON.bit.TENABLE	= 0;	// Disable T3
//	EvbRegs.T4CON.bit.SET3PR	= 0;	// T4PR using T3PR. (Reserved bit in T3CON)

// GP Timer Control Register
//	EvbRegs.GPTCONB.bit.T3TOADC = 2;	// Starts ADC by interrupt flag of: Tx period
//	EvbRegs.GPTCONB.bit.T4TOADC = 2;
//	EvbRegs.GPTCONB.bit.T4CTRIPE= 1;	// TxCTRIP enabled, TxCMPR output=HI-Z when TxCTRIP=low (active only when EXTCONB.0=1)
//	EvbRegs.GPTCONB.bit.T3CTRIPE= 1;
//	EvbRegs.GPTCONB.bit.TCMPOE	= 0;	// TxPWM_TxCMP are in HI-Z state
//	EvbRegs.GPTCONB.bit.T4CMPOE = 0;
//	EvbRegs.GPTCONB.bit.T3CMPOE = 0;
//	EvbRegs.GPTCONB.bit.T4PIN	= 0;	// Tx compare output polarity: Force low
//	EvbRegs.GPTCONB.bit.T3PIN	= 0;
	
// Compare Control Register
	EvbRegs.COMCONB.bit.CENABLE	= 1;	// Compare operation: Enabled
	EvbRegs.COMCONB.bit.CLD		= 0;	// CMPRx reload when: Underflow
	EvbRegs.COMCONB.bit.SVENABLE= 0;	// SVPWM mode: disable
	EvbRegs.COMCONB.bit.ACTRLD	= 0;	// ACTRx reload when: T3CNT=0
	EvbRegs.COMCONB.bit.FCOMPOE	= 0;	// Full compare output: disable
//	EvbRegs.COMCONB.bit.FCMP3OE = 1;	// CMPRx output: enable (active only when EXTCONB.0=0)
//	EvbRegs.COMCONB.bit.FCMP2OE = 1;
//	EvbRegs.COMCONB.bit.FCMP1OE = 1;
//	EvbRegs.COMCONB.bit.C3TRIPE	= 1;	// CxTRIP enabled, CMPRx output=HI-Z when CxTRIP=low (active only when EXTCONB.0=1)
//	EvbRegs.COMCONB.bit.C2TRIPE	= 1;
//	EvbRegs.COMCONB.bit.C1TRIPE	= 1;

// Compare Action Control Register
//	EvbRegs.ACTRB.bit.SVRDIR	= 0;	// SVPWM rotation direction: positive(CCW)
//	EvbRegs.ACTRB.bit.D			= 0;	// Basic space vector bits
	EvbRegs.ACTRB.all 			= 0x999;// CMPR7,9,11 active low; CMPR8,10,12 active high; 

// Dead-Band Timer Control Register
	EvbRegs.DBTCONB.bit.DBT		= VDBT;
	EvbRegs.DBTCONB.bit.DBTPS	= VDBTPS;	// DBtime = T_HSPCLK*(DBT*(2^DBTPS))	(DBTPS<=5)
	EvbRegs.DBTCONB.bit.EDBT1	= 1;		// Dead-band timer x enable
	EvbRegs.DBTCONB.bit.EDBT2	= 1;
	EvbRegs.DBTCONB.bit.EDBT3	= 1;
	
    EvbRegs.T3CNT	= 0x0000;
    EvbRegs.T3PR 	= T1PR_VAL;
//	EvbRegs.CMPR4	= T1PR_VAL>>1;
//	EvbRegs.CMPR5	= T1PR_VAL>>1;
//	EvbRegs.CMPR6	= T1PR_VAL>>1;

// IMR, IFR
//	EvbRegs.EVBIMRA.bit.T3UFINT	= 0;
//	EvbRegs.EVBIFRA.bit.T3UFINT	= 1;
//	EvbRegs.EVBIMRA.bit.PDPINTB	= 0;
//	EvbRegs.EVBIFRA.bit.PDPINTB	= 1;
#endif

}

/*funName:		InitL3C()
* Inputs: 		NONE
* Outputs: 		NONE
* Description: 	初始化L3C
******************************************************************************/
void InitL3C()
{
	int16	i;
	
	l3c.uStep		= STEP_DEFAULT;
	l3c.iFaultC		= CINI;
	l3c.iStartC		= CINI;
	l3c.iStopC		= CINI;
	l3c.iPdpintC	= CINI;
	l3c.iUdcovC		= CINI;
	l3c.iIdcocC		= CINI;
	l3c.uCap		= 0;
	l3c.iErrTwinkC	= 1000;							// 置大数关闭ERR灯闪烁计数器

//	l3c.qSina		= _IQ(0);
//	l3c.qSina		= _IQ(1);
	l3c.qTheta		= 0;
	l3c.qDTheta		= _IQ(DTHETA_DEFAULT_q);
	l3c.qIdR		= _IQ(OPEN_REF_ID_q);
	l3c.qIqR		= _IQ(OPEN_REF_IQ_q);
	l3c.qUnpe		= 0;
	
	for(i=0;i<AD_CH_NUM;i++)
		l3c.qAD[i] = 0;
	
	// LPF参数
	lpfUdc.qkin[0]	= _IQ(LPF_KIN0_q);
	lpfUdc.qkin[1]	= _IQ(LPF_KIN1_q);
	lpfUdc.qkin[2]	= _IQ(LPF_KIN2_q);
	lpfUdc.qkout[1]	= _IQ(LPF_KOUT1_q);
	lpfUdc.qkout[2]	= _IQ(LPF_KOUT2_q);

	lpfUnp.qkin[0]	= _IQ(LPF_KIN0_q);		// AD采样处理用
	lpfUnp.qkin[1]	= _IQ(LPF_KIN1_q);
	lpfUnp.qkin[2]	= _IQ(LPF_KIN2_q);
	lpfUnp.qkout[1] = _IQ(LPF_KOUT1_q);
	lpfUnp.qkout[2] = _IQ(LPF_KOUT2_q);
	
	lpfUnpe.qkin[0]	= _IQ(LPF_KIN0_q);		// FBDCL用
	lpfUnpe.qkin[1]	= _IQ(LPF_KIN1_q);
	lpfUnpe.qkin[2]	= _IQ(LPF_KIN2_q);
	lpfUnpe.qkout[1]= _IQ(LPF_KOUT1_q);
	lpfUnpe.qkout[2]= _IQ(LPF_KOUT2_q);
	
	// 灭灯
	LED_BIT_START = LED_OFF;				// 熄灭START
	LED_BIT_STOP  = LED_ON;					// 点亮STOP
#if	(L3_SP_MODE!=L3_SP_NONPC)				// !=: Ei0,Ei3用于启动NPC_ON,NPC_OFF控制	==:用于FAULT,RESET
	LED_BIT_NpcOn = LED_OFF;				// 熄灭NPCON
	LED_BIT_NpcOff= LED_OFF;				// 熄灭NPCOFF
#else
	LED_BIT_RESET = LED_OFF;				// 熄灭RESET
	LED_BIT_IDCOC = LED_OFF;				// 熄灭IDCOC
#endif
	
	// 清标志位
	l3c.flag.bit.start	= KEY_OFF;
	l3c.flag.bit.stop	= KEY_OFF;
	l3c.flag.bit.reset	= KEY_OFF;
	l3c.flag.bit.err 	= KEY_OFF;
	l3c.flag.bit.err_idcoc = KEY_OFF;
	l3c.flag.bit.err_udcov = KEY_OFF;
	l3c.flag.bit.err_1 = KEY_OFF;
	l3c.flag.bit.err_2 = KEY_OFF;
	l3c.flag.bit.NpcOn = KEY_OFF;
	
	npc.qKb1 = _IQ(1);
	npc.qKb2 = _IQ(1);
	npc.qKs  = _IQ(0);
	
	// CMPR幅初值
	EvaRegs.CMPR1	= 16101;
	EvaRegs.CMPR2	= 37500;
	EvaRegs.CMPR3	= 37500;

	EvbRegs.CMPR4	= 0;
	EvbRegs.CMPR5	= 4681;
	EvbRegs.CMPR6	= 16720;
}

/*funName:		CheckNormal()
* Inputs: 		NONE
* Outputs: 		NONE
* Description: 	判断运行状态,正常则开中断
******************************************************************************/
void CheckNormal()
{
// 判断按键输入，记在flag软件标志中
//************************************************************************
// 检查START
	if(l3c.flag.bit.start==KEY_OFF){
		if(IN_BIT_START==KEY_ON){					//防抖检测按键 按键输入低有效
			l3c.iStartC--;
			if(l3c.iStartC<=0){
				l3c.iStartC = CMAX;
				l3c.flag.bit.start = KEY_ON;		//置START
				l3c.flag.bit.stop  = KEY_OFF;		//清STOP
				l3c.flag.bit.reset = KEY_OFF;		//清RESET
			}
		}
		else
			if(l3c.iStartC<CMAX)					// 没有故障则故障计数器+1
				l3c.iStartC++;
	}
// 检查STOP
	if(l3c.flag.bit.stop==KEY_OFF){
		if(IN_BIT_STOP==KEY_ON){					//防抖检测按键 按键输入低有效
			l3c.iStopC--;
			if(l3c.iStopC<=0){
				l3c.iStopC=CMAX;
				l3c.flag.bit.stop = KEY_ON;			//置STOP
				l3c.flag.bit.start = KEY_OFF;		//清START
				l3c.flag.bit.reset = KEY_OFF;		//清RESET
			}
		}
		else
			if(l3c.iStopC<CMAX)						// 没有故障则故障计数器+1
				l3c.iStopC++;
	}
#if	(L3_SP_MODE!=L3_SP_NONPC)						// !=: Ei0,Ei3用于启动NPC_ON,NPC_OFF控制	==:用于FAULT,RESET
// 检查NPC_ON
	if(l3c.flag.bit.NpcOn==KEY_OFF){
		if(IN_BIT_NpcOn==KEY_ON){					//防抖检测按键 按键输入低有效
			l3c.iNpcOnC--;
			if(l3c.iNpcOnC<=0){
				l3c.iNpcOnC=CMAX;
				l3c.flag.bit.NpcOn 	= KEY_ON;		//置NpcOn
			}
		}
		else
			if(l3c.iNpcOnC<CMAX)
				l3c.iNpcOnC++;
	}
// 检查NPC_OFF
	if(l3c.flag.bit.NpcOn==KEY_ON){
		if(IN_BIT_NpcOff==KEY_ON){					//防抖检测按键 按键输入低有效
			l3c.iNpcOffC--;
			if(l3c.iNpcOffC<=0){
				l3c.iNpcOffC=CMAX;
				l3c.flag.bit.NpcOn	= KEY_OFF;		//清NpcOn
			}
		}
		else
			if(l3c.iNpcOffC<CMAX)
				l3c.iNpcOffC++;
	}
#else						// (KEY_NPC==0)
// 检查RESET
	if(l3c.flag.bit.reset==KEY_OFF){
		if(IN_BIT_RESET==KEY_ON){					//防抖检测按键 按键输入低�
			l3c.iResetC--;
			if(l3c.iResetC<=0){
				l3c.iResetC=CMAX;
				l3c.flag.bit.reset 	= KEY_ON;		//置STOP
				l3c.flag.bit.start = KEY_OFF;		//清START
				l3c.flag.bit.stop  = KEY_OFF;		//清STOP
			}
		}
		else
			if(l3c.iResetC<CMAX)					// 没有故障则故障计数器+1
				l3c.iResetC++;
	}
#endif

// 根据uRunflag软件标志进行操作
//************************************************************************
// START==1,STOP==0,ERR==0
	if((l3c.flag.bit.start==KEY_ON)
	 &&(l3c.flag.bit.stop ==KEY_OFF)
	 &&(l3c.flag.bit.err  ==KEY_OFF)){
		LED_BIT_STOP  = LED_OFF;				// 熄灭STOP
		LED_BIT_RESET = LED_OFF;				// 熄灭RESET

	// 启动L3系统
#if DUAL_EV_MODE
		EvbRegs.T3CON.bit.TENABLE	= 1;		// 使能T3
	#if	PWM_ON
		EvbRegs.COMCONB.bit.FCOMPOE	= 1;		// 使能全比较PWM输出
	#endif
#endif
		EvaRegs.T1CON.bit.TENABLE	= 1;		// 使能T1
	#if	PWM_ON
		EvaRegs.COMCONA.bit.FCOMPOE	= 1;		// 使能全比较PWM输出
	#endif

		l3c.flag.bit.start = KEY_OFF;			// 用完即复位
		EINT;
	}

// STOP==1
	if(l3c.flag.bit.stop==KEY_ON){
		IDEL();
		LED_BIT_STOP  = LED_ON;					// 点亮STOP
		LED_BIT_START = LED_OFF;				// 熄灭START
#if	(L3_SP_MODE==L3_SP_NONPC)
		LED_BIT_RESET = LED_OFF;				// 熄灭RESET
#endif
		delayx(10);
	
	// 计数器清零
		l3c.iFaultC = 0;
		l3c.iIdcocC = 0;
		l3c.iPdpintC = 0;
//		uGisr1 = 0;
//		uGisr2 = 0;

		l3c.flag.bit.stop = KEY_OFF;			// 用完即复位
	}

#if	(L3_SP_MODE!=L3_SP_NONPC)					// !=: Ei0,Ei3用于启动NPC_ON,NPC_OFF控制	==:用于FAULT,RESET
// NpcOn==1
	if(l3c.flag.bit.NpcOn==KEY_ON){
		LED_BIT_NpcOn  = LED_ON;
		LED_BIT_NpcOff = LED_OFF;
	}
// NpcOff==1
	if(l3c.flag.bit.NpcOn==KEY_OFF){
		LED_BIT_NpcOff  = LED_ON;
		LED_BIT_NpcOn	= LED_OFF;
	}
#else					// (KEY_NPC==0)
// RESET==1
	if(l3c.flag.bit.reset==KEY_ON){
		IDEL();
		LED_BIT_RESET = LED_ON;					// 点亮RESET
		LED_BIT_START = LED_OFF;				// 熄灭START
		LED_BIT_STOP  = LED_OFF;				// 熄灭STOP
		LED_BIT_IDCOC = LED_OFF;				// 熄灭IDCOC
		l3c.iErrTwinkC = 1000;					// 置大数关闭ERR灯闪烁计数器
		delayx(10);
		
	// 计数器清零
		l3c.iFaultC = 0;
		l3c.iIdcocC = 0;
		l3c.iPdpintC = 5;
		
		l3c.flag.bit.reset = KEY_OFF;			// 用完即复位
	}

// ERR==1
	if(l3c.flag.bit.err==KEY_ON){
		IDEL();
		LED_BIT_START = LED_OFF;				// 熄灭START
		LED_BIT_RESET = LED_OFF;				// 熄灭RESET
		delayx(10);
		l3c.iErrTwinkC = 0;						// 启动ERR灯闪烁计数器
				
	// 计数器清零
		l3c.iFaultC = 0;
		l3c.iIdcocC = 0;
		l3c.iPdpintC = 0;
//		uGisr1 = 0;
//		uGisr2 = 0;
		l3c.flag.bit.err = KEY_OFF;				// 用完即复位
	}

// 有故障时闪烁STOP灯(PB.14)
	if(l3c.iErrTwinkC<100){						// 计数器值<100时才闪烁
		if(l3c.iErrTwinkC>=0){
			l3c.iErrTwinkC--;
			delayx(150);
		}
		else{
			l3c.iErrTwinkC = 20;
			LED_BIT_TWI_IDCOC = 1;				// 闪烁IDCOC灯
		}
	}
#endif
}

/*funName:		CheckFault()
* Inputs: 		NONE
* Outputs: 		NONE
* Description: 	判断故障输入信号
******************************************************************************/
void CheckFault()
{
#if	(L3_SP_MODE==L3_SP_NONPC)				// !=: Ei0,Ei3用于启动NPC_ON,NPC_OFF控制	==:用于FAULT,RESET
// IDCOC	PB8
 #if (!CHEAT_NO_IDCOC)
	if(IN_BIT_IDCOC==KEY_ON){
		if(l3c.iIdcocC>0)				// 有一次故障则故障计数器-1
			l3c.iIdcocC--;
		if(l3c.iIdcocC<=0){				// 减到0则判发生故障，点LED、置标志
			LED_BIT_IDCOC = LED_ON;		// 点亮IDCOC灯
			l3c.flag.bit.err = KEY_ON;
			l3c.flag.bit.err_idcoc = KEY_ON;
		}
	}
	else{
		if(l3c.iIdcocC<CMAX)			// 没有故障则故障计数器+1
			l3c.iIdcocC++;
	}
 #endif
#endif
}

/*funName:		IDEL()
* Inputs: 		NONE
* Outputs: 		NONE
* Description: 	进入待机状态
******************************************************************************/
void IDEL()
{
//	l3c.flag = 0;						// 这里不能清运行标志
//	l3c.flagBak = 0;

	EvaRegs.CMPR1	= T1PR_VAL>>1;			// 强制拉到N
	EvaRegs.CMPR2	= T1PR_VAL>>1;
	EvaRegs.CMPR3	= T1PR_VAL>>1;
	EvaRegs.COMCONA.bit.FCOMPOE = 0;	// 禁止比较输出
	EvaRegs.T1CON.bit.TENABLE	= 0;	// 关闭T1
#if	DUAL_EV_MODE						// 1:使能双EV输出三电平PWM模式
	EvbRegs.CMPR4	= T1PR_VAL>>1;
	EvbRegs.CMPR5	= T1PR_VAL>>1;
	EvbRegs.CMPR6	= T1PR_VAL>>1;
	EvbRegs.COMCONB.bit.FCOMPOE = 0;	// 禁止比较输出
	EvbRegs.T3CON.bit.TENABLE	= 0;	// 关闭T3
#endif
	
	LED_BIT_START = LED_OFF;			// 熄灭START
//	LED_BIT_RESET = LED_OFF;			// 点亮RESET
//	LED_BIT_STOP  = LED_OFF;			// 熄灭STOP
//	LED_BIT_IDCOC = LED_ON;				// 熄灭IDCOC

	DINT;
}

/*funName:		test_io_ad()
* Inputs: 		NONE
* Outputs: 		NONE
* Description: 	IO,PWM,AD等本功能调试
******************************************************************************/
void test_io_ad(void)
{
	int16	i;
	
//OUT test
   	GpioDataRegs.GPBDAT.bit.GPIOB15 = 0;	// OUT3
	GpioDataRegs.GPBDAT.bit.GPIOB14 = 0;	// OUT2
	GpioDataRegs.GPBDAT.bit.GPIOB13 = 0;	// OUT1
	GpioDataRegs.GPBDAT.bit.GPIOB12 = 0;	// OUT0

	GpioDataRegs.GPBDAT.bit.GPIOB15 = 1;
	GpioDataRegs.GPBDAT.bit.GPIOB14 = 1;
	GpioDataRegs.GPBDAT.bit.GPIOB13 = 1;
	GpioDataRegs.GPBDAT.bit.GPIOB12 = 1;

// PWM test
	GpioDataRegs.GPADAT.bit.GPIOA0 = 0;		// PWM1
	GpioDataRegs.GPADAT.bit.GPIOA0 = 1;

	GpioDataRegs.GPADAT.bit.GPIOA1 = 0;		// PWM2
	GpioDataRegs.GPADAT.bit.GPIOA1 = 1;

	GpioDataRegs.GPADAT.bit.GPIOA2 = 0;		// PWM3
	GpioDataRegs.GPADAT.bit.GPIOA2 = 1;

	GpioDataRegs.GPADAT.bit.GPIOA3 = 0;		// PWM4
	GpioDataRegs.GPADAT.bit.GPIOA3 = 1;

	GpioDataRegs.GPADAT.bit.GPIOA4 = 0;		// PWM5
	GpioDataRegs.GPADAT.bit.GPIOA4 = 1;

	GpioDataRegs.GPADAT.bit.GPIOA5 = 0;		// PWM6
	GpioDataRegs.GPADAT.bit.GPIOA5 = 1;

	GpioDataRegs.GPBDAT.bit.GPIOB0 = 0;		// PWM7
	GpioDataRegs.GPBDAT.bit.GPIOB0 = 1;

	GpioDataRegs.GPBDAT.bit.GPIOB1 = 0;		// PWM8
	GpioDataRegs.GPBDAT.bit.GPIOB1 = 1;

	GpioDataRegs.GPBDAT.bit.GPIOB2 = 0;		// PWM9
	GpioDataRegs.GPBDAT.bit.GPIOB2 = 1;

	GpioDataRegs.GPBDAT.bit.GPIOB3 = 0;		// PWM10
	GpioDataRegs.GPBDAT.bit.GPIOB3 = 1;

	GpioDataRegs.GPBDAT.bit.GPIOB4 = 0;		// PWM11
	GpioDataRegs.GPBDAT.bit.GPIOB4 = 1;

	GpioDataRegs.GPBDAT.bit.GPIOB5 = 0;		// PWM12
	GpioDataRegs.GPBDAT.bit.GPIOB5 = 1;

//IN test
	;				// 加断点刷新观察B8~B11
	
//AD test
	ad.start(&ad);	// 不断读取AD值, 观察ad.out[]
	delayx(1);
	ad.read(&ad);

	for(i=0;i<AD_CH_NUM;i++)
		l3c.qAD[i] -= _IQ(1.5);

	l3c.qAD[AD_CH_IA]  = _IQmpy(ad.qOut[AD_CH_IA ], AD_K_IA);
	l3c.qAD[AD_CH_IB]  = _IQmpy(ad.qOut[AD_CH_IB ], AD_K_IB);
	l3c.qAD[AD_CH_IDC] = _IQmpy(ad.qOut[AD_CH_IDC], AD_K_IDC);
	l3c.qAD[AD_CH_INP] = _IQmpy(ad.qOut[AD_CH_INP], AD_K_INP);
	l3c.qAD[AD_CH_UDC] = _IQmpy(ad.qOut[AD_CH_UDC], AD_K_UDC);	// AD_K_UDC定义时右移了4位(以增大整数范围), 则Udc为IQ20格式
	l3c.qAD[AD_CH_UNP] = _IQmpy(ad.qOut[AD_CH_UNP], AD_K_UNP);	// AD_K_UNP定义时右移了4位
}


/*funName:		delayx()
* Inputs: 		n
* Outputs: 		NONE
* Description: 	执行n个单位的小延时
******************************************************************************/
void delayx(Uint16 n)
{
    Uint16	i,j;
	for(i=0;i<n;i++){
		for(j=0;j<1000;j++){}
	}
}

//===========================================================================
// Function format demo
//===========================================================================
/*funName:		functionxx()
* Inputs: 		NONE
* Outputs: 		NONE
* Description: 	函数的主要功能
******************************************************************************/
/*
void functionxx()
{
}
*/
