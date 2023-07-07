/*****************************************************************************************************
  Programs for Arduino - Copyright of the author Stuart Robinson - 16/03/20

  This program is supplied as is, it is up to the user of the program to decide if the program is
  suitable for the intended purpose and free from errors.
*******************************************************************************************************/

#define Program_Version "V1.0"
#define NSS 10
#define RFBUSY 7
#define NRESET 9
#define LED1 8
#define DIO1 5

#define LORA_DEVICE DEVICE_SX1280                //we need to define the device we are using

#include <SPI.h>
#include <SX128XLT.h>

SX128XLT LT;


#ifdef ENABLEOLED
#include <U8x8lib.h>                                        //https://github.com/olikraus/u8g2 
//U8X8_SSD1306_128X64_NONAME_HW_I2C disp(U8X8_PIN_NONE);      //standard 0.96" SSD1306
U8X8_SH1106_128X64_NONAME_HW_I2C disp(U8X8_PIN_NONE);     //1.3" OLED often sold as 1.3" SSD1306
#endif


//LoRa Modem Parameters
const uint32_t Frequency = 2445000000;           //frequency of transmissions in hz
const int32_t Offset = 0;                        //offset frequency in hz for calibration purposes
const uint8_t Bandwidth = LORA_BW_0800;          //LoRa bandwidth
const uint8_t SpreadingFactor = LORA_SF8;        //LoRa spreading factor
const uint8_t CodeRate = LORA_CR_4_5;            //LoRa coding rate
const uint16_t Calibration = 11350;              //Manual Ranging calibrarion value

const int8_t RangingTXPower = 10;                //Transmit power used
const uint32_t RangingAddress = 16;              //must match address in recever

const uint16_t  waittimemS = 10000;              //wait this long in mS for packet before assuming timeout
const uint16_t  TXtimeoutmS = 5000;              //ranging TX timeout in mS
const uint16_t  packet_delaymS = 0;              //forced extra delay in mS between ranging requests
const uint16_t  rangeingcount = 5;               //number of times ranging is cqarried out for each distance measurment
float distance_adjustment = 1.0000;              //adjustment factor to calculated distance


uint16_t rangeing_errors, rangeings_valid, rangeing_results;
uint16_t IrqStatus;
uint32_t endwaitmS, startrangingmS, range_result_sum, range_result_average;
float distance, distance_sum, distance_average;
bool ranging_error;
int32_t range_result;
int16_t RangingRSSI;
unsigned long loopTimeStart;
unsigned long loopTimeEnd;
unsigned long loopTimeTotal;


void setup()
{
  pinMode(LED1, OUTPUT);                                   //setup pin as output for indicator LED
  led_Flash(4, 125);                                       //two quick LED flashes to indicate program start

  Serial.begin(9600);

  SPI.begin();

  led_Flash(2, 125);

  if (LT.begin(NSS, NRESET, RFBUSY, DIO1, LORA_DEVICE))
  {
    Serial.println(F("Device found"));
    led_Flash(2, 125);
    delay(1000);
  }
  else
  {
    Serial.println(F("No device responding"));
    while (1)
    {
      led_Flash(50, 50);                                 //long fast flash indicates device error
    }
  }

  LT.setupRanging(Frequency, Offset, SpreadingFactor, Bandwidth, CodeRate, RangingAddress, RANGING_MASTER);
}

void loop()
{
  loopTimeStart = millis();
  uint8_t index;
  distance_sum = 0;
  range_result_sum = 0;
  rangeing_results = 0;                           //count of valid results in each loop

  for (index = 1; index <= rangeingcount; index++)
  {

    startrangingmS = millis()

    LT.transmitRanging(RangingAddress, TXtimeoutmS, RangingTXPower, WAIT_TX);

    IrqStatus = LT.readIrqStatus();

    if (IrqStatus & IRQ_RANGING_MASTER_RESULT_VALID)
    {
      rangeing_results++;
      rangeings_valid++;
      digitalWrite(LED1, HIGH);
      range_result = LT.getRangingResultRegValue(RANGING_RESULT_RAW);

      if (range_result > 800000)
      {
        range_result = 0;
      }
      range_result_sum = range_result_sum + range_result;

      distance = LT.getRangingDistance(RANGING_RESULT_RAW, range_result, distance_adjustment);
      distance_sum = distance_sum + distance;
      loopTimeEnd = millis();
      loopTimeTotal = (loopTimeEnd - loopTimeStart);
      Serial.print(distance, 1);
      Serial.println(F(","));
      digitalWrite(LED1, LOW);
    }
    else
    {
      rangeing_errors++;
      distance = 0;
      range_result = 0;
    }
    delay(packet_delaymS);

    if (index == rangeingcount)
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
    }
    Serial.println();
  }
  //loopTimeEnd = millis();
  //loopTimeTotal = (loopTimeEnd - loopTimeStart)*1000;
  //Serial.print("Total elapsed time: ");
  //Serial.print(loopTimeTotal);
  //Serial.println();
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
