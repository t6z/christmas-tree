#ifndef __RTTTL_H__
#define __RTTTL_H__

#include <stdint.h>

#define NAME_LEN (1)

typedef enum {
    RTTTL_NOERR=0,
    RTTTL_DONE,
} Rtttl_TuneStatus_e;

typedef enum {
    RTTTL_D1 = 1,
    RTTTL_D2 = 2,
    RTTTL_D4 = 4,
    RTTTL_D8 = 8,
    RTTTL_D16 = 16,
    RTTTL_D32 = 32
} Rtttl_Duration_e;

typedef enum {
    RTTTL_PP = 0,
    RTTTL_PC,
    RTTTL_PCs,
    RTTTL_PD,
    RTTTL_PDs,
    RTTTL_PE,
    RTTTL_PF,
    RTTTL_PFs,
    RTTTL_PG,
    RTTTL_PGs,
    RTTTL_PA,
    RTTTL_PAs,
    RTTTL_PB
} Rtttl_Pitch_e;

typedef enum {
    RTTTL_O4 = 4,
    RTTTL_O5,
    RTTTL_O6,
    RTTTL_O7
} Rtttl_Octave_e;

typedef struct {
    Rtttl_Duration_e d; /* duration */
    Rtttl_Pitch_e p; /* pitch */
    Rtttl_Octave_e o; /* octave */
    uint8_t s; /* special duration */
} Rtttl_Note_s;

typedef struct {
    Rtttl_TuneStatus_e st;
    uint8_t d; /* default note length */
    uint8_t o; /* default scale octave */
    uint8_t b; /* beats per minute */
    uint16_t c; /* cursor position in t string */
    uint8_t n[NAME_LEN]; /* Tune name */
    char* t; /* pointer to the t string */
} Rtttl_Tune_s;

void rtttl_init (Rtttl_Tune_s* t);
Rtttl_Note_s rtttl_next_note (Rtttl_Tune_s* t);

#if defined(__AVR__)
// void rtttl_play_tune (Rtttl_Tune_s* t);
#else
void rtttl_print_tune (Rtttl_Tune_s* t);
void rtttl_print_note (Rtttl_Note_s n);
#endif // __AVR__

#endif // __RTTTL_H__
