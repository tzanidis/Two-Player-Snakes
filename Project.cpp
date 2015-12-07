//#includes up here - libraries
#include <Arduino.h>
#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library
#include <assert.h>

//standard U of A library settings, assuming Atmel Mega SPI pins
#define SD_CS    5  // Chip select line for SD card
#define TFT_CS   6  // Chip select line for TFT display
#define TFT_DC   7  // Data/command line for TFT
#define TFT_RST  8  // Reset line for TFT (or connect to +5V)

//standard joystick pins
const int VERT = 0;  // analog input
const int HORIZ = 1; // analog input
const int SEL = 9;   // digital input 
const int srvCliPin = 13; //srv/cli pin

//Initialize init_horiz and init_vert as global variables
int init_horiz;
int init_vert;

//Define the tft
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

typedef struct {//size is 50 by default
  int x[50]; //Holds snake x coordinates
  int y[50]; //Holds snake y coordinates
  int head;//4 //Holds snake's head position in array
  int tail;//0 //Holds snake's tail position in array
  int length;//5 //Length of snake, used just incase timeout is reached
  char delay; // 'Y' For eating the point dot, Y is yes, N is no
}Snake;

//Create snakes - global variables
//assert() is called later in main() to make sure arduino has enough space
Snake* snakeCli = (Snake*) malloc(sizeof(Snake));
Snake* snakeSrv = (Snake*) malloc(sizeof(Snake));

//sendChar(c)
//sends a character indicated by argument msg to other arduino
void sendChar(char msg){
//Character naming conventions for state machine (syncCli() and syncSrv())
  //Start = S
  //Sync = A
  //Up = U
  //Down = D
  //Left = L
  //Right = R
  //Up + len = V
  //Down + len = B  
  //Left + len = N
  //Right + len = M
  
  Serial3.write(msg);
  //debug
  //Serial.print("msg sent: "); Serial.println(msg);
}//Done

//waitOnSerial3()
//waits for nbytes bytes to show up in Serial3.available
//if timesout, returns false
//used to sync state machines
bool waitOnSerial3(uint8_t nbytes, long timeout){
  unsigned long deadline = millis() + timeout;
  while((Serial3.available() < nbytes) && (timeout < 0 || millis() < deadline)){
    delay(1);
  }
  return Serial3.available() >= nbytes;
}//Done

//listen()
//listens for an indicated character and returns true if found in 'listen buffer'
bool listen(char c){
  if(waitOnSerial3(1,1000)){
    char msg = Serial3.read();
    //debug
    Serial.print("msg heard: "); Serial.println(msg);
    if(msg == c){
      return true;
    }
  }
  return false;
}//Done

//listenDir()
//listens for a character, if null then returns 'Z'
  char listenDir(){
  char msg = 'Z';
  if(waitOnSerial3(1,1000)){
    msg = Serial3.read();
    //debug
    Serial.print("msg heard: "); Serial.println(msg);
  }
  return msg;
}//Done

//Finds random position to place dot
//Used for x coord and y coord
int randomDot(){
  int r = random(40); //0 - 39
  //debug
  Serial.print("random num generated: "); Serial.println(r);
  return r;
}//Done

//pointDot()
//Finds another random location to place dot, then creates a pixel there.
//Arguments: dot x coord., dot y coord.,
//Returns x coord and y coord
void pointDot(int* x, int* y){
  //get rand spot draw dot
  *x = randomDot();
  *y = randomDot();
  //draw dot
  tft.fillCircle((*x*3)+5, (*y*3)+21, 1, 0xFFFF);
  //debug
  Serial.print("point generated at x: "); Serial.print(*x);Serial.print(" and y: ");Serial.println(*y);
 }

