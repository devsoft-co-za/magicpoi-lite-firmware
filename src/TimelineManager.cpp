#include "TimelineManager.h"

/**
 * @brief Constructs an instance of the TimelineManager class.
 *
 * This constructor initializes an instance of the TimelineManager class with the specified
 * file path for JWT tokens, server IP address, email, password for JWT authentication,
 * and a WiFiClient object for network communication.
 *
 * @param jwtFilePath The file path for storing JWT tokens.
 * @param serverIP The IP address or hostname of the server for communication.
 * @param email The email address used for authentication.
 * @param passwordJwt The JWT password used for authentication.
 * @param client A WiFiClient object for network communication.
 *
 * @note The constructor initializes class members with the provided values and can be
 *       used to set up the TimelineManager instance for managing timelines and
 *       authentication.
 * @note You can customize this class further based on your application's needs.
 */
TimelineManager::TimelineManager(const char* jwtFilePath, const char* serverIP, const char* email, const char* passwordJwt, WiFiClient client) : 
jwtFilePath(jwtFilePath), serverIP(serverIP), email(email), passwordJwt(passwordJwt), client(client){
    // Initialize any members if needed
   
}

/**
 * @brief Reads a JWT token from a file and returns it as a String.
 *
 * This function attempts to read a JWT token from a specified file path using LittleFS.
 * If the file exists, it reads the token from the file and returns it as a String.
 * If the file does not exist, it calls the `updateToken()` function to potentially
 * generate a new token and then reads it from the file.
 *
 * @return A String containing the JWT token read from the file, or an empty String if
 *         the token could not be read or generated.
 *
 * @see updateToken() - This function may call `updateToken()` to generate a new token if
 *                      the file does not exist.
 */
String TimelineManager::readJWTTokenFromFile() { //todo: else update token? 
  Serial.println("readJWTTokenFromFile called");
  String jwtToken = "";

  if (LittleFS.begin()) {
    if (LittleFS.exists(jwtFilePath)) {
      File file = LittleFS.open(jwtFilePath, "r");
      if (file) {
        jwtToken = file.readString();
        Serial.println("jwtToken from disk: " + jwtToken);
        file.close();
      }
    } else {
      updateToken(); 
      File file = LittleFS.open(jwtFilePath, "r");
      if (file) {
        jwtToken = file.readString();
        Serial.println("jwtToken from disk: " + jwtToken);
        file.close();
      }
    }
    LittleFS.end();
  }

  return jwtToken;
}

/**
 * @brief Saves a JWT token to a file.
 *
 * This function takes a JWT token as input and saves it to a specified file path using LittleFS.
 *
 * @param token The JWT token to be saved to the file.
 */
void TimelineManager::saveJWTTokenToFile(const char* token) {
  if (LittleFS.begin()) {
    File file = LittleFS.open(jwtFilePath, "w");
    if (file) {
      file.print(token);
      file.close();
      Serial.println("JWT token saved to file.");
    }
    LittleFS.end();
  }
}

/**
 * @brief Clears the data for a specific timeline.
 *
 * This function clears the data for a specific timeline identified by its number.
 *
 * @param timelineNumber The number of the timeline to be cleared.
 */
void TimelineManager::clearTimeline(String timelineNumber) {
  String timelineFilePath = "/timeline" + timelineNumber + ".txt"; 
  if (LittleFS.begin()) {
    File file = LittleFS.open(timelineFilePath, "w");
    if (file) {
      file.close();
      Serial.println("Timeline data cleared.");
    }
    LittleFS.end();
  }
}

/**
 * @brief Saves timeline data to a specified file.
 *
 * This function takes timeline data and a file path as input and saves the data to the
 * specified file using LittleFS.
 *
 * @param timelineData The timeline data to be saved to the file.
 * @param timelineFilePath The file path where the timeline data should be saved.
 */
void TimelineManager::saveTimeline(const String& timelineData, String timelineFilePath) {
  if (LittleFS.begin()) {
    File file = LittleFS.open(timelineFilePath, "w");
    if (file) {
      file.print(timelineData);
      file.close();
      Serial.println("Timeline data saved to file.");
    }
    LittleFS.end();
  }
}

