
#define EXTADC_PORT			MDR_PORTF
#define EXTADC_CLK_PIN		1
#define EXTADC_TXD_PIN		0
#define EXTADC_FSS_PIN		2
#define EXTADC_RXD_PIN		3





void ExtADC_Initialize(void);
void ExtADC_UpdateCurrent(void);
int32_t ExtADC_GetCurrent(void);

