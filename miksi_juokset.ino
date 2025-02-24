#include <Wire.h>
#include <LiquidCrystal_I2C.h>

const int ledPinRed = 8;
const int ledPinGreen = 9;
const int buttonPinUp = 6;
const int buttonPinRight = 3;
const int buttonPinDown = 5;

unsigned long startTime; //
unsigned long lastUpdate = 0;
bool standing = true;
bool jumping = true;
bool bending = true;
long randNum;

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

  startTime = millis();
}

void spikeAttack() {
    int buttonUpState = digitalRead(buttonPinUp);
    int buttonDownState = digitalRead(buttonPinDown);
    static int spikePos = 15;
    static unsigned long lastSpikeMove = 0;
    static bool spikeActive = false;
    static String randAttack = "";
    
    if (!spikeActive) {
        spikePos = 15;
        randAttack = String(random(1, 3));
        spikeActive = true;
    }

    if (millis() - lastSpikeMove >= 300) {
      lastSpikeMove = millis();
        
      lcd.setCursor(spikePos, randAttack == "1" ? 0 : 1);
      lcd.write(' ');

      spikePos--;

      if (spikePos >= 0) {
        lcd.createChar(6, randAttack == "1" ? upperSpike : lowerSpike);
        lcd.setCursor(spikePos, randAttack == "1" ? 0 : 1);
        lcd.write(6);

        if (spikePos == 0) {
          if (randAttack == "1") {
            if (buttonDownState == HIGH) {
              lcd.createChar(7, headUnderSpike);
              lcd.setCursor(0, 0);
              lcd.write(7);
            } else {
              lcd.clear();
              lcd.print("GAME OVER");
              delay(2000);
              lcd.clear();
              spikeActive = false;
              return;
            }
          } else if (randAttack == "2") {
              if (buttonUpState == HIGH) {
                lcd.createChar(8, bodyOverSpike);
                lcd.setCursor(0, 1);
                lcd.write(8);
            } else {
              lcd.clear();
              lcd.print("GAME OVER");
              delay(2000);
              lcd.clear();
              spikeActive = false;
              return;
            }
          } else {
            spikeActive = false;
          }
        }
      } else {
          spikeActive = false;
      }
    }
}


void loop() {
  if (millis() - startTime >= 90000) return;

  digitalWrite(ledPinRed, LOW);
  digitalWrite(ledPinGreen, LOW);

  int buttonUpState = digitalRead(buttonPinUp);
  int buttonDownState = digitalRead(buttonPinDown);

  if (millis() - lastUpdate >= 500) {
    lastUpdate = millis();

    if (standing) {
      lcd.createChar(0, humanHead);
      lcd.setCursor(0, 0);
      lcd.write(0);

      lcd.createChar(1, bodyStanding);
      lcd.setCursor(0, 1);
      lcd.write(1);
    } else {
      lcd.createChar(0, humanHead);
      lcd.setCursor(0, 0);
      lcd.write(0);

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
    jumping = !jumping;

    if (buttonDownState == HIGH) {
      if (bending) {
        lcd.createChar(4, headBending);
        lcd.setCursor(0, 0);
        lcd.write(4);
      } else {
        lcd.setCursor(1, 0);
        lcd.write(0);
      }
    }

    if (millis() - startTime >= 3000) {
      spikeAttack();
      delay(400);
    }
  }

  delay(50);
}