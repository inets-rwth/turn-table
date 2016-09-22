#include <EEPROM.h>
// pin functions  on Arduino


#define OTT1 2

#define PIN_EN  4
#define PIN_STEP 5
#define PIN_DIR 6

#define OTT2 7

#define MICRO1_PIN 8
#define MICRO2_PIN 9
#define MICRO3_PIN 10

#define TRANSMISSION 10
#define EEPROM_ADDR_TRANS 0
#define MICRO 8

// transmission ratio 1:5 or 1:10, 200 steps, 16 microsteps;
// per trigger event table should rotate by 1.125 deg
// global variables


int transmission = TRANSMISSION;
int spr = 200 * MICRO * TRANSMISSION;
int trig_step_width = 10 * transmission;
double delvar;
char serialBuffer[128];

void setup() {
    pinMode(PIN_STEP, OUTPUT);
    pinMode(PIN_DIR, OUTPUT);
    pinMode(PIN_EN, OUTPUT);
    pinMode(MICRO1_PIN, OUTPUT);
    pinMode(MICRO2_PIN, OUTPUT);
    pinMode(MICRO3_PIN, OUTPUT);

    pinMode(OTT1, INPUT_PULLUP); //From other Turntable
    pinMode(OTT2, OUTPUT); //To other Turntable

    digitalWrite(PIN_EN, LOW);
    digitalWrite(OTT2, HIGH);

    digitalWrite(MICRO1_PIN, HIGH);
    digitalWrite(MICRO2_PIN, HIGH);
    digitalWrite(MICRO3_PIN, LOW);

    Serial.begin(9600); // Open Serial connection

    int trans_tmp = EEPROM.read(EEPROM_ADDR_TRANS);
    if(trans_tmp != 0xFF) {
        transmission = trans_tmp;
        spr = 200 * MICRO * transmission;
        trig_step_width = 10 * transmission;
    }
}

void loop() {
  int val = 0;
  if (Serial.available()) {

    byte num = Serial.readBytesUntil('\n', serialBuffer, 128);

    if (num == 0) {
      return;
    }

    char* tok = strtok(serialBuffer, " \r");
    if(tok == 0) {
      return;
    }

    if (strcmp(tok, "help") == 0) {
      Serial.println("get trig_width : get steps per trigger input");
      Serial.println("get spr : get steps per revolution");
      Serial.println("get trans : get transmission ratio");
      Serial.println("set trans x: set transmission ratio of table");
      Serial.println("move x: move x steps");
      Serial.println("trig : trigger remote table");
    }

    if (strcmp(tok, "get") == 0) {
      char* tok2 = strtok(0, " \r");
      if(tok2 == 0) {
        return;
      }
      if (strcmp(tok2, "spr") == 0) {
        Serial.println(spr);
      }
      if (strcmp(tok2, "trig_width") == 0) {
        Serial.println(trig_step_width);
      }
      if (strcmp(tok2, "trans") == 0) {
        Serial.println(transmission);
      }
    }

    if (strcmp(tok, "set") == 0) {
      char* tok2 = strtok(0, " \r");
      if(tok2 == 0) {
        return;
      }
      if (strcmp(tok2, "trans") == 0) {
        char* tok3 = strtok(0, " \r");
        if(tok3 == 0) {
          return;
        }
        char* next;
        transmission = strtol(tok3, &next, 10);
        EEPROM.write(EEPROM_ADDR_TRANS, transmission);
        spr = 200 * MICRO * transmission;
        trig_step_width = 10 * transmission;
        Serial.println("OK");
      }
    }

    if (strcmp(tok, "move") == 0) {
      char* tok2 = strtok(0, " \r");
      if(tok2 == 0) {
        return;
      }
      char* next;
      int steps = strtol((const char*)tok2, &next, 10);
      if (steps < 0) {
        digitalWrite(PIN_DIR, LOW);
        steps = abs(steps);
      }
      else {
        digitalWrite(PIN_DIR, HIGH);
      }
      if (steps != 0) {
        Step(steps);
        Serial.println("OK");
      }
    }

    if (strcmp(tok, "trig") == 0) {
      digitalWrite(OTT2, LOW);
      delay(1); // 1ms
      digitalWrite(OTT2, HIGH);
      Serial.println("OK");
    }

  }
  else {
    val = digitalRead(OTT1);
    if (val == 0) {
      Step(trig_step_width);
      Serial.println(trig_step_width);
      delay(2); //make sure to not catch the same trigger again
    }
  }
}

void Step(int steps)
{
  int x;
  int dt = 2000;
  bool accel = true;
  int ramp_len = 100;

  for (x = 0; x < steps; x++)  {
    digitalWrite(PIN_STEP, HIGH); //Trigger one step forward
    delayMicroseconds(100);
    digitalWrite(PIN_STEP, LOW);
    delayMicroseconds(dt);

    if(x < ramp_len && ((steps - x) > ramp_len)) {
      if(dt > 150) {
        dt-=5;
      }
    } else {
      accel = false;
    }
    if(!accel && ((steps - x) <= ramp_len)) {
      if(dt < 2000) {
        dt+=5;
      }
    }
  }
}

