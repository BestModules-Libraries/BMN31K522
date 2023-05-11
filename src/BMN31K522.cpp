/*************************************************
File:               BMN31K522.cpp
Author:             BESTMODULES
Description:        UART communication with the BMN31K522 and obtain the corresponding value 
History：            
  V1.00.1   -- initial version；2023-01-09；Arduino IDE :  ≥v1.8.13
**************************************************/
#include "BMN31K522.h"   
uint8_t dataTxBuf[32] = {DATA_HEADER, 0};  //Array of data frames sent
uint8_t dataRxBuf[32] = {0};               //Array of data frames receive 

/*************************************************
Description:        Constructed function
Parameters:         *theSerial：HardwareSerial
                    Arduino Uno: &Serial
                    BMduino: &Serial、&Serial1、&Serial2、&Serial3、&Serial4
Return:             None
Others:             
*************************************************/
BMN31K522::BMN31K522(HardwareSerial *theSerial)
{
  _softSerial = NULL;
  _serial = theSerial;
}

/*************************************************
Description:        Constructed function
Parameters:         rxPin：RX PIN of SoftwareSerial
                    txPin：TX PIN of SoftwareSerial
Return:             None
Others:             
*************************************************/
BMN31K522::BMN31K522(uint8_t rxPin, uint8_t txPin)
{
  _serial = NULL;
  _softSerial = new SoftwareSerial(rxPin, txPin); 
}

/*************************************************
Description:        Initialize
Parameters:         None
Return:             None
Others:             
*************************************************/
void BMN31K522::begin()
{
  if (_softSerial != NULL)
  {
    _softSerial->begin(BAUDRATE);
  }
  else
  {
    _serial->begin(BAUDRATE);
  }

  dataTxBuf[1] = ATOMIZER_M / 256;
  dataTxBuf[2] = ATOMIZER_M & 0xff;
}

/*************************************************
Description:        Set atomization rank
Parameters:         rank：atomization rank 0~255
Return:             Communication status: COMMUNICATION_OK or COMMUNICATION_ERROR 
Others:             
*************************************************/
uint8_t BMN31K522::setAtomizationRank(uint8_t rank)
{
  uint8_t res;

  dataTxBuf[3] = SET_RANK_CMD;
  dataTxBuf[4] = 0x01;
  dataTxBuf[5] = rank;
  dataTxBuf[6] = 0;
  for (int i = 0; i < 6; i++)
  {
    dataTxBuf[6] += dataTxBuf[i];
  }

  res = _uartCommunication(dataTxBuf, dataRxBuf, 7);

  return res;
}

/*************************************************
Description:        Set atomization time
Parameters:         times：atomization time 0~255(unit: min)
Return:             Communication status: COMMUNICATION_OK or COMMUNICATION_ERROR
Others:             
*************************************************/
uint8_t BMN31K522::setAtomizationTime(uint8_t times)
{
  uint8_t res;
  
  dataTxBuf[3] = SET_TIME_CMD;
  dataTxBuf[4] = 0x01;
  dataTxBuf[5] = times;
  dataTxBuf[6] = 0;
  for (int i = 0; i < 6; i++)
  {
    dataTxBuf[6] += dataTxBuf[i];
  }

  res = _uartCommunication(dataTxBuf, dataRxBuf, 7);

  return res;
}


/*************************************************
Description:        Get atomization rank
Parameters:         rankValue: Variables for storing atomization rank 
Return:             rankValue: Variables for storing atomization rank
                    0:ERROR
Others:             
*************************************************/
uint8_t BMN31K522::getAtomizationRank()
{
  uint8_t res;
  uint8_t rankValue=0;
  dataTxBuf[3] = GET_RANK_CMD;
  dataTxBuf[4] = 0x01;
  dataTxBuf[5] = 0x00;
  dataTxBuf[6] = 0;
  for (int i = 0; i < 6; i++)
  {
    dataTxBuf[6] += dataTxBuf[i];
  }

  res = _uartCommunication(dataTxBuf, dataRxBuf, 7);
  if(res == 0)
  {
    rankValue = dataRxBuf[5];        
  }
         
  return rankValue;
}