//readInput()
//reads joystick input and returns corresponding character of input
//coordinated with snake(), joystick can't return a character indicating that the snake is turning more than 90 degrees.
char readInput(char oldChar){
    int horizontal = analogRead(HORIZ); //0-1024, left to right
    int vertical = analogRead(VERT);//0-1024, up to down
    int delta_horizontal = horizontal - 512; //512 is init_horizontal
    int delta_vertical = vertical - 512; //512 is init_vertical
    Serial.print("delta H: "); Serial.print(delta_horizontal);
    Serial.print(" delta V: "); Serial.println(delta_vertical);
    
    //Serial.println explains features, can be removed since it's debugs but kept for convenience
    
    //case 1: horizontal is larger than vertical or equal
    if(abs(delta_horizontal) >= abs(delta_vertical)){//Go horizontal
        //Left
        if(delta_horizontal<-300){
            if(oldChar == 'R'){//|| oldChar == 'M'
				Serial.print("using oldchar R, can't go backwards");
              return 'R';
            }
            Serial.print("using new char, L");
            return 'L';
        }
        //Right
        else if(delta_horizontal > 300){
            if(oldChar == 'L'){//|| oldChar == 'N'
				Serial.print("using oldchar L, can't go backwards");
              return 'L';
            }
            Serial.print("using new char, R");
            return 'R';
        }else{
        	//case 3: no input is entered
			Serial.print("using old char, no input entered/deadzone");
			return oldChar;
		}
    }else
    //case 2: vertical is larger than horizontal
    {//(delta_vertical > delta_horizontal) Go vertical
        //Up
        if(delta_vertical < -300){
            if(oldChar == 'D'){//||oldChar == 'B'
				Serial.print("using oldchar D, can't go backwards");
              return 'D';
            }
            Serial.print("using new char, U");
            return 'U';
        }
        //Down
        else if(delta_vertical > 300){
          if(oldChar == 'U'){//||oldChar == 'V'
			  Serial.print("using oldchar U, can't go backwards");
              return 'U';
            }
            Serial.print("using newchar D");
            return 'D';
        }else{
        		//case 3: no input is entered
			Serial.print("using oldchar, no input/deadzone");
			return oldChar;
		}
    }
}

//Winlose is called inside collision(), or after time() returns true 
//val decides winner: 0 is server, 1 is client, 2 is tie
//draws a victory screen depending on outcome
void winLose(int val){
  tft.fillScreen(0x0000);
  if(val == 1){//srv win
    Serial.println("Host / Srv wins");
    tft.setCursor(52,76);
    tft.print("Host");
    tft.setCursor(49,84);
    tft.print("Wins!");
  }else if(val == 0){//cli win
    Serial.println("Client wins");
    tft.setCursor(46,76);
    tft.print("Client");
    tft.setCursor(49,84);
    tft.print("Wins!");
  }else{//tie
    Serial.print("Tie");
    tft.setCursor(46,76);
    tft.print("It's a");
    tft.setCursor(42,84);
    tft.print("Tie!");
      
  }
  //Tell to press reset key
  tft.setCursor(0,0);
  tft.print("Press Reset");
  Serial.println("Draw Screen, tell to reset.");
}//Done

