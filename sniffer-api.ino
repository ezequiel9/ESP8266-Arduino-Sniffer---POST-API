// Smell MAc address from wifi area and post a json to a server.
// Show a fake ssid.

#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266HTTPClient.h>
#include <vector>

const char* apSsid     = "Free Wifi";
const char* apPassword = ""; //no password
const char* clientSsid     = "************";
const char* clientPassword = "******************";

HTTPClient http;

WiFiEventHandler probeRequestPrintHandler;

String macToString(const unsigned char* mac) {
  char buf[20];
  snprintf(buf, sizeof(buf), "%02x:%02x:%02x:%02x:%02x:%02x",
           mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
  return String(buf);
}

std::vector<WiFiEventSoftAPModeProbeRequestReceived> myList;

void onProbeRequestPrint(const WiFiEventSoftAPModeProbeRequestReceived& evt) {
  myList.push_back(evt);
}

void setup() {
  Serial.begin(115200);
  Serial.println("Hello, is now set up!");

  // Don't save WiFi configuration in flash - optional
  WiFi.persistent(false);

  WiFi.mode(WIFI_AP_STA);
  WiFi.softAP(apSsid, apPassword);
  WiFi.begin(clientSsid, clientPassword);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(100);
  }
  Serial.println("");
  probeRequestPrintHandler = WiFi.onSoftAPModeProbeRequestReceived(&onProbeRequestPrint);
}

void loop() {
  delay(3000);
  String json = "";
  DynamicJsonBuffer jsonBuffer;
  JsonObject& root = jsonBuffer.createObject();
  JsonArray& probes = root.createNestedArray("probes");
  for(WiFiEventSoftAPModeProbeRequestReceived w : myList){
    JsonObject& probe = probes.createNestedObject();
    probe["mac"] = macToString(w.mac);
    probe["rssi"] = w.rssi;
    probe["cluster_id"] = 1;
    probe["read_at"] = '2019-03-03 22:22:22';
  }
  myList.clear();
  root.printTo(json);
  Serial.println("json:" + json);

  if(WiFi.status()== WL_CONNECTED){ //Check WiFi connection status
    http.begin("http://nodes.ezequielfernandez.com/api/nodes");
    http.addHeader("Content-Type", "application/json");
    int httpCode = http.POST(json);
    
    Serial.println(httpCode);
    String payload = http.getString();
    Serial.println(payload);    //Print request response payload
    
    http.end();
  }else{
    Serial.print("Error in Wifi connection");
  }

}
