#!/bin/bash

src="src/main.c src/joystick.c src/ads1115.c src/gpio.c"
gcc $src -o driver.out
