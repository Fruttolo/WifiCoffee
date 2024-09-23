#include <WiFi.h>
#include <ArduinoWebsockets.h>

char* ssid = "SSID";
char* password = "PASSWORD";
const char* serverAddress = "192.168.1.137"; 
const int serverPort = 8080;   
const char* COFFEE_MACHINE_ID = "COFFEE_MACHINE_ID";
char* COFFEE_MACHINE_TOKEN = "COFFEE_MACHINE_TOKEN";  

websockets::WebsocketsClient webSocket;
unsigned long lastPingTime = 0;

void onMessageCallback(websockets::WebsocketsMessage message) {
    Serial.printf("Ricevuto messaggio: %s\n", message.data().c_str());
    if (message.data() == "MAN_ON") {
        Serial.println("MAN_ON");
        digitalWrite(12, HIGH);
    } else if (message.data() == "MAN_OFF") {
        Serial.println("MAN_OFF");
        digitalWrite(12, LOW);
    } else if (message.data() == "BLU_ON") {
        Serial.println("BLU_ON");
        digitalWrite(13, HIGH);
    } else if (message.data() == "BLU_OFF") {
        Serial.println("BLU_OFF");
        digitalWrite(13, LOW);
    } else if (message.data() == "ACC_ON") {
        Serial.println("ACC_ON");
        digitalWrite(33, HIGH);
    } else if (message.data() == "ACC_OFF") {
        Serial.println("ACC_OFF");
        digitalWrite(33, LOW);
    } else if (message.data().startsWith("Associate")) {
        char token[50];
        sscanf(message.data().c_str(), "Associate %s", token);
        COFFEE_MACHINE_TOKEN = token;
        Serial.printf("ID e TOKEN associati a: %s %s\n", COFFEE_MACHINE_ID, COFFEE_MACHINE_TOKEN);
        webSocket.send("{ \"ID\": \"" + String(COFFEE_MACHINE_ID) + "\", \"TOKEN\": \"" + String(COFFEE_MACHINE_TOKEN) + "\" }");
    } else if (message.data() == "Ping") {
        Serial.println("Ping ricevuto");
    } else if (message.data() == "Pong") {
        Serial.println("Pong ricevuto");
    }
}

void onEventsCallback(websockets::WebsocketsEvent event, String data) {
    if (event == websockets::WebsocketsEvent::ConnectionOpened) {
        Serial.println("Connesso al WebSocket server");
        webSocket.send("{ \"ID\": \"" + String(COFFEE_MACHINE_ID) + "\", \"TOKEN\": \"" + String(COFFEE_MACHINE_TOKEN) + "\" }");
    } else if (event == websockets::WebsocketsEvent::ConnectionClosed) {
        Serial.println("Disconnesso dal WebSocket server");
        // Try to reconnect to the WebSocket server
        while (!webSocket.available()) {
            Serial.println("Tentativo di riconnessione al WebSocket server...");
            webSocket.connect(serverAddress, serverPort, "/");
            delay(5000); // Wait 5 seconds before trying again
        }
        Serial.println("Riconnesso al WebSocket server");
    } else if (event == websockets::WebsocketsEvent::GotPing) {
        Serial.println("Ping ricevuto");
    } else if (event == websockets::WebsocketsEvent::GotPong) {
        Serial.println("Pong ricevuto");
    }
}

void setup() {
  Serial.begin(115200);
  
  pinMode(12, OUTPUT); // MAN
  pinMode(13, OUTPUT); // BLU
  pinMode(33, OUTPUT); // ACC
  
  // Connetti alla rete WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connessione al WiFi in corso...");
  }
  
  Serial.println("Connesso al WiFi");
  Serial.println(WiFi.localIP());

  // Configura WebSocket
  webSocket.onMessage(onMessageCallback);
  webSocket.onEvent(onEventsCallback);
  webSocket.connect(serverAddress, serverPort, "/");

  // Try to connect to the WebSocket server
  while (!webSocket.available()) {
    Serial.println("Tentativo di connessione al WebSocket server...");
    delay(5000); // Wait 5 seconds before trying again
  }
  Serial.println("Connesso al WebSocket server");
}

void loop() {
  webSocket.poll();
  
  if (millis() - lastPingTime >= 30000) {
    webSocket.send("Ping");
    lastPingTime = millis(); // Update the last ping time
  }
}