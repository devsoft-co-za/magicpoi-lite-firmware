#ifndef TIMELINEMANAGER_H
#define TIMELINEMANAGER_H

#include <Arduino.h>
#include <LittleFS.h>
#include <WiFiClient.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>

#include <LittleFS.h>

// #include "ColourPatterns.h" // not here? 
// #include "secrets.h"

class TimelineManager {
public:
    TimelineManager(const char* jwtFilePath, const char* serverIP, const char* email, const char* passwordJwt, WiFiClient client);
    String readJWTTokenFromFile();
    void saveJWTTokenToFile(const char* token);
    void clearTimeline(String timelineNumber);
    void saveTimeline(const String& timelineData, String timelineFilePath);
    String loadTimeline(String timelineNumber);
    void processTimelineData(const String& timelineData);
    uint8_t checkTimelineData();
    bool authenticate();
    String getTimelineNumber();
    String getTotalTimelines();
    void getTimeline(String tln);
    void getAllTimelines();   
    void updateToken(); 
    bool alreadyGotData();
    void setAlreadyGotData(bool setting);
    bool gotTokenTrue();
    void setToken(bool setting);
    void setPlaying(bool setting);

private:
    const char* jwtFilePath;
    const char* serverIP;
    const char* email;
    const char* passwordJwt;
    WiFiClient client;

    char token[256];
    bool gotToken = false;
    const char* timeline_keys[10]; //array for keys
    char jwtToken[256];

    long timings[50]; // todo: max should be > 50, was 50 to save space for attiny...
    uint8_t colours[50];
    uint8_t signal = 0; 
    uint8_t flashes[50];
    long currentMillisTimeline = 0;
    bool playing = true;
    int maxTimingsNum = 50; // using only 50 for now
    int runNum = 0;
    long playStartTime = 0;

    JsonVariant redVal;
    JsonVariant greenVal;
    JsonVariant blueVal;

    int redInt = 1;
    int greenInt = 1;
    int blueInt = 1;

    volatile bool already_got_data = false;

    String globaltimelineData = "";
    String timelineNumber = "0";
    String timelineFilePath = "/timeline" + timelineNumber + ".txt";
};

#endif
