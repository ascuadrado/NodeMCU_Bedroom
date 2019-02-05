// Luz habitación Alberto - Blynk
/*
Pines reales:
Relé (D2)
LED (D1) - 130 luces

Pines virtuales:
Menú (V0):
Luces OFF
Luces tranquilas,
Luces personalizadas
FirePlace personalizado

Slider (V4) (1023)
Slider (V5) (1023)
RGB (V2) (255)

EEPROM:
hora_obj - int
minuto_obj - int
alarmaON - int
color - CRGB
slider - int
slider2 - int

*/


// FastLED Configuration
#define FASTLED_ALLOW_INTERRUPTS 0
#define FASTLED_ESP8266_RAW_PIN_ORDER
#define NUM_LEDS 135

// Libraries
#include <FastLED.h>
#include <Arduino.h>
#include <EEPROM.h>
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <TimeLib.h>
#include <WidgetRTC.h>
#include <ESP8266WebServer.h>

// Pin layouts (physical and virtual)
#define relayPIN D2
#define DATA_PIN D1
#define MenuPIN V0
#define AlarmaPIN V1
#define RGBPIN V2
#define SliderPIN V4
#define Slider2PIN V5
#define RelayPIN D6
#define timePIN V3
#define bombillaVirtualPIN V6
#define bombillaPhysicalPIN D6

// WiFi settings
char auth[] = "b50f7cd1e4c04fdd9ec385f0cc1a1a24";
//char ssid[] = "Iphone 7 de Alberto";
//char pass[] = "albertosanchez";
char ssid[] = "MOVISTAR_52B8(2)";
char pass[] = "WQ8hFl99w5VW3djsRVYb";

// Initialize variables
CRGB leds[NUM_LEDS];
CRGB auxColors[128];
ESP8266WebServer server(80);
WidgetRTC rtc;
WidgetTerminal terminal(V1);
BlynkTimer timer;

CRGB color, dimmedColor, auxColor;

int auxNumber, menu, counter, slider, secondCounter;
int address, alarmaON, slider2, bombillaON;
int timer1ID, timer2ID, timer3ID, timer4ID, timer5ID, timer6ID, checkTimer;
int wakeUpTimer;
uint8_t ranN;
int hora, minuto, margen;
int hora_obj, minuto_obj;

void deleteTimers(){ // Delete all timers function
    switch(menu){
        case 1:
        timer.deleteTimer(timer1ID);
        break;
        case 2:
        timer.deleteTimer(timer2ID);
        break;
        case 3:
        timer.deleteTimer(timer3ID);
        break;
        case 4:
        timer.deleteTimer(timer4ID);
        break;
        case 5:
        timer.deleteTimer(timer5ID);
        break;
        case 6:
        timer.deleteTimer(timer6ID);
        break;
    }

    timer.deleteTimer(checkTimer);
    timer.deleteTimer(wakeUpTimer);
}

void saveVariables(){ // Save important variables into EEPROM
    address = 0;

    EEPROM.put(address, hora_obj);
    address += sizeof(hora_obj);

    EEPROM.put(address, minuto_obj);
    address += sizeof(minuto_obj);

    EEPROM.put(address, alarmaON);
    address += sizeof(alarmaON);

    EEPROM.put(address, color);
    address += sizeof(color);

    EEPROM.put(address, slider);
    address += sizeof(slider);

    EEPROM.put(address, slider2);
    address += sizeof(slider2);

    EEPROM.commit();

    Serial.println(" \nVariables saved: ");
    Serial.print("objHour - ");
    Serial.print(hora_obj);
    Serial.print(" objMin - ");
    Serial.print(minuto_obj);
    Serial.print(" alarmON - ");
    Serial.print(alarmaON);
    Serial.print(" color - (");
    Serial.print(color.r);
    Serial.print(",");
    Serial.print(color.g);
    Serial.print(",");
    Serial.print(color.b);
    Serial.print(") s1 - ");
    Serial.print(slider);
    Serial.print(" s2 - ");
    Serial.println(slider2);
}

void retrieveVariables(){ // Retrieve important variables from EEPROM and Initialize dependent variables
    address = 0;
    EEPROM.get(address, hora_obj);
    address += sizeof(hora_obj);
    EEPROM.get(address, minuto_obj);
    address += sizeof(minuto_obj);
    EEPROM.get(address, alarmaON);
    address += sizeof(alarmaON);
    EEPROM.get(address, color);
    address += sizeof(color);
    EEPROM.get(address, slider);
    address += sizeof(slider);
    EEPROM.get(address, slider2);
    address += sizeof(slider2);

    if(menu<1){
        menu = 1;
    }

    menu = 1;
    counter = 0;
    secondCounter = 0;
    color.maximizeBrightness(100);
    dimmedColor = color;
    dimmedColor.fadeToBlackBy(256 - slider2/4);

    Serial.println(" \nVariables retrieved: ");
    Serial.print("objHour - ");
    Serial.print(hora_obj);
    Serial.print(" objMin - ");
    Serial.print(minuto_obj);
    Serial.print(" alarmON - ");
    Serial.print(alarmaON);
    Serial.print(" color - (");
    Serial.print(color.r);
    Serial.print(",");
    Serial.print(color.g);
    Serial.print(",");
    Serial.print(color.b);
    Serial.print(") s1 - ");
    Serial.print(slider);
    Serial.print(" s2 - ");
    Serial.println(slider2);
}

