
#include <ESP8266WiFi.h>
#include "SinricPro.h"
#include "SinricProSwitch.h"

#include <map>

#define WIFI_SSID         "WIFI_SSID"    
#define WIFI_PASS         "WIFI_PASSKEY"
#define APP_KEY           "APP_KEY"      
#define APP_SECRET        "APP_SECRET"   

#define device_ID_1   "DEVICE_ID"
#define device_ID_2   "DEVICE_ID"

#define RelayPin1 D2 
#define RelayPin2 D5 

#define wifiLed   D0

#define BAUD_RATE   9600

#define DEBOUNCE_TIME 250


typedef struct {     
  int relayPIN;
} deviceConfig_t;

std::map<String, deviceConfig_t> devices = {
    //{deviceId, {relayPIN}}
    {device_ID_1, {  RelayPin1 }},
    {device_ID_2, {  RelayPin2 }},   
};

void setupRelays() { 
    pinMode(RelayPin1, OUTPUT);  
    pinMode(RelayPin2, OUTPUT);  
    digitalWrite(RelayPin1, HIGH);
    digitalWrite(RelayPin2, HIGH);
}

bool onPowerState(String deviceId, bool &state)
{
  Serial.printf("%s: %s\r\n", deviceId.c_str(), state ? "on" : "off");
  int relayPIN = devices[deviceId].relayPIN; 
  digitalWrite(relayPIN, !state);             // set the new relay state
  return true;
}


void setupWiFi()
{
  Serial.printf("\r\n[Wifi]: Connecting");
  WiFi.begin(WIFI_SSID, WIFI_PASS);

  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.printf(".");
    delay(250);
  }
  digitalWrite(wifiLed, LOW);
  Serial.printf("connected!\r\n[WiFi]: IP-Address is %s\r\n", WiFi.localIP().toString().c_str());
}

void setupSinricPro()
{
  for (auto &device : devices)
  {
    const char *deviceId = device.first.c_str();
    SinricProSwitch &mySwitch = SinricPro[deviceId];
    mySwitch.onPowerState(onPowerState);
  }
  SinricPro.begin(APP_KEY, APP_SECRET);
  SinricPro.restoreDeviceStates(true);
}

void setup()
{
  Serial.begin(BAUD_RATE);
  pinMode(wifiLed, OUTPUT);
  digitalWrite(wifiLed, HIGH);
  setupRelays();
  setupWiFi();
  setupSinricPro();
}

void loop()
{
  SinricPro.handle();
}
