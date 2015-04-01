

#define FREQ(x)	(uint16_t)(1200000/x)


const tone_t _beep_EncoderConfirm[] = {
//    { FREQ(1200),	10 },
	{0,	0}
};

const tone_t _beep_EncoderIllegal[] = {
    { FREQ(600),	22 },
	{0,	0}
};

const tone_t _beep_KeyConfirm[] = {
    { FREQ(1200),	20 },
	{0,	0}
};

const tone_t _beep_KeyReject[] = {
    { FREQ(600),	20 },
	{0,	0}
};

const tone_t _beep_SyclesDone[] = {
    { FREQ(1200),	500 },
	{0,	0}
};

const tone_t _melody_SettingConfirm[] = {
    { FREQ(1200),	40 },
	{ 0,	40 },
    { FREQ(1400),	40 },
	{0,	0}
};

const tone_t _melody_SettingIllegal[] = {
    { FREQ(600),	40 },
    { 0,	40 },
    { FREQ(600),	40 },
    { 0,	40 },
    { FREQ(600),	40 },
	{0,	0}
};



