#include <Wire.h>
#include <LiquidCrystal_I2C.h>

const int ledPinRed = 8;
const int ledPinGreen = 9;
const int buttonPinUp = 6;
const int buttonPinRight = 3;
const int buttonPinDown = 5;
int i = 15;

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

byte headSaving[] = {
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

void loop() {
  digitalWrite(ledPinRed, LOW);
  digitalWrite(ledPinGreen, LOW);
  int buttonUpState = digitalRead(buttonPinUp);
  int buttonRightState = digitalRead(buttonPinRight);
  int buttonDownState = digitalRead(buttonPinDown);
  unsigned long timeAfterTitle = millis();
  unsigned long lastUpdate = millis();
  Serial.println(timeAfterTitle);

  while (millis() - startTime < 90000) {
    lcd.createChar(0, humanHead);
    lcd.setCursor(0, 0);
    lcd.write(0);

    if (millis() - lastUpdate >= 500) {
      lastUpdate = millis();

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
    }
    delay(50);

    if (buttonPinUp == HIGH) {
      
    }
  }
}