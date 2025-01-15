#ifndef __NOTES_H__
#define __NOTES_H__

#include <avr/io.h>

#define NUM_NOTES (12U)

typedef enum {
   PC4 = 0,
   PCs4,
   PD4,
   PDs4,
   PE4,
   PF4,
   PFs4,
   PG4,
   PGs4,
   PA4,
   PAs4,
   PB4
} Notes_e;

extern uint16_t tca0_per[NUM_NOTES];
extern uint16_t tca0_cmp0[NUM_NOTES];

#endif // __NOTES_H__