void modo1() // OFF
{
    for(int i=0;i<NUM_LEDS;i++){
        leds[i] = CRGB(0,0,0);
    }
    FastLED.show();
}

void modo2() // Ambient lights effect
{
    counter = counter%80;

    if (counter == 0){
        ranN = random8(10, NUM_LEDS-10);
    }else if (counter == 40) {
        ranN = random8(NUM_LEDS);
    }

    for(int i = 0; i < NUM_LEDS; i++){
        auxColor = dimmedColor;
        auxColor.fadeToBlackBy(255*(1-0.1/((i-ranN)*(i-ranN)/100+1)));
        leds[i] = leds[i] +
        auxColor;
    }

    if (counter%2==0) {
        for(int i = 0; i < NUM_LEDS; i++){
            leds[i].fadeToBlackBy(1);
        }
    }

    counter++;
    FastLED.show();
}

void modo3() // Moving waves
{
    counter ++;
    counter = counter%20;

    leds[0] = color;
    leds[0].fadeToBlackBy(15*counter);
    leds[0].fadeToBlackBy(256 - slider2/4);

    for(int i=NUM_LEDS-1;i>0;i--){
        leds[i] = leds[i-1];
    }

    FastLED.show();
}

void modo4() // Fixed Color
{
    for(int i = 0; i < NUM_LEDS; i++){
        leds[i] = dimmedColor;
    }
    FastLED.show();
}

void modo5() // Breathing mode
{
    counter = counter%255; // (counter = 0-255)

    if(counter == 0){
        auxColor = dimmedColor;
        auxNumber = auxColor.getLuma();
    }else if (counter <= 127) {
        if (auxNumber < 20){
            if (counter%3==0) {
                auxColor.fadeToBlackBy(1);
            }
        } else if (auxNumber < 50){
            if (counter%2==0) {
                auxColor.fadeToBlackBy(1);
            }
        } else{
            auxColor.fadeToBlackBy(1);
        }

        auxColors[counter] = auxColor;
    }else{
        auxColor = auxColors[255 - counter];
    }

    for(int i = 0; i < NUM_LEDS; i++){
        leds[i] = auxColor;
    }
    FastLED.show();
    counter++;
}

void modo6() // Meteorites mode
{
    counter = counter%40;

    if (counter==0) {
        ranN = random8(10, NUM_LEDS-10);
        for(int i = 0; i < NUM_LEDS; i++){
            auxColor = dimmedColor;
            auxColor.fadeToBlackBy(255*(1-1/((i-ranN)*(i-ranN)/200+1)));
            leds[i] = leds[i] + auxColor;
            //leds[i] = leds[i] + dimColor(dimmedColor, 0.9/((i-ranN)*(i-ranN)/100+1));
        }
    }

    for(int i = 0; i < NUM_LEDS; i++){
        leds[i].fadeToBlackBy(1);
    }
    counter++;
    FastLED.show();
}

BLYNK_CONNECTED() {
    // Synchronize time on connection
    rtc.begin();
}

void wakeUp(){ // Wake Up routine

    CRGB light = CRGB(secondCounter, secondCounter, secondCounter);

    for (int j = 0; j<NUM_LEDS; j++){
        leds[j] = light;
        FastLED.show();
        delay(40);
    }

    if (secondCounter > 20){
        digitalWrite(relayPIN, HIGH);
        delay(100000);
        digitalWrite(relayPIN, LOW);

        alarmaON = 0;
        deleteTimers();
    }

    secondCounter++;
}

void check(){ // Check time till wakeUp
    hora = hour();
    minuto = minute();
    margen = (hora_obj-hora)*60 + (minuto_obj-minuto); // Margen en minutos

    if (margen <= 4 && margen > -5) {
        // Begin routine
        deleteTimers();
        secondCounter = 1;
        wakeUpTimer = timer.setInterval(10000L, wakeUp);
    }
}

void makeTimers(){ // Create new timers
    switch(menu){
        case 1:
        timer1ID = timer.setInterval(100L, modo1);
        break;
        case 2:
        timer2ID = timer.setInterval((1050 - slider)/20, modo2);
        break;
        case 3:
        timer3ID = timer.setInterval((1050 - slider)/20, modo3);
        break;
        case 4:
        timer4ID = timer.setInterval(250L, modo4);
        break;
        case 5:
        timer5ID = timer.setInterval((1050 - slider)/20, modo5);
        break;
        case 6:
        timer6ID = timer.setInterval((1050 - slider)/20, modo6);
        break;
    }
    if (alarmaON == 1){
        secondCounter = 0;
        checkTimer = timer.setInterval(60000L, check);
    }
}

