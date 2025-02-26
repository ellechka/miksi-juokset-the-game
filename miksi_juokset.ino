#include <Arduino.h>
 
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
 
const unsigned char ledPinRed = 8;
const int ledPinGreen = 9;
const int buttonPinUp = 6;
const int buttonPinRight = 3;
const int buttonPinDown = 5;
 
unsigned long startTime; 
unsigned long lastUpdate = 0;
unsigned long buttonUpTime = 0;
unsigned long buttonDownTime = 0;
bool standing = true;
bool jumping = false;
bool bending = false;
int spikePos = 15;
unsigned long lastSpikeMove = 0;
bool spikeActive = false;
static int randAttack;
bool buttonUpPress = false;
bool buttonDownPress = false;
const unsigned long scoreCounter = 0;
 
LiquidCrystal_I2C lcd(0x27, 16, 2);
 
 
const char HEAD_CHAR_ID = 0;
const char BODY_CHAR_ID = 1;
const char UPPER_SPIKE_ID = 2;
const char LOWER_SPIKE_ID = 3;
 
byte humanHead[] = { //createChar 1
  B00000,
  B00000,
  B11111,
  B11111,
  B11010,
  B11111,
  B11111,
  B00100,
};
 
byte headBending[] = { //createChar 2
  B00000,
  B00000,
  B00000,
  B11111,
  B11111,
  B11010,
  B11111,
  B11111,
};
 
byte headUnderSpike[] = { //createChar 3
  B01110,
  B00100,
  B00000,
  B11111,
  B11111,
  B11010,
  B11111,
  B11111,
};
 
byte bodyStanding[] = { //createChar 4
  B00100,
  B11111,
  B10101,
  B10101,
  B01110,
  B01010,
  B01010,
  B01010,
};
 
byte bodyWalking[] = { //createChar 5
  B00100,
  B11111,
  B10101,
  B10101,
  B01110,
  B00100,
  B01010,
  B10001,
};
 
byte bodyJumping[] = { //createChar 6
  B00100,
  B11111,
  B10101,
  B00100,
  B11111,
  B00000,
  B00000,
  B00000,
};
 
byte bodyOverSpike[] = { //createChar 7
  B00100,
  B11111,
  B10101,
  B00100,
  B11111,
  B00000,
  B00100,
  B01110,
};
 
byte upperSpike[] = { //createChar 8
  B01110,
  B00100,
  B00000,
  B00000,
  B00000,
  B00000,
  B00000,
  B00000,
};
 
byte lowerSpike[] = { //createChar 9
  B00000,
  B00000,
  B00000,
  B00000,
  B00000,
  B00000,
  B00100,
  B01110,
};
 
void setup() {
  Serial.begin(9600);
 
  pinMode(ledPinRed, OUTPUT);
  pinMode(ledPinGreen, OUTPUT);
  pinMode(buttonPinUp, INPUT_PULLUP);
  pinMode(buttonPinRight, INPUT_PULLUP);
  pinMode(buttonPinDown, INPUT_PULLUP);
 
  randomSeed(analogRead(0));
 
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Minigame");
  /*lcd.setCursor(0, 1);
  lcd.print("Miksi juokset?");
  delay(2000);
  digitalWrite(ledPinRed, HIGH);
  digitalWrite(ledPinGreen, LOW);
  delay(1500);
  digitalWrite(ledPinRed, LOW);
  digitalWrite(ledPinGreen, HIGH);
  delay(1500);*/
  lcd.clear(); 
 
  Serial.println(millis());
  startTime = millis();
}
 
void draw(const char id, byte symbol[], const char col, const char row) {
  lcd.createChar(id, symbol);
  lcd.setCursor(col, row);
  lcd.write(id);
}
 
