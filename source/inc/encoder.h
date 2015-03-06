

#define ENCODER_PORT    MDR_PORTD
#define ENCODER_PINA    5
#define ENCODER_PINB    6


extern int16_t encoder_delta;

void Encoder_Init(void);
void Encoder_Process(void);
void Encoder_UpdateDelta(void);
