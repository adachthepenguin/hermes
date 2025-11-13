#! /bin/bash

BOARD="arduino:megaavr:nona4809"
PORT=$(arduino-cli board list -b $BOARD | grep $BOARD | awk {'print $1'})