//collision()
//checks every movement if there has been a collision between snakes or on the wall
//takes addresses of snakeCli and snakeSrv as arguments
bool collision(Snake* snakeCli, Snake* snakeSrv){
  //Check for walls
  if((snakeCli->x[snakeCli->head]<0)
  ||(snakeCli->x[snakeCli->head]>39)
  ||(snakeCli->y[snakeCli->head]<0)
  ||(snakeCli->y[snakeCli->head]>39)){
    //Snake client hit a wall
    Serial.print("Srv Win / Snake client has hit a wall at x: "); Serial.print(snakeCli->x[snakeCli->head]); Serial.print(", y: "); Serial.println(snakeCli->y[snakeCli->head]);
    winLose(1);
    return true;
  }
  if((snakeSrv->x[snakeSrv->head]<0)
  ||(snakeSrv->x[snakeSrv->head]>39)
  ||(snakeSrv->y[snakeSrv->head]<0)
  ||(snakeSrv->y[snakeSrv->head]>39)){
    //Snake server hit a wall
    Serial.print("Cli Win / Snake server has hit a wall at x: "); Serial.print(snakeSrv->x[snakeSrv->head]); Serial.print(", y: "); Serial.println(snakeSrv->y[snakeSrv->head]);
    winLose(0);
    return true;
  }
   
   //Check for collision on itself
   //Client snake
   int tempTail = snakeCli->tail;
   int tempHead = snakeCli->head;
   while(tempTail!=tempHead){//compares head coordinates with other body coordinates
    if(snakeCli->x[tempHead]==snakeCli->x[tempTail] //Checks x
    &&snakeCli->y[tempHead]==snakeCli->y[tempTail]){ //Checks y
      //If both are true, then snake collision is true
      Serial.print("Client snake suicide "); Serial.print(snakeCli->x[snakeCli->head]); Serial.print(", y: "); Serial.println(snakeCli->y[snakeCli->head]);
      winLose(1);
      return true;
    }
    tempTail = (tempTail + 1)%50;
  }
   //Server snake
   tempTail = snakeSrv->tail;
   tempHead = snakeSrv->head;
   while(tempTail!=tempHead){//compares head coordinates with other body coordinates
    if(snakeSrv->x[tempHead]==snakeSrv->x[tempTail] //Checks x
    &&snakeSrv->y[tempHead]==snakeSrv->y[tempTail]){ //Checks y
      //If both are true, then snake collision is true
      Serial.print("Server snake suicide "); Serial.print(snakeSrv->x[snakeSrv->head]); Serial.print(", y: "); Serial.println(snakeSrv->y[snakeSrv->head]);
      winLose(0);
      return true;
    }
    tempTail = (tempTail + 1)%50;
  }
   
  //Check for collision with other snake
  //Snake heads collide
  if((snakeCli->x[snakeCli->head]==snakeSrv->x[snakeSrv->head])&&(snakeCli->y[snakeCli->head]==snakeSrv->y[snakeSrv->head])){
    Serial.println("Tie / Snakes heads collided");
    winLose(2);
    return true;
  }
  
  //Snake client's head into other snake
  tempTail = snakeSrv->tail;
  tempHead = (snakeSrv->head+1)%50;
  while(tempTail!=tempHead){//compares snakeCli head coordinates with snakeSrv body coordinates, doesn't end until body part from tail to head is checked
    if(snakeCli->x[snakeCli->head]==snakeSrv->x[tempTail] //Checks x
    &&snakeCli->y[snakeCli->head]==snakeSrv->y[tempTail]){ //Checks y
      //If both are true, then snake collision is true
      Serial.print("Srv Win / Snake client has hit other snake at x: "); Serial.print(snakeCli->x[snakeCli->head]); Serial.print(", y: "); Serial.println(snakeCli->y[snakeCli->head]);
      winLose(1);
      return true;
    }
    tempTail = (tempTail + 1)%50;
  }
    
  //Snake server's head into other snake
  tempTail = snakeCli->tail;
  tempHead = (snakeCli->head+1)%50;
  while(tempTail!=tempHead){//compares snakeSrv head coordinates with snakeCli body coordinates, doesn't end until body part from tail to head is checked
    if((snakeSrv->x[snakeSrv->head]==snakeCli->x[tempTail]) //Checks x
    &&(snakeSrv->y[snakeSrv->head]==snakeCli->y[tempTail])){ //Checks y
      //If both are true, then snake collision is true
      Serial.print("Cli Win / Snake server has hit other snake at x: "); Serial.print(snakeSrv->x[snakeSrv->head]); Serial.print(", y: "); Serial.println(snakeSrv->y[snakeSrv->head]);
      winLose(0);
      return true;
    }
    tempTail = (tempTail + 1)%50;
  }
  
  //If no collision check has returned true yet, then there's no collision
  return false;
}

//Bool time
//Returns true if timeout of 1 minute 30 seconds is reached, otherwise return false
bool time(int iTime){
  //int tempTime = millis();
  if((millis()-iTime)>90000){ //Constant timeout of 1 minute and 30 seconds
    //Point system located after snake() while loop
    return true;
  }
  return false;
}

