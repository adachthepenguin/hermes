#! /bin/bash

source ../configurations/boards/uno_r3.sh

echo Board:$BOARD
echo Port:$PORT

arduino-cli monitor --port $PORT --config 9600
