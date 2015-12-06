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
(The following applies to both Arduino 1 and Arduino 2. Arduino 1 is host, Arduino 2 is client.)
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

(The following applies to only arduino 1, the host.)


(The following applies to only arduino 2, the client.)


**Wiring Instructions End**

Coding instructions:
Run makefile, upload into arduino, then enter command "serial-mon" into console. Use tft screen and joystick to test.

Potential bugs (and their bugfixes):
Sometimes game will end off of a random collision in the beginning, just reset arduino and try again if this happens.
