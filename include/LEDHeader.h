
// Light programs

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
        leds[i] = leds[i] + auxColor;
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

    leds[0] = dimmedColor;
    leds[0].fadeToBlackBy(15*counter);

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
        }
    }

    for(int i = 0; i < NUM_LEDS; i++){
        leds[i].fadeToBlackBy(1);
    }
    counter++;
    FastLED.show();
}

void modo7() // Fiesta mode
{
    counter = counter % 2;
    if (counter==0) {
        for (int i = 0; i < NUM_LEDS; i++) {
            leds[i] = CRGB(100,100,100);
        }
    }else{
        for (int i = 0; i < NUM_LEDS; i++) {
            leds[i] = CRGB(0,0,0);
        }
    }
    FastLED.show();
    counter++;
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

        alarmON = 0;
        deleteTimers();
    }

    secondCounter++;
}
