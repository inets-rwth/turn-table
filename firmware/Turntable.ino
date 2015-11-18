//http://www.embedded.com/design/mcus-processors-and-socs/4006438/Generate-stepper-motor-speed-profiles-in-real-time

// pin functions  on Arduino
#define stp 5
#define dir 4
#define MS1 11
#define MS2 10
#define MS3 8
#define EN  12
#define OTT1 2
#define OTT2 7

// global variables
int steps;
int x;
int y;
int val;
double delvar;

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
  Serial.println(); 
  
  // Pull enable pin low to set FETs active and allow motor control
  digitalWrite(EN, LOW); 
}

void loop() {
  if (Serial.available()) {
    steps = Serial.parseInt();
    
    if (steps < 0) {
      digitalWrite(dir, LOW);
      steps = abs(steps);
    }
    else digitalWrite(dir, HIGH);
	
	// transmission ratio 1:5 or 1:10, 200 steps, 16 microsteps;
	
    if (steps != 0 && steps != 32767) {
      Step();	  
      Serial.println("Rotation done");
    }
    if (steps == 32767) {
      digitalWrite(OTT2, LOW);
      delay(100);
      digitalWrite(OTT2, HIGH);
      Serial.println("Requested other turntable to rotate");
    }
  }
  else {
    val = digitalRead(OTT1);
    if (val == 0) {
      steps = 100;
      Step();
      Serial.println("Rotation done");
      steps = 0;
      delay(100);
  }
  delay(10);
  }
}

void Step(){  
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
