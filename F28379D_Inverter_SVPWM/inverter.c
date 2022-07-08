#include "F28x_Project.h"
#include "F2837xD_Device.h"// DSP2833x Examples Include File
#include "math.h"

#define DEAD_TIME_RISE 80
#define DEAD_TIME_FALL 80
#define pi 3.1415926
#define digit 100000
//#define period 7500  // 10KHz¶ÔÓ¦Ê±ÖÓÊý£¬TBCLK = SYSCLKOUT
//#define period 60000  // 10KHz¶ÔÓ¦Ê±ÖÓÊý£¬TBCLK = SYSCLKOUT(for test)
#define M 0.8  // µ÷ÖÆ¶È
float32 ia[101], ib[101], ic[101], vc1[101], vc2[101];
float32 period_count = 0;  // ÔØ²¨ÖÜÆÚÊý
float32 period, fc, fs, Adc_results_vc1 = 0, Adc_results_vc2 = 0,
		Adc_results_ia = 0, Adc_results_ib = 0, Adc_results_ic = 0;
Uint16 count = 0;
int Tinv[3] = { 0, 0, 0 };  // ÈýÏà¶ÔÓ¦±È½ÏÖµ
int last[3];  // ÉÏÖÜÆÚTinvÖµ(for test)
double Ta = 0, Tb = 0, T0 = 0;  // Õ¼¿Õ±È
void ePWMSetup(void);
double roundn(double);  // ½Ø¶ÏÐ¡ÊýµãºóÎ»Êý
void Initialize_adc(void);
void GPIO_select(void);
interrupt void epwm1_timer_isr(void);

int main()
{
	fs = 50;
	fc = 5000;
	period = (100e6 / (2 * fc)) / 2;
	InitSysCtrl();

	DINT;

	InitPieCtrl();

	IER = 0x0000;
	IFR = 0x0000;

	InitPieVectTable();

	EALLOW;
	PieVectTable.EPWM1_INT = &epwm1_timer_isr;  // ePWM1ÖÐ¶Ïº¯ÊýÈë¿Ú
	EDIS;

	ePWMSetup();

	IER |= M_INT3;  // enable ePWM1 CPU_interrupt
	PieCtrlRegs.PIEIER3.bit.INTx1 = 1;  // enable ePWM1 pie_interrupt

	EINT;
	// ×ÜÖÐ¶Ï INTM Ê¹ÄÜ
	ERTM;
	// Enable Global realtime interrupt DBGM

	int i;
	for (;;)
	{
		asm("          NOP");
		for (i = 1; i <= 10; i++)
		{
		}
	}

	return 0;
}

