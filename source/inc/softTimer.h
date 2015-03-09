/*
 * softTimer.h
 *
 * Created: 22.04.2013 12:08:37
 *  Author: Avega
 */
 
#ifndef SOFT_TIMER_H_
#define SOFT_TIMER_H_

#include <stdint.h>



#define SFTMR8_USE_COMPA
#define SFTMR8_USE_COMPB

typedef struct {
	struct {
		uint8_t top			: 1;	// (counter >= top) ? 1 : 0
		uint8_t ovfl		: 1;	// (counter >= top) ? 1 			<- sticky
		uint8_t tgl			: 1;
#ifdef SFTMR8_USE_COMPA		
		uint8_t compa_eq	: 1;	// (counter == compa) ? 1 : 0
		uint8_t compa_ge	: 1;	// (counter >= compa) ? 1 : 0
		uint8_t compa_tgl	: 1;	// (counter == compa) ? (^=1)
#endif
#ifdef SFTMR8_USE_COMPB
		uint8_t compb_eq	: 1;	// (counter == compb) ? 1 : 0
		uint8_t compb_ge	: 1;	// (counter >= compb) ? 1 : 0
		uint8_t compb_tgl	: 1;	// (counter == compb) ? (^=1)
#endif
	} flags;
	uint8_t		enabled	: 1;		// Allows count
	uint8_t		runOnce : 1;		// If set, timer counts to top and then stops. If cleared, timer restarts from 0.
	uint8_t		counter;			// counter variable
	uint8_t		top;				// top value
#ifdef SFTMR8_USE_COMPA
	uint8_t		compa;				// compare A value
#endif
#ifdef SFTMR8_USE_COMPB
	uint8_t		compb;				// compare B value
#endif
} SoftTimer8b_t;
 



#define SFTMR16_USE_COMPA
#define SFTMR16_USE_COMPB
 
typedef struct {
	struct {
		uint8_t top			: 1;	// (counter >= top) ? 1 : 0
		uint8_t ovfl		: 1;	// (counter >= top) ? 1 			<- sticky
		uint8_t tgl			: 1;
#ifdef SFTMR16_USE_COMPA		
		uint8_t compa_eq	: 1;	// (counter == compa) ? 1 : 0
		uint8_t compa_ge	: 1;	// (counter >= compa) ? 1 : 0
		uint8_t compa_tgl	: 1;	// (counter == compa) ? (^=1)
#endif
#ifdef SFTMR16_USE_COMPB
		uint8_t compb_eq	: 1;	// (counter == compb) ? 1 : 0
		uint8_t compb_ge	: 1;	// (counter >= compb) ? 1 : 0
		uint8_t compb_tgl	: 1;	// (counter == compb) ? (^=1)
#endif
	} flags;
	uint8_t		enabled	: 1;		// Allows count
	uint8_t		runOnce : 1;		// If set, timer counts to top and then stops. If cleared, timer restarts from 0.
	uint16_t	counter;			// counter variable
	uint16_t	top;				// top value
#ifdef SFTMR16_USE_COMPA
	uint16_t	compa;				// compare A value
#endif
#ifdef SFTMR16_USE_COMPB
	uint16_t	compb;				// compare B value
#endif
} SoftTimer16b_t;




void processSoftTimer8b(SoftTimer8b_t *tmr);
void processSoftTimer16b(SoftTimer16b_t *tmr);


#endif
