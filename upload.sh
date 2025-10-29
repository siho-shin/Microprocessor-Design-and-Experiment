#!/bin/bash
set -e
export PROJECT=jkit-128
export PROGRAMMER=stk500
export PORT=/dev/ttyUSB0

sudo avrdude -c $PROGRAMMER -P $PORT -p m128 -U flash:w:$PROJECT.hex
