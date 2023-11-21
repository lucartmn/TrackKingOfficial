#include <SoftwareSerial.h>

const int sim808RxPin = 2;
const int sim808TxPin = 3;
const int SIM808_POWER_PIN = 4;

SoftwareSerial sim808(sim808RxPin, sim808TxPin);
String apikey = "key=5zWq43!#420";
const unsigned long smsCheckInterval = 1;
unsigned long lastSmsCheckTime = 0;
bool livetracking = false;
bool gprsConfigured = false;
bool simOn = false;
int runs;
int runssms;
int runssearch;
bool lowEnergyMode = false;
String lastReceivedSMS;

bool newCommandReceived = false;  // Flag, um zu überprüfen, ob ein neuer Befehl empfangen wurde

unsigned long lowEnergyModeStartTime = 0;
unsigned long lowEnergyModeDuration = 8 * 1000;  // Low Energy Mode dauert 15 Sekunden

void setup() {
  Serial.begin(9600);
  sim808.begin(9600);
  delay(500);

  if (isSIM808On()) {
    delay(1000);
    setupGPRS();
    runs = 1;
  }

  if (!isSIM808On()) {
    Serial.println("Activate SIM808");
    delay(800);
    startSIM808();
    delay(800);
    Serial.begin(9600);
    sim808.begin(9600);
    delay(1000);
    setupGPRS();
    runs = 1;
  }
}

void loop() {
  //checkSMSReceived();
  lowEnergyModeLoop();

  if (livetracking) {
    finding();
    startGPSLiveTracking();
    delay(100);
  }
}
void setupGPRS() {
  if (!gprsConfigured) {
    // Aktiviere das GPRS-Modul
    sim808.println("AT+CGATT=1");
    if (waitForResponse("OK", 1000)) {
      // Setze den PDP-Kontext
      sim808.println("AT+SAPBR=1,1");
      // Keine Überprüfung auf "OK" für diesen Befehl, da "ERROR" akzeptabel ist
      // Setze den APN
      sim808.println("AT+SAPBR=3,1,\"APN\",\"pinternet.interkom.de\"");
      if (waitForResponse("OK", 1000)) {
        // Setze den Verbindungstyp auf GPRS
        sim808.println("AT+SAPBR=3,1,\"CONTYPE\",\"GPRS\"");
        if (waitForResponse("OK", 1000)) {
          gprsConfigured = true;
          Serial.println("Setup done..");
        }
      }
    }
  }
}

void startSIM808() {
  // Den SIM808-Power-Pin als Ausgang festlegen
  pinMode(SIM808_POWER_PIN, OUTPUT);

  digitalWrite(SIM808_POWER_PIN, HIGH);
  delay(2000);  // Eine Sekunde warten
  digitalWrite(SIM808_POWER_PIN, LOW);
  delay(1500);  // Eine Sekunde warten
  if (isSIM808On()) {
    simOn = true;
    Serial.println("SIM808 ready");
    delay(2000);
  }
}

void stopSIM808() {
  // Den SIM808-Power-Pin als Ausgang festlegen
  if (isSIM808On()) {
    pinMode(SIM808_POWER_PIN, OUTPUT);
    digitalWrite(SIM808_POWER_PIN, HIGH);
    delay(2000);  // Eine Sekunde warten
    digitalWrite(SIM808_POWER_PIN, LOW);
    delay(1500);  // Eine Sekunde warten
    simOn = false;
    Serial.println("SIM808 deactivated");
    delay(2000);
  }
}

bool isSIM808On() {
  sim808.println("AT");
  return waitForResponse2("OK", 1000);  // Warte auf "OK" als Antwort innerhalb von 1 Sekunde
}

void finding() {
  sim808.println("AT+CGNSPWR?");
  String response = waitForResponse("+CGNSPWR: 1", 800);
  if (!isSIM808On()) {
    Serial.println("Activate SIM808");
    delay(800);
    startSIM808();
    delay(800);
  }

  if (response.indexOf("+CGNSPWR: 0") == -1) {
    sim808.println("AT+CGNSPWR=1");
    delay(300);
    Serial.println("Initialise GPS");
  }
}

void clearSimMemory() {
  sim808.println("AT+CMGD=1,4");
}

