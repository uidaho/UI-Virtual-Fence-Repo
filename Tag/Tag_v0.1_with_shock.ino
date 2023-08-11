#include <SPI.h>                                //the LoRa device is SPI based so load the SPI library
#include <SX128XLT.h>                           //include the appropriate library   

//*******  Setup hardware pin definitions here ! ***************

const uint8_t NSS = 10;                         //select pin on LoRa devicem
const uint8_t NRESET = 9;                       //reset pin on LoRa device
const uint8_t RFBUSY = 7;                       //busy pin on LoRa device
const uint8_t DIO1 = 5;                         //DIO1 pin on LoRa device, used for sensing RX and TX done
const uint8_t LED1 = 8;
const uint8_t LORA_DEVICE = DEVICE_SX1280;      //we need to define the device we are using
const uint8_t BUZZER = 2;


//*******  Setup LoRa modem parameters here ! ***************
const uint32_t Frequency = 2445000000;          //frequency of transmissions
const uint32_t Offset = 0;                      //offset frequency for calibration purposes
const int8_t  TXpower = 10;                      //LoRa transmit power
const int8_t RangingTXPower = 10;               //Transmit power used for ranging
const uint8_t Bandwidth = LORA_BW_1600;         //LoRa bandwidth
const uint8_t SpreadingFactor = LORA_SF6;       //LoRa spreading factor
const uint8_t CodeRate = LORA_CR_4_5;           //LoRa coding rate
const uint32_t ACKdelay = 100;                  //delay in mS before sending reply                      
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
const uint8_t ThisStation = 2;                //the number of this station for requests and ranging

SX128XLT LT;                                    //create a library class instance called LT

uint8_t RXBUFFER[251];                          //create the buffer that received packets are copied into
uint8_t RXPacketL;                              //stores length of packet received
uint8_t RXPayloadL;                             //stores length of payload received
uint8_t TXPayloadL;                             //stores length of payload sent
uint16_t TransmitterNetworkID;                  //the NetworkID from the transmitted and received packet
uint16_t StationNumber;
uint8_t RequestType;

uint8_t Warnings = 0;
int Duration = 40;
byte SIntensity =1;

#define DEBUG
#define SHOCK 6
#define CHARGE 2
#define PSS A0


void setup() {
  // put your setup code here, to run once:
  pinMode(SHOCK, OUTPUT);
  digitalWrite(SHOCK, LOW);
  pinMode(PSS, OUTPUT);
  digitalWrite(PSS, HIGH);
  pinMode(NRESET, OUTPUT);
  pinMode(LED1, OUTPUT);
  pinMode(A2, OUTPUT);
  digitalWrite(A2, HIGH);
  delay(200);
  digitalWrite(A2, LOW);
  //analogWrite(A2, 100);
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

void setPot(byte m, byte n)
{
  n = max(1,n); // To prevent from too low resistance, which will burn out the POT
  m = m<<4; 
  // Accoding to MCP41X2 manual, Memory address is first 4 bits, so must be shifted 4 bits to left. 
  // Next two bits is 00 to write command so no change required
  // Next bit is ignored
  // Last bit is 1 for value = 256. Normally 0 for wiper position from 0 to 255
  digitalWrite(PSS, LOW); //Activate the POT  
  //SPI.transfer(b); 
  SPI.transfer(m);  // send address and command byte
  SPI.transfer(n); // send value (0~255)
  digitalWrite(PSS, HIGH); //Deactivate the POT 

}

word getPot(byte m)
{
digitalWrite(PSS, LOW);
  m = m<<4;
  m = m +12;
  //Accoding to MCP41X2 manual, Memory address is first 4 bits, so must be shifted 4 bits to left. 
  // Next two bits is 11 to read. 1100b = 12d.
  // Anything after 11 is ignored, the chip will read the and send the register value after 7th bit
  
  byte x1 = SPI.transfer(m); 
  // send command byte to write. 
  //x1 = 1111 11 (First six bits are always 1), 7th bit is 1 if valid address or 0 if error, 
  // 8th bits is the MSB of the 9 bit value
  
  byte x2 = SPI.transfer(0x00);//the tranfer value 0x00 is ignored as as the command is read
  //x2 is the actual value if last bit of x1 is 0, otherwise data = 256
  word x = x2;//<<7;
  //x = x+x2;
  digitalWrite(PSS, HIGH);
  return x;

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
  digitalWrite(A2, HIGH);
  delay(200);
  digitalWrite(A2, LOW);
  //Set Potentiometer
  byte PotR = 10 - 2*SIntensity;
  word PotVal = getPot(0);
  while (PotVal!= PotR){
  setPot(0, PotR);
  if (true){
      Serial.print(F("Set Pot: "));
      Serial.println(PotR);
      Serial.print(F("Get Pot: "));
      Serial.println(PotVal);
      delay(1000);
  }
    delay(10);
    PotVal = getPot(0);
  }
  digitalWrite(CHARGE, HIGH);
  delay(10);
  digitalWrite(SHOCK, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(Duration);                // wait Duration
  digitalWrite(SHOCK, LOW);    // turn the LED off by making the voltage LOW
  digitalWrite(CHARGE, LOW);
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
