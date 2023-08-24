#ifndef colourPATTERNS_H
#define colourPATTERNS_H

#include <Arduino.h>

class ColourPatterns {
public:
    ColourPatterns(int redPin, int greenPin, int bluePin); // Constructor that takes pin numbers

    void runLoading();
    void changeColours(int choice);

private:
    void Red();
    void Green();
    void Blue();
    void Yellow();
    void Cyan();
    void Magenta();
    void White();
    void Fade();
    void StrobePlus();
    void RGBStrobe();
    void Rainbow();
    void Halfstrobe();
    void BGStrobe();
    void GRStrobe();
    void Off();

    int redLed;
    int greenLed;
    int blueLed;
    
    unsigned long previousMillis = 0;
    int rainbowWay = 0;
    int threeWay = 0;
    int ledState = LOW;
    bool upDownFade = false;
    unsigned int fadeSpeed = 500;
    unsigned long interval = 100;
};

#endif // colourPATTERNS_H