/*************************************************
Description:        Get atomization time
Parameters:         timeValue: Variables for storing atomization timing remaining time
Return:             Communication status: COMMUNICATION_OK or COMMUNICATION_ERROR
                    0:ERROR
Others:             
*************************************************/
uint8_t BMN31K522::getAtomizationTime()
{
  uint8_t res;
  uint8_t timeValue=0;
  dataTxBuf[3] = GET_TIME_CMD;
  dataTxBuf[4] = 0x01;
  dataTxBuf[5] = 0x00;
  dataTxBuf[6] = 0;
  for (int i = 0; i < 6; i++)
  {
    dataTxBuf[6] += dataTxBuf[i];
  }

  res = _uartCommunication(dataTxBuf, dataRxBuf, 7); //Send command frame and receive response frame
  if(res == 0)    
  {
    timeValue = dataRxBuf[5]; 
  }
     
  return timeValue;
}

/*************************************************
Description:        Get water status
Parameters:         waterStatus: Variables for storing atomizer water status
Return:             Communication status: COMMUNICATION_OK or COMMUNICATION_ERROR
                    0:ERROR
Others:             
*************************************************/
uint8_t BMN31K522::getWaterStatus()
{
  uint8_t res;
  uint8_t waterStatus=0;
  dataTxBuf[3] = GET_WATER_STATUE_CMD;
  dataTxBuf[4] = 0x01;
  dataTxBuf[5] = 0x00;
  dataTxBuf[6] = 0;
  for (int i = 0; i < 6; i++)
  {
    dataTxBuf[6] += dataTxBuf[i];
  }

  res = _uartCommunication(dataTxBuf, dataRxBuf, 7); //Send command frame and receive response frame
  if(res == 0)                                      //res-- Normal: return value is 0; Abnormal：return error message
  {
    waterStatus = dataRxBuf[5];
  }
  
  return waterStatus;
}

/*************************************************
Description:        Sending Command frame and receiving response data frame
Parameters:         dataSend：Command frame
                    dataRec：Array for storing response frame
                    dataLen：Data frame length
Return:             Data frame status: COMMUNICATION_OK - no error; COMMUNICATION_ERROR - error
Others:             
*************************************************/
uint8_t BMN31K522::_uartCommunication(uint8_t dataSend[], uint8_t dataRec[], uint8_t dataLen)
{
  uint8_t errFlag;
  writeBytes(dataSend, dataLen);
  delay(2);//tDEL-RSP 
  errFlag=readBytes(dataRec, dataLen,30); 
  if(dataRec[3]==0xff)
  {
    return COMMUNICATION_ERROR;
  }
  else
  {
    return errFlag;
  }
  delay(10);//tINR    
}
/**********************************************************
Description: Read data through UART
Parameters: rbuf: Used to store received data
            rlen: Length of data to be read
Return: 0: check ok
        1: check error
        2: timeout error
Others: None
**********************************************************/
uint8_t BMN31K522::readBytes(uint8_t rbuf[], uint8_t rlen, uint16_t timeout)
{
  uint8_t i = 0, delayCnt = 0, checkSum = 0;
/* Select SoftwareSerial Interface */
  if (_softSerial != NULL)
  {
    for (i = 0; i < rlen; i++)
    {
      delayCnt = 0;
      while (_softSerial->available() == 0)
      {
        if (delayCnt > timeout)
        {
          return TIMEOUT_ERROR; // Timeout error
        }
        delay(1);
        delayCnt++;
      }
      rbuf[i] = _softSerial->read();
    }
  }
/* Select HardwareSerial Interface */
  else
  {
    for (i = 0; i < rlen; i++)
    {
      delayCnt = 0;
      while (_serial->available() == 0)
      {
        if (delayCnt > timeout)
        {
          return TIMEOUT_ERROR; // Timeout error
        }
        delay(1);
        delayCnt++;
      }
      rbuf[i] = _serial->read();
    }
  }
/* check Sum */
  for (i = 0; i < (rlen - 1); i++)
  {
    checkSum += rbuf[i];
  }
  if (checkSum == rbuf[rlen - 1])
  {
    return CHECK_OK; // Check correct
  }
  else
  {
    return CHECK_ERROR; // Check error
  }
}

/**********************************************************
Description: Write data through UART
Parameters: wbuf:The array for storing Data to be sent
            wlen:Length of data sent
Return: None
Others: None
**********************************************************/
void BMN31K522::writeBytes(uint8_t wbuf[], uint8_t wlen)
{
  /* Select SoftwareSerial Interface */
  if (_softSerial != NULL)
  {
    while (_softSerial->available() > 0)
    {
      _softSerial->read();
    }
    _softSerial->write(wbuf, wlen);
  }
  /* Select HardwareSerial Interface */
  else
  {
    while (_serial->available() > 0)
    {
      _serial->read();
    }
    _serial->write(wbuf, wlen);
  }
}
