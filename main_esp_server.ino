#include <WiFi.h>
#include <WebServer.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <SPI.h>
#include <MFRC522.h>
#include <EEPROM.h>
// ----- OLED Config -----
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

#define OLED_SDA 25   // your custom SDA pin
#define OLED_SCL 26  // your custom SCL pin

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// ----- RFID Config -----
#define RST_PIN  21
#define SS_PIN   5
MFRC522 rfid(SS_PIN, RST_PIN);

// ==== WiFi Credentials ====
const char* ssid     = "Yashasvi";
const char* password = "Yashasvi@hotspot21";

String currentPswd="admin";
boolean isAuthenticated=false;
// ==== Server Setup ====
WebServer server(80);

// ==== LED Pins ====
const int ledPins[4] = {-1, 5, 18, 19};  // index 0 unused, LEDs on pins 5, 18, 19
int ledStates[4]     = {0, 0, 0, 0};    // track logical state (LOW=0, HIGH=1)

// ==== HTML Page ====

String CtrlPage = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>Rooms Page</title>
  <style>
    body {
      margin: 0;
      padding: 0;
      font-family: Arial, sans-serif;
      display: flex;
      justify-content: center;
      align-items: center;
      height: 100vh;
      background: radial-gradient(50% 50% at 50% 50%, #ADD9F4 0%, #020202 100%);
    }

    .container {
      text-align: center;
    }

    .big-button {
      display: block;
      width: 200px;
      padding: 15px;
      margin: 15px auto;
      font-size: 18px;
      border: none;
      border-radius: 8px;
      cursor: pointer;
      color: #fff;
      background-color: #000000;
      transition: background 0.3s ease;
    }

    .big-button:hover {
      background-color: #626262;
    }

    .small-button {
      padding: 8px 15px;
      font-size: 14px;
      border: none;
      border-radius: 6px;
      cursor: pointer;
      background-color: #ffffff;
      color: #000000;
      transition: background 0.3s ease;
      margin-top: 20px;
    }

    .small-button:hover {
      background-color: #b7b7b7;
    }
  </style>
</head>
<body>
  <div class="container">
    <button onclick="window.location.href='http://10.184.22.236'" class="big-button">Room 1</button>
    <button class="big-button">Room 2</button>
  </div>
</body>
</html>

         )rawliteral";

// ==== Handle Root ====
void handleRoot() {
    server.send(200, "text/html", CtrlPage);
}
byte allowedUID[4] = {0x23, 0xD4, 0x88, 0x14};

bool isAuthorized(byte *uid) {
  for (int i = 0; i < 4; i++) {
    if (uid[i] != allowedUID[i]) return false;
  }
  return true;
}



// ==== Setup ====
void setup() {
  Serial.begin(115200);

  // Connect WiFi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected to WiFi!");
  Serial.print("ESP32 IP Address: ");
  Serial.println(WiFi.localIP());

  // Setup server routes
  server.on("/", handleRoot);
  server.begin();
  Serial.println("HTTP server started");

  Wire.begin(OLED_SDA, OLED_SCL);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);
  }
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(10, 25);
  display.println("Scan Card");
  display.display();

  SPI.begin();
  rfid.PCD_Init();
  Serial.println("Place your RFID card...");
}

void showMessage(String line1, String line2) {
  display.clearDisplay();
  display.setTextSize(2);
  display.setCursor(10, 10);
  display.println(line1);
  display.setCursor(10, 35);
  display.println(line2);
  display.display();
}

// ==== Loop ====
void loop() {
  server.handleClient();
  if (!rfid.PICC_IsNewCardPresent() || !rfid.PICC_ReadCardSerial()) {
    return;
  }

  Serial.print("UID tag :");
  for (byte i = 0; i < rfid.uid.size; i++) {
    Serial.print(rfid.uid.uidByte[i] < 0x10 ? " 0" : " ");
    Serial.print(rfid.uid.uidByte[i], HEX);
  }
  Serial.println();

  if (isAuthorized(rfid.uid.uidByte)) {
    Serial.println("Access Granted");
    showMessage("Access", "Granted");
  } else {
    Serial.println("Access Denied");
    showMessage("Access", "Denied");
  }

  delay(2000);
  display.clearDisplay();
  display.setTextSize(2);
  display.setCursor(10, 25);
  display.println("Scan Card");
  display.display();

  rfid.PICC_HaltA();
}
