#!/bin/sh
echo "Cleaning up build files.."
make clean -C src
echo "Start building.."
make -C src