// syncSrv()
// sends and receives characters as the server arduino
// argument: character, returns character
char syncSrv(char mov){
  typedef enum {SEND, WFR, STL, LIS, STD, ERR }State; //send, wait for received, send that listening, listen, wait for done. 
  State state = SEND;
  char otherPlayerMov;
  char comp;
  
  while((state != STD) && (state !=ERR)){
    if(state == SEND){
      sendChar(mov);
      //~ Serial.println("state = WFR");
      state = WFR;
    }else if(state == WFR){
      comp = listenDir();
      if(comp=='A' || comp=='S'){
        //~ Serial.println("state = LIS");
        state = LIS;
      }else{
        //~ Serial.println("state = SEND");
        state = SEND;
      }
    }else if(state == LIS){
      otherPlayerMov = listenDir();
        if(otherPlayerMov == 'U' || otherPlayerMov == 'D' ||otherPlayerMov == 'L' ||otherPlayerMov == 'R' || otherPlayerMov == 'S'){
          //~ Serial.println("state = STD");
          sendChar('A');
          state = STD;
        }else if(otherPlayerMov == 'V' || otherPlayerMov == 'B' ||otherPlayerMov == 'N' ||otherPlayerMov == 'M'){
          //~ Serial.println("state = STD + len");
          sendChar('A');
          snakeCli->length +=1;
          snakeCli->delay = 'Y';
          state = STD;
        }
    }else{
      //~ Serial.println("state = ERR");
      state = ERR;
    }
  }
  Serial.print("Sync done");
  Serial.print("other players movement: ");Serial.print(otherPlayerMov);
  return otherPlayerMov;
}

// syncCli()
// sends and receives characters as the client arduino
// argument: character, returns character
char syncCli(char mov){
  typedef enum {LIS, WFR, SEND, D, ERR }State; //listen for their move / send that received, send our move, wait for received, tell done 
  State state = LIS;
  char otherPlayerMov;
  char comp;
  while((state != D) && (state !=ERR)){
    if(state == LIS){
      otherPlayerMov = listenDir();
      //~ Serial.println("im here?");
      if(otherPlayerMov == 'U' || otherPlayerMov == 'D' ||otherPlayerMov == 'L' ||otherPlayerMov == 'R' || otherPlayerMov == 'S'){
        //~ Serial.println("state = SEND");
        sendChar('A');
        state = SEND;
      }else if(otherPlayerMov == 'V' || otherPlayerMov == 'B' ||otherPlayerMov == 'N' ||otherPlayerMov == 'M'){
        //~ Serial.println("state = STD + len");
        sendChar('A');
        snakeSrv->length +=1;
        snakeSrv->delay = 'Y';
        state = SEND;
      }
    }else if(state == SEND){
      sendChar(mov);
      //~ Serial.println("state = WFR");
      state = WFR;
    }else if(state == WFR){
      comp = listenDir();
      if(comp=='A' || comp=='S'){
        //~ Serial.println("state = D");
        state = D;
      }else{
        //~ Serial.println("state = SEND");
        state = SEND;
      }
    }else{
      //~ Serial.println("state = ERR");
      state = ERR;
    }
  }
  Serial.print("Sync done");
  Serial.print("other players movement: ");Serial.print(otherPlayerMov);
  return otherPlayerMov;
}

