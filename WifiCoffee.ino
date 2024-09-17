#include <WiFi.h>
#include <WebSocketsClient.h>

const char* ssid = "SSID";
const char* password = "PASSWORD";

bool accendi = false;
bool manopola = false;
int secondi_aspettare = 20;  

WebSocketsClient webSocket;
const char* serverAddress = "192.168.1.100"; 
const int serverPort = 8080;   

const char* COFFEE_MACHINE_ID = "COFFEE_MACHINE_ID";
const char* COFFEE_MACHINE_TOKEN = "COFFEE_MACHINE_TOKEN";

void webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {

    switch(type) {
        case WStype_DISCONNECTED:
            Serial.println("Disconnesso dal WebSocket server");
            // Try to reconnect to the WebSocket server
            while (!webSocket.isConnected()) {
              Serial.println("Tentativo di riconnessione al WebSocket server...");
              webSocket.begin(serverAddress, serverPort, "/");
              webSocket.loop();
              delay(5000); // Wait 5 seconds before trying again
            }
            Serial.println("Riconnesso al WebSocket server");
            break;
        case WStype_CONNECTED:
            Serial.println("Connesso al WebSocket server");
            webSocket.sendTXT("{ ID: \"" + String(COFFEE_MACHINE_ID) + "\", TOKEN: \"" + String(COFFEE_MACHINE_TOKEN) + "\" }");
            break;
        case WStype_TEXT:
            Serial.printf("Ricevuto messaggio: %s\n", payload);
            if (strcmp((const char*)payload, "Accendi") == 0 || strcmp((const char*)payload, "Spegni") == 0) {
                accendi = true;
                Serial.println("Accensione eseguita");
            } else if (strcmp((const char*)payload, "Manopola") == 0) {\
                manopola = true;
                Serial.println("Manopola girata");
            } else if (strncmp((const char*)payload, "SetSecondi", 10) == 0) {
                int new_seconds;
                sscanf((const char*)payload, "SetSecondi %d", &new_seconds);
                secondi_aspettare = new_seconds;
                Serial.printf("Secondi da aspettare impostati a: %d\n", secondi_aspettare);
            }
            break;
        case WStype_BIN:
            Serial.println("Ricevuto messaggio binario");
            webSocket.sendTXT("Messaggio binario ricevuto");
            break;
    }
}

void setup() {
  Serial.begin(115200);
  
  pinMode(12, OUTPUT); // man
  pinMode(13, OUTPUT); // blue
  pinMode(33, OUTPUT); // acc
  
  // Connetti alla rete WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connessione al WiFi in corso...");
  }
  
  Serial.println("Connesso al WiFi");
  Serial.println(WiFi.localIP());

  // Configura WebSocket
  webSocket.begin(serverAddress, serverPort, "/");
  webSocket.onEvent(webSocketEvent);

  // Try to connect to the WebSocket server
  while (!webSocket.isConnected()) {
    Serial.println("Tentativo di connessione al WebSocket server...");
    webSocket.loop();
    delay(5000); // Wait 5 seconds before trying again
  }
  Serial.println("Connesso al WebSocket server");
}

void loop() {
  webSocket.loop();
  
  if(accendi){
    digitalWrite(33, HIGH);
    delay(1000);    
    digitalWrite(33, LOW);
    accendi = false;
  }
  if(manopola){
    digitalWrite(13, HIGH);
    digitalWrite(12, HIGH);
    delay(1000);    
    digitalWrite(33, HIGH);
    delay(secondi_aspettare*1000);
    digitalWrite(33, LOW);
    delay(1000); 
    digitalWrite(13, LOW);
    digitalWrite(12, LOW);
    manopola = false;
  }
}

