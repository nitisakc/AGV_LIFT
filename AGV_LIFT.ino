#include <ESP8266WiFi.h>
#include "ESPAsyncWebServer.h"
#include <ESP8266HTTPClient.h>
//#include <avr/wdt.h>

AsyncWebServer server(80);

byte PIN_UP_POS_INPUT   = D5
    ,PIN_DOWN_POS_INPUT = D6
    ,PIN_UP_SW_INPUT    = D3
    ,PIN_DOWN_SW_INPUT  = D4
    ,PIN_UP             = D1
    ,PIN_DOWN           = D2;
int liftPos             = 0
   ,liftGo              = 0;
bool httpSw             = false;
int prevMillis = 0, currMillis = 0, disconn = 10;

void setup() { 
  Serial.begin(9600);

  pinMode(PIN_UP_POS_INPUT,   INPUT_PULLUP);
  pinMode(PIN_DOWN_POS_INPUT, INPUT_PULLUP);
  pinMode(PIN_UP_SW_INPUT,   INPUT_PULLUP);
  pinMode(PIN_DOWN_SW_INPUT, INPUT_PULLUP);
  pinMode(PIN_UP,   OUTPUT);    digitalWrite(PIN_UP   , LOW);
  pinMode(PIN_DOWN, OUTPUT);    digitalWrite(PIN_DOWN , LOW);

  WiFi.mode(WIFI_STA);
  WiFi.begin("AGV_AP01", "1q2w3e4r");
  int timeout = 30;
  while (WiFi.status() != WL_CONNECTED){
    delay(500); 
    Serial.print(".");
    if(timeout-- <= 0){ ESP.restart(); }
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println(WiFi.localIP()); 

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "text/plain", String(liftPos));
  });
  server.on("/up", HTTP_GET, [](AsyncWebServerRequest *request){
    liftGo = 2;
    httpSw = true;
    request->send(200, "text/plain", String(liftPos));
  });
  server.on("/down", HTTP_GET, [](AsyncWebServerRequest *request){
    liftGo = 1;
    httpSw = true;
    request->send(200, "text/plain", String(liftPos));
  });
  server.on("/stop", HTTP_GET, [](AsyncWebServerRequest *request){
    liftGo = 0;
    httpSw = false;
    request->send(200, "text/plain", String(liftPos));
  });
  
  server.begin();
}

void loop() {
  currMillis = millis();
  if ((currMillis - prevMillis) > 1000) {
    prevMillis = currMillis;
    if(WiFi.status() != WL_CONNECTED){ if(disconn-- <= 0){ ESP.restart(); } }  
  }
  
  if(digitalRead(PIN_UP_SW_INPUT) == LOW){
    liftGo = 2;     httpSw = false;
  }else if(digitalRead(PIN_DOWN_SW_INPUT) == LOW){
    liftGo = 1;     httpSw = false;
  }else if(!httpSw){   liftGo = 0;  }
  
  if(digitalRead(PIN_UP_POS_INPUT) == LOW){
    liftPos = 2;    
    if(liftGo == 2){ liftGo = 0;   httpSw = false; }
  }else if(digitalRead(PIN_DOWN_POS_INPUT) == LOW){
    liftPos = 1; 
    if(liftGo == 1){ liftGo = 0;   httpSw = false; }
  }else{  liftPos = 0; }

  if(liftGo == 2){      digitalWrite(PIN_UP   , HIGH); }
  else if(liftGo == 1){ digitalWrite(PIN_DOWN , HIGH); }
  else{
    digitalWrite(PIN_UP   , LOW);
    digitalWrite(PIN_DOWN , LOW);
  }
  Serial.print("LIFT POS: ");
  Serial.println(liftPos == 1 ? "DOWN" : liftPos == 2 ? "UP" : "---");
  Serial.print("LIFT : ");
  Serial.println(liftGo == 1 ? "DOWN" : liftGo == 2 ? "UP" : "---");
  delay(500);
}