BLYNK_WRITE(MenuPIN){ // Menu changed
    deleteTimers();

    menu = param.asInt();
    counter = 0;

    makeTimers();

    saveVariables();
}

BLYNK_WRITE(SliderPIN){ // First slider changed (speed)

    deleteTimers();
    slider = param.asInt();
    saveVariables();
    makeTimers();
}

BLYNK_WRITE(Slider2PIN){ // Second slider changed (intensity)
    slider2 = param.asInt();
    color.maximizeBrightness(100);
    dimmedColor = color;
    dimmedColor.fadeToBlackBy(256 - slider2/4);
    saveVariables();
}

BLYNK_WRITE(RGBPIN){ // Color changed
    color = CRGB(param[0].asInt(),param[1].asInt(), param[2].asInt());
    color.maximizeBrightness(100);
    dimmedColor = color;
    dimmedColor.fadeToBlackBy(256 - slider2/4);
    saveVariables();
}

BLYNK_WRITE(AlarmaPIN){ // Alarm settings changed
    deleteTimers();

    for (int i = 0; i<NUM_LEDS; i++){
        leds[i] = CRGB(0,0,10);
    }
    FastLED.show();

    alarmaON = param.asInt();
    saveVariables();

    delay(1000);
    for (int i = 0; i<NUM_LEDS; i++){
        leds[i] = CRGB(0,0,0);
    }
    FastLED.show();
    makeTimers();
}

BLYNK_WRITE(timePIN) { // Alarm settings
    for (int i = 0; i<NUM_LEDS; i++){
        leds[i] = CRGB(0,10,0);
    }
    FastLED.show();
    TimeInputParam t(param);

    if (t.hasStartTime()){
        hora_obj = t.getStartHour();
        minuto_obj = t.getStartMinute();
    }

    if (t.getStartSecond() < 10) {
        alarmaON = 1;
    }

    saveVariables();

    delay(1000);
    for (int i = 0; i<NUM_LEDS; i++){
        leds[i] = CRGB(0,0,0);
    }
    FastLED.show();
}

BLYNK_WRITE(bombillaVirtualPIN) { // Main light switch
    bombillaON = param.asInt();
    digitalWrite(bombillaPhysicalPIN, bombillaON);
}

void handle_on() {
    bombillaON = 1;
    digitalWrite(bombillaPhysicalPIN, bombillaON);
    Blynk.virtualWrite(6, bombillaON);
    server.send(200, "text/plain", String(bombillaON)); // Return 1
}

void handle_off() {
    bombillaON = 0;
    digitalWrite(bombillaPhysicalPIN, bombillaON);
    Blynk.virtualWrite(6, bombillaON);
    server.send(200, "text/plain", String(bombillaON)); // Return 1
}

void handle_state() {
  server.send(200, "text/plain", String(bombillaON)); // Return state
}

void handle_brightness() {
  server.send(200, "text/plain", String(slider2)); // Return state
}

void handle_set_brightness(){
    slider2 = server.arg("brightness").toInt();
    slider2 = slider2 % 1023;
    Blynk.virtualWrite(5, slider2);
}

void handle_leds_on() {
    
}

void handle_leds_off() {

}

void setup()
{
    EEPROM.begin(512);
    Serial.begin(9600);
    Blynk.begin(auth, ssid, pass);
    WiFi.begin(ssid, pass);
    retrieveVariables();
    pinMode(RelayPIN, OUTPUT);
    pinMode(DATA_PIN, OUTPUT);

    FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);
    FastLED.setMaxPowerInMilliWatts(12000);
    randomSeed(analogRead(A0));

    leds[0] = CRGB(10,10,10);

    while (Blynk.connect() == false) { // Wait for connection
        for(int i = NUM_LEDS-1; i>0; i--){
            leds[i] = leds[i-1];
        }
        FastLED.show();
        delay(1);
    }

    Blynk.notify("Device online!");

    for(int i = 0; i < NUM_LEDS ; i++) {
        leds[i] = CRGB(0,0,0);
    }
    FastLED.show();

    Serial.println("");
    Serial.print("Connected to ");
    Serial.println(ssid);
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());

    // Start the server
    server.on("/status", handle_state);
    server.on("/actions/ON", handle_on);
    server.on("/actions/OFF", handle_off);
    server.on("/brightness", handle_brightness);
    server.on("/actions/brightness", handle_set_brightness);

    server.begin();
    makeTimers();
}

void loop()
{
    Blynk.run(); // Blynk functions
    timer.run(); // Timers
    server.handleClient(); // Incoming client connections
}
