
void handle_on() {
    relayON = 1;
    digitalWrite(relayPIN, relayON);
    server.send(200, "text/plain", String(relayON)); // Return 1
    saveVariables();
}

void handle_off() {
    relayON = 0;
    digitalWrite(relayPIN, relayON);
    server.send(200, "text/plain", String(relayON)); // Return 1
    saveVariables();
}

void handle_state() {
  server.send(200, "text/plain", String(relayON)); // Return state
}

void handle_brightness() {
  server.send(200, "text/plain", String(hSlider/10)); // Return state
}

void handle_set_brightness(){
    hSlider = server.arg("brightness").toInt();
    server.send(200, "text/plain", String(hSlider)); // Return 1
    hSlider = hSlider * 10;
    color.maximizeBrightness(100);
    dimmedColor = color;
    dimmedColor.fadeToBlackBy(256 - hSlider/4);
    saveVariables();
}

void handle_leds_on() {
    deleteTimers();
    menu = 4;
    counter = 0;
    makeTimers();
    saveVariables();
    server.send(200, "text/plain", String(1)); // Return 1
}

void handle_leds_off() {
    deleteTimers();
    menu = 1;
    counter = 0;
    makeTimers();
    saveVariables();
    server.send(200, "text/plain", String(0)); // Return 1
}

void handle_leds_status() {
    if (menu == 4) {
        server.send(200, "text/plain", String(1)); // Return 1
    }else {
        server.send(200, "text/plain", String(0)); // Return 1
    }
}

void handle_fiesta_on() {
    deleteTimers();
    menu = 7;
    counter = 0;
    makeTimers();
    saveVariables();
    server.send(200, "text/plain", String(1)); // Return 1
}

void handle_fiesta_off() {
    deleteTimers();
    menu = 1;
    counter = 0;
    makeTimers();
    saveVariables();
    server.send(200, "text/plain", String(0)); // Return 1
}

void handle_fiesta_status() {
    if (menu == 7) {
        server.send(200, "text/plain", String(1)); // Return 1
    }else {
        server.send(200, "text/plain", String(0)); // Return 1
    }
}

void handle_fiesta_brightness() {
    server.send(200, "text/plain", String(hSlider/10)); // Return state
}

void handle_fiesta_set_brightness() {
    deleteTimers();
    hSlider = server.arg("brightness").toInt();
    server.send(200, "text/plain", String(hSlider)); // Return 1
    hSlider = hSlider * 10;
    saveVariables();
    makeTimers();
}

void startServer(){
    server.on("/status", handle_state);
    server.on("/actions/ON", handle_on);
    server.on("/actions/OFF", handle_off);

    server.on("/LEDsStatus", handle_leds_status);
    server.on("/actions/LEDsON", handle_leds_on);
    server.on("/actions/LEDsOFF", handle_leds_off);
    server.on("/brightness", handle_brightness);
    server.on("/actions/brightness", handle_set_brightness);

    server.on("/actions/fiestaOn", handle_fiesta_on);
    server.on("/actions/fiestaOff", handle_fiesta_off);
    server.on("/fiestaStatus", handle_fiesta_status);
    server.on("/fiestaBrightness", handle_fiesta_brightness);
    server.on("/actions/fiestaBrightness", handle_fiesta_set_brightness);

    server.begin();
}

void startOTA(){
    ArduinoOTA.onStart([]() {
        Serial.println("Start");
    });
    ArduinoOTA.onEnd([]() {
        Serial.println("End");
    });
    ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
        Serial.printf("Progress: %u%%\n", (progress / (total / 100)));
    });
    ArduinoOTA.onError([](ota_error_t error) {
        Serial.printf("Error[%u]: ", error);
        if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
        else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
        else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
        else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
        else if (error == OTA_END_ERROR) Serial.println("End Failed");
    });
    ArduinoOTA.begin();
}
