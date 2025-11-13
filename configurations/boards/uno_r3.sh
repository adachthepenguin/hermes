#! /bin/bash

BOARD="arduino:avr:uno"
PORT=$(arduino-cli board list -b $BOARD | grep $BOARD | awk {'print $1'})
