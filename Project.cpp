//#includes up here bro
#include <Arduino.h>
#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library

//standard GLOBAL VARIABLES
//standard U of A library settings, assuming Atmel Mega SPI pins
#define SD_CS    5  // Chip select line for SD card
#define TFT_CS   6  // Chip select line for TFT display
#define TFT_DC   7  // Data/command line for TFT
#define TFT_RST  8  // Reset line for TFT (or connect to +5V)

//standard joystick pins
const int VERT = 0;  // analog input
const int HORIZ = 1; // analog input
const int SEL = 9;   // digital input 

void menuSrv(){
  //menu with start selection and 
  snake();
}

void menuCli(){//done
  bool start = FALSE;
  while(start == FALSE){
    //wait for srv to start game
    delay(100);
  }
  snake();
}

void gameTimeStart(){

}

void gameTimeCalculate(){
  
}

void scoring(){
  
}

void winLose(bool val){
  
}

void pointBall(){
  
}

bool collision(){
  
}



void snake(){
  
}


int main(){
  //stuff go here
  
  
  
  
  
  
  //srv/cli part, needs to be in the main
  //setup pin
  int srvCliPin = 13;
  pinMode(srvCliPin, INPUT);
  digitalWrite(srvCliPin, LOW);
  
  if(digitalRead(srvCliPin) == HIGH){ // read pin / determine srv or cli
    Serial.println("pin HIGH Srv");
    function(); // call appropriate functions
  }else{
    Serial.println("pin low cli");
    function(); // call appropriate functions
  }
}
