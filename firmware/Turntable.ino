// pin functions  on Arduino
#define stp 5
#define dir 4
#define MS1 11
#define MS2 10
#define MS3 8
#define EN  12
#define OTT1 2
#define OTT2 7

// transmission ratio 1:5 or 1:10, 200 steps, 16 microsteps;
#define TRANSMISSION 5

#define SPR 200*16*TRANSMISSION

//per trigger event table should rotate by 1.125 deg
#define TRIG_STEP_WIDTH 10*TRANSMISSION

// global variables
int x;
int y;
int val;
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
    Serial.println("Steps per Revolution:");
    Serial.println(SPR);

    Serial.println();

    // Pull enable pin low to set FETs active and allow motor control
    digitalWrite(EN, LOW);
}


void loop() {
    if (Serial.available()) {

        byte num = Serial.readBytesUntil('\n', serialBuffer, 128);
        if(num == 0) {
            return;
        }

        if(strcmp(serialBuffer, "spr", 3) == 0) {
            Serial.println(SPR);
        }

        if(strcmp(serialBuffer, "move", 4) == 0) {
            steps = Serial.parseInt();
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

        if(strcmp(serialBuffer, "trig", 4) == 0) {
            digitalWrite(OTT2, LOW);
            delay(1); // 1ms
            digitalWrite(OTT2, HIGH);
            Serial.println("OK");
        }

    }
    else {
        val = digitalRead(OTT1);
        if (val == 0) {
            Step(TRIG_STEP_WIDTH);
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
        if (delvar > 150 && x < steps/2) {
            delvar -= 2*delvar/(4*x+1);
        }
        if ((steps > 1752 && x > steps - 876) || (steps <= 1752 && x >= steps/2)) {
            delvar -= 2*delvar/(4*(x-steps-1)+1);
        }
    }
}

