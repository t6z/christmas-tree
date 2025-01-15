#include <stdio.h>
#include <string.h>
#include "../src/rtttl.h"

Rtttl_Tune_s input_tune;

void print_help (void) {
    printf("Usage:\n./test_rtttl <RTTTL tune as string>\n");
}

int main (int argc, char *argv[]) {
    if (argc==2) {
        if (strcmp(argv[1],"--help") == 0) {
            print_help();
        } else {
            printf("Parsing argv[1] as RTTTL string...\n\n");
            input_tune.t = argv[1];
            rtttl_init(&input_tune);
            rtttl_print_tune(&input_tune);

            printf("Dur.\tPitch\tOctave\tSpecial\n");
            while(input_tune.st != RTTTL_DONE) {
                rtttl_print_note(rtttl_next_note(&input_tune));
            }
        }
    } else {
        printf("Expected only 1 RTTTL tune inputted as an argument.\n");
        print_help();
    }

    return 0;
}