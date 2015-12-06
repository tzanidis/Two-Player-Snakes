README

Name: Chris Tzanidis Henry Lo
Section: LBL A2 (60696)

Copied code includes code taught in class such as the function WaitOnSerial3(), and arduino library.

Accessories:
* 1 Arduino Mega Board (AMG)
* 1 TFT LCD Screen from Adafruit
* 1 Sparkfun Thumb Joystick
* 1 Potentiometer

Wiring Instructions:
For screen:
GND <--> BB GND bus
VCC <--> BB positive bus
RESET <--> Pin 8
D/C (Data/Command) <--> Pin 7
CARD_CS (Card Chip Select) <--> Pin 5
TFT_CS (TFT/screen Chip Select) <--> Pin 6
MOSI (Master Out Slave In) <--> Pin 51
SCK (Clock) <--> Pin 52
MISO (Master In Slave Out) <--> 50
LITE (Backlite) <--> BB positive bus

For joystick:
VCC <--> BB positive bus
VERT <--> Pin A0
HOR <--> Pin A1
SEL <--> Pin 9
GND <--> BB GND bus

Coding instructions:
Run makefile, upload into arduino, then enter command "serial-mon" into console. Use tft screen and joystick to test.

Potential bugs (and their bugfixes):
If it keeps switching between map and menu, replace if(!select) with if(select) for both modes.
If the SD card spews random big numbers, try replugging SD card or tossing around with the menu options.

Additional notes:
For some reason, the program seems to work better with many serial monitor commands. You may still delete them as you wish to unclutter the console.
