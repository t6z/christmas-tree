# Christmas Tree

## Building Code

### Prereqs

__AVR GNU Toolchain__:

I use the AVR GNU Toolchain (GNU Binutils, GCC, AVR-LibC) to build my code. The attiny402 is supported in the AVR-LibC 2.2.0 release, but ubuntu 24.04 LTS repository only has v2.0.0. You could build GNU Binutils, GCC, and AVR-LibC from scratch, but luckily this nice guy already has: [avr-gcc-build](https://github.com/ZakKemble/avr-gcc-build)

I'm using the 14.1.0 (2024-06-14) Release. Download it and extracted it into `~/avr`.

__AVR Dude__:

I downloaded the v8.0 Release of AVR Dude from the [official GitHub repository](https://github.com/avrdudes/avrdude) and extracted it into `~/avr`.


### Building

Run `make`

```bash
$ make
mkdir -p src/output
~/avr/avr-gcc-14.1.0-x64-linux/bin/avr-gcc-14.1.0 src/main.c -o src/output/main.elf -mmcu=attiny402 -DF_CPU=20000000UL -Os
~/avr/avr-gcc-14.1.0-x64-linux/bin/avr-objcopy src/output/main.elf -O ihex src/output/main.hex
```
