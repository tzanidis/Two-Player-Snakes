README

Name: Chris Tzanidis Henry Lo
Section: LBL A2 (60696)

Copied code includes code taught in class such as the function WaitOnSerial3(), and arduino library.

Accessories:
* 2 Arduino Mega Board (AMG)
* 2 TFT LCD Screen from Adafruit
* 2 Sparkfun Thumb Joystick

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

For serial pins: (1) is host, (2) is client
Arduino(1) TX3 Communication Pin (14) <--> Arduino(2) RX3 Communication Pin (15)
Arduino(2) TX3 Communication Pin (14) <--> Arduino(1) RX3 Communication Pin (15)

(The following applies to only arduino 1, the host.)
Arduino(1) Digital port 13 <--> Resistor <--> Arduino(1) 5.0V Port

(The following applies to only arduino 2, the client.)
Arduino(2) Digital port 13 <--> Resistor <--> Arduino(2) GND

Photo of wiring can be seen in the zip as .png

**Wiring Instructions End**

Coding instructions:
Run makefile, upload into arduino, then enter command "serial-mon" into console. Use tft screen and joystick to test.

Potential bugs (and their bugfixes):
Sometimes game will end off of a random collision in the beginning, just reset arduino and try again if this happens.

Functions/Milestones reached:
Drawn movable snakes, randomly placed points are added.
Collision with walls, other snakes, and itself and timeouts as victory conditions are added.
Menu displaying vital information, game informs the player to 'reset' the arduino are added.
Winner/Loser screen and handshake between two arduinos added.
Adjustable game speed is incomplete, but thus makes program more stable.
