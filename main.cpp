#include <Arduino.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include "SPIFFS.h"

// Replace with your network credentials
const char *ssid = "REDACTED";
const char *password = "REDACTED";
// Create AsyncWebServer object on port 80
AsyncWebServer server(80);
// Create a WebSocket object
AsyncWebSocket ws("/ws");
// Set LED GPIO
const int ledPin2 = 2;
const int ledPin4 = 4;
const int ledPin12 = 12;
const int ledPin14 = 14;
// setting PWM properties
const int freq = 5000;
const int ledChannel2 = 0;
const int ledChannel4 = 1;
const int ledChannel12 = 2;
const int ledChannel14 = 3;
const int resolution = 8;
String sliderValue2 = "0";
String sliderValue4 = "0";
String sliderValue12 = "0";
String sliderValue14 = "0";
int dutyCycle2;
int dutyCycle4;
int dutyCycle12;
int dutyCycle14;
// Initialize SPIFFS
void initSPIFFS()
{
  if (!SPIFFS.begin(true))
  {
    Serial.println("An error has occurred while mounting SPIFFS");
  }
  else
  {
    Serial.println("SPIFFS mounted successfully");
  }
}
// Initialize WiFi
void initWiFi()
{
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi ..");
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print('.');
    delay(1000);
  }
  Serial.println(WiFi.localIP());
}
void handleWebSocketMessage(void *arg, uint8_t *data, size_t len)
{
  AwsFrameInfo *info = (AwsFrameInfo *)arg;
  if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT)
  {
    data[len] = 0;
    sliderValue2 = (char *)data;
    dutyCycle2 = map(sliderValue2.toInt(), 0, 100, 0, 255);
    Serial.println(dutyCycle2);
  }
}
void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len)
{
  switch (type)
  {
  case WS_EVT_CONNECT:
    Serial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
    break;
  case WS_EVT_DISCONNECT:
    Serial.printf("WebSocket client #%u disconnected\n", client->id());
    break;
  case WS_EVT_DATA:
    handleWebSocketMessage(arg, data, len);
    break;
  case WS_EVT_PONG:
  case WS_EVT_ERROR:
    break;
  }
}
void initWebSocket()
{
  ws.onEvent(onEvent);
  server.addHandler(&ws);
}
void setup()
{
  Serial.begin(115200);
  pinMode(ledPin2, OUTPUT);
  pinMode(ledPin4, OUTPUT);
  pinMode(ledPin12, OUTPUT);
  pinMode(ledPin14, OUTPUT);
  initSPIFFS();
  initWiFi();
  // configure LED PWM functionalitites
  ledcSetup(ledChannel2, freq, resolution);
  // attach the channel to the GPIO to be controlled
  ledcAttachPin(ledPin2, ledChannel2);
    ledcSetup(ledChannel4, freq, resolution);
  // attach the channel to the GPIO to be controlled
  ledcAttachPin(ledPin4, ledChannel4);
    ledcSetup(ledChannel12, freq, resolution);
  // attach the channel to the GPIO to be controlled
  ledcAttachPin(ledPin12, ledChannel12);
  ledcSetup(ledChannel14, freq, resolution);
  // attach the channel to the GPIO to be controlled
  ledcAttachPin(ledPin14, ledChannel14);
  initWebSocket();
  // Web Server Root URL
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(SPIFFS, "/index.html", "text/html"); });
  server.serveStatic("/", SPIFFS, "/");
  server.on("/currentValue2", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(200, "/text/plain", String(sliderValue2).c_str()); });
  server.on("/currentValue4", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(200, "/text/plain", String(sliderValue4).c_str()); });
  server.on("/currentValue12", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(200, "/text/plain", String(sliderValue2).c_str()); });
  server.on("/currentValue14", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(200, "/text/plain", String(sliderValue4).c_str()); });
  // Start server
  server.begin();
}
void loop()
{
  ledcWrite(ledChannel2, dutyCycle2);
  ledcWrite(ledChannel4, dutyCycle4);
  ledcWrite(ledChannel12, dutyCycle12);
  ledcWrite(ledChannel14, dutyCycle14);
  ws.cleanupClients();
}