/*  ___   ___  ___  _   _  ___   ___   ____ ___  ____  
 * / _ \ /___)/ _ \| | | |/ _ \ / _ \ / ___) _ \|    \ 
 *| |_| |___ | |_| | |_| | |_| | |_| ( (__| |_| | | | |
 * \___/(___/ \___/ \__  |\___/ \___(_)____)___/|_|_|_|
 *                  (____/ 
 * Osoyoo Wifi Arduino Shield Testing Project
 * Get remote Temperature and toggle LED on/off with web browser
 * tutorial url: http://osoyoo.com/?p=21123
 */
// Install WifiEsp library from link below
// https://osoyoo.com/driver/mecanum_metal_chassis/for_mega2560/WiFiEsp-master.zip
#include "WiFiEsp.h"
#include "WiFiEspClient.h"
#include "WiFiEspUdp.h"

// Set Wifi config in header
#include "wifiConfig.h"

// Emulate Serial1 on pins 3/2 by default
// If you want to use Hard Serial1 in Mega2560 or Leonardo, please remove the wifi shield jumper cap on TX->D3 RX->D2
#ifndef HAVE_HWSERIAL1
#include "SoftwareSerial.h"
SoftwareSerial Serial1(18, 19); // RX, TX
#endif

int status = WL_IDLE_STATUS;     // the Wifi radio's status
String device_name="OSOYOO-ESP8266-1"; //you can assign any name

WiFiEspClient espClient;
 
long lastMsg = 0;
char msg[50];
//int value = 0;
 
const char*  server = "10.0.0.4"; //you can change this to your_webserver_IP or domain name

void setup()
{
 
  // initialize serial for debugging
  Serial.begin(9600);
  // initialize serial for ESP module
  Serial1.begin(115200); 
  // initialize ESP module
  WiFi.init(&Serial1);

  // check for the presence of the shield
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present");
    // don't continue
    while (true);
  }

  // attempt to connect to WiFi network
  while ( status != WL_CONNECTED) {
    Serial.print("Attempting to connect to WPA SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network
    status = WiFi.begin(ssid, wpa_passphrase);
  }
   printCurrentNet();
   printWifiData();
}
unsigned long lastSendTime=0;
void loop()
{ 
  unsigned long now=millis();  //send temperature and humidity every 60 seconds
  if (now-lastSendTime>60000)
  {
      delay(200);
      float temp=21;
      float humid=30;
      delay(500);
      Serial.print("connecting to ");
      Serial.println(server);

      // Use WiFiClient class to create TCP connections
      const int httpPort = 8098;
      if (!espClient.connect(server, httpPort)) {
        Serial.println("connection failed");
        return;
      }
      String url = "/esp8266.php?device_name="+device_name+"&temperature="+temp+"&humidity="+humid;
      Serial.print("Requesting URL: ");
      Serial.println(url);
      // This will send the GET request to the remote web server
      espClient.print(String("GET ") + url + " HTTP/1.1\r\n" +"Host: " + server + "\r\n" + 
               "Connection: close\r\n\r\n");
      delay(2000);
      // Read all the lines of the reply from server and print them to Serial
      Serial.println("Respond:");
      while(espClient.available()){
        String line = espClient.readStringUntil('\r');
        Serial.print(line);
      }
      Serial.println();
      Serial.println("closing connection");
  }
}

void printWifiData()
{
  // print your WiFi shield's IP address
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print your MAC address
  byte mac[6];
  WiFi.macAddress(mac);
  char buf[20];
  sprintf(buf, "%02X:%02X:%02X:%02X:%02X:%02X", mac[5], mac[4], mac[3], mac[2], mac[1], mac[0]);
  Serial.print("MAC address: ");
  Serial.println(buf);
}

void printCurrentNet()
{
  // print the SSID of the network you're attached to
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print the MAC address of the router you're attached to
  byte bssid[6];
  WiFi.BSSID(bssid);
  char buf[20];
  sprintf(buf, "%02X:%02X:%02X:%02X:%02X:%02X", bssid[5], bssid[4], bssid[3], bssid[2], bssid[1], bssid[0]);
  Serial.print("BSSID: ");
  Serial.println(buf);

  // print the received signal strength
  long rssi = WiFi.RSSI();
  Serial.print("Signal strength (RSSI): ");
  Serial.println(rssi);
} 
void sendDataToESP8266(String messageData,boolean debug)
{
 String response;
 Serial1.print(messageData);
 response= Serial1.readString(); // read response from esp8266
 if (debug)
    Serial.println(response); //displays the esp response messages in arduino Serial monitor 
}
