#include <SoftwareSerial.h>
const int sim808RxPin = 2;
const int sim808TxPin = 3;
const int SIM808_POWER_PIN = 4;
SoftwareSerial sim808(sim808RxPin, sim808TxPin);
// API-Key
const String API_KEY = "key=5zWq43!#420";
void setup() {
  Serial.begin(9600);  // Start Serial communication
  sim808.begin(9600);  // Start SIM808 communication
  delay(1000);         // Give time for module to initialize
  //activateSIM808IfNeeded();
  configureGPRSifneeded();  // Configure GPRS for internet access
}
void loop() {
  send_trackmode_buttonStatusRequest();
}
void send_on_off_buttonStatusRequest() {
  const char* postDataChar = "button_name=on_off_button";  // HTTP POST data
  //delay(800);
  // HTTP-Anfrage vorbereiten
  sendATCommand(2000, "AT+HTTPINIT");
  //delay(1500);
  sendATCommand(2000, "AT+HTTPPARA=\"CID\",1");
  //delay(1500);
  sendATCommand(2000, "AT+HTTPPARA=\"URL\",\"http://lets-stream.eu/get_button_status.php\"");
  //delay(1500);
  // Set content type
  sendATCommand(2000, "AT+HTTPPARA=\"CONTENT\",\"application/x-www-form-urlencoded\"");
  delay(1000);
  // Set HTTP data length
  sim808.println("AT+HTTPDATA=" + String(strlen(postDataChar)) + ",5000");
  delay(1000);
  // Send HTTP data
  sim808.println(postDataChar);
  delay(400);
  // Sende GET-Anfrage und führe die Aktion aus
  sendATCommand(2000, "AT+HTTPACTION=1");
  delay(1500);
  sim808.println("AT+HTTPREAD");
  int response = readHTTPResponse();
  Serial.println(response);
  delay(1500);
  // Überprüfen, ob die Antwort "buttonStatus": "0" enthält
  // Handling response
  if (response == 0) {
    Serial.println("GPS deactivated");
    //liveTracking = false;
  }
  if (response == 1) {
    Serial.println("GPS activated");
    //liveTracking = true;
  }
  // Beende die HTTP-Sitzung
  sendATCommand(2000, "AT+HTTPTERM");
  delay(1500);
}
void send_trackmode_buttonStatusRequest() {
  const char* postDataChar = "button_name=on_off_button";  // HTTP POST data
  //delay(800);
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
    //trackmode = false;
  }
  if (response == 1) {
    Serial.println("GPS activated");
    //trackmode = true;
  }
  // Beende die HTTP-Sitzung
  sim808.println("AT+HTTPTERM");
  delay(1500);
}
bool waitForResponsebool(String expectedResponse, unsigned long timeout) {
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
    delay(10);  // Short delay to avoid excessive CPU usage
  }
  // Return false if timeout reached without finding expected response
  return false;
}
void sendATCommand(int x, const char* command) {
  sim808.println(command);
  delay(x);  // Warte 1 Sekunde(n) für die Antwort
  while (sim808.available()) {
    Serial.write(sim808.read());
  }
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
int readHTTPResponse() {
  // Warte auf das Muster "+HTTPREAD:"
  if (sim808.find("+HTTPREAD:")) {
    // Muster gefunden, lese die Antwortlänge
    int responseLength = sim808.parseInt();

    // Lesen Sie die Antwortdaten
    sim808.find('\n'); // Überspringe die Zeile mit der Antwortlänge
    String responseData = sim808.readStringUntil('\n'); // Lese bis zum nächsten Zeilenumbruch
    //Serial.print("Server Response: ");
    //Serial.println(responseData);

    // Extrahiere den Wert von <data>
    int dataValue = responseData.toInt();
    //Serial.print("Extracted Data Value: ");
    //Serial.println(dataValue);
    return dataValue;
  } 
}
void activateSim808() {
  if (!isSIM808On()) {
    // Configure SIM808 power pin as output
    pinMode(SIM808_POWER_PIN, OUTPUT);
    // Power on SIM808 module
    digitalWrite(SIM808_POWER_PIN, HIGH);
    delay(2000);
    // Power off SIM808 module
    digitalWrite(SIM808_POWER_PIN, LOW);
    delay(2000);
  }
}
bool isSIM808On() {
  sim808.println("AT");                            // Send "AT" command to the module
  return waitForexpectedResponsebool("OK", 4000);
  delay(2000);  // Wait for "OK" response from the module within 1 second
}
void configureGPRSifneeded() {
  // If GPRS is not yet configured
  //if (!isGPRSConnected()) {
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
        if (isGPRSConnected()) {
          Serial.println("GPRS Setup done..");
          delay(3000);
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
void activateSIM808IfNeeded() {
  if (!isSIM808On()) {
    //activateSim808();
    Serial.println("SIM808 was down. Is now active.");
    delay(6000);
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