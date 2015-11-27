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

//Define the tft
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

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

bool collision(){
  
}



void snake(){
  //Initialize screen/map
  
  int size = 50; //Snake maximum possible length
  
  //Create snakes - 2d array
  //call with snakeCli[i][j]
  int** snakeCli = malloc(sizeof(int*)*size);
  int** snakeSrv = malloc(sizeof(int*)*size);
  for(int i = 0; i < size; ++i){
    int* snakeCli[i] = malloc(size*sizeof(int));
    int* snakeSrv[i] = malloc(size*sizeof(int));
  }
  
  //Spawn random dot, give coordinates
  int dotX;
  int dotY;
  
  randomDot(&dotX, &dotY);
  //3 2 1 GO!
  
  //Start snakes
  while(!collision()){
    
  }
  
}

void pointDot(int* X, int* Y){
  
  randomDot();
}

void randomDot(int* X, int* Y){
  for(int i = 0; i < 4; ++i){//Four loops
    *X += analogRead(3);
    *Y += analogRead(3);
  }
  //Loop again, but range is only 0 to 4
  *X += analogRead(3)/2;
  if(*X < 40){
    *X = 40; 
  }
  *Y += analogRead(3)/2;
  if(*Y < 40){
    *Y = 40; 
  }
}

int main(){
  //stuff go here
  
  init();
  
  tft.initR(INITR_BLACKTAB);
  
  Serial.begin(9600);
  
  randomSeed(analogRead(4));
  
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
  
  Serial.end();
}
