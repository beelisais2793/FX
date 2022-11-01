#!/bin/bash
#

RUNPATH=`readlink -f "$0"`
RUNPATH="$(dirname "$RUNPATH")"

LD_LIBRARY_PATH=$RUNPATH:$LD_LIBRARY_PATH $RUNPATH/chimera "$@"
