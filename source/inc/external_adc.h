
#define EXTADC_PORT			MDR_PORTF
#define EXTADC_CLK_PIN		1
#define EXTADC_TXD_PIN		0
#define EXTADC_FSS_PIN		2
#define EXTADC_RXD_PIN		3


#define EXTADC_LOW_RANGE        0
#define EXTADC_HIGH_RANGE       1
#define EXTADC_HIGH_OVERLOAD    2


void ExtADC_Initialize(void);
void ExtADC_UpdateCurrent(void);
int32_t ExtADC_GetCurrent(void);
uint8_t ExtADC_GetRange(void);


void ExtADC_SaveCalibrationPoint(uint8_t pointNum, uint32_t measuredValue);
void ExtADC_Calibrate(void);
void ExtADC_ApplyCalibration(void);
void ExtADC_SaveCalibration(void);



