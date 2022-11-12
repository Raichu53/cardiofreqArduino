#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Tone.h>

#include "functions.h"

#define sensorHeartCapt A0
#define clockPin 6
#define dataPin 7
#define resetPin 8
#define buttonPin 3
#define redLed 12
#define yellowLed 11
#define greenLed 10
#define buzzerPin 5

class clock
{
public:
    unsigned char buffer;  

    clock();
    bool initClock(); //call once
    unsigned char readRegister(unsigned char addr);
    void writeRegister(unsigned char addr,unsigned char data);
    void afficheHeure();
private:
};
class oled
{
public:
    vec2 screenDimension;
    Adafruit_SSD1306* pDisp;

    int size;
    vec2 cursorPos;
    uint16_t color;

    bool bButton;
    bool buffer;
    
    bool pressed;
    bool press;
    bool doublePressed;
    unsigned long delayMax;
    unsigned long doublePush;
    unsigned long secondPress;

    bool toggleScreen;
    oled();
    int isButtonPressed();
    void drawBlackScreen();
private:

};
class heartSensor
{
public:
    oled* screen;
    unsigned long startMillis;
    unsigned long currentMillis;
    unsigned long oldTime;
    bool start;
    bool stop;
    int bpm;

    float interval;
    int toneValue;
    unsigned long lastBeep;

    bool bBpm;
    heartSensor();
    void heartBeat();
    void healthLeds();
    void beebBpm();
private:
};

extern heartSensor* heart;
extern clock* horloge;
