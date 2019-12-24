#!/usr/bin/env bash

BASEDIR=$(dirname "$0")
echo "$BASEDIR"

$BASEDIR/skalang_test
$BASEDIR/skalang_bytecode_test