/**
 * @brief Loads timeline data from a file for a specific timeline.
 *
 * This function loads timeline data from a file associated with a specific timeline
 * identified by its number. It returns the loaded data as a String.
 *
 * @param timelineNumber The number of the timeline for which data should be loaded.
 *
 * @return A String containing the loaded timeline data, or an empty String if the data
 *         could not be loaded.
 */
 String TimelineManager::loadTimeline(String timelineNumber) {
  String timelineFilePath = "/timeline" + timelineNumber + ".txt";
  String timelineData="";
  if (LittleFS.begin()) {
    if (LittleFS.exists(timelineFilePath)) {
      File file = LittleFS.open(timelineFilePath, "r");
      if (file) {
        timelineData = file.readString();
        file.close();
        Serial.println("Timeline data loaded from disk:");
        Serial.println(timelineData);
        processTimelineData(timelineData); // Process the timeline data - todo: need to re-add this! 
      }
    }
    LittleFS.end();
  }
  return timelineData;
}

/**
 * @brief Processes timeline data from a JSON string.
 *
 * This function processes timeline data provided as a JSON string. It extracts timing
 * information and RGB color values for each event in the timeline, populating the
 * respective arrays for further use.
 *
 * @param timelineData A JSON-formatted string containing timeline data.
 *
 * @note This function assumes that the JSON data has a specific format with timing
 *       information and RGB color values.
 * @note The `colours` and `timings` arrays are populated with the RGB color values and
 *       corresponding timings, respectively.
 * @note If no data is present in the timeline, this function resets relevant flags and
 *       may clear the timeline file.
 * @note After processing the data, the function sets the `already_got_data` flag to
 *       indicate that data has been successfully processed.
 *
 * @see clearTimeline(String timelineNumber) - Clears the timeline data if no data is
 *                                            present.
 * @see getTimelineNumber() - Used to retrieve the timeline number for clearing the
 *                            timeline data.
 */
void TimelineManager::processTimelineData(const String& timelineData) {
  // Serial.println("processTimelineData");
  DynamicJsonDocument led_doc(1500);
  deserializeJson(led_doc, timelineData);
  JsonObject root = led_doc.as<JsonObject>();

  int iter = 0;
  for (JsonPair kv : root) {
    // String key = kv.key().c_str();
    // JsonObject subDict = kv.value().as<JsonObject>();
    // iter = 0;
    // // Process the sub-dictionary
    // for (JsonPair subKv : subDict) {
    Serial.print("got ");
    Serial.println(kv.key().c_str());
    timeline_keys[iter] = kv.key().c_str(); //should be the last one here..
    iter++;
    // }
  }
  Serial.print("iter: ");
  Serial.println(iter);
  if(iter == 0){ //nothing here? re-set? todo: does this solve freezing??
    already_got_data = false;
    gotToken = false;
    String timelineNumber = getTimelineNumber(); //todo: is this right, using current timeline here? 
    clearTimeline(timelineNumber);
    return;
  }

  maxTimingsNum = iter;

  for (int i = 0; i < iter; i++) {
    if (timeline_keys[i][0] == '\0') { // check for empty string
      Serial.println("end");
    } else {
      redVal = led_doc[timeline_keys[i]][0];
      greenVal = led_doc[timeline_keys[i]][1];
      blueVal = led_doc[timeline_keys[i]][2];

      redInt = redVal.as<int>();
      // initialise arrays for timings: 
      colours[i] = redInt;
      timings[i] = atol(timeline_keys[i]);
      
      // if (redInt == 13) {
      //   Serial.println("LUCKY 13!!!");
      // }
      greenInt = greenVal.as<int>();
      blueInt = blueVal.as<int>();
      Serial.print("got colours for ");
      Serial.print(timeline_keys[i]);
      Serial.print(": ");
      Serial.print(redInt);
      Serial.print(", ");
      Serial.print(greenInt);
      Serial.print(", ");
      Serial.println(blueInt);
    }
  }
  // Print the colours array
      Serial.print("Colours from disk: ");
      for (int i = 0; i < iter; i++) {
        Serial.print(colours[i]);
        Serial.print(" ");
      }
      Serial.println(); // Print a new line after the array
      // Print the timeline array
      Serial.print("Timings from disk: ");
      for (int i = 0; i < iter; i++) {
        Serial.print(timings[i]);
        Serial.print(" ");
      }
      Serial.println(); // Print a new line after the array
  already_got_data = true;
  // digitalWrite(led, LOW);

  //todo: test: 
  playStartTime = millis();
}


