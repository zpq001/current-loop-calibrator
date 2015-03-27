

#define FREQ(x)	(uint16_t)(1000000/x)


const tone_t _beep_EncoderConfirm[] = {
    { FREQ(1000),	10 },
	{0,	0}
};

const tone_t _beep_EncoderIllegal[] = {
    { FREQ(600),	10 },
	{0,	0}
};

const tone_t _beep_KeyConfirm[] = {
    { FREQ(1000),	20 },
	{0,	0}
};

const tone_t _beep_KeyReject[] = {
    { FREQ(600),	20 },
	{0,	0}
};

const tone_t _beep_SyclesDone[] = {
    { FREQ(1000),	500 },
	{0,	0}
};

const tone_t _melody_SettingConfirm[] = {
    { FREQ(1000),	40 },
	{ 0,	40 },
    { FREQ(1200),	40 },
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



