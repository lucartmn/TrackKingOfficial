#include <SoftwareSerial.h>

const int sim808RxPin = 2;
const int sim808TxPin = 3;
const int SIM808_POWER_PIN = 4;

SoftwareSerial sim808(sim808RxPin, sim808TxPin);
String API_KEY = "key=5zWq43!#420";
const unsigned long smsCheckInterval = 1;
unsigned long lastSmsCheckTime = 0;
bool livetracking = false;
bool trackmode = false;
bool newCommandReceived = false;  // Flag, um zu überprüfen, ob ein neuer Befehl empfangen wurde
unsigned long trackKingstarttime;
unsigned long trackKingduration = 20000;

void setup() {
  Serial.begin(9600);  // Start Serial communication
  sim808.begin(9600);  // Start SIM808 communication
  delay(1000);         // Give time for module to initialize
  activateSIM808IfNeeded();
  configureGPRSifneeded();  // Configure GPRS for internet access
}
void loop() {
  activateSIM808IfNeeded();
  configureGPRSifneeded();
  send_trackmode_buttonStatusRequest();
  checkSMSReceived();
  send_on_off_buttonStatusRequest();
  if (livetracking && !trackmode) {  // If live tracking is enabled
    Serial.println("Searching GPS Coordinates for live tracking");
    initializeGPS();
    startLiveTracking();  // Start live tracking
    send_trackmode_buttonStatusRequest();
    checkSMSReceived();
    send_on_off_buttonStatusRequest();
  }
  if (trackmode && !livetracking) {
    Serial.println("Searching GPS Coordinates for trackmode");
    initializeGPS();
    while (millis() - trackKingstarttime < trackKingduration) {
      startLiveTracking();  // Start live tracking
    }
  }
}
void clearSimMemory() {
  sim808.println("AT+CMGD=1,4");
}
void checkSMSReceived() {
  Serial.println("...    ");
  sim808.println("AT+CMGF=1");
  delay(300);
  sim808.println("AT+CMGL=\"REC UNREAD\"");
  String response = waitForResponse(2000);

  // Überprüfen, ob eine SMS empfangen wurde
  if (response.indexOf("Off") != -1) {
    Serial.println("GPS deactivated");
    livetracking = false;
    trackmode = false;
    sim808.println("AT+CGNSPWR=0");
    delay(300);
    clearSimMemory();           // Löscht die empfangene SMS
    newCommandReceived = true;  // Neuer Befehl empfangen
    return;
  }
  if (response.indexOf("On") != -1) {
    Serial.println("GPS activated");
    livetracking = true;
    trackmode = false;
    delay(2000);
    clearSimMemory();           // Löscht die empfangene SMS
    newCommandReceived = true;  // Neuer Befehl empfangen
    return;
  }
  if (response.indexOf("Reset") != -1) {
    Serial.println("Reset");
    livetracking = false;
    trackmode = false;
    delay(1000);
    setup();
    delay(1000);
    clearSimMemory();           // Löscht die empfangene SMS
    newCommandReceived = true;  // Neuer Befehl empfangen
    return;
  }
  if (response.indexOf("Trackmode") != -1) {
    int counter = 0;
    Serial.println("Trackmode activated");
    livetracking = false;
    trackmode = true;
    delay(300);
    clearSimMemory();
    delay(200);  // Löscht die empfangene SMS
    newCommandReceived = true;  // Neuer Befehl empfangen
    return;
  }
  if (response.indexOf("Status") != -1) {
    Serial.println("Status: ");
    Serial.println("Livetracking: " + String(livetracking));
    delay(100);
    Serial.println("Trackmode: " + String(trackmode));
    delay(100);
    Serial.println("newCommandReceived: " + newCommandReceived);
    delay(100);
    sim808.println("AT+CGNSPWR?");
    String response = waitForResponse(1000);
    Serial.println(response);
    delay(300);
    clearSimMemory();
    delay(200);  // Löscht die empfangene SMS
    return;
  }
}
void activateSim808() {
  // Configure SIM808 power pin as output
  pinMode(SIM808_POWER_PIN, OUTPUT);
  // Power on SIM808 module
  digitalWrite(SIM808_POWER_PIN, HIGH);
  delay(2000);
  // Power off SIM808 module
  digitalWrite(SIM808_POWER_PIN, LOW);
  delay(2000);
}
bool isSIM808On() {
  sim808.println("AT");                            // Send "AT" command to the module
  return waitForexpectedResponsebool("OK", 4000);  // Wait for "OK" response from the module within 1 second
}
void configureGPRSifneeded() {
  // If GPRS is not yet configured
  if (!isGPRSConnected()) {
    // Connect to the network
    sim808.println("AT+CGATT=1");
    if (waitForexpectedResponsebool("OK", 1000)) {
      // Define PDP context
      sim808.println("AT+SAPBR=1,1");
      // Set the APN and user ID
      sim808.println("AT+SAPBR=3,1,\"APN\",\"pinternet.interkom.de\"");
      if (waitForexpectedResponsebool("OK", 1000)) {
        // Set the connection type to GPRS
        sim808.println("AT+SAPBR=3,1,\"CONTYPE\",\"GPRS\"");
        if (waitForexpectedResponsebool("OK", 1000)) {
          // Mark GPRS as configured
          if (!isGPRSConnected()) {
          Serial.println("GPRS Setup done..");
          delay(3000);
          }
        }
      }
    }
  }
}
bool isGPRSConnected() {
  // Send AT command to check GPRS connection status
  sim808.println("AT+CGATT?");
  // Wait for the response
  return waitForexpectedResponsebool("+CGATT: 1", 4000);  // Adjust the expected response accordingly
}
void initializeGPS() {
  activateSIM808IfNeeded();
  // Query GPS status
  sim808.println("AT+CGNSPWR?");
  // Initialize GPS if not already
  if (!waitForexpectedResponsebool("+CGNSPWR: 1", 500)) {
    sim808.println("AT+CGNSPWR=1");
    delay(800);
    Serial.println("Initialized GPS");
  }
}
void activateSIM808IfNeeded() {
  if (!isSIM808On()) {
    activateSim808();
    Serial.println("SIM808 was down. Is now active.");
    delay(6000);
  }
}
void startLiveTracking() {
  Serial.println("Searching GPS Coordinates");
  // Get GPS data
  String gpsData = get_gps();
  // Check if GPS data is valid
  if (isValidGPSData(gpsData)) {
    // Update server data with valid GPS data
    updateServerData(gpsData);
  }
}
String get_gps() {
  // Request GPS data from the module
  sim808.println("AT+CGNSINF");
  String response = waitForResponse(400);
  // Check if GPS data is available
  if (response.indexOf("+CGNSINF:") != -1) {
    String gpsData = response.substring(response.indexOf("+CGNSINF:") + 10);
    String latitude = getValue(gpsData, ',', 3);
    String longitude = getValue(gpsData, ',', 4);
    String altitude = getValue(gpsData, ',', 5);
    String speed = getValue(gpsData, ',', 6);
    // Format the GPS data into a POST request
    String postData = API_KEY + "&latitude=" + latitude + "&longitude=" + longitude + "&altitude=" + altitude + "&speed=" + speed;
    Serial.println("postData: " + postData);
    return postData;
  } else {
    return "GPS Error";
    Serial.println("GPS-Data not available.");
  }
}
bool isValidGPSData(String gpsData) {
  // Get start indices of latitude and longitude
  int latitudeStart = gpsData.indexOf("latitude=");
  int longitudeStart = gpsData.indexOf("longitude=");
  // Check if both latitude and longitude exist
  if (latitudeStart != -1 && longitudeStart != -1) {
    // Extract latitude and longitude strings
    String latitudeStr = gpsData.substring(latitudeStart + 9, gpsData.indexOf("&", latitudeStart));
    String longitudeStr = gpsData.substring(longitudeStart + 10, gpsData.indexOf("&", longitudeStart));
    // Convert strings to float values
    float latitude = latitudeStr.toFloat();
    float longitude = longitudeStr.toFloat();
    // Check if both latitude and longitude are not zero
    if (latitude != 0.0 && longitude != 0.0) {
      return true;
    }
  }
  Serial.println("No Valid Data: " + gpsData);
  return false;
}
void updateServerData(String postData) {
  const char* postDataChar = postData.c_str();  // Convert String to char*
  // Initialize HTTP
  sim808.println("AT+HTTPINIT");
  delay(600);
  // Set connection ID
  sim808.println("AT+HTTPPARA=\"CID\",1");
  delay(600);
  // Set URL
  sim808.println("AT+HTTPPARA=\"URL\",\"http://lets-stream.eu/get_gps_data.php\"");
  delay(1100);
  // Set content type
  sim808.println("AT+HTTPPARA=\"CONTENT\",\"application/x-www-form-urlencoded\"");
  delay(700);
  // Set HTTP data length
  sim808.println(("AT+HTTPDATA=" + String(postData.length()) + ",5000").c_str());
  delay(600);
  // Send HTTP data
  sim808.println(postDataChar);
  delay(400);
  // Display updating message
  Serial.println("Updating Server...             ");
  // Send HTTP request
  sim808.println("AT+HTTPACTION=1");
  delay(800);
  // Terminate HTTP session
  sim808.println("AT+HTTPTERM");
  delay(500);
}
void send_on_off_buttonStatusRequest() {
  const char* postDataChar = "button_name=on_off_button";  // HTTP POST data
  Serial.println("Checke On/Off Button");
  delay(800);
  // HTTP-Anfrage vorbereiten
  sim808.println("AT+HTTPINIT");
  delay(1500);
  sim808.println("AT+HTTPPARA=\"CID\",1");
  delay(1500);
  sim808.println("AT+HTTPPARA=\"URL\",\"http://lets-stream.eu/get_button_status.php\"");
  delay(1500);
  // Set content type
  sim808.println("AT+HTTPPARA=\"CONTENT\",\"application/x-www-form-urlencoded\"");
  delay(1500);
  // Set HTTP data length
  sim808.println("AT+HTTPDATA=" + String(strlen(postDataChar)) + ",5000");
  delay(1000);
  // Send HTTP data
  sim808.println(postDataChar);
  delay(400);
  // Sende GET-Anfrage und führe die Aktion aus
  sim808.println("AT+HTTPACTION=1");
  delay(2000);
  sim808.println("AT+HTTPREAD");
  int response = readHTTPResponse();
  delay(1500);
  // Überprüfen, ob die Antwort "buttonStatus": "0" enthält
  // Handling response
  if (response == 0) {
    Serial.println("GPS deactivated");
    trackmode = false;
  }
  if (response == 1) {
    Serial.println("GPS activated");
    trackmode = true;
  }
  // Beende die HTTP-Sitzung
  sim808.println("AT+HTTPTERM");
  delay(1500);
}
void send_trackmode_buttonStatusRequest() {
  const char* postDataChar = "button_name=trackmode_button";  // HTTP POST data
  Serial.println("Checke Trackingmode Button");
  delay(800);
  // HTTP-Anfrage vorbereiten
  sim808.println("AT+HTTPINIT");
  delay(1500);
  sim808.println("AT+HTTPPARA=\"CID\",1");
  delay(1500);
  sim808.println("AT+HTTPPARA=\"URL\",\"http://lets-stream.eu/get_button_status.php\"");
  delay(1500);
  // Set content type
  sim808.println("AT+HTTPPARA=\"CONTENT\",\"application/x-www-form-urlencoded\"");
  delay(1500);
  // Set HTTP data length
  sim808.println("AT+HTTPDATA=" + String(strlen(postDataChar)) + ",5000");
  delay(1000);
  // Send HTTP data
  sim808.println(postDataChar);
  delay(400);
  // Sende GET-Anfrage und führe die Aktion aus
  sim808.println("AT+HTTPACTION=1");
  delay(2000);
  sim808.println("AT+HTTPREAD");
  int response = readHTTPResponse();
  delay(1500);
  // Überprüfen, ob die Antwort "buttonStatus": "0" enthält
  // Handling response
  if (response == 0) {
    Serial.println("Trackmode deactivated");
    trackmode = false;
  }
  if (response == 1) {
    Serial.println("Trackmode activated");
    trackmode = true;
  }
  // Beende die HTTP-Sitzung
  sim808.println("AT+HTTPTERM");
  delay(1500);
}
String waitForResponse(int timeout) {
  // start time
  unsigned long startTime = millis();
  // empty response string
  String response = "";
  // wait for timeout or data
  while (millis() - startTime < timeout) {
    if (sim808.available()) {  // check if data is available
      char c = sim808.read();  // read a character
      response += c;           // append to response string
    }
  }
  return response;  // return response string
}
String waitForexpectedResponse(String expectedResponse, unsigned long timeout) {
  // Get start time
  unsigned long startTime = millis();
  // Initialize response string
  String response = "";
  // While not timeout
  while (millis() - startTime < timeout) {
    // If SIM808 is available
    if (sim808.available()) {
      // Read character
      char c = sim808.read();
      // Add character to response string
      response += c;
      // If expected response found in response string
      if (response.indexOf(expectedResponse) != -1) {
        // Return response string
        return response;
      }
    }
  }
}
bool waitForexpectedResponsebool(String expectedResponse, unsigned long timeout) {
  // Record start time
  unsigned long startTime = millis();
  // Initialize response string
  String response = "";
  // Continue loop until timeout reached
  while (millis() - startTime < timeout) {
    // Check if sim808 is available for reading
    if (sim808.available()) {
      // Read a character from sim808
      char c = sim808.read();
      // Append character to response string
      response += c;
      // Check if expected response is found in response string
      if (response.indexOf(expectedResponse) != -1) {
        // Return true if found
        return true;
      }
    }
  }
  // Return false if timeout reached without finding expected response
  return false;
}
String getValue(String data, char separator, int index) {
  if (!isSIM808On()) {
    int found = 0;                     // Initial index found
    int strIndex[] = { 0, -1 };        // String indices
    int maxIndex = data.length() - 1;  // Maximum string index
    for (int i = 0; i <= maxIndex && found <= index; i++) {
      if (data.charAt(i) == separator || i == maxIndex) {
        found++;                                    // Increment found index
        strIndex[0] = strIndex[1] + 1;              // Start of substring
        strIndex[1] = (i == maxIndex) ? i + 1 : i;  // End of substring
      }
    }
    if (found > index) {  // Check if found index exists
      String extractedText = data.substring(strIndex[0], strIndex[1]);
      return (extractedText.length() == 0) ? "-" : extractedText;  // Return text or '-'
    } else {
      return "-";  // Return '-' if found index does not exist
    }
  }
}
int readHTTPResponse() {
  // Warte auf das Muster "+HTTPREAD:"
  if (sim808.find("+HTTPREAD:")) {
    // Muster gefunden, lese die Antwortlänge
    int responseLength = sim808.parseInt();
    // Lesen Sie die Antwortdaten
    sim808.find('\n'); // Überspringe die Zeile mit der Antwortlänge
    String responseData = sim808.readStringUntil('\n'); // Lese bis zum nächsten Zeilenumbruch
    // Extrahiere den Wert von <data>
    int dataValue = responseData.toInt();
    return dataValue;
  }
}