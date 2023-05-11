/*************************************************
File:             controlAtomization.ino
Description:      Press the switch to control the timing work of the atomizer;
                  An LED shows the working state of the atomizer: on - atomizer working; off - atomizer stop;
                  You can choose whether to use Serial to print atomizer water status and atomization work remaining time.
Note:               
**************************************************/
#include <BMN31K522.h>

#define SERIAL_DEBUG    (1)             //0 - disable Serial debug output; 1 - enable Serial debug output;
#define ATOMIZER_RANK   (255)           //0~255
#define ATOMIZER_TIME   (1)             //0~255(min)

/*Create Atomizer object*/
//BMN31K522 myAtomizer(&Serial1);         //Please comment out this line of code if you don't use HW Serial1 on BMduino
//BMN31K522 myAtomizer(&Serial2);         //Please uncomment out this line of code if you use HW Serial2 on BMduino
//BMN31K522 myAtomizer(&Serial3);         //Please uncomment out this line of code if you use HW Serial3 on BMduino
//BMN31K522 myAtomizer(&Serial4);         //Please uncomment out this line of code if you use HW Serial4 on BMduino
BMN31K522 myAtomizer(2,3);              //rxPin,txPin,Please uncomment out this line of code if you use SW Serial

byte atomizerLED = 5;                   //Atomizer work LED
byte atomizerSwitch = 4;                //Atomizer Switch PIN

unsigned long TimeCount_1s = 0;         //time count of 1s
byte waterStatus = 0;                   //Water status: 0 - lack; 1 - rich
unsigned long switchOnTime = 0;         //switch debounce time count
bool atomizerOnFlag = false;            //Atomizer working flag 

void setup() {
  if (SERIAL_DEBUG){
    Serial.begin(9600);                 //Serial debug output
  }
  
  myAtomizer.begin();

  pinMode(atomizerLED,OUTPUT);
  pinMode(atomizerSwitch,INPUT); 
  digitalWrite(atomizerLED,HIGH);  

  TimeCount_1s = millis();
}

void loop() {
  byte switchStatus;
  uint8_t getTime;
  
  /******** Get water status & remaining time every 1s ********/
  if (millis() - TimeCount_1s >= 1000)
  {
    TimeCount_1s = millis();
    /******** Get water status ********/
    waterStatus=myAtomizer.getWaterStatus();    

    if (SERIAL_DEBUG)
    {
      if (waterStatus)
      {
        Serial.println("Atomizer's water status is: Have water");
      }
      else
      {
        Serial.println("Atomizer's water status is: Lack of water");
      }
    }             

    /******** Get remaining time ********/
    if (atomizerOnFlag)
    {
      getTime=myAtomizer.getAtomizationTime(); 
      if (getTime == 0)
      {
        atomizerOnFlag = false;
        digitalWrite(atomizerLED,HIGH);
      }
      if (SERIAL_DEBUG)
      {
        Serial.print("Atomizer work remaining time is: ");
        Serial.println(getTime);
      }
    }    
  }  

  /******** Key scan********/
  if (waterStatus == 1)
  {
    switchStatus = digitalRead(atomizerSwitch);
    if (!switchStatus)
    {
      if ((millis() - switchOnTime) > 150)                  //switch debounce
      {
        switchOnTime = millis();
        if (!atomizerOnFlag)
        {
          atomizerOnFlag = true;
          digitalWrite(atomizerLED,LOW);
          myAtomizer.setAtomizationRank(ATOMIZER_RANK);     //set atomizer rank
          myAtomizer.setAtomizationTime(ATOMIZER_TIME);     //set atomizer timing
        }
        else
        {
          atomizerOnFlag = false;
          digitalWrite(atomizerLED,HIGH);
          myAtomizer.setAtomizationRank(0);                 //close atomizer work
          myAtomizer.setAtomizationTime(0);                 
        }
      }   
    }
    else
    {
      switchOnTime = millis();
    }
  }
  /******** Processing for lack of water ********/
  else
  {
    if (atomizerOnFlag)
    {
      atomizerOnFlag = false;
      digitalWrite(atomizerLED,HIGH);
    }
  }
}
