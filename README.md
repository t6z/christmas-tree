# Christmas Tree

I made a little Christmas tree that lights up and plays a song through a buzzer. I wanted a simple project to expand my skills with schematic capture and PCB layout, and to see how accurate the quick turnaround prototype shops (PCBWAY) do. This project uses an ATTiny402, 3 shift registers to control the LED's, and a simple circuit to try a piezo buzzer. I used Kicad as my EDA tool. It was fun using AI + InkScape to generate different graphics for the tree and import them for the edge cut and silkscreen layers.

![christmas-tree-leds](https://github.com/user-attachments/assets/a0ded5a0-d2da-4026-9995-bac7d6903f99)

## Building Code

### Prereqs

__AVR GNU Toolchain__:

I use the AVR GNU Toolchain (GNU Binutils, GCC, AVR-LibC) to build my code. The attiny402 is supported in the AVR-LibC 2.2.0 release, but ubuntu 24.04 LTS repository only has v2.0.0 (as of December 2024). You could build GNU Binutils, GCC, and AVR-LibC from scratch, but luckily this nice guy already has: [avr-gcc-build](https://github.com/ZakKemble/avr-gcc-build)

I'm using the 14.1.0 (2024-06-14) Release. I downloaded it and extracted it into `~/avr`.

__AVR Dude__:

I downloaded the v8.0 Release of AVR Dude from the [official GitHub repository](https://github.com/avrdudes/avrdude) and extracted it into `~/avr`.


### Building

Run `make`

```bash
$ make
mkdir -p build/src
~/avr/avr-gcc-14.1.0-x64-linux/bin/avr-gcc -c -o build/src/main.o  -std=c11 -Wall -Wextra -Werror -mmcu=attiny402 -DF_CPU=3333333UL -O1 -g -gdwarf-2 -Wl,-Map,build/christmas-tree.map -DDEBUG  src/main.c
mkdir -p build/src
~/avr/avr-gcc-14.1.0-x64-linux/bin/avr-gcc -c -o build/src/notes.o  -std=c11 -Wall -Wextra -Werror -mmcu=attiny402 -DF_CPU=3333333UL -O1 -g -gdwarf-2 -Wl,-Map,build/christmas-tree.map -DDEBUG  src/notes.c
mkdir -p build/src
~/avr/avr-gcc-14.1.0-x64-linux/bin/avr-gcc -c -o build/src/rtttl.o  -std=c11 -Wall -Wextra -Werror -mmcu=attiny402 -DF_CPU=3333333UL -O1 -g -gdwarf-2 -Wl,-Map,build/christmas-tree.map -DDEBUG  src/rtttl.c
mkdir -p build/src
~/avr/avr-gcc-14.1.0-x64-linux/bin/avr-gcc -c -o build/src/tunes.o  -std=c11 -Wall -Wextra -Werror -mmcu=attiny402 -DF_CPU=3333333UL -O1 -g -gdwarf-2 -Wl,-Map,build/christmas-tree.map -DDEBUG  src/tunes.c
~/avr/avr-gcc-14.1.0-x64-linux/bin/avr-gcc -o build/christmas-tree.elf build/src/main.o build/src/notes.o build/src/rtttl.o build/src/tunes.o  -std=c11 -Wall -Wextra -Werror -mmcu=attiny402 -DF_CPU=3333333UL -O1 -g -gdwarf-2 -Wl,-Map,build/christmas-tree.map -DDEBUG 
~/avr/avr-gcc-14.1.0-x64-linux/bin/avr-objdump -h -S build/christmas-tree.elf > build/christmas-tree.lst
~/avr/avr-gcc-14.1.0-x64-linux/bin/avr-size build/christmas-tree.elf
   text	  data	   bss	   dec	   hex	filename
   3586	   156	    44	  3786	   eca	build/christmas-tree.elf
~/avr/avr-gcc-14.1.0-x64-linux/bin/avr-objcopy -j .text -j .data -O ihex build/christmas-tree.elf build/christmas-tree.hex
~/avr/avr-gcc-14.1.0-x64-linux/bin/avr-objcopy -j .text -j .data -O binary build/christmas-tree.elf build/christmas-tree.bin
```
