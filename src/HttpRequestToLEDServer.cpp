/**
 * @file
 * @brief MagicPoi Lite Program
 *
 * This Arduino program controls MagicPoi Lite, a device that displays LED patterns based on
 * timeline data retrieved from a server. The program manages network connections, authentication,
 * and LED pattern updates.
 *
 * @note This program is designed for use with MagicPoi Lite hardware and supports features
 * like connecting to Wi-Fi, authenticating with the MagicPoi api, and displaying dynamic LED patterns.
 *
 * @author Tom Hastings
 * @copyright © Tom Hastings <tom@devsoft.co.za>
 * @license GNU General Public License Version 3 (GPL-3.0)
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details at <https://www.gnu.org/licenses/>.
 *
 * @see setup() - Initialization and setup of the program.
 * @see loop() - The main program loop that manages core functionality.
 */

#include <Arduino.h>
#include <LittleFS.h>
#include <WiFiClient.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>

#include <LittleFS.h>

#include <EEPROM.h>

#include "ColourPatterns.h"
#include "secrets.h"

#include "TimelineManager.h"

#define led D4 // built in LED on my D1 mini

// Pin definitions:
const int blueLed = D5;
const int greenLed = D6;
const int redLed = D7;
const int buttonPin = D8;
const int switchPin = D2;
const int switchPin2 = D1;

// timings stuff:
uint8_t signal = 0;

// WiFi credentials
const char *ssid = WIFI_SSID;
const char *password = WIFI_PASS;

// IP address of the server
const char* serverIP = SERVER_IP

const char *email = USER;
const char *passwordJwt = PASS;

WiFiClient client;
ESP8266WiFiMulti WiFiMulti;

int maxAttempts = 10; // Set the maximum number of connection attempts
int attempts = 0;    // Initialize the attempts counter

const char *jwtFilePath = "/jwt.txt";

String globaltimelineData = "";
String timelineNumber = "0";
int timelineNumberNum = 1; // starts at 1
bool checkServerForTimelineNumber = true;
int maxTimelineNumbers = 1; //todo: should be updated in setup()?

String timelineFilePath = "/timeline" + timelineNumber + ".txt";

// function declarations:
void buttonInterrupt();
void switchInterrupt();
void switchInterruptTwo();

ColourPatterns patternHandler(redLed, greenLed, blueLed);              // Create an instance of the ColourPatterns class
TimelineManager tm(jwtFilePath, serverIP, email, passwordJwt, client); // Create an instance of the TimelineManager class

// TODO: buttons and behaviour:
// Button 1: Fetch new timelines and colours - and re-set the timeline to the current one (on server). Todo: get all timelines and colour sequences, save all for toggle below.
// Button 2: Toggle between saved timelines (offline use) or series - or single patterns
// Button 2 (long press): Switch between modes - Timeline or Series (with set interval), or single pattern switching


// Debounce: 

long debounceTime = 400;
volatile unsigned long last_micros;

/**
 * @brief Interrupt service routine (ISR) for handling a button press event.
 *
 * This ISR is triggered when a button press event is detected. It checks for debounce
 * to prevent false triggers and performs specific actions when a valid button press
 * event occurs.
 *
 * @note pin 8
 * @note Debouncing is implemented to prevent false triggers; the `debounceTime`
 *       parameter controls the debounce duration in microseconds.
 * @note No actions implemented yet.
 *
 * @see debounceTime - Controls the debounce duration.
 * @see last_micros - Keeps track of the last microsecond timestamp.
 */
void IRAM_ATTR buttonInterrupt()
{ 
if ( long ( micros() - last_micros ) >= debounceTime * 1000 )
    {
      // already_got_data = false;
      Serial.println("buttonInterrupt ISR!!!");
      last_micros = micros();
    }
}

/**
 * @brief Interrupt service routine (ISR) for handling a switch state change event.
 *
 * This ISR is triggered when a change in the state of a switch is detected. It checks
 * for debounce to prevent false triggers and advances the timelineNumberNum and sets
 * the alreadyGotData flag to false - setting off a re-load of current timeline. 
 *
 * @note pin 2
 * @note Debouncing is implemented to prevent false triggers; the `debounceTime`
 *       parameter controls the debounce duration in microseconds.
 *
 * @see debounceTime - Controls the debounce duration.
 * @see last_micros - Keeps track of the last microsecond timestamp.
 * @see timelineNumberNum - Keeps track of the current timeline number.
 * @see maxTimelineNumbers - Defines the maximum timeline number allowed.
 * @see timelineNumber - Stores the current timeline number as a string.
 * @see checkServerForTimelineNumber - A flag to indicate whether to check the server
 *       for the timeline number.
 * @see tm.setAlreadyGotData() - Method to set the flag indicating timeline data status.
 */
