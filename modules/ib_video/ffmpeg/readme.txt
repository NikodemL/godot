Built from:
https://github.com/FFmpeg/FFmpeg.git
master @ 734d760e2fb2621040edef3536b5935e7bc45351

Command line:
#!/bin/sh 
./configure --enable-asm --enable-yasm --disable-doc --disable-ffplay --disable-ffprobe --disable-ffmpeg --enable-shared --disable-static --disable-bzlib --disable-libopenjpeg --disable-iconv --disable-zlib --toolchain=msvc --arch=x86
make clean
make