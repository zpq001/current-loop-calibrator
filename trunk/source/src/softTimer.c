/*
 * softTimer.c
 *
 * Created: 22.04.2013 12:06:42
 *  Author: Avega
 */ 

#include "softTimer.h"
 
 
 
void processSoftTimer8b(SoftTimer8b_t *tmr)
{
	uint8_t newCounterVal;

	if (!tmr->enabled)
	return;

	if (tmr->counter >= tmr->top)
	{
		if (tmr->runOnce)
		{
			tmr->enabled = 0;
		}
		tmr->flags.top = 1;
		tmr->flags.ovfl = 1;
		newCounterVal = 0;
	}
	else
	{
		tmr->flags.top = 0;
		newCounterVal = tmr->counter + 1;
	}
	
#ifdef SFTMR8_USE_COMPA
	tmr->flags.compa_eq = 0;
	tmr->flags.compa_ge = 0;
	if (tmr->counter == tmr->compa)
	{
		tmr->flags.compa_eq = 1;
		tmr->flags.compa_ge = 1;
		tmr->flags.compa_tgl ^= 1;
	}
	else if (tmr->counter > tmr->compa)
	{
		tmr->flags.compa_ge = 1;
	}
#endif

#ifdef SFTMR8_USE_COMPB
	tmr->flags.compb_eq = 0;
	tmr->flags.compb_ge = 0;
	if (tmr->counter == tmr->compb)
	{
		tmr->flags.compb_eq = 1;
		tmr->flags.compb_ge = 1;
		tmr->flags.compb_tgl ^= 1;
	}
	else if (tmr->counter > tmr->compb)
	{
		tmr->flags.compb_ge = 1;
	}
#endif

	tmr->counter = newCounterVal; 
}




void processSoftTimer16b(SoftTimer16b_t *tmr)
{
	uint16_t newCounterVal;

	if (!tmr->enabled)
	return;

	if (tmr->counter >= tmr->top)
	{
		if (tmr->runOnce)
		{
			tmr->enabled = 0;
		}
		tmr->flags.top = 1;
		tmr->flags.ovfl = 1;
		newCounterVal = 0;
	}
	else
	{
		tmr->flags.top = 0;
		newCounterVal = tmr->counter + 1;
	}
	
#ifdef SFTMR16_USE_COMPA
	tmr->flags.compa_eq = 0;
	tmr->flags.compa_ge = 0;
	if (tmr->counter == tmr->compa)
	{
		tmr->flags.compa_eq = 1;
		tmr->flags.compa_ge = 1;
		tmr->flags.compa_tgl ^= 1;
	}
	else if (tmr->counter > tmr->compa)
	{
		tmr->flags.compa_ge = 1;
	}
#endif

#ifdef SFTMR16_USE_COMPB
	tmr->flags.compb_eq = 0;
	tmr->flags.compb_ge = 0;
	if (tmr->counter == tmr->compb)
	{
		tmr->flags.compb_eq = 1;
		tmr->flags.compb_ge = 1;
		tmr->flags.compb_tgl ^= 1;
	}
	else if (tmr->counter > tmr->compb)
	{
		tmr->flags.compb_ge = 1;
	}
#endif

	tmr->counter = newCounterVal; 
}


