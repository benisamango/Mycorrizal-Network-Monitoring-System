/*
  This script is adapted from the Example01_SingleShot.ino script by Paul Clark. 

  By: Paul Clark
  SparkFun Electronics
  Date: 2023/12/11
  SparkFun code, firmware, and software is released under the MIT License.
  Please see LICENSE.md for further details.

  Hardware Connections:
  IoT RedBoard --> ADS1219
  QWIIC        --> QWIIC

  Open the serial monitor at 115200 baud to see the voltage.

  Feel like supporting our work? Buy a board from SparkFun!
  https://www.sparkfun.com/products/23455 - Qwiic ADS1219 1x1
*/

// You will need the SparkFun Toolkit. Click here to get it: http://librarymanager/All#SparkFun_Toolkit

#include <SparkFun_ADS1219.h> // Click here to get the library: http://librarymanager/All#SparkFun_ADS1219

// Additional includes for SD card and M5Core2 display
#include <M5Core2.h>
#include <SPI.h>
#include <SD.h>
#include <TimeLib.h>

//  Wednesday, August 28, 2024 12:03:00 PM
unsigned long startTime = 1724846580; // enter epoch start time

// File object for SD card
File dataFile;

SfeADS1219ArdI2C myADC;

void setup() {
  delay(1000); // Allow time for the microcontroller to start up
  
  /* Initialize M5 */
  M5.begin();
  M5.Lcd.setTextSize(2);
  M5.Lcd.setCursor(0, 0);
  M5.Lcd.println("Initializing Serial...");

  Serial.begin(115200); // Begin the Serial console
  while (!Serial) {
    delay(100); // Wait for the user to open the Serial Monitor
  }
  Serial.println("SparkFun ADS1219 Example");

  Wire.begin(); // Begin the I2C bus

  // Initialize ADC - this also performs a soft reset
  while (myADC.begin() == false) {
    Serial.println("ADC failed to begin. Please check your wiring! Retrying...");
    delay(1000);
  }

  // Initialize the SD card
  if (!SD.begin(10)) { // Adjust the chip select pin as necessary
    Serial.println("Card failed, or not present");
    M5.Lcd.println("Card failed, or not present");
    while (1);
  }

  Serial.println("Card initialized.");

  // Print date and time
  M5.Lcd.println("Time:");
  M5.Lcd.println(startTime);
}

void loop() {
  unsigned long currentMillis = millis();

  // Perform single-shot conversion
  if (myADC.startSingleShotConversion() == true) {
    delay(10); // Small delay for stability

    float milliVolts = myADC.getResult_mV(); // Read the result

    // Open file for appending
    dataFile = SD.open("/control-one-hour.csv", FILE_WRITE); // Or use alternative name
    if (dataFile) {
      // Move the file pointer to the end
      dataFile.seek(dataFile.size());

      unsigned long currentTime = startTime + (currentMillis / 1000); // Current time in Unix timestamp

      // Convert the current time to ISO 8601 format
      char isoTime[25];
      sprintf(isoTime, "%04d-%02d-%02dT%02d:%02d:%02dZ", 
              year(currentTime), 
              month(currentTime), 
              day(currentTime), 
              hour(currentTime), 
              minute(currentTime), 
              second(currentTime));

      // Write the time and voltage to the file
      dataFile.print(isoTime); // Print the current time in seconds
      dataFile.print(",");
      dataFile.println(milliVolts, 6); // Print voltage with 6 decimal places

      // Print to Serial
      Serial.print("time: ");
      Serial.print(currentMillis / 1000);
      Serial.print(", millivolts: ");
      Serial.println(milliVolts, 6);

      // Print to Screen
      M5.Lcd.setCursor(0, 0); // Reset cursor position to top-left corner
      M5.Lcd.fillScreen(BLACK); // Clear the screen
      M5.Lcd.setTextColor(WHITE);
      M5.Lcd.setTextSize(2);
      M5.Lcd.print("Time: ");
      M5.Lcd.println(currentMillis / 1000);
      M5.Lcd.print("Voltage: ");
      M5.Lcd.println(milliVolts, 6);

      dataFile.close();
    } else {
      Serial.println("Error opening /control-one-hour.csv");
      M5.Lcd.fillScreen(BLACK); // Clear the screen
      M5.Lcd.setCursor(0, 0);
      M5.Lcd.setTextColor(RED);
      M5.Lcd.setTextSize(2);
      M5.Lcd.println("Error opening file");
    }
  } else {
    Serial.println("ADC start conversion failed. Please check your wiring! Retrying...");
    delay(1000);
  }

  // Check every 1 second 
  delay(1000);
}