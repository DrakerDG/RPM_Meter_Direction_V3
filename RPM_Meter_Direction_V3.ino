/*
RPM Meter Direction
Use Square Encoder with Hall effect sensors
By DrakerDG (c)

https://www.youtube.com/user/DrakerDG
*/

#include <SPI.h>
#include <TFT_ST7735.h>
#include <SimplyAtomic.h>
#include <TimerOne.h>

#define	 GREEN 0x07E0
#define YELLOW 0x07FF

#define DC A0 //9
#define RS A1 //10
#define CS A2 //11

// Sensor pins
const byte PinX[4] = {2, 3, 10, 11};
// Limit of microseconds
const long uSeg = 100000;
// Left position RPM labels
const byte x1 = 75;
// Pulse timer counters
volatile unsigned long pwc[2];
// PWM periods
volatile unsigned long pwm[2];
// RPM values
unsigned long rpm[2];
// States of the second sensors
volatile bool Sen[2];
// Count variable printing millis
unsigned long prT = 0;

TFT_ST7735 tft = TFT_ST7735(CS, DC, RS);

void CountSA(void);
void CountSB(void);
void RPMc(void);
void Draw_Table(void);
void Print_Data(void);

void setup(){
  Serial.begin(9600);
  tft.begin();
  tft.setRotation(1);
  tft.clearScreen();
  tft.setTextWrap(true);
  tft.setTextColor(YELLOW, BLACK);
  tft.setCursor(0, 0);
  Draw_Table();
  
  for (byte i=0; i<4; i++){
    pinMode(PinX[i], INPUT);
    }
  for (byte i=0; i<2; i++){
    pwc[i] = uSeg;
    pwm[i] = uSeg;
    rpm[i] = 0;
  }
  // Interrupt to count period time
  Timer1.initialize(100);
  Timer1.attachInterrupt(RPMc);
  
  // Interrupt of Sensor 1 of Motor A
  attachInterrupt(digitalPinToInterrupt(PinX[0]), CountSA, FALLING);
  
  // Interrupt of Sensor 1 of Motor B
  attachInterrupt(digitalPinToInterrupt(PinX[1]), CountSB, FALLING);
}

void loop(){
  Print_Data();
}

void CountSA(){
  // 2nd sensor value
  Sen[0] = 1 & (PINB >> 7); 
  pwm[0] = pwc[0]; // Save the period
  pwc[0] = 0; // Reset the timer
}

void CountSB(){
  // 2nd sensor value
  Sen[1] = 1 & (PINB >> 6);
  pwm[1] = pwc[1]; // Save the period
  pwc[1] = 0; // Reset the timer
}

void RPMc(){
  for (byte i=0; i<2; i++){
    // Increase the time counter
    pwc[i]++;
    if (pwc[i] > (uSeg)){
      // Limit the timer & period
      pwc[i] = uSeg;
      pwm[i] = uSeg;
    }
  }  
}

void Draw_Table(){
  // Code to draw the table on screen
  tft.drawFastVLine(22, 0, 128, WHITE);
  for ( int i=0; i<11; i+=1 ){
    tft.drawFastHLine( 20, 5+i*12, 4, WHITE);
    if (!(i&1)){
      tft.setCursor( 0, i*12 + 2);
      tft.print((10.0-i)*0.5, 1);
      }
    }
  tft.drawFastHLine( 20, 125, 128, WHITE);
  tft.setTextSize(1);
  tft.setCursor(x1, 10);
  tft.print("Motor A");
  tft.setCursor(x1+30, 45);
  tft.print("RPM");
  tft.setCursor(x1, 70);
  tft.print("Motor B");
  tft.setCursor(x1+30, 105);
  tft.print("RPM");
  tft.setTextSize(2);
}

void Print_Data(){
  unsigned long nwT = millis();
  // Calculations and prints every 10ms
  if ((nwT - prT) > 10){
    prT = nwT;
    char sRPM[10];
    for (byte i=0; i<2; i++){
      //RPM
      tft.setCursor(x1, 25+60*i);
      // Protects math calculation
      ATOMIC()
      {
        // Detect Rotation Decrease
        if (pwc[i]>(pwm[i]*2)){
          pwm[i] *= 2;
          pwm[i] = constrain(pwm[i], 0, uSeg);
          pwc[i] = pwc[i]*2;
        }
        /* detects or not the
        rotation of the motors */
        if (pwm[i] < uSeg) rpm[i] = 6*uSeg/pwm[i]; // Detects rotation
        else if ((rpm[i] > 0)&&(pwm[i] == uSeg)) rpm[i] = int(rpm[i]/2); // No rotatiom
        
        // Limits the value of RPMs
        rpm[i] = constrain(rpm[i], 0, 9999);
      }

      dtostrf(rpm[i], 4, 0, sRPM);
      // Print the RPMs
      tft.print(sRPM);
      int valX = rpm[i]*120/500;
      // Prints the RPM bars
      tft.fillRect(30+15*i, 0, 10, 125 - valX, BLACK);
      tft.fillRect(30+15*i, 125 - valX, 10, valX, GREEN);
      
      tft.setCursor(x1-15, 25+60*i);
      /* Determines the direction
      of rotation of the motors.
      Prints + if it turns CW and
      - if it turns CCW */
      if ((rpm[i] == 0)&&(pwm[i] == uSeg)) tft.print(" ");
      else if (Sen[i]) tft.print("-");
      else tft.print("+");
      }
    }
}