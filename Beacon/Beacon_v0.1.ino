#include <SPI.h>                                //the LoRa device is SPI based so load the SPI library                                         
#include <SX128XLT.h>                           //include the appropriate library  

//*******  Setup hardware pin definitions here ! ***************

const uint8_t NSS = 10;                         //select pin on LoRa device
const uint8_t NRESET = 9;                       //reset pin on LoRa device
const uint8_t RFBUSY = 7;                       //busy pin on LoRa device
const uint8_t DIO1 = 5;                         //DIO1 pin on LoRa device, used for sensing RX and TX done
const uint8_t LORA_DEVICE = DEVICE_SX1280;      //we need to define the device we are using
const uint8_t LED1 = 8;

//*******  Setup LoRa modem parameters here ! ***************
const uint32_t Frequency = 2445000000;          //frequency of transmissions
const uint32_t Offset = 0;                      //offset frequency for calibration purposes
const int8_t  TXpower = 2;                      //LoRa transmit power
const uint8_t Bandwidth = LORA_BW_0800;         //LoRa bandwidth
const uint8_t SpreadingFactor = LORA_SF8;       //LoRa spreading factor
const uint8_t CodeRate = LORA_CR_4_5;           //LoRa coding rate


const uint16_t NetworkID = 0x3210;              //NetworkID identifies this connection, needs to match value in transmitter
const uint8_t RequestReset = 1;                 //request type number for reset
const uint8_t RequestRanging = 2;               //request type number for ranging
const uint8_t RequestShock = 3;                 //request type number for shock

const uint32_t ACKtimeout = 1000;               //Acknowledge timeout in mS, set to 0 if ACK not used.
const uint32_t TXtimeout = 1000;                //transmit timeout in mS. If 0 return from transmit function after send.

const int8_t RangingTXPower = 10;               //Ranging transmit power used
const uint16_t  RangingTimeoutmS = 1000;        //ranging master timeout in mS, time master waits for a reply
const uint16_t  RangingUpTimemS = 2000;         //time for slave to stay in ranging listen
const uint16_t  PacketDelaymS = 0;              //forced extra delay in mS between sending packets
const uint16_t  RangingCount = 3;               //number of times ranging is carried out for each distance measurment
const float DistanceAdjustment = 1.0000;        //adjustment factor to calculated distance
const uint16_t Calibration = 11350;             //Manual Ranging calibration value

SX128XLT LT;                                    //create a library class instance called LT

uint8_t TXPacketL;                              //length of transmitted packet
uint8_t TXPayloadL;
uint8_t RXPayloadL;
uint8_t RXPacketL;                              //length of received acknowledge
uint16_t PayloadCRC;

uint8_t RXBUFFER[16];                           //create the buffer that received packets are copied into
uint8_t RequestType;                            //type of request to send
uint8_t RequestStation;                         //station request is aimed at

float Latitude;                                 //variables for GPS location
float Longitude;
float Altitude;
uint8_t TrackerStatus;

float distance_master = -1;
float radius = 10.0;

#define DEBUG

void setup() {  
  pinMode(LED1, OUTPUT);
  led_Flash(2, 125);                                       //two quick LED flashes to indicate program start
   
  Serial.begin(115200);
  Serial.println();
  Serial.println();
  Serial.println(__FILE__);
  Serial.println();

  SPI.begin();

  if (LT.begin(NSS, NRESET, RFBUSY, DIO1, LORA_DEVICE))
  {
    Serial.println(F("LoRa Device found"));
    led_Flash(2, 125);                                   //two further quick LED flashes to indicate device found
  }
  else
  {
    Serial.println(F("ERROR No LoRa device responding"));
    while (1)
    {
      led_Flash(50, 50);                                 //long fast speed LED flash indicates device error
    }
  }

  Serial.println(F("Transmitter ready"));
  Serial.println();

}

