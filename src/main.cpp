// Luz habitación Alberto - Blynk

/*
Pines virtuales:


*/


// FastLED Configuration
#define FASTLED_ALLOW_INTERRUPTS 0      // Avoid flickering
#define FASTLED_ESP8266_RAW_PIN_ORDER   // Use D0,...
#define NUM_LEDS 135                    // Number of LEDs in strip

// Libraries
#include <Arduino.h>                // Basic arduino library
#include <FastLED.h>                // Control LEDs
#include <EEPROM.h>                 // Save variables in non-volatile memory
#include <BlynkSimpleEsp8266.h>     // Blynk app iOS
#include <WidgetRTC.h>              // Keep track of time
#include <ESP8266WebServer.h>       // WebServer
#include <ESP8266mDNS.h>            // mDNS service to not use IP
#include <WiFiUdp.h>                // Udp for Over The Air update
#include <ArduinoOTA.h>             // Over The Air updates

// Pin layouts (physical and virtual)
#define relayPIN D6 // Relay for main bedroom light
#define DATA_PIN D1 // Data pin for WS8212b LED strip

// WiFi settings
char auth[] = "b50f7cd1e4c04fdd9ec385f0cc1a1a24";   // Blynk auth token
char ssid[] = "MOVISTAR_52B8(2)";                   // Wifi AP
char pass[] = "WQ8hFl99w5VW3djsRVYb";               // Wifi Password

// Initialize variables
CRGB leds[NUM_LEDS];            // Main LED strip
CRGB auxColors[128];            // Aux colors for effects
ESP8266WebServer server(80);    // Web server
WidgetRTC rtc;                  // TimeKeeping rtc
BlynkTimer timer;               // Timer for tasks

CRGB color, dimmedColor, auxColor; // More auxiliary colors
int objHour, objMin;              // Time variables

int menu, vSlider, hSlider, alarmON, relayON; // Important variables

int auxNumber, counter, secondCounter, margen; // Auxiliary variables
uint8_t ranN;

// Timer IDs
int timers[15];

void deleteTimers(){ // Delete all timers
    for(int i=0; i<15;i++){
        timer.deleteTimer(timers[i]);
    }
}

#include "LEDHeader.h"              // LED programs and functions

void saveVariables(){ // Save important variables into EEPROM
    int address = 0;

    EEPROM.put(address, color);
    address += sizeof(color);

    EEPROM.put(address, objHour);
    address += sizeof(objHour);

    EEPROM.put(address, objMin);
    address += sizeof(objMin);

    EEPROM.put(address, menu);
    address += sizeof(menu);

    EEPROM.put(address, vSlider);
    address += sizeof(vSlider);

    EEPROM.put(address, hSlider);
    address += sizeof(hSlider);

    EEPROM.put(address, alarmON);
    address += sizeof(alarmON);

    EEPROM.put(address, relayON);
    address += sizeof(relayON);

    EEPROM.commit();
}

// Retrieve important variables from EEPROM and Initialize dependent variables
void retrieveVariables(){
    int address = 0;
    EEPROM.get(address, color);
    address += sizeof(color);
    EEPROM.get(address, objHour);
    address += sizeof(objHour);
    EEPROM.get(address, objMin);
    address += sizeof(objMin);
    EEPROM.get(address, menu);
    address += sizeof(menu);
    EEPROM.get(address, vSlider);
    address += sizeof(vSlider);
    EEPROM.get(address, hSlider);
    address += sizeof(hSlider);
    EEPROM.get(address, alarmON);
    address += sizeof(alarmON);
    EEPROM.get(address, relayON);
    address += sizeof(relayON);

    counter = 0;
    secondCounter = 0;
    auxNumber = 0;
    ranN = 0;
    color.maximizeBrightness(100);
    dimmedColor = color;
    auxColor = color;
    dimmedColor.fadeToBlackBy(256 - hSlider/4);
}

void check(){ // Check time till wakeUp
    margen = (objHour-hour())*60
    + (objMin-minute()); // Margin in minutes

    if (margen <= 4 && margen > -5) {
        // Begin routine
        deleteTimers();
        timers[10] = timer.setInterval(10000L, wakeUp);
    }
}

void makeTimers(){ // Create new timers
    counter = 0;
    secondCounter = 0;

    switch(menu){
        case 1:
        timers[1] = timer.setInterval(100L, modo1);
        break;
        case 2:
        timers[2] = timer.setInterval((1050 - vSlider)/20, modo2);
        break;
        case 3:
        timers[3] = timer.setInterval((1050 - vSlider)/20, modo3);
        break;
        case 4:
        timers[4] = timer.setInterval(250L, modo4);
        break;
        case 5:
        timers[5] = timer.setInterval((1050 - vSlider)/20, modo5);
        break;
        case 6:
        timers[6] = timer.setInterval((1050 - vSlider)/20, modo6);
        break;
        case 7:
        timers[7] = timer.setInterval((1050 - vSlider)/20, modo7);
        break;
    }
    if (alarmON == 1){
        timers[11] = timer.setInterval(60000L, check);
    }
}

#include "BlynkHeader.h"            // Perform Blynk operations
#include "ServerHeader.h"           // Perform Server operations
#include "HomeKitHeader.h"          // Perform HomeBridge operations

void setup()
{
    EEPROM.begin(512);              // Initialize EEPROM ()
    // Serial.begin(9600);
    Blynk.begin(auth, ssid, pass);  // Blynk start
    //WiFi.begin(ssid, pass);         // Wifi
    retrieveVariables();            // Get variables from EEPROM

    pinMode(relayPIN, OUTPUT);
    pinMode(DATA_PIN, OUTPUT);

    FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);// LED strip
    FastLED.setMaxPowerInMilliWatts(12000);             // 12 W max Power
    randomSeed(analogRead(A0));                         // Random starting seed

    leds[0] = CRGB(0,0,0);

    while (Blynk.connect() == false) {      // Wait for connection
        for(int i = NUM_LEDS-1; i>0; i--){  // Visual sequence
            leds[i] = leds[i-1];
        }
        FastLED.show();
    }

    for(int i = 0; i < NUM_LEDS ; i++) {
        leds[i] = CRGB(0,0,0);      // Turn off all LEDs
    }
    FastLED.show();

    Serial.println("");
    Serial.print("Connected to ");
    Serial.println(ssid);
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());

    // Start the server
    startServer();
    makeTimers();

    // Over The Air updates
    startOTA();

    MDNS.begin("nodemcu");      // MDNS (nodemcu.local)
}

void loop()
{
    Blynk.run();            // Blynk internal functions
    timer.run();            // Run timers
    server.handleClient();  // Incoming client connections to server
    ArduinoOTA.handle();    // OTA handling
}
