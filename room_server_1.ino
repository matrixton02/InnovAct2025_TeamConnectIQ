#include <WiFi.h>
#include <WebServer.h>
#include <led_js.h>
// ==== WiFi Credentials ====
const char* ssid     = "Yashasvi";
const char* password = "Yashasvi@hotspot21";

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
    <!-- Tailwind CSS for styling -->
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
            background: radial-gradient(50% 50% at 50% 50%, #ADD9F4 0%, #020202 100%);
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
            color: #c1c1c1;
            -webkit-text-stroke-width: 0.1px; 
            -webkit-text-stroke-color: #000000; 
            font-family: 'Raleway', sans-serif;
            font-size: 88px;
            font-style: normal;
            font-weight: 400;
            line-height: 129px;
        }

        .lighting, .living {
            background: radial-gradient(50% 50% at 50% 50%, #c1c1c1 25%, #ffffff 60%);
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
            font-weight: 600;
            color: #ffffff; 
            flex-grow: 1; 
            text-align: left;
        }
        .toggle-btn {
            padding: 0.75rem 1.5rem;
            border-radius: 9999px; /* Pill-shaped buttons */
            font-weight: 700;
            cursor: pointer;
            transition: all 0.2s ease-in-out;
            background-color: #343434; /* Teal color for ON */
            color: rgb(0, 0, 0);
            border: none;
            box-shadow: 0 4px 6px rgba(0, 0, 0, 0.1);
        }
        .toggle-btn.on {
            background-color: #38b2ac; /* Teal */
            box-shadow: 0 4px 6px rgba(56, 178, 172, 0.3);
        }
        .toggle-btn.off {
            background-color: #c2c2c2; 
            box-shadow: 0 4px 6px rgba(255, 255, 255, 0.3);
        }
        .toggle-btn:hover {
            transform: translateY(-2px); /* Slight lift on hover */
            box-shadow: 0 6px 10px rgba(0, 0, 0, 0.15);
        }
        .toggle-btn:active {
            transform: translateY(0); /* Press effect */
            box-shadow: 0 2px 4px rgba(0, 0, 0, 0.1);
        }

        /* LED Indicator styles */
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
                <button id="toggleBtn1" class="toggle-btn ml-4 off">Toggle</button>
            </div>
        </div>

        <div class="led-control-group">
            <span class="led-label">LED 2</span>
            <div class="flex items-center">
                <div id="indicator2" class="led-indicator"></div>
                <button id="toggleBtn2" class="toggle-btn ml-4 off">Toggle</button>
            </div>
        </div>

        <div class="led-control-group">
            <span class="led-label">LED 3</span>
            <div class="flex items-center">
                <div id="indicator3" class="led-indicator"></div>
                <button id="toggleBtn3" class="toggle-btn ml-4 off">Toggle</button>
            </div>
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
  server.begin();
  Serial.println("HTTP server started");
}

// ==== Loop ====
void loop() {
  server.handleClient();
}