uint8_t TimelineManager::checkTimelineData(){
  // test:
  if (playing)
  {
    // Serial.println("checking Timeline here");
    if (runNum > maxTimingsNum - 2) //todo: ???
    {
      // Serial.println("initialising runNum and playStartTime");s
      runNum = 0;
      playStartTime = millis();
    }
    // another sanity check:
    if (timings[runNum] > 0)
    {      
      // Serial.print("Checking timing for ");
      // Serial.print(timings[runNum]);
      currentMillisTimeline = millis() - playStartTime; // reset the clock! At the beginning of play this should be 0!
      
      if (currentMillisTimeline < timings[0])
      {
        // Serial.print("< ");
        // Serial.println(runNum);
        // runNum++;
      }
      //   I think currently timings[] array must be in time order to work. If the last time is smaller than the previous nothing happens? todo: sort?
      else if (currentMillisTimeline >= timings[runNum] && currentMillisTimeline <= timings[runNum + 1]) // todo: this omits the last signal..?
      {                                                                                    // saved signal
                                                                                           // Serial.print("== ");
        // Serial.print(runNum);
        // Serial.print(" of: ");
        // Serial.println(maxTimingsNum-1);
        // sanity check:
        // if (colours[runNum] < 13)
        // { // max is 22 otherwise it's a bogus signal
          signal = colours[runNum];
          // long time = timings[runNum];
          // flashy = flashes[runNum];
          // Serial.print("colour is set to: ");
          // Serial.println(colours[runNum]);
          // Serial.print("timings is set to: ");
          // Serial.println(timings[runNum]);
          // patternHandler.changeColours(colours[runNum]); //todo: do this in loop..
          runNum++;
          //todo: deal with colours here - separate funciton
        // }
      }
      else if (currentMillisTimeline >= timings[runNum+1])
      {
        runNum = maxTimingsNum; // went over - trigger reset
      }
      else 
      {
        //test code:  
        // Serial.println("what else? ");
        // Serial.println("runNum: ");
        // Serial.println(runNum);
        // Serial.print("millis: ");
        // Serial.println(millis());
        // Serial.print("currentMillis2: ");
        // Serial.println(currentMillis2);
        // Serial.print("timings[runNum]: ");
        // Serial.println(timings[runNum]);
        // Serial.print("timings[runNum + 1]: ");
        // Serial.println(timings[runNum + 1]);      
   
        // playStartTime = millis();
        // Serial.println("resetting playStartTime");
        // runNum++;
      }
    }
    
  }
  return signal;
}

/**
 * @brief Authenticates with a remote server to obtain an authentication token.
 *
 * This function sends an HTTP POST request to a remote server with the provided email
 * and password to obtain an authentication token. If the authentication is successful,
 * the token is saved to a file using the `saveJWTTokenToFile` function.
 *
 * @return `true` if authentication is successful and the token is obtained, `false`
 *         otherwise.
 *
 * @note This function relies on external HTTP communication and assumes the availability
 *       of the server at the specified URL.
 * @note The authentication token is parsed from the JSON response received from the
 *       server.
 * @note If authentication is successful, the `gotToken` flag may be set or handled
 *       globally in your application.
 * @note In case of any errors or failed authentication, this function returns `false`.
 *
 * @see saveJWTTokenToFile(const char* token) - Used to save the obtained token to a file.
 */
bool TimelineManager::authenticate() {
  HTTPClient http;
  http.begin(client, "http://" + String(serverIP) + "/api/login"); //localhost
  // http.begin(client, "http://magicpoi.circusscientist.com/api/login"); //server - todo: delete hard coding if above works..
  http.addHeader("Content-Type", "application/json");

  Serial.println("[HTTP] POST...");
  int httpCode = http.POST("{\"email\":\"" + String(email) + "\",\"password\":\"" + String(passwordJwt) + "\"}");

  // httpCode will be negative on error
  if (httpCode > 0) {
    if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_CREATED) {
      // Parse the response JSON to get the token
      DynamicJsonDocument doc(1024);
      DeserializationError error = deserializeJson(doc, http.getString());
      if (error) {
        Serial.println("Failed to parse JSON.");
        return false;
      }
      
    //   strcpy(token, doc["token"]);
      saveJWTTokenToFile(doc["token"]);
      Serial.println("Authentication successful.");
    //   gotToken = true; //todo: global - need to return and deal with in main
      return true;
    } else {
      Serial.print("[HTTP] Error code: ");
      Serial.println(httpCode);
    }
  } else {
    Serial.println("Connection failed.");
  }

  http.end();
  return false;
}

