#! /bin/bash

source ../configurations/boards/nano_every.sh

echo Board:$BOARD
echo Port:$PORT

arduino-cli monitor --port $PORT --config 9600
