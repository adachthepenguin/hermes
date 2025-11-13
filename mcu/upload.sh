#! /bin/bash

source ../configurations/boards/uno_r3.sh

echo Board:$BOARD
echo Port:$PORT

arduino-cli upload --port $PORT --fqbn $BOARD --build-path output --verbose
