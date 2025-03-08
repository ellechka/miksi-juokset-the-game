#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <EEPROM.h>
 
const unsigned char ledPinRed = 8;
const int ledPinGreen = 9;
const int buttonPinUp = 6;
const int buttonPinRight = 3;
const int buttonPinDown = 5;
 
unsigned int startTime = 0; 
unsigned long lastUpdate = 0;
unsigned long buttonUpTime = 0;
unsigned long buttonDownTime = 0;
unsigned long lastSpikeMove = 0;
unsigned long scoreCounter = 0;
unsigned long ledRedBlinkTime = 0;
unsigned long ledGreenBlinkTime = 0;
unsigned long recordTime = 0;
unsigned long passTime = 0;
unsigned long nextSpikeTime = 0;
bool standing = true;
bool jumping = false;
bool bending = false;
bool spikeActive1 = false;
bool spikeActive2 = false;
bool buttonUpPress = false;
bool buttonDownPress = false;
bool ledRedState = false;
bool ledGreenState = false;
bool thirdStage = false;
static int spikePos1 = 15;
static int spikePos2 = 15;
static int randAttack1;
static int randAttack2;
static int difficultyLevel = 500;
 
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

  Serial.println("EEPROM cleared.");
 
  pinMode(ledPinRed, OUTPUT);
  pinMode(ledPinGreen, OUTPUT);
  pinMode(buttonPinUp, INPUT_PULLUP);
  pinMode(buttonPinRight, INPUT_PULLUP);
  pinMode(buttonPinDown, INPUT_PULLUP);
 
  randomSeed(analogRead(0));
  recordTime = ((unsigned long)EEPROM.read(0) << 24) | 
             ((unsigned long)EEPROM.read(1) << 16) | 
             ((unsigned long)EEPROM.read(2) << 8)  | 
             (unsigned long)EEPROM.read(3);
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
  lcd.setCursor(0, 1);
  lcd.print(recordTime);
  delay(1000);
  lcd.clear(); 
  
}
 
void draw(const char id, byte symbol[], const char col, const char row) {
  lcd.createChar(id, symbol);
  lcd.setCursor(col, row);
  lcd.write(id);
}

void blinked(int pin, bool &ledState, unsigned long &ledBlinkTime) {
  if (millis() - ledBlinkTime >= 200) {
        ledBlinkTime = millis();
        ledState = !ledState;
        digitalWrite(pin, ledState);
      }
}

void recorded() {
  recordTime = passTime;
  EEPROM.update(0, (recordTime >> 24) & 0xFF);
  EEPROM.update(1, (recordTime >> 16) & 0xFF);
  EEPROM.update(2, (recordTime >> 8) & 0xFF);
  EEPROM.update(3, recordTime & 0xFF);
}

void gameOver() {
  passTime = millis() - scoreCounter;
  scoreCounter = millis();
    if (passTime >= recordTime) {
      recorded();
    }
    lcd.clear();
    lcd.setCursor(4, 0);
    lcd.print("GAME OVER");
    delay(500);

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("PASS TIME");
    lcd.setCursor(0, 1);
    lcd.print(passTime);
    delay(2000);   
}
 
void spikeAttack() {
  int buttonUpState = digitalRead(buttonPinUp);
  int buttonDownState = digitalRead(buttonPinDown);
 
  if (!spikeActive1) {
    spikePos1 = 15;
    randAttack1 = random(1, 3);
    spikeActive1 = true;
  }

  if (millis() - startTime >= 10000) {
    if (!spikeActive2 && spikePos1 <= 5) {
      spikePos2 = 15;
      randAttack2 = random(1, 3);
      spikeActive2 = true;
    }
  }
 
  if (millis() - lastSpikeMove >= 200) {
    lastSpikeMove = millis();

    if (spikeActive1) {
      moveSpike(spikePos1, randAttack1, spikeActive1);
    }

    if (millis() - startTime >= 6000) {
      if (spikeActive2) {
      moveSpike(spikePos2, randAttack2, spikeActive2);
      }
    }
  }
}

void moveSpike(int &spikePos, int randAttack, bool &spikeActive) {
  if (randAttack == 1) {
    blinked(ledPinRed, ledRedState, ledRedBlinkTime);
    lcd.setCursor(spikePos, 0);
    lcd.write(' ');
    spikePos--;
 
    if (spikePos >= 0) {
      draw(UPPER_SPIKE_ID, upperSpike, spikePos, 0);
    }

    if (spikePos == 0) {
      if (bending) {
        draw(HEAD_CHAR_ID, headUnderSpike, 0, 0);
        delay(200);
        ledRedState = !ledRedState;
      } else {
        gameOver();
        lcd.clear();
        spikeActive = false;
        return;
        }
    }
  }
 
  if (randAttack == 2) {
    blinked(ledPinGreen, ledGreenState, ledGreenBlinkTime);
    lcd.setCursor(spikePos, 1);
    lcd.write(' ');
    spikePos--;
 
    if (spikePos >= 0) {
      draw(LOWER_SPIKE_ID, lowerSpike, spikePos, 1);
    }
 
    if (spikePos == 0) {
      if (jumping) {
        draw(BODY_CHAR_ID, bodyOverSpike, 0, 1);
        delay(200);
        ledGreenState = !ledGreenState;
      } else {
        gameOver();
        lcd.clear();
 
        spikeActive = false;
        return;
        }
    }
  }

  if (spikePos < 0) {
    spikeActive = false;
  }
}


 
 
void loop() {
  if (millis() - startTime >= 3600000) return;
 
  bool buttonUpState = digitalRead(buttonPinUp);
  bool buttonDownState = digitalRead(buttonPinDown);
 
  if (millis() - lastUpdate >= difficultyLevel) {
    lastUpdate = millis();

    draw(HEAD_CHAR_ID, humanHead, 0, 0);
 
    if (standing) {
      draw(BODY_CHAR_ID, bodyStanding, 0, 1);
    } else {
      draw(BODY_CHAR_ID, bodyWalking, 0, 1);
    }
    standing = !standing;

    if (buttonUpState == HIGH) {
      if (millis() - buttonUpTime >= 1000) {
        jumping = false;
        buttonUpPress = false;
      } else if (millis() - buttonUpTime >= 100 && !jumping) {
        buttonUpPress = true;
        jumping = true;

        draw(BODY_CHAR_ID, bodyJumping, 0, 1);
        } 
    } else {
      buttonUpPress = false;
      jumping = false;
      buttonUpTime = millis();
    }
 
    if (buttonDownState == HIGH) {
      if (millis() - buttonDownTime >= 1000) {
        bending = false;
        buttonDownPress = false;
      } else if (millis() - buttonDownTime >= 100 && !bending) {
        buttonDownPress = true;
        bending = true;

        draw(HEAD_CHAR_ID, headBending, 0, 0);
        }

    } else {
      buttonDownPress = false;
      bending = false;
      buttonDownTime = millis();
    }
 
    if (millis() - startTime >= 500) {
      spikeAttack();
    }

    if (millis() - startTime >= 3000) { //the second round - only speedup 
      difficultyLevel = 500;
      if (millis() - startTime >= 10000) { //the third - there are several spikes on the screen, as well as spikes from two lines
        difficultyLevel = 300;
        if (millis() - startTime >= 80000) { //the fourth - The right spike flies out, oops..
          difficultyLevel = 200;
        }
      }
    }
  }
}
