/*****************************************************************
File:             BMN31K522.h
Author:           BESTMODULES
Description:      Define classes and required variables
Version:          V1.0.2   -- 2024-05-17
******************************************************************/
#ifndef _BMN31K522_H_
#define _BMN31K522_H_
#include <SoftwareSerial.h>
#include "arduino.h"
#define BAUDRATE        9600
#define CHECK_OK        0
#define CHECK_ERROR     1
#define TIMEOUT_ERROR   2

#define ATOMIZER_S            (0x3101)    //Microporous atomization module
#define ATOMIZER_M            (0x3103)    //Medium power atomization module
#define ATOMIZER_L            (0x3104)    //High power atomization module

#define DATA_HEADER           (0x55)
#define SET_RANK_CMD          (0x01)      //Set atomization rank command
#define SET_TIME_CMD          (0x02)      //Set atomization time command
#define GET_RANK_CMD          (0x11)      //Get atomization rank command
#define GET_TIME_CMD          (0x12)      //Get atomization time command
#define GET_WATER_STATUE_CMD  (0x13)      //Get water status command

#define COMMUNICATION_OK      (0x00)
#define COMMUNICATION_ERROR   (0xFF)   
class BMN31K522
{
  public:
    BMN31K522(HardwareSerial *theSerial = &Serial);   //constructed function：choose HW UART
    BMN31K522(uint8_t rxPin, uint8_t txPin);          //constructed function：choose SW UART
    void begin();                                     //initialize 
    uint8_t setAtomizationRank(uint8_t rank);         //Set atomization rank
    uint8_t setAtomizationTime(uint8_t times);        //Set atomization time
    uint8_t getAtomizationRank();   //Get atomization rank
    uint8_t getAtomizationTime();   //Get atomization time
    uint8_t getWaterStatus();     //Get water status

  private:
    HardwareSerial *_serial = NULL;                   //HW UART
    SoftwareSerial *_softSerial = NULL;               //SW UART

    uint8_t _uartCommunication(uint8_t dataSend[], uint8_t dataRec[], uint8_t dataLen);
    uint8_t readBytes(uint8_t rbuf[], uint8_t rlen, uint16_t timeout);
    void  writeBytes(uint8_t wbuf[], uint8_t wlen);
};


#endif
