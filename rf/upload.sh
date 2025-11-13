#! /bin/bash

source ../configurations/boards/nano_every.sh

echo Board:$BOARD
echo Port:$PORT

arduino-cli upload --port $PORT --fqbn $BOARD --build-path output --verbose