void IRAM_ATTR switchInterrupt()
{ 
if ( long ( micros() - last_micros ) >= debounceTime * 1000 )
    {
      Serial.println("switchInterrupt ISR!!!");
      timelineNumberNum++;
      
      if(timelineNumberNum > maxTimelineNumbers){
        timelineNumberNum = 1;
      }
      Serial.println("Switched to number " + timelineNumberNum);
      timelineNumber = String(timelineNumberNum);
      checkServerForTimelineNumber = false;
      tm.setAlreadyGotData(false); //re-load timeline data with new number - currently re-loads all (bad?) 
      last_micros = micros();
    }
  
}

/**
 * @brief Interrupt service routine (ISR) for handling a second switch state change event.
 *
 * This ISR is triggered when a change in the state of a second switch is detected. It checks
 * for debounce to prevent false triggers and sets the trigger for fetching the latest timeline
 * from the api
 *
 * @note Debouncing is implemented to prevent false triggers; the `debounceTime` parameter
 *       controls the debounce duration in microseconds.
 *
 * @see debounceTime - Controls the debounce duration.
 * @see last_micros - Keeps track of the last microsecond timestamp.
 * @see checkServerForTimelineNumber - A flag to indicate whether to check the server for
 *       the timeline number.
 * @see tm.setAlreadyGotData() - Method to set the flag indicating timeline data status.
 */
void IRAM_ATTR switchInterruptTwo()
{ // pin 1
  if ( long ( micros() - last_micros ) >= debounceTime * 1000 )
    {
      // handle the ISR routine here then update last_micros
    checkServerForTimelineNumber = true;
    tm.setAlreadyGotData(false); //sets off update of current timeline from api in loop()
    Serial.println("switchTwo ISR!!!");
      last_micros = micros();
    }
  
}

/**
 * @brief Arduino setup function executed once on startup.
 *
 * This function initializes pins, sets up interrupts for switches, connects to Wi-Fi,
 * updates the JWT token, and performs other necessary setup tasks.
 *
 * @note Pin initialization includes configuring pins as inputs with pull-up resistors.
 * @note Interrupts are attached to switches to respond to button presses and state changes.
 * @note Wi-Fi is configured and connected to the specified network using Wi-FiMulti.
 * @note JWT token is updated using the `tm.updateToken()` method.
 * @note The `maxTimelineNumbers` variable is set to the total number of available timelines.
 * @note The `patternHandler.runLoading()` method is called to initiate an RGB loading pattern.
 * @note The `last_micros` variable is initialized for debounce testing.
 *
 * @see pinMode() - Configures pins as inputs with pull-up resistors.
 * @see attachInterrupt() - Attaches interrupt service routines (ISRs) to handle switch events.
 * @see WiFiMulti - Manages Wi-Fi connections.
 * @see tm.updateToken() - Updates the JWT token.
 * @see maxTimelineNumbers - Stores the total number of available timelines.
 * @see patternHandler.runLoading() - Initiates an RGB loading pattern.
 * @see last_micros - Keeps track of the last microsecond timestamp for debounce testing.
 */
void setup()
{
  //pin init: 
  pinMode(led, OUTPUT);
  digitalWrite(led, HIGH); // HIGH is off for D1 mini

  //some pins don't have input_pullup and need a 10k resistor to GND. See datasheet here: https://escapequotes.net/esp8266-wemos-d1-mini-pins-and-diagram/
  pinMode(buttonPin, INPUT_PULLUP);                                            
  attachInterrupt(digitalPinToInterrupt(buttonPin), buttonInterrupt, FALLING); 

  pinMode(switchPin, INPUT_PULLUP);                                            
  attachInterrupt(digitalPinToInterrupt(switchPin), switchInterrupt, FALLING); 

  pinMode(switchPin2, INPUT_PULLUP);                                               
  attachInterrupt(digitalPinToInterrupt(switchPin2), switchInterruptTwo, FALLING); 

  Serial.begin(115200);

  WiFi.mode(WIFI_STA);
  WiFiMulti.addAP(ssid, password);

  // Attempt to connect to WiFi: 
  Serial.print("Connecting to Wi-Fi");
  while (WiFiMulti.run() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    attempts++;

    if (attempts >= maxAttempts) {
      Serial.println("\nFailed to connect to Wi-Fi. Maximum attempts reached.");
      break;
    }
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
    
    tm.updateToken(); // check for saved token, load:

    if(tm.gotTokenTrue()){
      Serial.println("////////////////////////////Setup vars (on WiFi)//////////////////////////////////////////////");    
      maxTimelineNumbers = tm.getTotalTimelines().toInt(); // fetch total number of timelines from server
      Serial.println("maxTimelineNumbers is: " + String(maxTimelineNumbers));
      timelineNumberNum = tm.getTimelineNumber().toInt(); // fetch current active timeline from server
      Serial.println("timelineNumberNum is: " + String(timelineNumberNum));
      Serial.println("/////////////////////////////////////////////////////////////////////////////////////////");
    }
    
  } else {
    Serial.println("WiFi Failed to connect in Setup()");
    // todo: handle the connection failure
  } 
  
  patternHandler.runLoading(); // loading pattern RGB in ColourPatterns.cpp
  last_micros = micros(); // initialise for for button debounce
}