//snake()
//Main game function, runs the entire game
//calls sync functions, collision function, time function every while loop
//makes sure snake movement is synced and correct
void snake(int* dotX, int* dotY){
  
  //Initialize snakes
  snakeSrv->head = 4;
  snakeSrv->tail = 0;
  snakeSrv->length = 5;
  snakeSrv->delay = 'N';
  
  snakeCli->head = 4;
  snakeCli->tail = 0;
  snakeCli->length = 5;
  snakeCli->delay = 'N';
  
  //Position snakes
  snakeSrv->x[0] = 2;
  snakeSrv->x[1] = 3;
  snakeSrv->x[2] = 4;
  snakeSrv->x[3] = 5;
  snakeSrv->x[4] = 6;
  
  snakeSrv->y[0] = 2;
  snakeSrv->y[1] = 2;
  snakeSrv->y[2] = 2;
  snakeSrv->y[3] = 2;
  snakeSrv->y[4] = 2;
  
  snakeCli->x[0] = 37;
  snakeCli->x[1] = 36;
  snakeCli->x[2] = 35;
  snakeCli->x[3] = 34;
  snakeCli->x[4] = 33;
  
  snakeCli->y[0] = 37;
  snakeCli->y[1] = 37;
  snakeCli->y[2] = 37;
  snakeCli->y[3] = 37;
  snakeCli->y[4] = 37;
  
  int iTime = millis(); //Initial time
  
  //Inititialize general directions to be used in while loop
  char oldDir, dirSrv, dirCli;
  bool srv; //variable for reading server or client
  if(digitalRead(srvCliPin) == HIGH){ // read pin / determine srv or cli
    Serial.println("pin HIGH Srv");
    srv = true;
    oldDir = 'R';
  }else{
    Serial.println("pin low Cli");
    srv = false;
    oldDir = 'L';
  }
  dirCli = 'L';
  dirSrv = 'R';
  
  //Start snakes
  //Check if snakes have collided with anything after every movement, and check for timeout
  while(!collision(snakeCli,snakeSrv)&&!time(iTime)){
    //Check for if one of the snakes has touched its own dot
    bool dotTouch = false;
    
    if(srv){
      //Server can only eat own dot
      if((snakeSrv->x[snakeSrv->head]==*dotX)&&(snakeSrv->y[snakeSrv->head]==*dotY)){
        snakeSrv->delay = 'Y';
        snakeSrv->length += 1;
        dotTouch = true;
      }
    }else{
      //Client can only eat own dot
      if((snakeCli->x[snakeCli->head]==*dotX)&&(snakeCli->y[snakeCli->head]==*dotY)){
        snakeCli->delay = 'Y';
        snakeCli->length += 1;
        dotTouch = true;
      }
    }
    
    if(dotTouch){//Move dot to new location if dot has been touched
      pointDot(dotX,dotY);
    }else{//Redraw dot incase other snake has 'hidden' it
        tft.fillCircle(((*dotX)*3)+5, ((*dotY)*3)+21, 1, 0xFFFF);
    }
  
    //Now that winLose conditions are complete, delay time by potentiometer
    delay(25);
  
    //Read input, depends on whether is srv or cli
    if(srv){
      //Server
      //Read joystick input and update old direction
      dirSrv = readInput(oldDir);
      oldDir = dirSrv;
      //If there is delay, change character to 'delay' character so other arduino acknowledges the delay
      if(snakeSrv->delay == 'Y'){
		  if(dirSrv == 'U'){//Up
			  dirSrv = 'V';
		  }else if(dirSrv == 'D'){//Down
			  dirSrv = 'B';
		  }else if(dirSrv == 'L'){//Left
			  dirSrv = 'N';
		  }else if(dirSrv =='R'){//Right
			  dirSrv = 'M';
		  }
	  }
		dirCli = syncSrv(dirSrv); // call appropriate functions
    }else{
      //Client
      //Read joystick input and update old direction
      dirCli = readInput(oldDir);
      oldDir = dirCli;
      //If there is delay, change character to 'delay' character so other arduino acknowledges the delay
      if(snakeCli->delay == 'Y'){
		  if(dirCli == 'U'){//Up
			  dirCli = 'V';
		  }else if(dirCli == 'D'){//Down
			  dirCli = 'B';
		  }else if(dirCli == 'L'){//Left
			  dirCli = 'N';
		  }else if(dirCli == 'R'){//Right
			  dirCli = 'M';
		  }
	  }
      dirSrv = syncCli(dirCli); // call appropriate functions
    }
      
    //If haven't eaten dot, delete tail and undraw
    //Client's Tail
    if(snakeCli->delay == 'N'){
      //Delete tail on display
      tft.fillRect((snakeCli->x[snakeCli->tail]*3)+4,(snakeCli->y[snakeCli->tail]*3)+20,3,3,0x0000); //black
      //Move tail by one
      snakeCli->tail = (snakeCli->tail + 1)%50;
    }else{
    	//If there is delay, leave tail there but change delay variable back to 'N';
      snakeCli->delay = 'N';
    }
    
    //Server's Tail
    if(snakeSrv->delay == 'N'){
      //Delete tail on display
      tft.fillRect((snakeSrv->x[snakeSrv->tail]*3)+4,(snakeSrv->y[snakeSrv->tail]*3)+20,3,3,0x0000); //black
      //Move tail by one
      snakeSrv->tail = (snakeSrv->tail + 1)%50;
    }else{
    	//If there is delay, leave tail there but change delay variable back to 'N';
      snakeSrv->delay = 'N';
    }
    
    //Move snake head
    //Client
    if((dirCli == 'U')||(dirCli == 'V')){//Up
      //Transfer coordinates to new head
      snakeCli->y[(snakeCli->head+1)%50] = snakeCli->y[snakeCli->head] - 1;
      snakeCli->x[(snakeCli->head+1)%50] = snakeCli->x[snakeCli->head];
    }
    if((dirCli == 'R')||(dirCli == 'M')){//Right
      //Transfer coordinates to new head
      snakeCli->x[(snakeCli->head+1)%50] = snakeCli->x[snakeCli->head] + 1;
      snakeCli->y[(snakeCli->head+1)%50] = snakeCli->y[snakeCli->head];
    }
    if((dirCli == 'D')||(dirCli == 'B')){//Down
    //Transfer coordinates to new head
    snakeCli->y[(snakeCli->head+1)%50] = snakeCli->y[snakeCli->head] + 1;
    snakeCli->x[(snakeCli->head+1)%50] = snakeCli->x[snakeCli->head];
    }
    if((dirCli == 'L')||(dirCli == 'N')){//Left
      //Transfer coordinates to new head
      snakeCli->x[(snakeCli->head+1)%50] = snakeCli->x[snakeCli->head] - 1;
      snakeCli->y[(snakeCli->head+1)%50] = snakeCli->y[snakeCli->head];
    }
    snakeCli->head = (snakeCli->head + 1)%50;
    //Draw new client head
    tft.fillRect((snakeCli->x[snakeCli->head]*3)+4,(snakeCli->y[snakeCli->head]*3)+20,3,3,0xFFFF); //0 = black
    
    //Server
    if((dirSrv == 'U')||(dirSrv == 'V')){//Up
      //Transfer coordinates to new head
      snakeSrv->y[(snakeSrv->head+1)%50] = snakeSrv->y[snakeSrv->head] - 1;
      snakeSrv->x[(snakeSrv->head+1)%50] = snakeSrv->x[snakeSrv->head];
    }
    if((dirSrv == 'R')||(dirSrv == 'M')){//Right
      //Transfer coordinates to new head
      snakeSrv->x[(snakeSrv->head+1)%50] = snakeSrv->x[snakeSrv->head] + 1;
      snakeSrv->y[(snakeSrv->head+1)%50] = snakeSrv->y[snakeSrv->head];
    }
    if((dirSrv == 'D')||(dirSrv == 'B')){//Down
      //Transfer coordinates to new head
      snakeSrv->y[(snakeSrv->head+1)%50] = snakeSrv->y[snakeSrv->head] + 1;
      snakeSrv->x[(snakeSrv->head+1)%50] = snakeSrv->x[snakeSrv->head];
    }
    if((dirSrv == 'L')||(dirSrv == 'N')){//Left
      //Transfer coordinates to new head
      snakeSrv->x[(snakeSrv->head+1)%50] = snakeSrv->x[snakeSrv->head] - 1;
      snakeSrv->y[(snakeSrv->head+1)%50] = snakeSrv->y[snakeSrv->head];
    }
    snakeSrv->head = (snakeSrv->head + 1)%50;
    //Draw new server head
    tft.fillRect((snakeSrv->x[snakeSrv->head]*3)+4,(snakeSrv->y[snakeSrv->head]*3)+20,3,3,0xFFFF); //0 = black
  }
  
  if(time(iTime)){
    //Timeout indicated
    if(snakeCli->length > snakeSrv->length){
      //Client's snake is longer, therefore client wins
      Serial.println("Cli Win / Client snake is longer");
      winLose(0);
    }else if(snakeSrv->length > snakeCli->length){
      //Server's snake is longer, therefore server wins
      Serial.println("Srv Win / Server snake is longer");
      winLose(1);
    }else{
      //Snake lengths are equal. Tie.
      Serial.println("Tie / Snakes are of equal length");
      winLose(2);
    }
  }
}