void ePWMSetup(void)
{
	EALLOW;

	GpioCtrlRegs.GPAMUX1.bit.GPIO0 = 1; // GPIO ³õÊ¼»¯ÎªepwmÊä³ö
	GpioCtrlRegs.GPAMUX1.bit.GPIO1 = 1;
	GpioCtrlRegs.GPAMUX1.bit.GPIO2 = 1;
	GpioCtrlRegs.GPAMUX1.bit.GPIO3 = 1;
	GpioCtrlRegs.GPAMUX1.bit.GPIO4 = 1;
	GpioCtrlRegs.GPAMUX1.bit.GPIO5 = 1;
	GpioCtrlRegs.GPAMUX1.bit.GPIO6 = 1;
	GpioCtrlRegs.GPAMUX1.bit.GPIO7 = 1;
	GpioCtrlRegs.GPAMUX1.bit.GPIO8 = 1;
	GpioCtrlRegs.GPAMUX1.bit.GPIO9 = 1;
	GpioCtrlRegs.GPAMUX1.bit.GPIO10 = 1;
	GpioCtrlRegs.GPAMUX1.bit.GPIO11 = 1;

	EDIS;

	// ----------------EPwm1---------------------
	EPwm1Regs.TBPRD = period;
	EPwm1Regs.TBPHS.bit.TBPHS = 0x0000;  // Ê±»ùÖÜÆÚ¼Ä´æÆ÷
	EPwm1Regs.TBCTR = 0;  // Ê±»ù¼ÆÊý¼Ä´æÆ÷ÖÃÁã
	EPwm1Regs.TBCTL.bit.PHSDIR = TB_UP;
	EPwm1Regs.TBCTL.bit.CLKDIV = 0;
	EPwm1Regs.TBCTL.bit.HSPCLKDIV = 0;
	EPwm1Regs.TBCTL.bit.SYNCOSEL = TB_CTR_ZERO;
	EPwm1Regs.TBCTL.bit.CTRMODE = TB_COUNT_UPDOWN;

	EPwm1Regs.CMPA.bit.CMPA = period / 2; // duty_cycle = 0.5
	EPwm1Regs.CMPB.bit.CMPB = 0;
	EPwm1Regs.CMPCTL.bit.SHDWAMODE = CC_SHADOW;
	EPwm1Regs.CMPCTL.bit.LOADAMODE = CC_CTR_ZERO;
	EPwm1Regs.AQCTLA.bit.CAU = AQ_SET;
	EPwm1Regs.AQCTLA.bit.CAD = AQ_CLEAR;

	EPwm1Regs.DBCTL.bit.IN_MODE = DBA_ALL;
	EPwm1Regs.DBCTL.bit.POLSEL = DB_ACTV_HIC;  // A²»·­×ª£¬B·­×ª
	EPwm1Regs.DBCTL.bit.OUT_MODE = DB_FULL_ENABLE;
	// DBRED Register
	EPwm1Regs.DBRED.bit.DBRED = DEAD_TIME_RISE;
	EPwm1Regs.DBFED.bit.DBFED = DEAD_TIME_FALL;
	// DBFED Register

	EPwm1Regs.ETSEL.bit.INTSEL = ET_CTR_ZERO;
	EPwm1Regs.ETSEL.bit.INTEN = 1;                // Enable INT
	EPwm1Regs.ETPS.bit.INTPRD = ET_1ST;

	// ----------------EPwm2---------------------
	EPwm2Regs.TBPRD = period;
	EPwm2Regs.TBPHS.bit.TBPHS = 0;  // Ê±»ùÖÜÆÚ¼Ä´æÆ÷
	EPwm2Regs.TBCTR = 0;  // Ê±»ù¼ÆÊý¼Ä´æÆ÷ÖÃÁã
	EPwm2Regs.TBCTL.bit.PHSDIR = TB_UP;
	EPwm2Regs.TBCTL.bit.CLKDIV = 0;
	EPwm2Regs.TBCTL.bit.HSPCLKDIV = 0;
	EPwm2Regs.TBCTL.bit.SYNCOSEL = TB_CTR_ZERO;
	EPwm2Regs.TBCTL.bit.CTRMODE = TB_COUNT_UPDOWN;

	EPwm2Regs.CMPA.bit.CMPA = period / 2; // duty_cycle = 0.5
	EPwm2Regs.CMPB.bit.CMPB = 0;
	EPwm2Regs.CMPCTL.bit.SHDWAMODE = CC_SHADOW;
	EPwm2Regs.CMPCTL.bit.LOADAMODE = CC_CTR_ZERO;

	EPwm2Regs.AQCTLA.bit.CAU = AQ_SET;
	EPwm2Regs.AQCTLA.bit.CAD = AQ_CLEAR;

	EPwm2Regs.DBCTL.bit.IN_MODE = DBA_ALL;
	EPwm2Regs.DBCTL.bit.POLSEL = DB_ACTV_HIC;  // A²»·­×ª£¬B·­×ª
	EPwm2Regs.DBCTL.bit.OUT_MODE = DB_FULL_ENABLE;

	EPwm2Regs.DBRED.bit.DBRED = DEAD_TIME_RISE;
	EPwm2Regs.DBFED.bit.DBFED = DEAD_TIME_FALL;

	EPwm2Regs.ETSEL.bit.SOCAEN = 1;
	EPwm2Regs.ETSEL.bit.SOCASEL = 1;
	EPwm2Regs.ETPS.bit.SOCAPRD = 1;
	// ----------------EPwm6---------------------
	EPwm3Regs.TBPRD = period;
	EPwm3Regs.TBPHS.bit.TBPHS = 0;  // Ê±»ùÖÜÆÚ¼Ä´æÆ÷
	EPwm3Regs.TBCTR = 0;  // Ê±»ù¼ÆÊý¼Ä´æÆ÷ÖÃÁã
	EPwm3Regs.TBCTL.bit.PHSDIR = TB_UP;
	EPwm3Regs.TBCTL.bit.CLKDIV = 0;
	EPwm3Regs.TBCTL.bit.HSPCLKDIV = 0;
	EPwm3Regs.TBCTL.bit.SYNCOSEL = TB_CTR_ZERO;
	EPwm3Regs.TBCTL.bit.CTRMODE = TB_COUNT_UPDOWN;

	EPwm3Regs.CMPA.bit.CMPA = period / 2; // duty_cycle = 0.5
	EPwm3Regs.CMPB.bit.CMPB = 0;
	EPwm3Regs.CMPCTL.bit.SHDWAMODE = CC_SHADOW;
	EPwm3Regs.CMPCTL.bit.LOADAMODE = CC_CTR_ZERO;

	EPwm3Regs.AQCTLA.bit.CAU = AQ_SET;
	EPwm3Regs.AQCTLA.bit.CAD = AQ_CLEAR;

	EPwm3Regs.DBCTL.bit.IN_MODE = DBA_ALL;
	EPwm3Regs.DBCTL.bit.POLSEL = DB_ACTV_HIC;  // A²»·­×ª£¬B·­×ª
	EPwm3Regs.DBCTL.bit.OUT_MODE = DB_FULL_ENABLE;
	EPwm3Regs.DBRED.bit.DBRED = DEAD_TIME_RISE;
	EPwm3Regs.DBFED.bit.DBFED = DEAD_TIME_FALL;

	EPwm3Regs.ETSEL.bit.SOCAEN = 1;
	EPwm3Regs.ETSEL.bit.SOCASEL = 1;
	EPwm3Regs.ETPS.bit.SOCAPRD = 1;
}

