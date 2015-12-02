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

typedef struct {//size is 50 by default
    int* x[50];
    int* y[50];
    int head = 4;
    int tail = 0;
    int length = 5;
} Snake;

void sendChar(char msg){
  //Start = S
  //Sync = A
  //Up = U
  //Down = D
  //Left = L
  //Right = R
  
  Serial3.write(msg);
  //debug
  //Serial.print("msg sent: "); Serial.println('msg');
}//Done

bool waitOnSerial3(uint8_t nbytes, long timeout){
    unsigned long deadline = millis() + timeout;
    while((Serial3.available() < nbytes) && (timeout < 0 || millis() < deadline)){
        delay(1);
    }
    return Serial3.available() >= nbytes;
}

bool listen(char c){
  if(waitOnSerial3(1,1000)){
    char msg = Serial3.read();
    //debug
    //Serial.print("msg heard: "); Serial.println('msg');
    if(msg == c){
      return true;
    }
  }
  return false;
}//Done

char listenDir(char c){
  if(waitOnSerial3(1,1000)){
    char msg = Serial3.read();
    //debug
    //Serial.print("msg heard: "); Serial.println('msg');
    if(msg == c){
      return true;
    }
  }
  return false;
}//Done

void startUp(){
  tft.setTextColor(0xFFFF, 0x0000);
  
  //Draw boundaries
  fillRect(16, 0, 4, 128, 0xFFFF);
  fillRect(140, 0, 4, 128, 0xFFFF);
  fillRect(0, 20, 120, 4, 0xFFFF);
  fillRect(108, 20, 120, 4, 0xFFFF);
  
  //Initialize snakes
  //initSnake();
  
  //Draw player ID
  tft.setCursor(29,10);
  tft.print("HOST SNAKE");
  tft.setCursor(131,70);
  tft.print("CLIENT SNAKE");
  
  //pointDot();
  pointDot();
  
  //3,2,1,GO
  tft.setCursor(0,62);
  tft.print("3");
  delay(900);
  fillRect(0, 62, 4, 8, 0x0000);
  tft.print("2");
  delay(900);
  fillRect(0, 62, 4, 8, 0x0000);
  tft.print("1");
  delay(900);
  tft.setCursor(0,60);
  fillRect(0, 62, 4, 8, 0x0000);
  
  //Clear player ID
  fillRect(29, 10, 40, 8, 0x0000);
  fillRect(131, 70, 48, 8, 0x0000);
  
  tft.print("GO");
  fillRect(0, 60, 8, 8, 0x0000);
  delay(50);
  
  //Start game
  snake();
}//Almost

int randomDotX(){
    int x = analogRead(3) * 4;
    //Loop again, but range is only 0 to 4
    x += analogRead(3)/2;
    if(x > 40){
        x = 40; 
    }
    return x;
}//Done

int randomDotY(){
    int y = analogRead(3) * 4;
    //Loop again, but range is only 0 to 4
    y += analogRead(3)/2;
    if(y > 40){
        y = 40; 
    }
    return y;
}//Done

void pointDot(){//get rand spot draw dot
  int x = randomDotX();
  int y = randomDotY();
  fillCircle(((x*3)-1)+20.5, ((y*3)-1)+4.5, 2, 0xFFFF);
}//Done

void menuSrv(){
  tft.fillScreen(0x0000);
  tft.setTextColor(0xFFFF, 0x0000);
  tft.setCursor(0,0);
  tft.print("host");
  tft.setCursor(54,76);
  tft.print("START");
  while(true){ //when not pressed
    if(digitalRead(SEL) == 1){
      sendChar('S');
      delay(50);
      startUp();
    }
    delay(50);
  }
}//Done

void menuCli(){
  tft.fillScreen(0x0000);
  tft.setTextColor(0xFFFF, 0x0000);
  tft.setCursor(0,0);
  tft.print("client");
  tft.setCursor(50,72);
  tft.print("Waiting");
  tft.setCursor(58,70);
  tft.print("For Host");
  bool start = FALSE;
  while(start == FALSE){
    start = listen('S');
  }
  startUp();
}//Done

//Winlose is called inside time and collision, val decides winner: 0 is server, 1 is client
void winLose(bool val){
  tft.fillScreen(0x0000);
  if(val == TRUE){//Host? cli?
      tft.setCursor(56,72);
      tft.print("Host");
      tft.setCursor(54,80);
      tft.print("Wins!");
  }else{
      tft.setCursor(56,72);
      tft.print("Client");
      tft.setCursor(54,80);
      tft.print("Wins!");
  }
  //Tell to press reset key
  tft.setCursor(0,152);
  tft.print("Press Reset");
}//Done

