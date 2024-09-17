#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <SPIFFS.h>

const char* ssid = "SSID";
const char* password = "PASSWORD";

AsyncWebServer server(80);

bool accendi = false;
bool manopola = false;

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

  // Route per accendere il dispositivo
  server.on("/accendi", HTTP_GET, [](AsyncWebServerRequest *request){
    Serial.println("Accensione");
    accendi = true;
    request->send(200, "text/plain", "Accensione eseguita");
  });

  // Route per fare il caffè
  server.on("/manopola", HTTP_GET, [](AsyncWebServerRequest *request){
    Serial.println("Giramento manopola");
    manopola = true;
    request->send(200, "text/plain", "Manopola girata");
  });
  
  // Route per fare il caffè
  server.on("/faicaffe", HTTP_GET, [](AsyncWebServerRequest *request){
    Serial.println("Preparazione caffè in corso");
    request->send(200, "text/plain", "Caffè preparato");
  });

  // Servire il file HTML
  //server.serveStatic("/", SPIFFS, "/index.html");

  // Avvia il server
  server.begin();
}

void loop() {
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
    delay(1000*20);
    digitalWrite(33, LOW);
    delay(1000); 
    digitalWrite(13, LOW);
    digitalWrite(12, LOW);
    manopola = false;
  }
}