void checkSMSReceived() {
  checker();
  runssms++;
  Serial.print("...    ");
  Serial.print(runssms);
  Serial.println();
  sim808.println("AT+CMGF=1");
  delay(300);
  sim808.println("AT+CMGL=\"REC UNREAD\"");
  String response = waitForResponse1(2000);

  // Überprüfen, ob eine SMS empfangen wurde
  if (response.indexOf("Off") != -1) {
    Serial.println("GPS deactivated");
    livetracking = false;
    gprsConfigured = false;
    lowEnergyMode = false;
    lastReceivedSMS = "Off";
    runs = 1;
    sim808.println("AT+CGNSPWR=0");
    delay(300);
    clearSimMemory();           // Löscht die empfangene SMS
    newCommandReceived = true;  // Neuer Befehl empfangen
    return;
  }
  if (response.indexOf("On") != -1) {
    Serial.println("GPS activated");
    lowEnergyMode = false;
    livetracking = true;
    lastReceivedSMS = "On";
    
    delay(2000);
    clearSimMemory();           // Löscht die empfangene SMS
    newCommandReceived = true;  // Neuer Befehl empfangen
    return;
  }
  if (response.indexOf("Reset") != -1) {
    Serial.println("Reset");
    livetracking = false;
    lastReceivedSMS = "Reset";
    gprsConfigured = false;
    runssms = 0;
    runssearch = 0;
    simOn = false;
    lowEnergyMode = false;
    runs = 1;
    delay(1000);
    setup();
    delay(1000);
    clearSimMemory();           // Löscht die empfangene SMS
    newCommandReceived = true;  // Neuer Befehl empfangen
    return;
  }
  if (response.indexOf("Low") != -1) {
    int counter = 0;
    Serial.println("Low energy mode activated");
    livetracking = false;
    lastReceivedSMS = "Low";
    gprsConfigured = false;
    simOn = false;
    runs = 1;
    delay(300);
    clearSimMemory();
    delay(200);  // Löscht die empfangene SMS
    lowEnergyMode = true;
    newCommandReceived = true;  // Neuer Befehl empfangen
    return;
  }
  if (response.indexOf("Status") != -1) {
    Serial.println("Status: ");
    lastReceivedSMS = "Status";
    Serial.println("livetracking: " + livetracking);
    delay(100);
    Serial.println("lastReceivedSMS: " + lastReceivedSMS);
    delay(100);
    Serial.println("gprsConfigured: " + gprsConfigured);
    delay(100);
    Serial.println("simOn: " + simOn);
    delay(100);
    Serial.println("runs: " + runs);
    delay(100);
    Serial.println("lowEnergyMode: " + lowEnergyMode);
    delay(100);
    Serial.println("newCommandReceived: " + newCommandReceived);
    delay(100);
    sim808.println("AT+CGNSPWR?");
    String response = waitForResponse1(1000);
    Serial.println(response);
    delay(300);
    clearSimMemory();
    delay(200);  // Löscht die empfangene SMS
    return;
  }
}

void checker() {

  if (!isSIM808On()) {
    Serial.println("Activate SIM808");
    delay(800);
    startSIM808();
    delay(800);
    Serial.begin(9600);
    sim808.begin(9600);
    delay(1000);
    setupGPRS();
    runs = 1;
  }
}

void startGPSLiveTracking() {
  runssearch++;
  Serial.print("Searching GPS..         ");
  Serial.print(runssearch);
  Serial.println();

  String gpsData = get_gps();
  // Überprüfen, ob die GPS-Daten gültig sind (z.B., ob Latitude und Longitude vorhanden und gültig sind)
  if (isValidGPSData(gpsData)) {
    update_serverdata(gpsData);
    runs++;
  }
}

bool isValidGPSData(String gpsData) {
  // Suchen Sie nach "latitude=" und "longitude=" im String
  int latitudeStart = gpsData.indexOf("latitude=");
  int longitudeStart = gpsData.indexOf("longitude=");

  // Wenn "latitude=" und "longitude=" gefunden wurden
  if (latitudeStart != -1 && longitudeStart != -1) {
    // Extrahieren Sie die Werte für Breitengrad und Längengrad
    String latitudeStr = gpsData.substring(latitudeStart + 9, gpsData.indexOf("&", latitudeStart));
    String longitudeStr = gpsData.substring(longitudeStart + 10, gpsData.indexOf("&", longitudeStart));

    // Konvertieren Sie die extrahierten Werte in Floats
    float latitude = latitudeStr.toFloat();
    float longitude = longitudeStr.toFloat();

    // Überprüfen, ob sowohl Breitengrad als auch Längengrad ungleich 0 sind
    if (latitude != 0.0 && longitude != 0.0) {
      return true;  // Gültige GPS-Daten vorhanden
    }
  }

  return false;  // GPS-Daten ungültig oder nicht verfügbar
}

