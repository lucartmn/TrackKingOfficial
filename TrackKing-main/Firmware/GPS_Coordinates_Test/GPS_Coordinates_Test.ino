#include <SoftwareSerial.h>

const int sim808RxPin = 2;
const int sim808TxPin = 3;

SoftwareSerial sim808(sim808RxPin, sim808TxPin);

void setup() {
  Serial.begin(9600);
  sim808.begin(9600);
  delay(1000);
}

void loop() {
  warmup();
  get_gps();
}

void warmup() {
  sendATCommand("AT+CGNSPWR=1");
  delay(2000);
}

void get_gps() {
  // Überprüfen Sie, ob die GPRS-Verbindung erfolgreich ist, bevor Sie GPS-Daten abrufen
  sim808.println("AT+CGNSINF");
  //Antwort aufnehmen
  String response = waitForResponse(5000);
  delay(2000);
  Serial.println("Response:" + response);
  delay(1000);


  // Überprüfen, ob die Antwort "+CGNSINF:" enthält
  if (response.indexOf("+CGNSINF:") != -1) {
    // Teilen Sie die GPS-Daten in einzelne Werte auf, getrennt durch Kommas
    String gpsData = response.substring(response.indexOf("+CGNSINF:") + 10);
    Serial.println(gpsData);

    // ..
    String latitude = getValue(gpsData, ',', 3);
    String longitude = getValue(gpsData, ',', 4);
    String altitude = getValue(gpsData, ',', 5);
    String speed = getValue(gpsData, ',', 6);
    String course = getValue(gpsData, ',', 7);
    String fixStatus = getValue(gpsData, ',', 8);
    String fixMode = getValue(gpsData, ',', 9);
    String hdop = getValue(gpsData, ',', 10);
    String pdop = getValue(gpsData, ',', 11);
    String vdop = getValue(gpsData, ',', 12);
    String satellitesInView = getValue(gpsData, ',', 13);
    String satellitesInUse = getValue(gpsData, ',', 14);
    String gnssSvUsed = getValue(gpsData, ',', 15);
    String cn0Max = getValue(gpsData, ',', 16);
    String hpa = getValue(gpsData, ',', 17);
    String vpa = getValue(gpsData, ',', 18);

    // Ausgabe der Koordinaten und weiterer Informationen
    Serial.println("Latitude:" + latitude);
    Serial.println("Longitude:" + longitude);
    Serial.println("Altitude:" + altitude);
    Serial.println("Speed:" + speed);
    Serial.println("Course:" + course);
    Serial.println("Fix Status:" + fixStatus);
    Serial.println("Fix Mode:" + fixMode);
    Serial.println("HDOP:" + hdop);
    Serial.println("PDOP:" + pdop);
    Serial.println("VDOP:" + vdop);
    Serial.println("Satellites in View:" + satellitesInView);
    Serial.println("Satellites in Use:" + satellitesInUse);
    Serial.println("GNSS SV Used:" + gnssSvUsed);
    Serial.println("C/N0 Max:" + cn0Max);
    Serial.println("HPA:" + hpa);
    Serial.println("VPA:" + vpa);
    
    // Erstelle die POST-Daten
    String postData = "key=5zWq43!#420&latitude=" + latitude + "&longitude=" + longitude + "&altitude=" + altitude + "&speed=" + speed;
    Serial.println("GPS Data:" + postData);
  } else {
    Serial.println("GPS-Daten nicht verfügbar.");
  }

  delay(8000);
}

void sendATCommand(const char* command) {
  sim808.println(command);
  delay(1500);  // Warte 1 Sekunde für die Antwort
  while (sim808.available()) {
    Serial.write(sim808.read());
  }
}


String waitForResponse(int timeout) {
  unsigned long startTime = millis();
  String response = "";

  while (millis() - startTime < timeout) {
    if (sim808.available()) {
      char c = sim808.read();
      response += c;
    }
  }  // Überprüfen, ob "OK" oder "+CGNSINF" in der Antwort gefunden wurde
  if (response.indexOf("OK") != -1 || response.indexOf("+CGNSINF") != -1) {
    return response;
  }
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

