//  This program sends  bits to a simple 433MHz OOK ASK transmitter module
// Used to replace a AC114-01B remote control
// Button UP / STOP  /  DOWN  only
// bits for each command were scanned through RTL-SDR dongle + Universal Radio Hacker
//
// Control via simple WEB interface or directly   http://ESP_IP/up  or /stop or /down

// inital Wifi connect throught normal AP for ESP8266.
// Reset all WIFI settings by 3 seconds press on Flash button

// phfu  -  June 2019  -  Free to use  :)


#include <ESP8266WiFi.h>
#include <WiFiManager.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>

#define DEBUG     // activate Debug mode = sends debug messages on serial

#define TX_PIN D5       // 433MHz TX pin on PIN D5 for ESP8266  ESP12 
#define TRIGGER_PIN 0   // Flash button on ESP12
#define TIME 202        //duration of a bit in µs
#define LED_PIN 2       // LED pin 

// Define Debug 
#ifdef DEBUG
#define DEBUG_PRINT(x) Serial.println(x)
#else
#define DEBUG_PRINT(x)
#endif


// this are the bits from URH
const char Up []  = "11111111111111111111111111100011101000111010001000100011101110100010001110100010001110111011...";
const char Down []  = "11111111111111111111111111100011101000111010001000100011101110100010001110100010001110111011101110100010001110111011101001...";
const char Stop [] = "111111111111111111111111111000111010001110100010001000111011101000100011101000100011101110111...";

//  some time counters
unsigned long previousMillis = 0;
unsigned long currentMillis = 0;
unsigned long secCounter = 0;
#define SEC1 1000
int mdnsQueryTimeout = 0;
int ResetCounter = 0;
int ResetCounter2=0;

// Wifi manager
WiFiManager wifiManager;
ESP8266WebServer server(80);    // Create a webserver object that listens for HTTP request on port 80

void reset_wifi() {           // Flash button pressed for 3 sec : clear all WIFI param & reset
  DEBUG_PRINT("Clear & restart");
  digitalWrite(LED_PIN,!digitalRead(LED_PIN));    // Change the state of the LED for a second
  delay(SEC1);
  
  wifiManager.resetSettings();
  delay(500);
  ESP.reset();
}


void sendData(const char * data, long int size)     // sends the 433 MHz message  by togling the Data pin of the TX module
{Serial.println(data);
  for(long int i = 0; i < size; ++i)
  {   
    if (data[i]=='0')  
    {
     digitalWrite(TX_PIN, LOW);
     delayMicroseconds(TIME);
    }
    else if (data[i]=='1')  
    {
     digitalWrite(TX_PIN, HIGH);
     delayMicroseconds(TIME); 
    }
  }
  digitalWrite(TX_PIN, LOW);
  delayMicroseconds(TIME*20);
}

//*************************************************************
// These are the Web Server routines for each GET possibility
//*************************************************************

void handleRoot() {                         // When URI / is requested, send a web page with  buttons like on the remote control
  String message = "AC114-01B  REMOTE CONTROL  FOR BEN  :) <br>\n";
  message += "<form action=\"/up\" method=\"GET\"><input type=\"submit\" value=\"UP\"></form>";
  message += "\n<br>";
  message += "<form action=\"/stop\" method=\"GET\"><input type=\"submit\" value=\"STOP\"></form>";
  message += "\n<br>";
  message += "<form action=\"/down\" method=\"GET\"><input type=\"submit\" value=\"DOWN\"></form>";
  message += "\n<br>\n<br>";
  message += "<form action=\"/LED\" method=\"POST\"><input type=\"submit\" value=\"Toggle LED\"></form>";
  
  DEBUG_PRINT(message);
  server.send(200, "text/html", message);
}


void handle_Up() {          // Up command
   String message = "Up OK";
   DEBUG_PRINT(message);
   server.send(200, "text/html", message);
   sendData(Up, sizeof(Up));    // Send the Message over RF
   DEBUG_PRINT(Up);
   delay(500);
}

void handle_Down() {      // down command
   String message = "Down OK";
   DEBUG_PRINT(message);
   server.send(200, "text/html", message);
   sendData(Down , sizeof(Down));    // Send the Message over RF
   DEBUG_PRINT(Down);
   delay(500);
}

void handle_Stop() {      // Stop command
   String message = "Stop OK";
   DEBUG_PRINT(message);
   server.send(200, "text/html", message);
   sendData(Stop , sizeof(Stop));    // Send the Message over RF
   DEBUG_PRINT(Stop);
   delay(500);
}

void handle_LED() {                          // If a POST request is made to URI /LED
  digitalWrite(LED_PIN,!digitalRead(LED_PIN));      // Change the state of the LED
  server.sendHeader("Location","/");        // Add a header to respond with a new location for the browser to go to the home page again
  server.send(303);                         // Send it back to the browser with an HTTP status 303 (See Other) to redirect
}

void handleNotFound(){
  server.send(404, "text/plain", "404: Not found"); // Send HTTP status 404 (Not Found) when there's no handler for the URI in the request
}


//************************************************************************************

void setup() 
{  

  #ifdef DEBUG
  Serial.begin(115200);
  #endif
 
  pinMode(TX_PIN, OUTPUT);
  digitalWrite(TX_PIN, LOW);

  pinMode(LED_PIN, OUTPUT);     // LED
  digitalWrite(LED_PIN,HIGH);

  String accessPointName = "ESP8622-";
  accessPointName += ESP.getChipId();


   #ifdef DEBUG
      wifiManager.setDebugOutput(true);
    #else
      wifiManager.setDebugOutput(false);
    #endif

    // wifiManager.setConfigPortalTimeout(300);   // timeout 5 min before going into deep sleep
    // if (!wifiManager.autoConnect(accessPointName.c_str(), NULL)) {    // if we want a specific name
    if (!wifiManager.autoConnect()) {
        DEBUG_PRINT("failed to connect to any wifi");
        ESP.reset();
        delay(1000);
    }

    DEBUG_PRINT("Wifi connected to");
    DEBUG_PRINT(WiFi.SSID());
    DEBUG_PRINT("IP address:\t");
    DEBUG_PRINT(WiFi.localIP());

    server.on("/", HTTP_GET, handleRoot);     // Call the 'handleRoot' function when a client requests URI "/"
    server.on("/LED", HTTP_POST, handle_LED);  // Call the 'handleLED' function when a POST request is made to URI "/LED"

    server.on("/up", HTTP_GET, handle_Up);    
    server.on("/stop", HTTP_GET, handle_Stop);    
    server.on("/down", HTTP_GET, handle_Down);    
    
    server.onNotFound(handleNotFound);        // When a client requests an unknown URI (i.e. something other than "/"), call function "handleNotFound"

    server.begin();                           // Actually start the server
    DEBUG_PRINT("HTTP server started");

  
    previousMillis = millis();   // initialise the milliseconds counter    
        


    
}

void loop() 
{  

   currentMillis = millis();
   server.handleClient();                    // Listen for HTTP requests from clients
    

    if (currentMillis - previousMillis >= SEC1) {      // runs every second
      secCounter ++;
      DEBUG_PRINT(secCounter);
      previousMillis = currentMillis; 

        if (digitalRead(TRIGGER_PIN) == HIGH) {           // Flash button not pressed : do nothing
        ResetCounter = 0;
      }     
      else {                                            // Flash button pressed : increase counter
        DEBUG_PRINT("Trigger pressed");
        ResetCounter ++;
        DEBUG_PRINT(ResetCounter);
        if (ResetCounter == 3) {
          reset_wifi();
        }
        delay(10);
      }
    }
}
