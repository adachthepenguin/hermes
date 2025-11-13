#! /bin/bash

source ../configurations/boards/uno_r3.sh

echo Board:$BOARD

rm -rf ./shared
cp -r ../shared ./shared

rm -rf ./output
mkdir output
arduino-cli compile --fqbn $BOARD --output-dir output --verbose