//Startup()
//After client and server are set up, initialize screen, board, and snake.
//Run snake() as last statement
void startUp(){
	//Clear Buffer
	while(Serial3.available()>0){
		char dump = Serial3.read();
	}
	tft.fillScreen(0x0000);
  tft.setTextColor(0xFFFF, 0x0000);
  
  //Draw boundaries
  //debug
  Serial.println("Drawing boundaries");
  tft.fillRect(0, 16, 128, 4, 0xFFFF);
  tft.fillRect(0, 140, 128, 4, 0xFFFF);
  tft.fillRect(0, 16, 4, 124, 0xFFFF);
  tft.fillRect(124, 16, 4, 124, 0xFFFF);
  
  //Draw player ID
  //debug
  Serial.println("Drawing player ID");
  tft.setCursor(10,29);
  tft.print("HOST SNAKE");
  tft.setCursor(46,128);
  tft.print("CLIENT SNAKE");
  
  //3,2,1,GO
  //debug
  Serial.println("Draw Countdown");
  tft.setCursor(62,80);
  tft.print("3");
  delay(1000);
  tft.setCursor(62,80);
  tft.print("2");
  delay(1000);
  tft.setCursor(62,80);
  tft.print("1");
  delay(1000);  
  
  //Clear player ID
  tft.fillRect(10, 29, 60, 8, 0x0000);
  tft.fillRect(46, 128, 72, 8, 0x0000);
  
  tft.setCursor(58,80);
  tft.print("GO");
  delay(1000);
  tft.fillRect(58, 80, 12, 8, 0x0000);
  
  //debug
  Serial.println("Make start dot");
  int x,y;
  pointDot(&x,&y);
  
  //Start game
  //debug
  Serial.println("Call snake / Start game");
  snake(&x,&y);
}//Done

