# Rev03 Hardware documentation

## Change Log

| Iteration |Change Description|
|---|---|
|3.1|Effort to put all components into a library with called out BOM for ease of manufacturing.  Slight position changes warranted uprev but no significant changes were made.|



## Pinout

|  Description       | Arduino Pin Number |Port|
| ----------- | -----------|-----------|
|FTDI|   D0  |PD0|
|FTDI|   D1|PD1|
|Shock Boost Enable|D2|PD2|
|J5|D3|PD3|
|J5|D4|PD4|
|Radio Dio1|D5|PD5|
|Trigger Shock|D6|PD6|
|Radio Busy|D7|PD7|
|D3 Radio LED|D8|PB0|
|Radio Reset|D9|PB1|
|Radio NSS|D10|PB2|
|MOSI|D11|PB3|
|MISO|D12|PB4|
|SCK|D13|PB5|
|Boost Feedback CS|A0|PC0|
|D5/J10 - Cuttable LED|A1|PC1|
|Buzzer Pin|A2|PC2|
|Flash Chip Select|A3|PC3|
|N/A|A4|PC4|
|N/A|A5|PC5|
|D4/J10 - Cuttable LED|A6||
|D6/J10 - Cuttable LED|A7||

## Intended use

Note: The intended first step of initializing the device is to drag D6 and D2 down to ensure that the shock device doesn't trigger.

Note: This revision assumes the same hardware revision on each end. Rev03.XX devices should all be able to work together with the same firmware.

The device was design to take the SX1280 on board and use it to measure time of flight distance. Based on the resulting distance between multiple devices a relative position can be determined. It is possible to tie this relative position to an offboard GPS via Header J5 to make it an absolute position.

With that position the system can then either use the onboard buzzer or offboard shock unit to deliver a negative stimulus based on assigned boundaries.

If the user so chooses there is onboard flash memory that can be leveraged in this process.

The device has onboard LiPo charging and a voltage regulation so it can be battery powered and a solar panel can be attached to make the lifetime theoretically infinite.

The device relied heavily on SPI enabled devices.

## Microcontroller

To be supply chain friendly the microcontroller can either be installed during manufacturing or an arduino pro mini can be used in its place.  If installed during manufacturing the clock speed should be set to 8Mhz due to the fact we are using 3.3v through out our system.

## Mounting
Currently the entire Rev03.XX has M3 holes spaced at 1.71" X 2.28" (43.5mm X 58mm)

## Connectors
At this time all connectors are the molex brand microfit lineup. These are easy to work with, keyed, and locking.  This combination of features makes them perfect for our need.
