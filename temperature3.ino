#include <PrivateData.h> /*Stores Private Data*/

#include <ESP8266WiFi.h>
#include <DallasTemperature.h>
#include <OneWire.h>
#include <time.h>

#define ONE_WIRE_BUS 2                       
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);            
const char* server = "api.thingspeak.com";
const char* server2 = "ec2-13-209-64-204.ap-northeast-2.compute.amazonaws.com";
const char* streamId = "log";

//int sent = 0;
void setup(void)
{
  Serial.begin(115200);
  Serial.println("ds18b20 temperature sensor with NODEMCU");
  
  sensors.begin();
  connectWifi();
  configTime(3 * 3600, 0, "pool.ntp.org", "time.nist.gov");
   Serial.println("\nWaiting for time"); 
   while (!time(nullptr)) { 
      Serial.print("."); 
      delay(1000); 
   }
   Serial.println("");
}

void loop(void)
{ 
  float temp;
  Serial.println("Requesting temperature...");
  sensors.requestTemperatures();
  temp = sensors.getTempCByIndex(0);
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

  if(wifi_client.connect(server2, 3000)) {
    Serial.println("WiFi to AWS connected ");

    String a[6];

    time_t now = time(nullptr);
    struct tm * timeinfo;
    String sample = "2019-03-23";
    now += 21600;
    timeinfo = localtime(&now);
/*    int tmphour = timeinfo->tm_hour;
    tmphour += 6;
    if(tmphour >= 24) {
      tmphour -= 24;
    }
    timeinfo->tm_hour = tmphour;*/
    a[0] = String(timeinfo->tm_sec, DEC);
    a[1] = String(timeinfo->tm_min, DEC);
    a[2] = String(timeinfo->tm_hour, DEC);
    a[3] = String(timeinfo->tm_mday, DEC);
    a[4] = String((timeinfo->tm_mon + 1), DEC);
    a[5] = String((timeinfo->tm_year + 1900), DEC);

    if(timeinfo->tm_sec < 10) a[0] = "0" + a[0];
    if(timeinfo->tm_min < 10) a[1] = "0" + a[1];
    if(timeinfo->tm_hour < 10) a[2] = "0" + a[2];
    if(timeinfo->tm_mday < 10) a[3] = "0" + a[3];
    if(timeinfo->tm_mon < 10) a[4] = "0" + a[4];
    String url = "/log";
/*    url += streamId;*/
    url += "?date=";
    url += a[5];
    url += a[4];
    url += a[3];
    url += "&time=";
    url += a[2];
    url += a[1];
    url += a[0];
/*    url += sample;*/
    url += "&value=";
    url += temp;
    Serial.print("Requesting URL : ");
    Serial.println(url);

    wifi_client.print(String("GET ") + url + " HTTP/1.1\r\n" +
                      "HOST: " + server2 + "\r\n" +
                      "Connection: close\r\n\r\n");
    unsigned long timeout = millis();
    while(wifi_client.available() == 0) {
      if(millis() - timeout > 5000) {
        Serial.println(">>>Client Timeout !");
        wifi_client.stop();
        return; 
      }
    }
    while(wifi_client.available()) {
      String line = wifi_client.readStringUntil('\r');
      Serial.print(line);
    }
  }
  else {
    Serial.println("AWS connect failed...");
  }
//  sent++;
  wifi_client.stop();
}