//Menu for server
//Draws menu text for server arduino
//Waits for server to activate select of joystick
void menuSrv(){
  //debug
  Serial.println("Draw menu for srv");
  tft.fillScreen(0x0000);
  tft.setTextColor(0xFFFF, 0x0000);
  tft.setCursor(0,0);
  tft.print("host");
  tft.setCursor(54,76);
  tft.print("START");
  while(true){ //when not pressed
    if(digitalRead(SEL) == 0){
    	//Button is pressed, call startUp()
      //debug
      Serial.println("Start pressed");
      char dump = syncSrv('S');
      //sendChar('S');
      //debug
      Serial.println("Call startup");
      startUp();
      break;
    }
  }
}//Done

//Menu for client
//Draws menu text for client arduino
void menuCli(){
  tft.fillScreen(0x0000);
  tft.setTextColor(0xFFFF, 0x0000);
  tft.setCursor(0,0);
  tft.print("client");
  tft.setCursor(50,72);
  tft.print("Waiting");
  tft.setCursor(50,80);
  tft.print("For Host");
  char dump = syncCli('S');
  Serial.println("Start heard");
  Serial.println("Call startup");
  startUp();
}//Done

//Function to start 
int main(){
  
  //Initialize arduino functions
  init();
  
  //Initialize screen
  tft.initR(INITR_BLACKTAB);
  tft.setRotation(2);
  
  //Initialize arduino
  Serial.begin(9600);
  Serial3.begin(9600);
  
  randomSeed(analogRead(3));
  
  //srv/cli part, needs to be in the main
  //setup pin
  pinMode(srvCliPin, INPUT);
  digitalWrite(srvCliPin, LOW);
  pinMode(SEL, INPUT);
  digitalWrite(SEL, HIGH);
  
  //Makes sure there is enough memory for snakeCli and snakeSrv
  assert(snakeCli != NULL);
  assert(snakeSrv != NULL);
  
  //Initialize init_horiz and init_vert values
  init_horiz = analogRead(HORIZ);
  init_vert = analogRead(VERT);
  
  if(digitalRead(srvCliPin) == HIGH){ // read pin / determine srv or cli
    Serial.println("pin HIGH Srv");
    menuSrv(); // call appropriate functions
  }else{
    Serial.println("pin low cli");
    menuCli(); // call a*ppropriate functions
  }
  
  Serial3.end();
  Serial.end();
}