/**
 * @brief Arduino loop function executed repeatedly after setup.
 *
 * This function manages the main program logic, including handling network connections,
 * authentication, and timeline retrieval. It also updates LED patterns based on timeline data.
 *
 * @note The loop function continuously runs the main program logic once the setup is complete.
 * @note It checks for the availability of timeline data and network connectivity to perform actions.
 * @note Actions include authenticating, retrieving timeline data, and updating LED patterns.
 * @note LED patterns are updated based on the signal received from timeline data.
 *
 * @see tm.alreadyGotData() - Checks if timeline data has already been retrieved.
 * @see WiFiMulti.run() - Manages Wi-Fi connections.
 * @see tm.gotTokenTrue() - Checks if a JWT token has been obtained.
 * @see tm.authenticate() - Authenticates with the server and obtains a JWT token.
 * @see checkServerForTimelineNumber - A flag to indicate whether to check the server for the timeline number.
 * @see tm.getTimelineNumber() - Retrieves the current timeline number.
 * @see tm.clearTimeline() - Clears existing timeline data.
 * @see tm.getAllTimelines() - Requests all available timelines from the server.
 * @see tm.getTimeline() - Retrieves timeline data from the server.
 * @see tm.loadTimeline() - Loads timeline data for playback.
 * @see tm.setAlreadyGotData() - Sets the flag indicating timeline data status.
 * @see tm.setPlaying() - Sets the flag to indicate timeline playback.
 * @see signal - Stores the signal received from timeline data for LED pattern updates.
 * @see patternHandler.changeColours() - Updates LED patterns based on the signal.
 */
void loop()
{

  if (!tm.alreadyGotData())
  {
    // wait for WiFi connection to update
    if ((WiFiMulti.run() == WL_CONNECTED))
    {
      Serial.println("already_got_data false");
      if (!tm.gotTokenTrue())
      {
        if (tm.authenticate())
        {
          if(checkServerForTimelineNumber){
            timelineNumber = tm.getTimelineNumber();
          }
          
          if (!timelineNumber.length() == 0)
          {
            Serial.print("got timeline number in loop: ");
            Serial.println(timelineNumber);
            tm.clearTimeline(timelineNumber); // make sure we start from scratch..
            tm.getAllTimelines(); 
            tm.getTimeline(timelineNumber);
            tm.loadTimeline(timelineNumber);
            tm.setAlreadyGotData(true);
          }
          else
          {
            Serial.println("no timelineNumber available in loop?");
          }
        }
        else
        {
          Serial.println("Authentication failed.");
          return;
        }
      }
      else
      {
        Serial.println("Got token already, no authentication needed..");
        if(checkServerForTimelineNumber){
            timelineNumber = tm.getTimelineNumber();
          }
        if (!timelineNumber.length() == 0)
        {
          Serial.print("got timeline number in loop: ");
          Serial.println(timelineNumber);
          tm.getAllTimelines(); 
          tm.getTimeline(timelineNumber);
          tm.loadTimeline(timelineNumber);
          tm.setAlreadyGotData(true);
        }
        else
        {
          Serial.println("no timelineNumber available in loop?");
          tm.setToken(false);
        }
      }
    }
  }
  else
  {
    // Serial.println("already_got_data true"); // most times this is true
    tm.setPlaying(true);
    signal = tm.checkTimelineData(); // this plays back the timeline in getTimeline(timelineNumber);

    patternHandler.changeColours(signal);
  }
}
