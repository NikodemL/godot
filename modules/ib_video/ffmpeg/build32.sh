#!/bin/sh 
./configure --enable-asm --enable-yasm --disable-doc --disable-ffplay --disable-ffprobe --disable-ffmpeg --enable-shared --disable-static --disable-bzlib --disable-libopenjpeg --disable-iconv --disable-zlib --toolchain=msvc --arch=x86
make clean
make