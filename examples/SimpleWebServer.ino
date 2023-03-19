#include <WiFi.h>
#include <Esp.h>
#include "ImprovWiFiLib.h"

#define LED_BUILTIN 2
#define MAX_ATTEMPTS_WIFI_CONNECTION 20
#define ARDUINO 1


//*** Web Server
WiFiServer server(80);

char linebuf[80];
int charcount=0;


void onImprovWiFiError(improv::Error err) {
  blink_led(2000, 3);
}

void onImprovWiFiConnected(std::string ssid, std::string password) {
  server.begin();
  blink_led(100, 3);
}


void setup() {
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);

  WiFi.mode(WIFI_STA);
  WiFi.disconnect();

  ImprovWiFi::getParams()->firmwareName = "ImprovWiFiLib";
  ImprovWiFi::getParams()->firmwareVersion = "1.0.0";
  ImprovWiFi::getParams()->deviceName = "BasicWebServer";
  ImprovWiFi::getParams()->deviceUrl = "http://{LOCAL_IPV4}?name=Jonathas&test=1";
  ImprovWiFi::getParams()->chipFamily = ImprovWiFi::CF_ESP32;


  ImprovWiFi::setOnImprovWiFiError(onImprovWiFiError);
  ImprovWiFi::setOnImprovWiFiConnected(onImprovWiFiConnected);
  
  
  ImprovWiFi::setup();


  Serial.println("Setup finished");
  blink_led(100, 5);

}

void loop() {

  ImprovWiFi::loop();


  if (ImprovWiFi::isConnected()) {
    handle_request();  
  }
  
}



void handle_request() {

  WiFiClient client = server.available();
  if (client) 
  {
    blink_led(100, 1);
    memset(linebuf,0,sizeof(linebuf));
    charcount=0;
    // an http request ends with a blank line
    boolean currentLineIsBlank = true;
    while (client.connected()) 
    {
      if (client.available()) 
      {
        char c = client.read();
        //read char by char HTTP request
        linebuf[charcount]=c;
        if (charcount<sizeof(linebuf)-1) charcount++;

        if (c == '\n' && currentLineIsBlank) 
        {
          // send a standard http response header
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println("Connection: close");  // the connection will be closed after completion of the response
          client.println();
          client.println("<!DOCTYPE HTML><html><body>");
          client.println("<h1 id=\"welcome\">Welcome!</h1>");
          client.println("<p>This is a simple webpage served by your ESP32</p>");
          client.println("<h3>Chip Info</h3>");
          client.println("<ul><li>Model:");
          client.println(ESP.getChipModel());
          client.println("</li><li>Cores: ");
          client.println(ESP.getChipCores());
          client.println("</li><li>Revision: ");
          client.println(ESP.getChipRevision());
          client.println("</li></ul>");
          client.println("<h3>Network Info</h3>");
          client.println("<ul><li>SSID: ");
          client.println(WiFi.SSID());
          client.println("</li><li>IP Address: ");
          client.println(WiFi.localIP());
          client.println("</li><li>MAC Address: ");
          client.println(WiFi.macAddress());
          client.println("</li></ul>");
          client.println("<script>const params = new URLSearchParams(document.location.search);document.getElementById('welcome').innerHTML = 'Welcome' + (params.get(\"name\") ? ', ' + params.get('name') : '') + '!';</script>");
          client.println("</body></html>");
          break;
        }
      }
    }
    delay(1);
    client.stop();
  }  
}

void blink_led(int d, int times) {
  for (int j=0; j<times; j++){
    digitalWrite(LED_BUILTIN, HIGH);
    delay(d);
    digitalWrite(LED_BUILTIN, LOW);
    delay(d);
  }
  
}


