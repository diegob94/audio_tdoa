#!/bin/bash

set -v

xtensa-esp32-elf-gdb -x debug/gdbinit build/audio_tdoa.elf