char syncSrv(char mov){
    typedef enum {SEND, WFR, STL, LIS, STD, ERR }State; //send, wait for received, send that listening, listen, wait for done. 
    State state = SEND;
    char otherPlayerMov;
    
    while((state != STD) || (state !=ERR)){
        if(state == SEND){
            sendChar(mov);
            Serial.println();
            state = WFR;
        }else if(state == WFR){
            if(listen('A')){
                Serial.println();
                state = LIS;
            }else{
                state = SEND;
            }
        }else if(state == LIS){
            otherPlayerMov = listenDir();
            if(otherPlayerMov == ){
                Serial.println();
                send('A');
                state = STD;
            }
        }else{
            Serial.println();
            state = ERR;
        }
    }
    return otherPlayerMov;
}

char syncCli(char mov){
    typedef enum {LIS, WFR, SEND, WFR, D, ERR }State; //listen for their move / send that received, send our move, wait for received, tell done 
    State state = SEND;
    char otherPlayerMov;
    
    while((state != D) || (state !=ERR)){
        if(state == LIS){
            otherPlayerMov = listenDir();
            if(otherPlayerMov == ){
                Serial.println();
                send('A');
                state = WFD;
            }
        }else if(state == SEND){
            sendChar(mov);
            Serial.println();
            state = WFR;
        }else if(state == WFR){
            if(listen('A')){
                Serial.println();
                state = D;
            }else{
                state = SEND;
            }
        }else{
            Serial.println();
            state = ERR;
        }
    }
    return otherPlayerMov;
}

bool collision(Snake* snakeCli, Snake* snakeSrv){
    //Check for walls
    if((snakeCli->x[snakeCli->head]<0)
    ||(snakeCli->x[snakeCli->head]>39)
    ||(snakeCli->y[snakeCli->head]<0)
    ||(snakeCli->y[snakeCli->head]>39)){
        //Snake client hit a wall
        winLose(0);
        return true;
    }
    if((snakeSrv->x[snakeSrv->head]<0)
    ||(snakeSrv->x[snakeSrv->head]>39)
    ||(snakeSrv->y[snakeSrv->head]<0)
    ||(snakeSrv->y[snakeSrv->head]>39)){
        //Snake server hit a wall
        winLose(1);
        return true;
    }
    
    //Check for collision with other snake
    //Snake client's head into other snake
    int tempTail = snakeSrv->tail;
    while(tempTail!=(snakeSrv->head)+1){
        if(snakeCli->x[snakeCli->head]==snakeSrv->x[tempTail] //Checks x
        &&snakeCli->y[snakeCli->head]==snakeSrv->y[tempTail]){ //Checks y
        //If both are true, then snake collision is true
            winLose(0);
            return true;
        }
        ++tempTail;
    }
    
    //Snake server's head into other snake
    tempTail = snakeCli->tail;
    while(tempTail!=(snakeCli->head)+1){
        if(snakeSrv->x[snakeSrv->head]==snakeCli->x[tempTail] //Checks x
        &&snakeSrv->y[snakeSrv->head]==snakeCli->y[tempTail]){ //Checks y
        //If both are true, then snake collision is true
            winLose(1);
            return true;
        }
        ++tempTail;
    }
    
    //No collision
    return false;
}

//Bool time FeelsBadMan
//Returns true if time is up, otherwise return false
bool time(int* iTime){
    int tempTime = millis();
    if((tempTime-iTime)>90000){ //Constant timeout of 1 minute and 30 seconds
        //TODO: Point system
        return true;
    }
    return false;
}

//Main game function, runs the entire game
void snake(){//up = N down = S left = W right = E
  
  //Create snakes - 2d array
  //call with snakeCli[i][j]
  Snake* snakeCli = malloc(sizeof(Snake));
  Snake* snakeSrv = malloc(sizeof(Snake));
  assert(snakeCli != NULL);
  assert(snakeSrv != NULL);
  //Position snakes
  snakeSrv->x = {2,3,4,5,6};
  snakeSrv->y = {2,2,2,2,2};
  snakeCli->x = {37,36,35,34,33};
  snakeCli->y = {37,37,37,37,37};
  
  //Spawn random dot, give coordinates
  int dotX;
  int dotY;
  
  randomDot(&dotX, &dotY);
  //3 2 1 GO! Enter startup function
  
  int iTime = millis(); //Initial time
  
  //Start snakes
  while(!collision(snakeCli,snakeSrv)&&!time(&iTime)){
       //Put snake code in here - That means moving snakes
       
  }
  
}

//Function to start 
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
