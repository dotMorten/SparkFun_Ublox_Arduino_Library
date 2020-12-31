/*
  Configuring the GNSS to automatically send odometer reports over I2C and display the data using a callback
  By: Paul Clark
  SparkFun Electronics
  Date: December 30th, 2020
  License: MIT. See license file for more information but you can
  basically do whatever you want with this code.

  This example shows how to configure the u-blox GNSS to send odometer reports automatically
  and display the data via a callback. No more polling!

  Feel like supporting open source hardware?
  Buy a board from SparkFun!
  ZED-F9P RTK2: https://www.sparkfun.com/products/15136
  NEO-M8P RTK: https://www.sparkfun.com/products/15005
  SAM-M8Q: https://www.sparkfun.com/products/15106

  Hardware Connections:
  Plug a Qwiic cable into the GPS and a BlackBoard
  If you don't have a platform with a Qwiic connection use the SparkFun Qwiic Breadboard Jumper (https://www.sparkfun.com/products/14425)
  Open the serial monitor at 115200 baud to see the output
*/

#include <Wire.h> //Needed for I2C to GPS

#include <SparkFun_Ublox_Arduino_Library.h> //http://librarymanager/All#SparkFun_u-blox_GNSS
SFE_UBLOX_GPS myGPS;

// Callback: printODOdata will be called when new NAV ODO data arrives
// See u-blox_structs.h for the full definition of UBX_NAV_ODO_data_t *packetUBXNAVODOcopy
void printODOdata()
{
    Serial.println();

    Serial.print(F("TOW: ")); // Print the Time Of Week
    unsigned long iTOW = myGPS.packetUBXNAVODOcopy->iTOW; // iTOW is in milliseconds
    Serial.print(iTOW);
    Serial.print(F(" (ms)"));
    
    Serial.print(F(" Distance: "));
    unsigned long distance = myGPS.packetUBXNAVODOcopy->distance; // Print the distance
    Serial.print(distance);
    Serial.print(F(" (m)"));

    Serial.print(F(" Total Distance: "));
    unsigned long totalDistance = myGPS.packetUBXNAVODOcopy->totalDistance; // Print the total distance
    Serial.print(totalDistance);
    Serial.println(F(" (m)"));
}

void setup()
{
  Serial.begin(115200);
  while (!Serial); //Wait for user to open terminal
  Serial.println("SparkFun u-blox Example");

  Wire.begin();

  //myGPS.enableDebugging(); // Uncomment this line to enable helpful debug messages on Serial

  if (myGPS.begin() == false) //Connect to the Ublox module using Wire port
  {
    Serial.println(F("Ublox GPS not detected at default I2C address. Please check wiring. Freezing."));
    while (1);
  }

  myGPS.setI2COutput(COM_TYPE_UBX); //Set the I2C port to output UBX only (turn off NMEA noise)
  myGPS.saveConfigSelective(VAL_CFG_SUBSEC_IOPORT); //Save (only) the communications port settings to flash and BBR
  
  myGPS.setNavigationFrequency(1); //Produce one solution per second

  //myGPS.resetOdometer(); //Uncomment this line to reset the odometer
  
  myGPS.setAutoNAVODOcallback(printODOdata); // Enable automatic NAV ODO messages with callback to printODOdata
}

void loop()
{
  myGPS.checkUblox(); // Check for the arrival of new data and process it.
  myGPS.checkCallbacks(); // Check if any callbacks are waiting to be processed.
  
  Serial.print(".");
  delay(50);
}
