#include <EEPROM.h>
// pin functions  on Arduino
#define stp 5
#define dir 4
#define MS1 11
#define MS2 10
#define MS3 8
#define EN  12
#define OTT1 2
#define OTT2 7
#define TRANSMISSION 5
#define EEPROM_ADDR_TRANS 0
// transmission ratio 1:5 or 1:10, 200 steps, 16 microsteps;
//per trigger event table should rotate by 1.125 deg
// global variables
int x;
int y;
int val;
int transmission = TRANSMISSION;
int spr = 200 * 16 * transmission;
int trig_step_width = 10 * transmission;
double delvar;
char serialBuffer[128];

void setup() {
    pinMode(stp, OUTPUT);
    pinMode(dir, OUTPUT);
    pinMode(MS1, OUTPUT);
    pinMode(MS2, OUTPUT);
    pinMode(MS3, OUTPUT);
    pinMode(EN, OUTPUT);
    pinMode(OTT1, INPUT_PULLUP); //From other Turntable
    pinMode(OTT2, OUTPUT); //To other Turntable

    digitalWrite(EN, HIGH);
    digitalWrite(OTT2, HIGH);

    // 16 microsteps
    digitalWrite(MS1, HIGH);
    digitalWrite(MS2, HIGH);
    digitalWrite(MS3, HIGH);

    Serial.begin(9600); // Open Serial connection
    Serial.println("Ready");

    // Pull enable pin low to set FETs active and allow motor control
    digitalWrite(EN, LOW);

    int trans_tmp = EEPROM.read(EEPROM_ADDR_TRANS);
    if(trans_tmp != 0xFF) {
        transmission = trans_tmp;
        spr = 200 * 16 * transmission;
        trig_step_width = 10 * transmission;
    }
}

void loop() {
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
        spr = 200 * 16 * transmission;
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
        digitalWrite(dir, LOW);
        steps = abs(steps);
      }
      else {
        digitalWrite(dir, HIGH);
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
      delay(2); //make sure to not catch the same trigger again
    }
  }
}

void Step(int steps)
{
  delvar = 6000;
  for (x = 1; x <= steps; x++)  {
    digitalWrite(stp, HIGH); //Trigger one step forward
    delayMicroseconds(delvar);
    digitalWrite(stp, LOW);
    delayMicroseconds(delvar);
    if (delvar > 150 && x < steps / 2) {
      delvar -= 2 * delvar / (4 * x + 1);
    }
    if ((steps > 1752 && x > steps - 876) || (steps <= 1752 && x >= steps / 2)) {
      delvar -= 2 * delvar / (4 * (x - steps - 1) + 1);
    }
  }
}