void loop() {
  // put your main code here, to run repeatedly:
  LT.setupLoRa(Frequency, Offset, SpreadingFactor, Bandwidth, CodeRate);
  RequestStation = 123;

  if (sendRequest(RequestStation, 5, RequestRanging))                     //send request for station 123, make 5 attempts
  {
    Serial.println(F("Valid ACK received"));
#ifdef DEBUG
    packet_is_OK();
#endif
    Serial.println();
    //note that RequestStation = the ranging address
    distance_master = actionRanging(RequestStation, RangingCount, RangingTimeoutmS, RangingTXPower);
  }
  else
  {
    Serial.println(F("No valid ACK received"));
  }

  Serial.print(F("Distance Master: "));
  Serial.println(distance_master);

  LT.setupLoRa(Frequency, Offset, SpreadingFactor, Bandwidth, CodeRate);
  RequestStation = 123;

  if (distance_master<radius && distance_master>0){
    Serial.println(F("Too close!"));
    if(sendRequest(RequestStation, 5, RequestShock)){
      Serial.println(F("Valid ACK received"));
#ifdef DEBUG
      packet_is_OK();
#endif
      Serial.println();
      //note that RequestStation = the ranging address
    }
  }
  else{
    Serial.println(F("Out of perimiter"));
    if(sendRequest(RequestStation, 5, RequestReset)){
      Serial.println(F("Valid ACK received"));
#ifdef DEBUG
      packet_is_OK();
#endif
      Serial.println();
      //note that RequestStation = the ranging address
    }
  }
  distance_master = -1;
  delay(2000);

}

uint8_t sendRequest(uint8_t station, uint8_t sendcount, uint8_t requestType)
{
  uint8_t ackrequesttype;
  uint8_t ackstation;
  uint8_t startcount;

  startcount = sendcount;

  do
  {
    Serial.print(startcount - sendcount + 1);
    Serial.print(F(" Transmit request"));
    printRequestType(requestType);
    Serial.print(F(" to station "));
    Serial.println(station);
    Serial.flush();

    //build the request payload
    LT.startWriteSXBuffer(0);                   //initialise SX buffer write at address 0
    LT.writeUint8(requestType);              //identify type of request
    LT.writeUint8(station);                     //station to reply to request
    LT.writeUint32(RangingUpTimemS);            //time for slave to stay in ranging listen
    TXPayloadL = LT.endWriteSXBuffer();         //close SX buffer write

    Serial.print(F("request built: "));
    Serial.println(LT.getTXPayloadCRC(TXPacketL));

    //now transmit the request
    digitalWrite(LED1, HIGH);
    TXPacketL = LT.transmitSXReliable(0, TXPayloadL, NetworkID, TXtimeout, TXpower, WAIT_TX);
    digitalWrite(LED1, LOW);
    PayloadCRC = LT.getTXPayloadCRC(TXPacketL);
    RXPacketL = LT.waitSXReliableACK(0, NetworkID, PayloadCRC, ACKtimeout);

    Serial.println(F("request transmitted"));

    if (RXPacketL > 0)
    {
      //if waitSXReliableACK() returns > 0 then valid ack was received
      ackrequesttype = LT.getByteSXBuffer(0);
      ackstation = LT.getByteSXBuffer(1);

      if ((ackrequesttype == RequestRanging) && (ackstation == station))
      {
        return RXPacketL;
      }
      delay(200);                                        //small delay between tranmission attampts
    }
    sendcount--;
  }
  while ((RXPacketL == 0) && (sendcount > 0));

  Serial.println(F("ERROR send request failed"));
  return 0;
}


void printRequestType(uint8_t type)
{
  switch (type)
  {
    case 1:
      Serial.print(F(" RequestGPSLocation"));
      break;

    case 2:
      Serial.print(F(" RequestRanging"));
      break;

    default:
      Serial.print(F(" not recognised"));
      break;
  }
}