String get_gps() {
  // Überprüfen Sie, ob die GPRS-Verbindung erfolgreich ist, bevor Sie GPS-Daten abrufen
  sim808.println("AT+CGNSINF");
  //Antwort aufnehmen
  String response = waitForResponse1(100);
  // Überprüfen, ob die Antwort "+CGNSINF:" enthält
  if (response.indexOf("+CGNSINF:") != -1) {
    // Teilen Sie die GPS-Daten in einzelne Werte auf, getrennt durch Kommas
    String gpsData = response.substring(response.indexOf("+CGNSINF:") + 10);
    String latitude = getValue(gpsData, ',', 3);
    String longitude = getValue(gpsData, ',', 4);
    String altitude = getValue(gpsData, ',', 5);
    String speed = getValue(gpsData, ',', 6);

    // Erstelle die POST-Daten
    String postData = apikey
                      + "&latitude=" + latitude
                      + "&longitude=" + longitude
                      + "&altitude=" + altitude
                      + "&speed=" + speed;
    Serial.println(postData);
    return postData;
  } else {
    return "GPS Error";
    Serial.println("GPS-Data not available.");
  }
}

void update_serverdata(String postData) {
  const char* postDataChar = postData.c_str();  // Wandele den String in einen const char* um
  sim808.println("AT+HTTPINIT");
  delay(500);
  sim808.println("AT+HTTPPARA=\"CID\",1");
  delay(500);
  sim808.println("AT+HTTPPARA=\"URL\",\"http://lets-stream.eu/get_gps_data.php\"");
  delay(1000);
  sim808.println("AT+HTTPPARA=\"CONTENT\",\"application/x-www-form-urlencoded\"");
  delay(500);
  // Sende POST-Daten und führe die Aktion aus
  sim808.println(("AT+HTTPDATA=" + String(postData.length()) + ",5000").c_str());
  delay(500);
  sim808.println(postDataChar);  // Verwende den umgewandelten const char* statt des String-Objekts
  delay(300);
  Serial.print("Updating Server...             ");
  Serial.print(runs);
  Serial.println();

  sim808.println("AT+HTTPACTION=1");
  delay(500);
  // Beende die HTTP-Sitzung
  sim808.println("AT+HTTPTERM");
  delay(300);
}

String waitForResponse1(int timeout) {
  unsigned long startTime = millis();
  String response = "";

  while (millis() - startTime < timeout) {
    if (sim808.available()) {
      char c = sim808.read();
      response += c;
    }
  }

  return response;
}

String waitForResponse(String expectedResponse, unsigned long timeout) {
  unsigned long startTime = millis();
  String response = "";

  while (millis() - startTime < timeout) {
    if (sim808.available()) {
      char c = sim808.read();
      response += c;
      if (response.indexOf(expectedResponse) != -1) {
        return response;
      }
    }
  }
  return "Unerwartete Antwort";
}

bool waitForResponse2(String expectedResponse, unsigned long timeout) {
  unsigned long startTime = millis();
  String response = "";

  while (millis() - startTime < timeout) {
    if (sim808.available()) {
      char c = sim808.read();
      response += c;
      if (response.indexOf(expectedResponse) != -1) {
        return true;
      }
    }
  }
  return false;
}

String getValue(String data, char separator, int index) {
  int found = 0;
  int strIndex[] = { 0, -1 };
  int maxIndex = data.length() - 1;

  for (int i = 0; i <= maxIndex && found <= index; i++) {
    if (data.charAt(i) == separator || i == maxIndex) {
      found++;
      strIndex[0] = strIndex[1] + 1;
      strIndex[1] = (i == maxIndex) ? i + 1 : i;
    }
  }

  if (found > index) {
    String extractedText = data.substring(strIndex[0], strIndex[1]);
    return (extractedText.length() == 0) ? "-" : extractedText;
  } else {
    return "-";
  }
}

void enterLowEnergyMode() {
  stopSIM808();  // SIM808-Modul deaktivieren
  lowEnergyModeStartTime = millis();
}

void exitLowEnergyMode() {
  startSIM808();  // SIM808-Modul aktivieren
  delay(500);
  setup();  // Setup-Funktion aufrufen, um die Konfiguration neu zu initialisieren
}

void lowEnergyModeLoop() {
  if (!lowEnergyMode) {
    checkSMSReceived();
  }

  if (lowEnergyMode && !livetracking) {
    unsigned long currentTime = millis();

    // Überprüfen, ob der Low Energy Mode abgelaufen ist
    if (currentTime - lowEnergyModeStartTime >= lowEnergyModeDuration) {
      exitLowEnergyMode();
      lowEnergyMode = false;
      delay(1000);
      checkSMSReceived();
      delay(1000);
      checkSMSReceived();
      delay(1000);
      checkSMSReceived();
      delay(1000);
      enterLowEnergyMode();

      lowEnergyMode = true;
      lowEnergyModeStartTime = millis();
    }
  }
}
