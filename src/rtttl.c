/******************************************************************************************************
RTTTL Format Specifications

RTTTL (RingTone Text Transfer Language) is the primary format used to distribute
ringtones for Nokia phones. An RTTTL file is a text file, containing the
ringtone name, a control section and a section containing a comma separated
sequence of ring tone commands. White space must be ignored by any reader
application.

Example:
Simpsons:d=4,o=5,b=160:32p,c.6,e6,f#6,8a6,g.6,e6,c6,8a,8f#,8f#,8f#,2g
Greensleaves:d=4,o=5,b=140:g,2a#,c6,d.6,8d#6,d6,2c6,a,f.,8g,a,2a#,g,g.,8f,g,2a,f,2d,g,2a#,c6,d.6,8e6,d6,2c6,a,f.,8g,a,a#.,8a,g,f#.,8e,f#,2g

This file describes a ringtone whose name is 'Simpsons'. The control section
sets the beats per minute at 160, the default note length as 4, and the default
scale as Octave 5.
<RTX file> := <name> ":" [<control section>] ":" <tone-commands>

    <name> := <char> ; maximum name length 10 characters

    <control-section> := <control-pair> ["," <control-section>]

        <control-pair> := <control-name> ["="] <control-value>

        <control-name> := "o" | "d" | "b"
        ; Valid in control section: o=default scale, d=default duration, b=default beats per minute.
        ; if not specified, defaults are 4=duration, 6=scale, 63=beats-per-minute
        ; any unknown control-names must be ignored

        <tone-commands> := <tone-command> ["," <tone-commands>]

        <tone-command> :=<note> | <control-pair>

        <note> := [<duration>] <note> [<scale>] [<special-duration>] <delimiter>

            <duration> := "1" | "2" | "4" | "8" | "16" | "32"
            ; duration is divider of full note duration, eg. 4 represents a quarter note

            <note> := "P" | "C" | "C#" | "D" | "D#" | "E" | "F" | "F#" | "G" | "G#" | "A" | "A#" | "B"

            <scale> :="4" | "5" | "6" | "7"
            ; Note that octave 4: A=440Hz, 5: A=880Hz, 6: A=1.76 kHz, 7: A=3.52 kHz
            ; The lowest note on the Nokia 61xx is A4, the highest is B7

            <special-duration> := "." ; Dotted note

; End of specification 
******************************************************************************************************/

#if defined(__AVR__)
#include <avr/io.h>
#else
#include <stdio.h>
#include <string.h>
#endif

#include <stdlib.h>

#include "rtttl.h"

/* Default Tune Parameters */
#define DEFAULT_NOTE_LENGTH (4)
#define DEFAULT_OCTAVE (5)
#define DEFAULT_BPM (63)

/* Default Note Parameters */
#define DEFAULT_DURATION (1)

#define IS_NUM(c) ((c>47)&&(c<58))
#define IS_DURATION(d) ((d==RTTTL_D1)||(d==RTTTL_D2)||\
                        (d==RTTTL_D4)||(d==RTTTL_D8) ||\
                        (d==RTTTL_D16)||(d==RTTTL_D32))

#define IS_OCTAVE(o) ((o==RTTTL_O4)||(o==RTTTL_O5)||\
                      (o==RTTTL_O6)||(o==RTTTL_O7))

#define IS_PITCH(p) ((p=='p')||(p=='P')||\
                     (p=='a')||(p=='A')||\
                     (p=='b')||(p=='B')||\
                     (p=='c')||(p=='C')||\
                     (p=='d')||(p=='D')||\
                     (p=='e')||(p=='E')||\
                     (p=='f')||(p=='F')||\
                     (p=='g')||(p=='G'))


static void parse_name (Rtttl_Tune_s* t) {
    /* Step through string until reaching a colon or null */
    for (uint8_t i=0;t->t[t->c]!=':' && t->t[t->c];t->c++,i++) {
        /* save name up to 10 characters */
        if (i<NAME_LEN) t->n[i] = t->t[t->c];
    }
    /* Don't leave it on a colon outta respec */
    t->c++;
}

static void parse_control (Rtttl_Tune_s* t) {
    /* Step through string until reaching a colon or null */
    while (t->t[t->c]!=':' && t->t[t->c]) {
        switch (t->t[t->c]) {
        case 'd':
            /* move the cursor to the first number */
            t->c+=2;
            t->d=atoi(&t->t[t->c]);
            break;
        case 'o':
            t->c+=2;
            t->o=atoi(&t->t[t->c]);
            break;
        case 'b':
            t->c+=2;
            t->b=atoi(&t->t[t->c]);
            break;
        default:
            t->c++;
            break;
        }
    }

    /* Don't leave it on a colon outta respec */
    t->c++;

    /* Set defaults if not parsed */
    if (t->d==0) t->d=DEFAULT_NOTE_LENGTH;
    if (t->o==0) t->o=DEFAULT_OCTAVE;
    if (t->b==0) t->b=DEFAULT_BPM;
}

