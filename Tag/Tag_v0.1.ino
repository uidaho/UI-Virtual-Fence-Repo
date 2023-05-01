#include <SPI.h>                                //the LoRa device is SPI based so load the SPI library
#include <SX128XLT.h>                           //include the appropriate library   

//*******  Setup hardware pin definitions here ! ***************

const uint8_t NSS = 10;                         //select pin on LoRa device
const uint8_t NRESET = 9;                       //reset pin on LoRa device
const uint8_t RFBUSY = 7;                       //busy pin on LoRa device
const uint8_t DIO1 = 5;                         //DIO1 pin on LoRa device, used for sensing RX and TX done
const uint8_t LED1 = 8;
const uint8_t LORA_DEVICE = DEVICE_SX1280;      //we need to define the device we are using


//*******  Setup LoRa modem parameters here ! ***************
const uint32_t Frequency = 2445000000;          //frequency of transmissions
const uint32_t Offset = 0;                      //offset frequency for calibration purposes
const int8_t  TXpower = 2;                      //LoRa transmit power
const int8_t RangingTXPower = 10;               //Transmit power used for ranging
const uint8_t Bandwidth = LORA_BW_0800;         //LoRa bandwidth
const uint8_t SpreadingFactor = LORA_SF8;       //LoRa spreading factor
const uint8_t CodeRate = LORA_CR_4_5;           //LoRa coding rate
const uint32_t ACKdelay = 200;                  //delay in mS before sending reply                      
const uint32_t RXtimeout = 5000;                //receive timeout in mS.

const uint8_t RequestReset = 1;                 //request type number for reset
const uint8_t RequestRanging = 2;               //request type for ranging
const uint8_t RequestShock = 3;               //request type for shocking

//GPS co-ordinates to use for the GPS location request
const float TestLatitude  = 51.48230;           //GPS co-ordinates to use for test
const float TestLongitude  = -3.18136;          //Cardiff castle keep, used for testing purposes
const float TestAltitude = 25.5;
const uint8_t TrackerStatus = 1;                //set status bit to represent tracker GPS has fix

const uint16_t NetworkID = 0x3210;              //NetworkID identifies this connection, needs to match value in transmitter
const uint8_t ThisStation = 123;                //the number of this station for requests and ranging

SX128XLT LT;                                    //create a library class instance called LT

uint8_t RXBUFFER[251];                          //create the buffer that received packets are copied into
uint8_t RXPacketL;                              //stores length of packet received
uint8_t RXPayloadL;                             //stores length of payload received
uint8_t TXPayloadL;                             //stores length of payload sent
uint16_t TransmitterNetworkID;                  //the NetworkID from the transmitted and received packet
uint16_t StationNumber;
uint8_t RequestType;

uint8_t Warnings = 0;

#define DEBUG


void setup() {
  // put your setup code here, to run once:
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

  Serial.println(F("Receiver ready"));
  Serial.println();
}

void loop() {
  // put your main code here, to run repeatedly:
  uint8_t requesttype;

  LT.setupLoRa(Frequency, Offset, SpreadingFactor, Bandwidth, CodeRate);   //need to be in LoRa mode to receiv requests

  requesttype = waitRequest(ThisStation, 5000);                        //listen for 5000mS, will return 0 if there is no request

  Serial.println(requesttype);

  switch (requesttype)
  {
    case 0:
      break;
      
    case RequestReset:
      actionReset();
      break;

    case RequestRanging:
      actionRanging(ThisStation);
      break;

    case RequestShock:
      actionShock();
      break;

    default:
      Serial.println(F("Request not recognised"));
      break;
  }
  Serial.println();
}

uint8_t waitRequest(uint8_t station, uint32_t timeout)
{
  //wait for an incoming request, returns 0 if no request in timeout period

  Serial.println(F("Wait request"));

  RXPacketL = LT.receiveSXReliable(0, NetworkID, timeout, WAIT_RX);

  if (RXPacketL)
  {
#ifdef DEBUG
    Serial.print(F("Reliable packet received > "));
    packet_is_OK();
#endif
    RequestType = LT.getByteSXBuffer(0);
    StationNumber = LT.getByteSXBuffer(1);

    Serial.print(F("Received "));
    printRequestType(RequestType);
    Serial.print(F(" for station "));
    Serial.println(StationNumber);

    if (StationNumber == station)
    {
      return RequestType;
    }
    else
    {
      Serial.println(F("ERROR Request not for this station"));
    }
  }
  else
  {
    if (LT.readIrqStatus() & IRQ_RX_TIMEOUT)
    {
      Serial.println(F("ERROR Timeout waiting for valid request"));
    }
    else
    {
      packet_is_Error();
    }
  }
  return 0;
}

