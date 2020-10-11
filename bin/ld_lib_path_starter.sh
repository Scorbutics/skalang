#!/bin/sh
export LD_LIBRARY_PATH="/data/data/com.termux/files/home/scorbutics.dev.lab/skalang/lib/:$LD_LIBRARY_PATH"

#SKALANG_LIB_DIR=/data/data/com.termux/files/home/scorbutics.dev.lab/skalang/lib/
#LD_PRELOAD="$SKALANG_LIB_DIR/libskalang.so:$SKALANG_LIB_DIR/libskabase.so"

$1
