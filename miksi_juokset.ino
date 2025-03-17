#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <EEPROM.h>
 
const unsigned char ledPinRed = 7;
const int ledPinGreen = 8;
const int ledPinYellow = 9;
const int buttonPinUp = 6;
const int buttonPinRight = 4;
const int buttonPinDown = 5;
 
unsigned int startTime = 0; 
unsigned long lastUpdate = 0;
unsigned long buttonUpTime = 0;
unsigned long buttonDownTime = 0;
unsigned long buttonRightTime = 0;
unsigned long lastSpikeMove = 0;
unsigned long scoreCounter = 0;
unsigned long ledRedBlinkTime = 0;
unsigned long ledGreenBlinkTime = 0;
unsigned long ledYellowBlinkTime = 0;
unsigned long recordTime = 0;
unsigned long passTime = 0;
unsigned long nextSpikeTime = 0;
unsigned long leftSpikeWarningStart = 0;
bool leftSpikeWarningStarted = false;
bool standing = true;
bool jumping = false;
bool bending = false;
bool evading = false;
bool spikeActive1 = false;
bool spikeActive2 = false;
bool buttonUpPress = false;
bool buttonDownPress = false;
bool buttonRightPress = false;
bool ledRedState = false;
bool ledGreenState = false;
bool ledYellowState = false;
bool leftSpikeActive = false;
static int spikePos1 = 15;
static int spikePos2 = 15;
static int randAttack1;
static int randAttack2;
static int numberOfAttacks = 3;
static int difficultyLevel = 600;
 
LiquidCrystal_I2C lcd(0x27, 16, 2);
 
const char HEAD_CHAR_ID = 0;
const char BODY_CHAR_ID = 1;
const char UPPER_SPIKE_ID = 2;
const char LOWER_SPIKE_ID = 3;
const char LEFT_SPIKE_ID = 4;
 
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

byte headLeftSpike[] = {
  B00000,
  B00000,
  B11111,
  B11111,
  B01011,
  B01011,
  B11111,
  B00100,
};

byte bodyLeftSpike[] = {
  B00010,
  B00011,
  B00011,
  B10011,
  B11010,
  B10010,
  B00010,
  B00010,
};

byte leftSpike[] = {
  B00000,
  B00000,
  B00000,
  B10000,
  B11000,
  B10000,
  B00000,
  B00000,
};

byte headGoingToRight[] = {
  B00000,
  B00000,
  B11111,
  B11111,
  B01011,
  B11111,
  B11111,
  B00100,
};

byte bodyGoingToRight[] = {
  B00010,
  B00011,
  B00011,
  B00011,
  B00010,
  B00010,
  B00010,
  B00010,
};

byte headDied[] = {
  B00000,
  B00000,
  B11111,
  B11111,
  B11010,
  B11010,
  B11111,
  B00100,
};

int melody[] {
  //notes
};

int noteDurations[] {
  //4 or 8
};

