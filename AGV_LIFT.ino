#include <ESP8266WiFi.h>
#include "ESPAsyncWebServer.h"
#include <ESP8266HTTPClient.h>

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

void setup() { 
  Serial.begin(9600);

  pinMode(PIN_UP_POS_INPUT,   INPUT_PULLUP);
  pinMode(PIN_DOWN_POS_INPUT, INPUT_PULLUP);
  pinMode(PIN_UP_SW_INPUT,   INPUT_PULLUP);
  pinMode(PIN_DOWN_SW_INPUT, INPUT_PULLUP);
  pinMode(PIN_UP,   OUTPUT);    digitalWrite(PIN_UP   , LOW);
  pinMode(PIN_DOWN, OUTPUT);    digitalWrite(PIN_DOWN , LOW);
  
  WiFi.begin("AGV_AP_EXT", "1q2w3e4r");
  while (WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.print(".");
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
  if(digitalRead(PIN_UP_SW_INPUT) == LOW){
    liftGo = 2;     httpSw = false;
  }else if(digitalRead(PIN_DOWN_SW_INPUT) == LOW){
    liftGo = 1;     httpSw = false;
  }else if(!httpSw){   liftGo = 0;  }
  
  if(digitalRead(PIN_UP_POS_INPUT) == LOW){
    liftPos = 2;    liftGo = 0;   httpSw = false;
  }else if(digitalRead(PIN_DOWN_POS_INPUT) == LOW){
    liftPos = 1;    liftGo = 0;   httpSw = false;
  }else{  liftPos = 0; }

  if(liftGo == 2){      digitalWrite(PIN_UP   , HIGH); }
  else if(liftGo == 1){ digitalWrite(PIN_DOWN , HIGH); }
  else{
    digitalWrite(PIN_UP   , LOW);
    digitalWrite(PIN_DOWN , LOW);
  }
}