static Rtttl_Pitch_e get_pitch (uint8_t c, uint8_t sharp) {
    Rtttl_Pitch_e p = RTTTL_PP;

    switch (c) {
    default:
    case 'P':
        break;
    case 'C': p = RTTTL_PC; break;
    case 'D': p = RTTTL_PD; break;
    case 'E': p = RTTTL_PE; break;
    case 'F': p = RTTTL_PF; break;
    case 'G': p = RTTTL_PG; break;
    case 'A': p = RTTTL_PA; break;
    case 'B': p = RTTTL_PB; break;
    }
    if (sharp) p++;

    return p;
}

static Rtttl_Note_s parse_note (Rtttl_Tune_s* t, uint16_t address_start) {
    /* Note should be in the form: */
    /* [duration]note[scale][special duration] */
    /* Examples: g 2a# c6 d.6 8d#6 d6 2c6 a f. 8g */
    Rtttl_Note_s n;
    uint16_t addr = address_start;
    int tmp;
    uint8_t tmp_pitch; /* temp pitch */

    /* if the first character is a number, then there is a duration */
    if (IS_NUM(t->t[addr])) {
        tmp=atoi(&t->t[addr]);
        if (IS_DURATION(tmp)) n.d=tmp;
        else n.d = t->d;
    /* if there's no duration, set it to the default */
    } else n.d = t->d;

    while(IS_NUM(t->t[addr])) addr++;

    /* check if the next character is a pitch */
    tmp_pitch = (uint8_t)t->t[addr];
    if (IS_PITCH(tmp_pitch)) {
        /* if the pitch is in lower case, make it upper case */
        if (tmp_pitch > 96) tmp_pitch -= 32;
    } else tmp_pitch = (uint8_t)('P');
    /* move cursor past pitch */
    addr++;
    /* check if its a sharp and get pitch */
    if (t->t[addr] == '#') {
        n.p = get_pitch(tmp_pitch,1);
        /* move cursor past sharp symbol */
        addr++;
    } else n.p = get_pitch(tmp_pitch,0);

    /* check if an octave is specified */
    if (IS_NUM(t->t[addr])) {
        /* convert to an integer */
        tmp=atoi(&t->t[addr]);
        /* if integer is a legit octave, set it, otherwise set it to default */
        if (IS_OCTAVE(tmp)) n.o = tmp;
        else n.o = t->o;
        /* move cursor past numbers */
        for (;IS_NUM(t->t[addr]);addr++);
    } else n.o = t->o;

    /* check for special duration */
    if (t->t[addr] == '.') {
        n.s = 1;
        addr++;

        /* check again for octave, since you could put one before the other */
        if (IS_NUM(t->t[addr])) {
            /* convert to an integer */
            tmp=atoi(&t->t[addr]);
            /* if integer is a legit octave, set it. It was already set to default above */
            /* so no need to do anything if its not a legit octave */
            if (IS_OCTAVE(tmp)) n.o = tmp;
        }
    }
    else n.s = 0;

    return n;
}

Rtttl_Note_s rtttl_next_note (Rtttl_Tune_s* t) {
    Rtttl_Note_s n;

    /* return if the tune is done */
    if (t->st == RTTTL_DONE) {
        n.p = RTTTL_PP;
        n.d = RTTTL_D1;
        n.o = RTTTL_O4;
        n.s = 0;
        return n;
    }
    /* set start to current cursor position */
    uint16_t start = t->c;
    /* step through string until reaching a comma or null */
    for (;t->t[t->c]!=',' && t->t[t->c];t->c++);
    /* if the character is null, then this is the last note */
    if (t->t[t->c]==0) t->st=RTTTL_DONE;
    /* parse note */
    n = parse_note(t,start);
    /* don't leave cursor on the comma*/
    if (t->st != RTTTL_DONE) t->c++;

    return n;
}

void rtttl_init (Rtttl_Tune_s* t) {
    parse_name(t);
    parse_control(t);
}

#if defined(__AVR__)
#else
void rtttl_print_tune (Rtttl_Tune_s* t) {
    printf("Tune Name:%s\n",t->n);
    printf("Default Note Length:%d\n",t->d);
    printf("Default Octave:%d\n",t->o);
    printf("Beats Per Minute:%d\n",t->b);
}

void rtttl_print_note (Rtttl_Note_s n) {
    printf("%d\t%d\t%d\t%d\n",n.d,n.p,n.o,n.s);
}
#endif