float actionRanging(uint32_t rangingaddress, uint8_t number, uint32_t timeout, int8_t txpower)
{
  uint8_t index;
  uint16_t rangeings_valid = 0;
  float distance_sum = 0;
  int16_t RangingRSSI;
  uint16_t rangeing_errors = 0;
  uint16_t rangeing_results = 0;
  uint16_t IrqStatus;
  uint32_t range_result_sum = 0;
  uint32_t range_result_average;
  float distance;
  float distance_average;
  int32_t range_result;

  Serial.print(F("Ranging address "));
  Serial.println(rangingaddress);
  Serial.flush();

  LT.setupRanging(Frequency, Offset, SpreadingFactor, Bandwidth, CodeRate, rangingaddress, RANGING_MASTER);
  LT.setRangingCalibration(Calibration);               //override automatic lookup of calibration value from library table

  for (index = 1; index <= number; index++)
  {
    Serial.print(F("Ranging attempt "));
    Serial.println(index);

    digitalWrite(LED1, HIGH);
    LT.transmitRanging(rangingaddress, timeout, txpower, WAIT_TX);
    digitalWrite(LED1, LOW);
    
    IrqStatus = LT.readIrqStatus();

    if (IrqStatus & IRQ_RANGING_MASTER_RESULT_VALID)
    {
      rangeing_results++;
      rangeings_valid++;
      digitalWrite(LED1, HIGH);
      Serial.print(F("RangingValid"));
      range_result = LT.getRangingResultRegValue(RANGING_RESULT_RAW);
#ifdef DEBUG
      Serial.print(F(",RAWRegisterValue,"));
      Serial.print(range_result);
      Serial.print(F(","));
#endif
      if (range_result > 800000)
      {
        range_result = 0;
      }
      range_result_sum = range_result_sum + range_result;

      distance = LT.getRangingDistance(RANGING_RESULT_RAW, range_result, DistanceAdjustment);
      distance_sum = distance_sum + distance;

      Serial.print(F("Distance,"));
      Serial.print(distance, 1);
      RangingRSSI = LT.getRangingRSSI();
      Serial.print(F(",RSSI,"));
      Serial.print(RangingRSSI);
      Serial.print(F("dBm"));
      digitalWrite(LED1, LOW);
    }
    else
    {
      rangeing_errors++;
      distance = 0;
      range_result = 0;
      Serial.print(F("ERROR not valid"));
      LT.printIrqStatus();
    }
    delay(PacketDelaymS);

    if (index == number)
    {
      range_result_average = (range_result_sum / rangeing_results);

      if (rangeing_results == 0)
      {
        distance_average = 0;
      }
      else
      {
        distance_average = (distance_sum / rangeing_results);
      }

      Serial.println();
      Serial.print(F("TotalValid,"));
      Serial.print(rangeings_valid);
      Serial.print(F(",TotalErrors,"));
      Serial.print(rangeing_errors);
      Serial.print(F(",AverageRAWResult,"));
      Serial.print(range_result_average);
      Serial.print(F(",AverageDistance,"));
      Serial.print(distance_average, 1);
      Serial.flush();
    }
    Serial.println();
  }
  return distance_average;
}


void packet_is_OK()
{
  Serial.print(F("LocalNetworkID,0x"));
  Serial.print(NetworkID, HEX);
  Serial.print(F(",TransmittedPayloadCRC,0x"));        //print CRC of transmitted packet
  Serial.print(PayloadCRC, HEX);
}


void packet_is_Error()
{
  Serial.print(F("SendError"));
  LT.printIrqStatus();                                 //prints the text of which IRQs set
  LT.printReliableStatus();                            //print the reliable status
}


void led_Flash(uint16_t flashes, uint16_t delaymS)
{
  uint16_t index;
  for (index = 1; index <= flashes; index++)
  {
    digitalWrite(LED1, HIGH);
    delay(delaymS);
    digitalWrite(LED1, LOW);
    delay(delaymS);
  }
}