/**
 * @brief Fetches the number of the current active timeline from the magic poi server.
 * 
 * This function sends an HTTP GET request to a specified URL to retrieve the
 * total number of timelines. It uses the provided client object and server
 * authentication token for communication.
 * 
 * @return String containing the API response with the current active timeline number.
 *         An empty string is returned if the request fails.
 */
String TimelineManager::getTimelineNumber() {
  String token = readJWTTokenFromFile();
  HTTPClient http;
  http.begin(client, "http://" + String(serverIP) + "/lite/api/get-current-timeline-number"); //localhost
  // http.begin(client, "http://magicpoi.circusscientist.com/lite/api/get-current-timeline-number"); //server - todo: delete hard coding
  http.addHeader("Authorization", "Bearer " + String(token));

  Serial.println("[HTTP] GET...");
  int httpCode = http.GET();
  String response = "";
  // httpCode will be negative on error
  if (httpCode > 0) {
    if (httpCode == HTTP_CODE_OK) {
      // Print the API response
      response = http.getString();
      Serial.println(response);
      
    } else {
      Serial.print("[HTTP] Error code: ");
      Serial.println(httpCode);
      
    }
  } else {
    Serial.println("Connection failed.");
    
  }

  http.end();
  return response;
}

/**
 * @brief Fetches the total number of timelines from the magic poi server.
 * 
 * This function sends an HTTP GET request to a specified URL to retrieve the
 * total number of timelines. It uses the provided client object and server
 * authentication token for communication.
 * 
 * @return String containing the API response with the total number of timelines.
 *         An empty string is returned if the request fails.
 */
String TimelineManager::getTotalTimelines(){
  Serial.println("getTotalTimelines called");
  String token = readJWTTokenFromFile();
  HTTPClient http;
  http.begin(client, "http://" + String(serverIP) + "/lite/api/get-current-timeline-number"); //localhost url
  // http.begin(client, "http://magicpoi.circusscientist.com/lite/api/get-total-timelines"); //todo: delete hard coding
  http.addHeader("Authorization", "Bearer " + String(token));

  // Send the GET request and receive HTTP response code
  int httpCode = http.GET();
  String response = "";
  if (httpCode > 0) {
    if (httpCode == HTTP_CODE_OK) {
      // Read the API response content
      response = http.getString();
      Serial.println(response);
      
    } else {
      // Print error code if response indicates an error
      Serial.print("[HTTP] Error code: ");
      Serial.println(httpCode);
      
    }
  } else {
    // Connection failed
    Serial.println("Connection failed.");
    
  }

  http.end();

  return response; 
}

void TimelineManager::getTimeline(String tln) {
  String timelineFilePath = "/timeline" + tln + ".txt";
  String token = readJWTTokenFromFile();
  Serial.println("downloading timeline number: " + tln);
  HTTPClient http;
  http.begin(client, "http://" + String(serverIP) + "/lite/api/load-timeline?number=" + tln); //localhost
  // http.begin(client, "http://magicpoi.circusscientist.com/lite/api/load-timeline?number=" + tln); todo: delete hard coding
  http.addHeader("Authorization", "Bearer " + String(token));

  Serial.println("[HTTP] GET...");
  int httpCode = http.GET();
  String response = "";
  // httpCode will be negative on error
  if (httpCode > 0) {
    if (httpCode == HTTP_CODE_OK) {
      // Print the API response
      // DynamicJsonDocument led_doc(1500);
      String payload = http.getString();
      saveTimeline(payload, timelineFilePath);
      delay(10);
      loadTimeline(tln); //todo: don't do this every time, for multiple loads! 
      
    //   already_got_data = true; //todo: global, need to return and do in main 
    //   digitalWrite(led, LOW); //todo: global, need to return and do in main 
      
    } else {
      Serial.print("[HTTP] Error code: ");
      Serial.println(httpCode);
      // gotToken=false;
      
    }
  } else {
    Serial.println("Connection failed.");
    // gotToken=false;
  }

  http.end();
  
}

