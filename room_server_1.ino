#include <WiFi.h>
#include <WebServer.h>
#include <led_js.h>
#include "DHT.h"
#define DHTPIN 2
#define DHTTYPE DHT11
#define LIGHT_SENSOR_PIN 36
#define PIR_PIN 33
DHT dht(DHTPIN,DHTTYPE);
// ==== WiFi Credentials ====
const char* ssid     = "Yashasvi";
const char* password = "Yashasvi@hotspot21";
float temp;
float hum;
unsigned long previousMillis = 0;
const long interval = 3000;
int lastMotion=LOW;
int ledState=LOW;
// ==== Server Setup ====
WebServer server(80);

// ==== LED Pins ====
const int ledPins[4] = {-1, 5, 18, 19};  // index 0 unused, LEDs on pins 5, 18, 19
int ledStates[4]     = {0, 0, 0, 0};    // track logical state (LOW=0, HIGH=1)

// ==== HTML Page ====
const char htmlPage[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>ESP32 LED Control</title>
    <script src="https://cdn.tailwindcss.com"></script>
    <link href="https://fonts.googleapis.com/css2?family=Raleway:ital,wght@0,100..900;1,100..900&display=swap" rel="stylesheet">
    <link href="https://fonts.googleapis.com/css2?family=Orbitron:wght@400;700&display=swap" rel="stylesheet">
    <style>
        body {
            margin: 0;
            padding: 0;
            min-height: 100vh;
            display: flex;
            justify-content: center;
            align-items: center;
            background: linear-gradient(180deg, #EDF2F4 0%, #8D99AE 25%, #2B2D42 70%, #020202 100%);
        }
        .smart-banner {
            text-align: left;
            color: #e0e0e0;
        }
        .headline, .subheadline {
            line-height: 1.05;
            letter-spacing: 0.01em;
        }
    
        .smart, .effortless {
            color: #000000;
            -webkit-text-stroke-width: 0.1px; 
            -webkit-text-stroke-color: #ffffff; 
            font-family: 'Raleway', sans-serif;
            font-size: 88px;
            font-style: normal;
            font-weight: 400;
            line-height: 129px;
        }

        .lighting, .living {
            background: radial-gradient(50% 50% at 50% 50%, #444444 12.02%, #000000 100%);
            background-clip: text;
            -webkit-text-stroke-width: 0.5px; 
            -webkit-text-stroke-color: #36454F; 
            -webkit-background-clip: text;
            -webkit-text-fill-color: transparent;
            font-family: 'Orbitron', sans-serif;
            font-size: 96px;
            font-style: normal;
            font-weight: 500;
            line-height: 129px;
        }

        .led-control-group {
            display: flex;
            align-items: center;
            justify-content: space-between;
            margin-bottom: 1.5rem;
            border-radius: 1rem;
            padding: 1rem 1.5rem;
        }
        .led-label {
            font-size: 1.25rem;
            font-weight: 1200;
            color: #ffffff; 
            flex-grow: 1; 
            text-align: left;
        }
        .toggle-btn {
            padding: 0.75rem 1.5rem;
            border-radius: 9999px; 
            font-weight: 700;
            cursor: pointer;
            transition: all 0.2s ease-in-out;
            background-color: #343434; 
            color: rgb(0, 0, 0);
            border: none;
            box-shadow: 0 4px 6px rgba(0, 0, 0, 0.1);
        }
        .toggle-btn.on {
            background-color: #EF233C; 
            box-shadow: 0 4px 6px #D90429;
        }
        .toggle-btn.off {
            background-color: #c2c2c2; 
            box-shadow: 0 4px 6px rgba(255, 255, 255, 0.3);
        }
        .toggle-btn:hover {
            transform: translateY(-2px); 
            box-shadow: 0 6px 10px rgba(0, 0, 0, 0.15);
        }
        .toggle-btn:active {
            transform: translateY(0); 
            box-shadow: 0 2px 4px rgba(0, 0, 0, 0.1);
        }

        .led-indicator {
            width: 20px;
            height: 20px;
            border-radius: 50%;
            background-color: #e2e8f0;
            margin-left: 1rem;
            transition: background-color 0.3s ease-in-out;
            border: 2px solid #cbd5e0; 
        }
        .led-indicator.on {
            background-color: #ffffff; 
            box-shadow: 0 0 10px #e3e3e3, 0 0 20px #dee2e1; 
        }
        .status-message {
            margin-top: 2rem;
            color: #718096;
            font-size: 0.9rem;
        }

        .temp-hum-container {
            margin-top: 1rem;
            color: white;
            min-height: 100px;
            border: 1px solid #ccc;
            padding: 1rem;
            background-color: transparent; /* transparent background */
            border-radius: 0.5rem;
    
            display: flex;
            flex-direction: column; /* vertical stack */
            justify-content: center; /* center vertically */
            align-items: center; /* center horizontally */
            text-align: center;
            gap: 0.5rem; /* space between the two lines */
        }

    </style>
</head>
<body>
    <div class="smart-banner">
        <div class="headline">
            <span class="smart">Smart </span><span class="lighting">lighting,</span>
        </div>
        <div class="subheadline">
            <span class="effortless">Effortless </span><span class="living">living.</span>
        </div>

        <div class="led-control-group">
            <span class="led-label">LED 1</span>
            <div class="flex items-center">
                <div id="indicator1" class="led-indicator"></div>
                <button id="toggleBtn1" class="toggle-btn ml-4">Toggle</button>
            </div>
        </div>

        <div class="led-control-group">
            <span class="led-label">LED 2</span>
            <div class="flex items-center">
                <div id="indicator2" class="led-indicator"></div>
                <button id="toggleBtn2" class="toggle-btn ml-4">Toggle</button>
            </div>
        </div>

        <div class="led-control-group">
            <span class="led-label">LED 3</span>
            <div class="flex items-center">
                <div id="indicator3" class="led-indicator"></div>
                <button id="toggleBtn3" class="toggle-btn ml-4">Toggle</button>
            </div>
        </div>

        <div id="tempHumContainer" class="temp-hum-container" style="display: block;">
            <div id="temperatureLine">Temperature:  -- </div>
            <div id="humidityLine">Humidity: -- </div>
        </div>
    </div>
<script src="/led.js"></script>
</body>
</html>
)rawliteral";

// ==== Handle Root ====
void handleRoot() {
  server.send_P(200, "text/html", htmlPage);
}

// ==== Toggle LED ====
void handleToggleLED() {
  if (!server.hasArg("led") || !server.hasArg("state")) {
    server.send(400, "text/plain", "Missing parameters");
    return;
  }

  int ledNumber = server.arg("led").toInt();
  int requestedState = server.arg("state").toInt();

  if (ledNumber < 1 || ledNumber > 3) {
    server.send(400, "text/plain", "Invalid LED number");
    return;
  }

  // Write state & update tracking
  digitalWrite(ledPins[ledNumber], requestedState);
  ledStates[ledNumber] = requestedState;

  String response = "{\"led\":" + String(ledNumber) + ",\"state\":" + String(ledStates[ledNumber]) + "}";
  server.send(200, "application/json", response);
}

// ==== Get All States ====
void handleGetStates() {
  String response = "{";
  for (int i = 1; i <= 3; i++) {
    response += "\"led" + String(i) + "\":" + String(ledStates[i]);
    if (i < 3) response += ",";
  }
  response += "}";
  server.send(200, "application/json", response);
}

void handleTemp(){
  String response="{\"temp\":"+String(temp)+"}";
  server.send(200,"application/json",response);
}
void handleHum(){
  String response="{\"hum\":"+String(hum)+"}";
  server.send(200,"application/json",response);
}

// ==== Setup ====
void setup() {
  Serial.begin(115200);

  // Setup pins
  for (int i = 1; i <= 3; i++) {
    pinMode(ledPins[i], OUTPUT);
    digitalWrite(ledPins[i], LOW);
    ledStates[i] = 0;
  }

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
  server.on("/ToggleLED", handleToggleLED);
  server.on("/getStates", handleGetStates);
  server.on("/led.js", [](){              // Serve JS file
      server.send_P(200, "application/javascript", jsfile);
  });
  server.on("/gettemp",handleTemp);
  server.on("/gethum",handleHum);
  server.begin();
  Serial.println("HTTP server started");
  dht.begin();
  analogSetAttenuation(ADC_11db);
  pinMode(PIR_PIN, INPUT);
}

// ==== Loop ====
void loop() {
  server.handleClient();
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
      previousMillis = currentMillis;
      hum=dht.readHumidity();
      temp=dht.readTemperature();
      Serial.print("Humidity: ");
      Serial.print(hum);
      Serial.print(" %\t");
      Serial.print("Temperature: ");
      Serial.print(temp);
      Serial.println(" *C");
      int analogValue = analogRead(LIGHT_SENSOR_PIN);
      int motion = digitalRead(PIR_PIN);
      if (motion == HIGH && lastMotion == LOW) {
         if (ledState){
            if (analogValue < 40) {
                digitalWrite(ledPins[2],HIGH);
            }
         }
        else{
            digitalWrite(ledPins[2],LOW);

        }
      }
      lastMotion = motion; // update previous state
  }
}