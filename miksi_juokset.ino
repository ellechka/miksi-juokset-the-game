#include <Wire.h>
#include <LiquidCrystal_I2C.h>

const int ledPinRed = 8;
const int ledPinGreen = 9;
const int buttonPinUp = 6;
const int buttonPinRight = 3;
const int buttonPinDown = 5;

unsigned long startTime = millis();

LiquidCrystal_I2C lcd(0x27, 16, 2);

byte humanHead[] = {
  B00000,
  B00000,
  B11111,
  B11111,
  B11010,
  B11111,
  B11111,
  B00100,
};

byte headBending[] = {
  B00000,
  B00000,
  B00000,
  B11111,
  B11111,
  B11010,
  B11111,
  B11111,
};

byte headUnderSpike[] = {
  B01110,
  B00100,
  B00000,
  B11111,
  B11111,
  B11010,
  B11111,
  B11111,
};

byte bodyStanding[] = {
  B00100,
  B11111,
  B10101,
  B10101,
  B01110,
  B01010,
  B01010,
  B01010,
};

byte bodyWalking[] = {
  B00100,
  B11111,
  B10101,
  B10101,
  B01110,
  B00100,
  B01010,
  B10001,
};

byte bodyJumping[] = {
  B00100,
  B11111,
  B10101,
  B00100,
  B11111,
  B00000,
  B00000,
  B00000,
};

byte bodyOverSpike[] = {
  B00100,
  B11111,
  B10101,
  B00100,
  B11111,
  B00000,
  B00100,
  B01110,
};

byte upperSpike[] = {
  B01110,
  B00100,
  B00000,
  B00000,
  B00000,
  B00000,
  B00000,
  B00000,
};

byte lowerSpike[] = {
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

  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Minigame");
  lcd.setCursor(0, 1);
  lcd.print("Miksi juokset?");
  delay(2000);
  digitalWrite(ledPinRed, HIGH);
  digitalWrite(ledPinGreen, LOW);
  delay(1500);
  digitalWrite(ledPinRed, LOW);
  digitalWrite(ledPinGreen, HIGH);
  delay(1500);
  lcd.clear(); /*прошло 5 секунд*/
}

void spikeAttack() {
  const int upperAttack = 1; 
  const int lowerAttack = 2;


  if (upperAttack) {
    unsigned long attackTime = millis();

    if (millis() - attackTime >= 400) {
      attackTime = millis();
      for (int i = 15; i > 2; i--) {
        lcd.createChar(5, upperSpike);
        lcd.setCursor(i, 0);
        lcd.write(5);
      }
    }
  }
}

void loop() {
  digitalWrite(ledPinRed, LOW);
  digitalWrite(ledPinGreen, LOW);
  unsigned long buttonPressTime = millis();
  unsigned long lastUpdate = millis();
  bool standing = true;
  bool jumping = true;

  while (millis() - startTime < 90000) {

    int buttonUpState = digitalRead(buttonPinUp);
    int buttonDownState = digitalRead(buttonPinDown);

    if (millis() - lastUpdate >= 500) {
      lastUpdate = millis();
      lcd.createChar(0, humanHead);
      lcd.setCursor(0, 0);
      lcd.write(0);

      static bool standing = true;
      if (standing) {
        lcd.createChar(1, bodyStanding);
        lcd.setCursor(0, 1);
        lcd.write(1);
      } else {
        lcd.createChar(2, bodyWalking);
        lcd.setCursor(0, 1);
        lcd.write(2);
      }
      standing = !standing;

      if (buttonUpState == HIGH) {
        static bool jumping = true;
        if (jumping) {
          lcd.createChar(3, bodyJumping);
          lcd.setCursor(0, 1);
          lcd.write(3);
        } else {
          lcd.setCursor(0, 1);
          lcd.write(1);
        }
      }

      if (buttonDownState == HIGH) {
        static bool bending = true;
        if (bending) {
          lcd.createChar(4, headBending);
          lcd.setCursor(0, 0);
          lcd.write(4);
        } else {
          lcd.setCursor(0, 0);
          lcd.write(0);
        }
      }
      spikeAttack();
    }
    delay(50);
  }
}