void setup() {
  Serial.begin(9600);

  pinMode(ledPinRed, OUTPUT);
  pinMode(ledPinGreen, OUTPUT);
  pinMode(ledPinYellow, OUTPUT);
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
  digitalWrite(ledPinRed, HIGH);
  digitalWrite(ledPinGreen, HIGH);
  digitalWrite(ledPinYellow, HIGH);
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

void resetGame() {
  standing = true;
  jumping = false;
  bending = false;
  evading = false;
  spikeActive1 = false;
  spikeActive2 = false;

  startTime = 0;
  lastUpdate = 0;
  buttonUpTime = 0;
  buttonDownTime = 0;
  buttonRightTime = 0;
  scoreCounter = millis();
  passTime = 0;

  difficultyLevel = 600;

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Minigame");
  delay(1000);
  lcd.clear(); 
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

    resetGame();
    lastUpdate = millis();   
}
 
void spikeAttack() {
  int buttonUpState = digitalRead(buttonPinUp);
  int buttonDownState = digitalRead(buttonPinDown);
  int buttonRightState = digitalRead(buttonPinRight);
 
  if (!spikeActive1) {
    spikePos1 = 15;
    randAttack1 = random(1, numberOfAttacks);
    spikeActive1 = true;
  }

  if (millis() - startTime >= 2000) { //12000
    if (!spikeActive2 && spikePos1 <= 5) {
      spikePos2 = 15;
      randAttack2 = random(1, numberOfAttacks);
      spikeActive2 = true;
    }
  }

  if (millis() - startTime >= 3000) { //15000
    numberOfAttacks = 4;
  }
 
  if (millis() - lastSpikeMove >= 200) { //200
    lastSpikeMove = millis();

    if (spikeActive1) {
      moveSpike(spikePos1, randAttack1, spikeActive1);
    }

    if (millis() - startTime >= 2000) { // 12000
      if (spikeActive2) {
      moveSpike(spikePos2, randAttack2, spikeActive2);
      if (randAttack1 == 3 || randAttack2 == 3) {
        moveSpike(spikePos1, randAttack1, spikeActive1);
      }
      }
    }
  }
}

void moveSpike(int &spikePos, int randAttack, bool &spikeActive) {
  if (randAttack == 1) {
    ledRedState = false;
    ledYellowState = false;
    blinked(ledPinGreen, ledGreenState, ledGreenBlinkTime);
    lcd.setCursor(spikePos, 0);
    lcd.write(' ');
    spikePos--;
 
    if (spikePos >= 1) {
      draw(UPPER_SPIKE_ID, upperSpike, spikePos, 0);
    }

    if (spikePos == 1) {
      if (bending) {
        draw(HEAD_CHAR_ID, headUnderSpike, 1, 0);
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
 
  if (randAttack == 2) {
    ledGreenState = false;
    ledRedState = false;
    blinked(ledPinYellow, ledYellowState, ledYellowBlinkTime);
    lcd.setCursor(spikePos, 1);
    lcd.write(' ');
    spikePos--;
 
    if (spikePos >= 1) {
      draw(LOWER_SPIKE_ID, lowerSpike, spikePos, 1);
    }
 
    if (spikePos == 1) {
      if (jumping) {
        draw(BODY_CHAR_ID, bodyOverSpike, 1, 1);
        delay(200);
        ledYellowState = !ledYellowState;
      } else {
        gameOver();
        lcd.clear();
 
        spikeActive = false;
        return;
        }
    }
  }

  if (randAttack == 3) {
    if (!leftSpikeWarningStarted) {
      leftSpikeWarningStart = millis();
      leftSpikeWarningStarted = true;
      leftSpikeActive = false;
    }

    if (millis() - leftSpikeWarningStart < 5000) {
      ledYellowState = false;
      ledGreenState = false;
      blinked(ledPinRed, ledRedState, ledRedBlinkTime);
      return;
    } 
    if (!leftSpikeActive) {
      draw(LEFT_SPIKE_ID, leftSpike, 0, 1);
      leftSpikeActive = true;
    }

    if (leftSpikeActive) {
      if (evading) {
        lcd.clear();
        draw(HEAD_CHAR_ID, headLeftSpike, 1, 0);
        draw(BODY_CHAR_ID, bodyLeftSpike, 1, 1);
        delay(200);
        ledRedState = !ledRedState;

        spikeActive = false;
        leftSpikeActive = false;
        leftSpikeWarningStarted = false;
        lcd.clear();
      } else if (millis() - leftSpikeWarningStart >= 7000) {
        gameOver();
        lcd.clear();

        spikeActive = false;
        leftSpikeActive = false;
        leftSpikeWarningStarted = false;
        return;
      }
    }
  }
    
  if (spikePos < 1) {
    spikeActive = false;
  }
}


 
 
void loop() {
  bool buttonUpState = digitalRead(buttonPinUp);
  bool buttonDownState = digitalRead(buttonPinDown);
  bool buttonRightState = digitalRead(buttonPinRight);
 
  if (millis() - lastUpdate >= difficultyLevel) {
    lastUpdate = millis();

    draw(HEAD_CHAR_ID, humanHead, 1, 0);
 
    if (standing) {
      draw(BODY_CHAR_ID, bodyStanding, 1, 1);
    } else {
      draw(BODY_CHAR_ID, bodyWalking, 1, 1);
    }
    standing = !standing;

    if (buttonUpState == HIGH) {
      if (millis() - buttonUpTime >= 1000) {
        jumping = false;
        buttonUpPress = false;
      } else if (millis() - buttonUpTime >= 100 && !jumping) {
        buttonUpPress = true;
        jumping = true;

        draw(BODY_CHAR_ID, bodyJumping, 1, 1);
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

        draw(HEAD_CHAR_ID, headBending, 1, 0);
        }

    } else {
      buttonDownPress = false;
      bending = false;
      buttonDownTime = millis();
    }

    if (buttonRightState == HIGH) {
      if (millis() - buttonRightTime >= 1000) {
        evading = false;
        buttonRightPress = false;
      } else if (millis() - buttonRightTime >= 100 && !evading){
        buttonRightPress = true;
        evading = true;

        draw(HEAD_CHAR_ID, headGoingToRight, 1, 0);
        draw(BODY_CHAR_ID, bodyGoingToRight, 1, 1);
      }
    } else {
      buttonRightPress = false;
      evading = false;
      buttonRightTime = millis();
    }
 
    if (millis() - startTime >= 500) {
      spikeAttack();
    }

    if (millis() - startTime >= 1000) { //the second round - only speedup 9000
      difficultyLevel = 500;
      if (millis() - startTime >= 2000) { //the third - there are several spikes on the screen, as well as spikes from two lines 12000
        difficultyLevel = 400;
        if (millis() - startTime >= 3000) { //the fourth - The right spike flies out, oops.. 15000
          difficultyLevel = 400;
        }
      }
    }
  }
}
