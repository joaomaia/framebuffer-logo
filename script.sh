#!/bin/sh

#compile the program which will dump the raw data from the JPEG
g++ dump.cpp -o dump -Wall -lSDL2_image

#run it!
./dump

#create the object file based on the raw image data
objcopy --input binary --output elf64-x86-64 --binary-architecture i386:x86-64 image.raw image.o

#compile!
g++ framebuffer-logo.cpp image.o -o framebuffer-logo -Wall

#remove temporary data
rm image.raw image.o dump


