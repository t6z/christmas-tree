#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "notes.h"
#include "rtttl.h"
#include "tunes.h"

#define HIGH (1U)
#define LOW  (0U)

#define PIN_SER (1U) /* PA1 */
#define PIN_SHIFT_CLK (2U) /* PA2 */
#define PIN_STORE_CLK (7U) /* PA7 */

#define NUM_LEDS (24U)

Rtttl_Tune_s tune;
volatile Rtttl_Note_s next_note = {
   .d = RTTTL_D1,
   .o = RTTTL_O4,
   .p = RTTTL_PC,
   .s = 0
};
uint16_t cnts_default_note = 0;

uint8_t led_duration[3] = {9,11,13};
uint8_t led_duration_cnt = 0;
uint16_t led_counter = 0;
uint8_t led_turnon[NUM_LEDS] = {0};
uint8_t led_order[NUM_LEDS] = {12, 7, 15, 13, 19, 23, 18, 6, 16, 20, 3, 9, 21, 14, 1, 17, 0, 10, 8, 4, 5, 11, 2, 22};
uint32_t output = 0;
uint8_t counter = 0;

void digitalWrite(uint8_t pin, uint8_t level) {
   if (level) {
      VPORTA_OUT |= (uint8_t)(1U<<pin);
   } else {
      VPORTA_OUT &= (uint8_t)(~(1U<<pin));
   }
}

void shiftOut(uint8_t data)
{
   for (uint8_t i = 0; i < 8; i++)  {
      /* Update serial data pin */
      digitalWrite(PIN_SER, (data & (1 << i)));
      /* Cycle shift clock */
      digitalWrite(PIN_SHIFT_CLK, HIGH);
      digitalWrite(PIN_SHIFT_CLK, LOW);
   }
}

void updateShiftRegister(uint32_t data) {
   /* Set store clock pin low */
   digitalWrite(PIN_STORE_CLK,LOW);

   /* shift upper 8 bits out */
   shiftOut((uint8_t)(data&0xFF));
   shiftOut((uint8_t)((data>>8)&0xFF));
   shiftOut((uint8_t)((data>>16)&0xFF));

   /* Setting store clock pin high again updates shift register's output */
   digitalWrite(PIN_STORE_CLK,HIGH);
}

void update_note (Rtttl_Pitch_e pitch, Rtttl_Octave_e octave) {
   uint16_t period, compare;

   if (pitch == RTTTL_PP) {
      TCA0.SINGLE.CMP0BUF = 0;
   } else {
      if (octave == RTTTL_O4) {
         period = tca0_per[pitch-1];
         compare = tca0_cmp0[pitch-1];
      } else {
         period = tca0_per[pitch-1] >> (octave-4);
         compare = tca0_cmp0[pitch-1] >> (octave-4);
      }
      TCA0.SINGLE.PERBUF = period;
      TCA0.SINGLE.CMP0BUF = compare;
   }
}

void init_clock (void) {
   /* set the 32KHz Oscillator in Run Standby mode so it is on for the RTC */
   CLKCTRL.OSC32KCTRLA = CLKCTRL_RUNSTDBY_bm;
   /* wait for 32KHz Oscillator to be stable */
   while (!((CLKCTRL.MCLKSTATUS&0x10)==0x10));
}

/* Initialize Port */
void init_pins (void) {
   /* GPIO pins are cleared upon RESET */
   /* set PA1,2,3,7 as output pins */
   PORTA.DIR = PIN1_bm | PIN2_bm | PIN3_bm | PIN7_bm;
}

/* Initialize Real Time Clock */
void init_rtc (void) {
   /* select 32KHz clock */
   RTC.CLKSEL = 0;
   /* Set CMP value */
   RTC.CMP = 0xFFFF;
   /* Enable CMP interrupt */
   RTC.INTCTRL = RTC_CMP_bm;
   /* Enable RTC */
   RTC.CTRLA = RTC_RTCEN_bm;
}

