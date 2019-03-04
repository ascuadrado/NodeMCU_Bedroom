
// Pins for Blynk app
#define MenuPIN V0
#define AlarmaPIN V1
#define RGBPIN V2
#define vSliderPIN V4
#define hSliderPIN V5
#define timePIN V3
#define relayVirtualPIN V6

BLYNK_WRITE(MenuPIN){ // Menu changed
    deleteTimers();
    menu = param.asInt();
    makeTimers();
    saveVariables();
}

BLYNK_WRITE(hSliderPIN){ // First slider changed (speed)
    deleteTimers();
    hSlider = param.asInt();
    saveVariables();
    makeTimers();
}

BLYNK_WRITE(vSliderPIN){ // Second slider changed (intensity)
    vSlider = param.asInt();
    color.maximizeBrightness(100);
    dimmedColor = color;
    dimmedColor.fadeToBlackBy(256 - vSlider/4);
    saveVariables();
}

BLYNK_WRITE(RGBPIN){ // Color changed
    color = CRGB(param[0].asInt(),param[1].asInt(), param[2].asInt());
    color.maximizeBrightness(100);
    dimmedColor = color;
    dimmedColor.fadeToBlackBy(256 - vSlider/4);
    saveVariables();
}

BLYNK_WRITE(AlarmaPIN){ // Alarm settings changed
    deleteTimers();

    for (int i = 0; i<NUM_LEDS; i++){
        leds[i] = CRGB(0,0,10);
    }
    FastLED.show();

    alarmON = param.asInt();
    saveVariables();

    delay(500);
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

    if(t.hasStartTime()){
        objHour = t.getStartHour();
        objMin = t.getStartMinute();
    }

    saveVariables();

    delay(500);
    for (int i = 0; i<NUM_LEDS; i++){
        leds[i] = CRGB(0,0,0);
    }
    FastLED.show();
}

BLYNK_WRITE(relayVirtualPIN) { // Main light switch
    relayON = param.asInt();
    digitalWrite(relayPIN, relayON);
}

BLYNK_CONNECTED() {
    // Synchronize time on connection
    rtc.begin();
}

void syncBlynk(){
    Blynk.virtualWrite(0, menu);
    Blynk.virtualWrite(1, alarmON);
    Blynk.virtualWrite(4, vSlider);
    Blynk.virtualWrite(5, hSlider);
    Blynk.virtualWrite(6, relayON);
}
