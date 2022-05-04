#!/bin/bash

set -v

sudo $(which openocd) -f debug/esp32-ftdi.cfg