/* Initialize Timer A */
void init_tca0 (void) {
   /* Enable CMP0 and set Waveform Generation Mode to Dual-Slope PWM */
   TCA0.SINGLE.CTRLB = TCA_SINGLE_CMP0EN_bm | TCA_SINGLE_WGMODE_DSBOTTOM_gc;

   /* Set Period */
   TCA0.SINGLE.PERBUF = 0x3FFU;
   /* Set CMP0 */
   TCA0.SINGLE.CMP0BUF = 0x1FFU;

   /* Select Clock Divider and Enable Timer */
   TCA0.SINGLE.CTRLA = TCA_SINGLE_CLKSEL_DIV4_gc | TCA_SINGLE_ENABLE_bm;
}

/* Initialize Timer B */
void init_tcb0 (void) {
   TCB0.CCMP = 0xFFF;
   TCB0.INTCTRL = TCB_CAPT_bm;
   TCB0.CTRLA = TCB_CLKSEL_CLKTCA_gc | TCB_ENABLE_bm;
}

void init (void) {
   init_clock();
   init_pins();
   init_rtc();
   init_tca0();
   init_tcb0();
}

void init_tune (char* tune_p) {
   tune.c = 0;
   tune.st = RTTTL_NOERR;
   tune.t = &tune_p[0];
   rtttl_init(&tune);

   cnts_default_note = (uint16_t)((60.0/tune.b)*32768);

   /* if the default length is not a quarter note */
   if (tune.d > RTTTL_D4) {
      cnts_default_note = cnts_default_note * (tune.d/RTTTL_D4);
   } else if (tune.d < RTTTL_D4) {
      cnts_default_note = cnts_default_note * (RTTTL_D4/tune.d);
   }
}

int main (void) {
   uint8_t playing_tune = 0;

   init();
   sei(); /* enable interrupts */

   TCA0.SINGLE.CMP0BUF = 0;

   init_tune((char*)&tune_green[0]);
   next_note = rtttl_next_note(&tune);
   playing_tune = 1;
   /* everything is handled in interrupts */
   /* loop forever */
   for(;;) {
      if (!playing_tune) {
         _delay_ms(5000);
         init_tune((char*)&tune_green[0]);
         next_note = rtttl_next_note(&tune);

         playing_tune = 1;
      } else if (tune.st == RTTTL_DONE){
         playing_tune = 0;
      }
   }

   return 0;
}

ISR(TCB0_INT_vect) {
   if (counter%13==0) {
      led_turnon[led_order[led_counter]] = led_duration[led_duration_cnt];
      led_duration_cnt++;
      if (led_duration_cnt%3==0) led_duration_cnt=0;
      led_counter++;
      if (led_counter%NUM_LEDS==0) led_counter=0;
      output = 0;
      for (uint8_t i = 0; i < NUM_LEDS; i++) {
         if (led_turnon[i]) {
            led_turnon[i]--;
            output |= ((uint32_t)1)<<i;
         }
      }
      updateShiftRegister(output);
   }

   /* update counter */
   counter++;
   if (counter%50==0) counter = 0;
   /* Clear the interrupt flag */
   TCB0.INTFLAGS = TCB_CAPT_bm;
}

ISR(RTC_CNT_vect) {
   uint16_t cnt = RTC.CNT;
   uint16_t cnt_add;

   update_note(next_note.p,next_note.o);

   /* if its the default length */
   if (tune.d == next_note.d) {
      cnt_add = cnts_default_note;
   } else if (((uint8_t)(tune.d/next_note.d)) > 0) {
      cnt_add = (uint16_t)(cnts_default_note * (tune.d/next_note.d));
   } else {cnt_add = (uint16_t)(cnts_default_note / (next_note.d/tune.d));}

   if (next_note.s) {cnt_add += (cnt_add>>1);}

   next_note = rtttl_next_note(&tune);

   RTC.CMP = cnt+cnt_add;
   /* Clear the Compare Match Interrupt Flag */
   RTC.INTFLAGS = RTC_CMP_bm;
}