/**
 * @brief Retrieves and saves a timeline from a remote server.
 *
 * This function sends an HTTP GET request to a remote server to retrieve a timeline
 * identified by its number. The timeline data is saved to a file and can be loaded
 * using the `loadTimeline` function.
 *
 * @param tln The number of the timeline to be retrieved.
 *
 * @note This function relies on external HTTP communication and assumes the availability
 *       of the server at the specified URL.
 * @note It first reads the JWT token from a file using the `readJWTTokenFromFile`
 *       function to include in the HTTP request's authorization header.
 * @note If the HTTP request is successful (HTTP_CODE_OK), the received timeline data is
 *       saved to a file and can be loaded later.
 * @note In case of any errors or a failed HTTP request, this function handles errors and
 *       does not load the timeline data.
 *
 * @see readJWTTokenFromFile() - Used to obtain the JWT token for authentication.
 * @see saveTimeline(const String& timelineData, String timelineFilePath) - Used to save
 *        the received timeline data to a file.
 * @see loadTimeline(String timelineNumber) - Used to load the timeline data after it has
 *        been saved.
 */
void TimelineManager::getAllTimelines(){
  int number_of_timelines = getTotalTimelines().toInt();
  Serial.println("number_of_timelines: " + String(number_of_timelines));
  if(number_of_timelines > 10){
    number_of_timelines = 10; //limit number, can be removed
  }
  for(int i = 1; i < number_of_timelines; i++){
    
    String timelineNumberString = String(i);
    Serial.println("should be downloading timeline number: " + timelineNumberString);
    getTimeline(timelineNumberString);
  }
}

/**
 * @brief Updates the authentication token from a saved token.
 *
 * This function attempts to update the authentication token by reading it from a
 * saved token file. If a saved token is found, it is copied to the `token` array, and
 * the `gotToken` flag is set to `true`.
 *
 * @note This function is used to update the authentication token when the application
 *       starts to potentially avoid the need for re-authentication.
 * @note It reads the saved token from a file using the `readJWTTokenFromFile` function.
 * @note If a saved token is successfully loaded, it is copied into the `token` array for
 *       further use.
 * @note The `gotToken` flag is set to `true` if a saved token is found and loaded.
 */
void TimelineManager::updateToken(){
  Serial.println("updating jwt token?");
  String savedToken = readJWTTokenFromFile();
  if (!savedToken.isEmpty()) {
    strncpy(token, savedToken.c_str(), sizeof(token) - 1);
    token[sizeof(token) - 1] = '\0'; // Null-terminate the token string
    // strncpy(jwtToken, savedToken.c_str(), sizeof(jwtToken) - 1);
    // jwtToken[sizeof(jwtToken) - 1] = '\0'; // Null-terminate the token string
    gotToken = true;
    Serial.println("Using saved JWT token:");
    Serial.println(token);
  }
  // globaltimelineData = loadTimeline(); //todo: if there is timeline already in timeline.txt then none of loop will run currently
//todo: need a websocket GUI interface to turn this on again
}

/**
 * @brief Checks if timeline data has already been obtained.
 *
 * This function returns `true` if timeline data has been successfully obtained and
 * processed; otherwise, it returns `false`.
 *
 * @return `true` if timeline data has already been obtained; `false` otherwise.
 */
bool TimelineManager::alreadyGotData(){
  return already_got_data;
}

/**
 * @brief Sets the flag indicating whether timeline data has been obtained.
 *
 * This function allows you to set the `already_got_data` flag, which indicates whether
 * timeline data has been successfully obtained and processed.
 *
 * @param setting The value to set for the `already_got_data` flag (`true` or `false`).
 */
void TimelineManager::setAlreadyGotData(bool setting){
  already_got_data = setting;
}

/**
 * @brief Checks if an authentication token has been obtained.
 *
 * This function returns `true` if an authentication token has been obtained; otherwise,
 * it returns `false`.
 *
 * @return `true` if an authentication token has been obtained; `false` otherwise.
 */
bool TimelineManager::gotTokenTrue(){
  return gotToken;
}

/**
 * @brief Sets the flag indicating whether an authentication token has been obtained.
 *
 * This function allows you to set the `gotToken` flag, which indicates whether an
 * authentication token has been obtained.
 *
 * @param setting The value to set for the `gotToken` flag (`true` or `false`).
 */
void TimelineManager::setToken(bool setting){
  gotToken = setting;
}

/**
 * @brief Sets the flag indicating whether playback is in progress.
 *
 * This function allows you to set the `playing` flag, which indicates whether playback
 * of timeline data is in progress.
 *
 * @param setting The value to set for the `playing` flag (`true` or `false`).
 */
void TimelineManager::setPlaying(bool setting){
  playing = setting;
}