void spikeAttack() {
  int buttonUpState = digitalRead(buttonPinUp);
  int buttonDownState = digitalRead(buttonPinDown);
 
  if (!spikeActive) {
    spikePos = 15;
    randAttack = random(1, 3);
    spikeActive = true;
  }
 
  if (millis() - lastSpikeMove >= 200) {
    lastSpikeMove = millis();
 
    if (randAttack == 1) {
      lcd.setCursor(spikePos, 0);
      lcd.write(' ');
      spikePos--;
 
      if (spikePos >= 0) {
 
        draw(UPPER_SPIKE_ID, upperSpike, spikePos, 0);
      }
      if (spikePos == 0) {
        if (buttonDownPress) {
          lcd.createChar(HEAD_CHAR_ID, headUnderSpike);
          lcd.setCursor(0, 0);
          lcd.write(HEAD_CHAR_ID);
        } else {
          lcd.clear();
          lcd.setCursor(4, 0);
          lcd.print("GAME OVER");
          delay(1000);
          lcd.setCursor(0, 0);
          lcd.print("Pass time:");
          lcd.setCursor(0, 1);
          lcd.print(' ');
          delay(1000);
          lcd.clear();
 
          startTime = millis();
          spikeActive = false;
          spikePos = 15;
          return;
          }
        }
      }
 
    if (randAttack == 2) {
      lcd.setCursor(spikePos, 1);
      lcd.write(' ');
      spikePos--;
 
        if (spikePos >= 0) {
          lcd.createChar(LOWER_SPIKE_ID, lowerSpike);
          lcd.setCursor(spikePos, 1);
          lcd.write(LOWER_SPIKE_ID);
        }
 
        if (spikePos == 0) {
          if (buttonUpPress) {
            lcd.createChar(BODY_CHAR_ID, bodyOverSpike);
            lcd.setCursor(0, 1);
            lcd.write(BODY_CHAR_ID);
          } else {
            lcd.clear();
            lcd.setCursor(4, 0);
            lcd.print("GAME OVER");
            delay(1000);
            lcd.setCursor(0, 0);
            lcd.print("Pass time:");
            lcd.setCursor(0, 1);
            lcd.print(' ');
            delay(1000);
            lcd.clear();
 
            startTime = millis();
            spikeActive = false;
            spikePos = 15;
            return;
          }
        }
      }
    }
    if (spikePos < 0) {
      spikeActive = false;
      return;
    }
}
 
 
void loop() {
  if (millis() - startTime >= 90000) return;
 
  digitalWrite(ledPinRed, LOW);
  digitalWrite(ledPinGreen, LOW);
 
  bool buttonUpState = digitalRead(buttonPinUp);
  bool buttonDownState = digitalRead(buttonPinDown);
 
  if (millis() - lastUpdate >= 500) {
    lastUpdate = millis();
 
    lcd.createChar(HEAD_CHAR_ID, humanHead);
    lcd.setCursor(0, 0);
    lcd.write(HEAD_CHAR_ID);
 
    if (standing) {
      lcd.createChar(BODY_CHAR_ID, bodyStanding);
      lcd.setCursor(0, 1);
      lcd.write(BODY_CHAR_ID);
    } else {
      lcd.createChar(BODY_CHAR_ID, bodyWalking);
      lcd.setCursor(0, 1);
      lcd.write(BODY_CHAR_ID);
    }
    standing = !standing;

    if (buttonUpState == HIGH) {
      unsigned long currentMillisUp = millis();
      
      if (currentMillisUp - buttonUpTime >= 1000) {
        jumping = false;
        buttonUpPress = false;
      } else if (currentMillisUp - buttonUpTime >= 500 && !jumping) {
        buttonUpPress = true;
        jumping = true;
    
        lcd.createChar(BODY_CHAR_ID, bodyJumping);
        lcd.setCursor(0, 1);
        lcd.write(BODY_CHAR_ID);
        } 
    } else {
      buttonUpPress = false;
      jumping = false;
      buttonUpTime = millis();
    }


    Serial.println(buttonUpPress);
 
    if (buttonDownState == HIGH) {
      unsigned long currentMillisDown = millis();

      if (currentMillisDown - buttonDownTime >= 1000) {
        bending = false;
        buttonDownPress = false;
      } else if (currentMillisDown - buttonDownTime >= 500 && !jumping) {
        buttonDownPress = true;
        bending = true;

        lcd.createChar(HEAD_CHAR_ID, headBending);
        lcd.setCursor(0, 0);
        lcd.write(HEAD_CHAR_ID);
      }
    } else {
      buttonDownPress = false;
      bending = false;
      buttonDownTime = millis();
    }
 
    if (millis() - startTime >= 3000) {
      spikeAttack();
    }
  }
}