void actionRanging(uint32_t rangingaddress)
{
  uint16_t IrqStatus;
  uint16_t RangingUpTimemS;
  uint16_t RXPayloadCRC;
  uint32_t startmS;

  LT.startReadSXBuffer(2);                   //initialise SX buffer write at location 2
  RangingUpTimemS = LT.readUint16();         //the ranging listen time was sent with request.
  LT.endReadSXBuffer();

  Serial.print("RangingUpTimemS ");
  Serial.println(RangingUpTimemS);

  RXPayloadCRC = LT.getRXPayloadCRC(RXPacketL);       //fetch received payload crc to return with ack

  LT.startWriteSXBuffer(0);                   //initialise SX buffer write at address startaddr
  LT.writeUint8(RequestRanging);              //identify type of request
  LT.writeUint8(ThisStation);                 //who is the request reply from
  TXPayloadL = LT.endWriteSXBuffer();         //close SX buffer write

  delay(ACKdelay);

  LT.sendSXReliableACK(0, TXPayloadL, NetworkID, RXPayloadCRC, TXpower);
  Serial.println(F("Ranging request replied"));

  LT.setupRanging(Frequency, Offset, SpreadingFactor, Bandwidth, CodeRate, rangingaddress, RANGING_SLAVE);
  LT.receiveRanging(rangingaddress, 0, RangingTXPower, NO_WAIT);

  startmS = millis();

  digitalWrite(LED1, HIGH);

  do
  {
    if (digitalRead(DIO1))
    {
      IrqStatus = LT.readIrqStatus();

      if (IrqStatus & IRQ_RANGING_SLAVE_RESPONSE_DONE)
      {
        Serial.print("Ranging response sent");
        LT.receiveRanging(ThisStation, 0, RangingTXPower, NO_WAIT);
      }
      else
      {
        Serial.print("ERROR Ranging slave error,");
        Serial.print(",Irq,");
        Serial.print(IrqStatus, HEX);
        LT.printIrqStatus();
      }
      Serial.println();
    }

  }
  while ( ((uint32_t) (millis() - startmS) < RangingUpTimemS));

  Serial.println("Ranging receive timeout!!");
  digitalWrite(LED1, LOW);
}


void actionShock(){
  Serial.println(F("actionShock"));

  Warnings = Warnings + 1;

  if(Warnings == 1){
    Serial.println(F("BEEP!"));
  }
  else if(Warnings == 2){
    Serial.println(F("zap"));
  }
  else if(Warnings == 3){
    Serial.println(F("Zap"));
  }
  else if(Warnings == 4){
    Serial.println(F("ZAP"));
  }
  
  /*uint16_t RXPayloadCRC;

  RXPayloadCRC = LT.getRXPayloadCRC(RXPacketL);       //fetch received payload crc to return with ack

  LT.startWriteSXBuffer(0);                   //initialise SX buffer write at address 0
  LT.writeUint8(RequestShock);          //identify type of request
  LT.writeUint8(ThisStation);                 //who is the request reply from
  LT.writeFloat(TestLatitude);                //add latitude
  LT.writeFloat(TestLongitude);               //add longitude
  LT.writeFloat(TestAltitude);                //add altitude
  LT.writeUint8(TrackerStatus);               //add status byte
  TXPayloadL = LT.endWriteSXBuffer();         //close SX buffer write

  delay(ACKdelay);

  digitalWrite(LED1, HIGH);
  LT.sendSXReliableACK(0, TXPayloadL, NetworkID, RXPayloadCRC, TXpower);
  Serial.print(F("RequestShock Replied > "));
  Serial.print(ThisStation);
  Serial.print(F(","));
  Serial.print(TestLatitude, 6);
  Serial.print(F(","));
  Serial.print(TestLongitude, 6);
  Serial.print(F(","));
  Serial.print(TestAltitude, 1);
  Serial.print(F(","));
  Serial.print(TrackerStatus);
  Serial.println();
  Serial.flush();
  digitalWrite(LED1, LOW);*/
  return true;
}

void actionReset(){
  Warnings = 0;
  return true;
}

void printRequestType(uint8_t type)
{
  switch (type)
  {
    case RequestReset:
      Serial.print(F(" RequestReset"));
      break;
    
    case RequestShock:
      Serial.print(F(" RequestShock"));
      break;

    case RequestRanging:
      Serial.print(F(" RequestRanging"));
      break;

    default:
      Serial.print(F(" Request type not recognised"));
      break;
  }
}


void packet_is_OK()
{
  printPacketDetails();
  Serial.println();
}


void packet_is_Error()
{
  uint16_t IRQStatus;

  IRQStatus = LT.readIrqStatus();                   //read the LoRa device IRQ status register
  Serial.print(F("Error "));

  if (IRQStatus & IRQ_RX_TIMEOUT)                   //check for an RX timeout
  {
    Serial.print(F(" RXTimeout "));
  }
  else
  {
    printPacketDetails();
  }
}


void printPacketDetails()
{
  Serial.print(F("LocalNetworkID,0x"));
  Serial.print(NetworkID, HEX);
  Serial.print(F(",TransmitterNetworkID,0x"));
  Serial.print(LT.getRXNetworkID(RXPacketL), HEX);
  Serial.print(F(",RXPayloadCRC,0x"));
  Serial.print(LT.getRXPayloadCRC(RXPacketL), HEX);
  LT.printReliableStatus();
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
