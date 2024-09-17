#include <WiFi.h>
#include <ArduinoWebsockets.h>
#include <HTTPClient.h>

const char* ssid = "SSID";
const char* password = "PASSWORD";

bool accendi = false;
bool manopola = false;
int secondi_aspettare = 20;  

websockets::WebsocketsClient webSocket;
HTTPClient http;
const char* serverAddress = "192.168.1.137"; 
const int serverPort = 8080;   

const char* COFFEE_MACHINE_ID = "COFFEE_MACHINE_ID";
const char* COFFEE_MACHINE_TOKEN = "COFFEE_MACHINE_TOKEN";

void onMessageCallback(websockets::WebsocketsMessage message) {
    Serial.printf("Ricevuto messaggio: %s\n", message.data().c_str());
    if (message.data() == "Accendi" || message.data() == "Spegni") {
        accendi = true;
        Serial.println("Accensione eseguita");
    } else if (message.data() == "Manopola") {
        manopola = true;
        Serial.println("Manopola girata");
    } else if (message.data().startsWith("SetSecondi")) {
        int new_seconds;
        sscanf(message.data().c_str(), "SetSecondi %d", &new_seconds);
        secondi_aspettare = new_seconds;
        Serial.printf("Secondi da aspettare impostati a: %d\n", secondi_aspettare);
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

  // Effettua una richiesta GET al percorso /hello
  String url = String("http://") + serverAddress + ":" + String(serverPort) + "/hello";
  http.begin(url);
  int httpCode = http.GET();
  if (httpCode > 0) {
    String payload = http.getString();
    Serial.println("Risposta GET: " + payload);
  } else {
    Serial.println("Errore nella richiesta GET");
  }
  http.end();

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