void GPIO_select(void)
{
	EALLOW;
	GpioCtrlRegs.GPAMUX2.all = 0x00000000;
	GpioCtrlRegs.GPADIR.all = 0xFFFFFFFF;
	EDIS;
	//GpioDataRegs.GPACLEAR.bit.GPIO16 = 1;
	//GpioDataRegs.GPASET.bit.GPIO16 = 1;
}
void Initialize_adc(void)
{
	EALLOW;
	AdcSetMode(ADC_ADCA, ADC_RESOLUTION_12BIT, ADC_SIGNALMODE_SINGLE);
	AdcSetMode(ADC_ADCB, ADC_RESOLUTION_12BIT, ADC_SIGNALMODE_SINGLE);
	AdcSetMode(ADC_ADCC, ADC_RESOLUTION_12BIT, ADC_SIGNALMODE_SINGLE);
	AdcSetMode(ADC_ADCD, ADC_RESOLUTION_12BIT, ADC_SIGNALMODE_SINGLE);

	AdcaRegs.ADCCTL1.bit.INTPULSEPOS = 1; //Interrupt pulse generation occurs at the end of the conversion

	AdcaRegs.ADCCTL2.bit.PRESCALE = 7; //ADCCLK = Input Clock/4.5
	AdcbRegs.ADCCTL2.bit.PRESCALE = 7;
	AdccRegs.ADCCTL2.bit.PRESCALE = 7;
	AdcdRegs.ADCCTL2.bit.PRESCALE = 7;

	AdcaRegs.ADCSOC0CTL.bit.TRIGSEL = 7; //ADCTRIG7-ePWM2, ADCSOCA
	AdcaRegs.ADCSOC1CTL.bit.TRIGSEL = 7;
	AdcaRegs.ADCSOC2CTL.bit.TRIGSEL = 7;
	AdcaRegs.ADCSOC3CTL.bit.TRIGSEL = 7;

	AdcbRegs.ADCSOC0CTL.bit.TRIGSEL = 7;
	AdcbRegs.ADCSOC1CTL.bit.TRIGSEL = 7;
	AdcbRegs.ADCSOC2CTL.bit.TRIGSEL = 7;
	AdcbRegs.ADCSOC3CTL.bit.TRIGSEL = 7;

	AdccRegs.ADCSOC0CTL.bit.TRIGSEL = 7;
	AdccRegs.ADCSOC1CTL.bit.TRIGSEL = 7;
	AdccRegs.ADCSOC2CTL.bit.TRIGSEL = 7;
	AdccRegs.ADCSOC3CTL.bit.TRIGSEL = 7;

	AdcdRegs.ADCSOC0CTL.bit.TRIGSEL = 7;
	AdcdRegs.ADCSOC1CTL.bit.TRIGSEL = 7;
	AdcdRegs.ADCSOC2CTL.bit.TRIGSEL = 7;
	AdcdRegs.ADCSOC3CTL.bit.TRIGSEL = 7;

	AdcaRegs.ADCSOC0CTL.bit.CHSEL = 3;
	AdcaRegs.ADCSOC1CTL.bit.CHSEL = 3;
	AdcaRegs.ADCSOC2CTL.bit.CHSEL = 3;
	AdcaRegs.ADCSOC3CTL.bit.CHSEL = 3;

	AdcbRegs.ADCSOC0CTL.bit.CHSEL = 2;
	AdcbRegs.ADCSOC1CTL.bit.CHSEL = 2;
	AdcbRegs.ADCSOC2CTL.bit.CHSEL = 2;
	AdcbRegs.ADCSOC3CTL.bit.CHSEL = 2;

	AdcdRegs.ADCSOC0CTL.bit.CHSEL = 3;
	AdcdRegs.ADCSOC1CTL.bit.CHSEL = 3;
	AdcdRegs.ADCSOC2CTL.bit.CHSEL = 3;
	AdcdRegs.ADCSOC3CTL.bit.CHSEL = 3;

	AdccRegs.ADCSOC0CTL.bit.CHSEL = 2;
	AdccRegs.ADCSOC1CTL.bit.CHSEL = 2;
	AdccRegs.ADCSOC2CTL.bit.CHSEL = 2;
	AdccRegs.ADCSOC3CTL.bit.CHSEL = 2;

	AdcaRegs.ADCCTL1.bit.ADCPWDNZ = 1; // Power on ADC core
	AdcbRegs.ADCCTL1.bit.ADCPWDNZ = 1;
	AdccRegs.ADCCTL1.bit.ADCPWDNZ = 1;
	AdcdRegs.ADCCTL1.bit.ADCPWDNZ = 1;

	AdcaRegs.ADCINTSEL1N2.bit.INT1E = 1; // ADCINT1 is enabled
	AdcaRegs.ADCINTSEL1N2.bit.INT1CONT = 0; // No further ADCINT1 is generated until ADCINT1 flag is cleared by the user
	AdcaRegs.ADCINTSEL1N2.bit.INT1SEL = 3; // EOC3 is trigger for ADCINT1
	AdcaRegs.ADCINTFLGCLR.bit.ADCINT1 = 1; // Clear the flag bit in ADCINTFLG Register

	AdcaRegs.ADCSOC0CTL.bit.ACQPS = 127;  // Acquisition window 640ns
	AdcaRegs.ADCSOC1CTL.bit.ACQPS = 127;
	AdcaRegs.ADCSOC2CTL.bit.ACQPS = 127;
	AdcaRegs.ADCSOC3CTL.bit.ACQPS = 127;

	AdcbRegs.ADCSOC0CTL.bit.ACQPS = 127;
	AdcbRegs.ADCSOC1CTL.bit.ACQPS = 127;
	AdcbRegs.ADCSOC2CTL.bit.ACQPS = 127;
	AdcbRegs.ADCSOC3CTL.bit.ACQPS = 127;

	AdccRegs.ADCSOC0CTL.bit.ACQPS = 127;
	AdccRegs.ADCSOC1CTL.bit.ACQPS = 127;
	AdccRegs.ADCSOC2CTL.bit.ACQPS = 127;
	AdccRegs.ADCSOC3CTL.bit.ACQPS = 127;

	AdcdRegs.ADCSOC0CTL.bit.ACQPS = 127;
	AdcdRegs.ADCSOC1CTL.bit.ACQPS = 127;
	AdcdRegs.ADCSOC2CTL.bit.ACQPS = 127;
	AdcdRegs.ADCSOC3CTL.bit.ACQPS = 127;
	EDIS;
}
interrupt void epwm1_timer_isr(void)
{
	GpioDataRegs.GPASET.bit.GPIO20 = 1;
	/*-----------------Read ADC------------------------*/
	if (AdcaRegs.ADCINTFLG.bit.ADCINT1 == 1)
	{
		Adc_results_vc1 = (AdcaResultRegs.ADCRESULT0 + AdcaResultRegs.ADCRESULT1
				+ AdcaResultRegs.ADCRESULT2 + AdcaResultRegs.ADCRESULT3) / 4;
		Adc_results_vc1 = (307.1277 * (Adc_results_vc1 / 4096) - 2);

		Adc_results_vc2 = (AdcbResultRegs.ADCRESULT0 + AdcbResultRegs.ADCRESULT1
				+ AdcbResultRegs.ADCRESULT2 + AdcbResultRegs.ADCRESULT3) / 4;
		Adc_results_vc2 = (329.25 * (Adc_results_vc2 / 4096) - 7.81);

		Adc_results_ia = (AdccResultRegs.ADCRESULT0 + AdccResultRegs.ADCRESULT1
				+ AdccResultRegs.ADCRESULT2 + AdccResultRegs.ADCRESULT3) / 4;
		Adc_results_ia = (3.3 * (Adc_results_ia / 4096));

		Adc_results_ib = (AdcdResultRegs.ADCRESULT0 + AdcdResultRegs.ADCRESULT1
				+ AdcdResultRegs.ADCRESULT2 + AdcdResultRegs.ADCRESULT3) / 4;
		Adc_results_ib = (3.3 * (Adc_results_ib / 4096));

		ia[count] = Adc_results_ia;
		ib[count] = Adc_results_ib;
		ic[count] = -Adc_results_ia - Adc_results_ib;
		vc1[count] = Adc_results_vc1;
		vc2[count] = Adc_results_vc2;
		if (count == 100)
		{
			count = 0;
		}
		else
		{
			count++;
		}

		AdcaRegs.ADCINTFLGCLR.bit.ADCINT1 = 1;
	}
	/*---------------------Giải thuật MPC---------------------------------*/

	/*-----------------------Space vector---------------------------------*/
	double Angle = 0;
	double theta = 0;
	int sector = 0;

	Angle = fmod((2 * fs * pi * (period_count / 10000.0)), (2 * pi));
	theta = fmod(Angle, 1 / 3.0 * pi);
	sector = floor(Angle / (1 / 3.0 * pi)) + 1;
	Ta = M * sin(1 / 3.0 * pi - theta);
	Tb = M * sin(theta);
	T0 = 0.5 * (1 - Ta - Tb);
	Ta = roundn(Ta);
	Tb = roundn(Tb);
	T0 = roundn(T0);
	if (T0 < 0)
		T0 = 0;

	switch (sector)
	{
	case 1:
		Tinv[0] = floor(period * (T0));
		Tinv[1] = floor(period * (T0 + Ta));
		Tinv[2] = floor(period * (T0 + Ta + Tb));
		break;
	case 2:
		Tinv[0] = floor(period * (T0 + Tb));
		Tinv[1] = floor(period * (T0));
		Tinv[2] = floor(period * (T0 + Ta + Tb));
		break;
	case 3:
		Tinv[0] = floor(period * (T0 + Ta + Tb));
		Tinv[1] = floor(period * (T0));
		Tinv[2] = floor(period * (T0 + Ta));
		break;
	case 4:
		Tinv[0] = floor(period * (T0 + Ta + Tb));
		Tinv[1] = floor(period * (T0 + Tb));
		Tinv[2] = floor(period * (T0));
		break;
	case 5:
		Tinv[0] = floor(period * (T0 + Ta));
		Tinv[1] = floor(period * (T0 + Ta + Tb));
		Tinv[2] = floor(period * (T0));
		break;
	case 6:
		Tinv[0] = floor(period * (T0));
		Tinv[1] = floor(period * (T0 + Ta + Tb));
		Tinv[2] = floor(period * (T0 + Tb));
	}
	if (Tinv[0] == 0)
		Tinv[0]++;
	if (Tinv[0] == period)
		Tinv[0]--;
	if (Tinv[1] == 0)
		Tinv[1]++;
	if (Tinv[1] == period)
		Tinv[1]--;
	if (Tinv[2] == 0)
		Tinv[2]++;
	if (Tinv[2] == period)
		Tinv[2]--;
	EPwm1Regs.CMPA.bit.CMPA = Tinv[0];
	EPwm2Regs.CMPA.bit.CMPA = Tinv[1];
	EPwm3Regs.CMPA.bit.CMPA = Tinv[2];

	period_count++;
	if (period_count == 10000)
		period_count = 0;

	last[0] = Tinv[0];
	last[1] = Tinv[1];
	last[2] = Tinv[2];

	EPwm1Regs.ETCLR.bit.INT = 1;
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP3;
	GpioDataRegs.GPACLEAR.bit.GPIO20 = 1;
}

double roundn(double input)
{
	double temp;
	temp = input * digit;
	temp = floor(temp);
	temp = temp / digit;
	return temp;
}
