# Handheld Console
Based on the Raspberry Pi Zero 2 and capable of emulating popular retro consoles such as NES, SNES, GBA or PSP.
Can also run simple games, if they are Linux and gamepad compatible.

![Handheld](./images/cables-1.png)

The main challange with this project was getting GPIO and I2C support - since this specific Raspberry
Pi model came out just a few weeks before I've started the project I had to write everything from scratch
in C - no existing Python library worked on this architecture yet.

## Parts
<pre>
board:        Raspberry Pi Zero 2 W
power supply: DFRobot UPS Hat (MAX17043)
display:      Waveshare 12030
case:         Modified Steam Controller CAD files
adc:          Adafruit ADS1115
joysticks:    ALPS Stick Controller (RKJXV1220001)
</pre>
Special thanks to [Cryen](https://github.com/JustCryen) for creating the case and helping with electronics.
