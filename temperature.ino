#include <ESP8266WiFi.h>
#include <DallasTemperature.h>
#include <OneWire.h>

#define ONE_WIRE_BUS 2                       
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);            
const char* server = "api.thingspeak.com";
String apiKey = "L3S0NR6OYCQ0F69J";
const char* MY_SSID = "Xiaomi_F705";
const char* MY_PWD = "12890321";
//int sent = 0;
void setup(void)
{
  //pinMode(ONE_WIRE_BUS, INPUT_PULLUP);
  Serial.begin(115200);
  Serial.println("Dallas Temperature");
  
  sensors.begin();
  connectWifi();
}

void loop(void)
{ 
  float temp;
  Serial.print("Requesting temperature...");
  sensors.requestTemperatures();
  temp = sensors.getTempCByIndex(0);
  Serial.println("DONE");  
  Serial.print("Temperature is: ");
  Serial.println(temp);   
  sendTemperatureTS(temp);
  delay(60000);
}

void connectWifi() {
  Serial.print("Connecting to "+*MY_SSID);
  WiFi.begin(MY_SSID, MY_PWD);
  while(WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("Connected");
  Serial.println("");
}

void sendTemperatureTS(float temp) {
  WiFiClient wifi_client;

  if(wifi_client.connect(server, 80)) {
  Serial.println("WiFi Client connected ");

  String postStr = apiKey;
  postStr += "&field1=";
  postStr += String(temp);
  postStr += "\r\n\r\n";
  wifi_client.print("POST /update HTTP/1.1\n");
  wifi_client.print("Host: api.thingspeak.com\n");
  wifi_client.print("Connection: close\n");
  wifi_client.print("X-THINGSPEAKAPIKEY: " + apiKey + "\n");
  wifi_client.print("Content-Type: application/x-www-form-urlencoded\n");
  wifi_client.print("Content-Length: ");
  wifi_client.print(postStr.length());
  wifi_client.print("\n\n");
  wifi_client.print(postStr);
  }
//  sent++;
  wifi_client.stop();
}
