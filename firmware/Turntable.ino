// pin functions on Arduino
#define stp 5
#define dir 4
#define MS1 11
#define MS2 10
#define MS3 8
#define EN  12
#define LED 13

// global variables
int angle = 5;
int steps;
int x;
int y;

void setup() {
  pinMode(stp, OUTPUT);
  pinMode(dir, OUTPUT);
  pinMode(MS1, OUTPUT);
  pinMode(MS2, OUTPUT);
  pinMode(MS3, OUTPUT);
  pinMode(EN, OUTPUT);
  pinMode(LED, OUTPUT);

  digitalWrite(EN, HIGH);
  
  // 16 microsteps
  digitalWrite(MS1, HIGH);
  digitalWrite(MS2, HIGH);
  digitalWrite(MS3, HIGH); 
  
  analogWrite(LED, 50);
  
  Serial.begin(9600); // Open Serial connection
  Serial.println("Ready");
  Serial.println(); 
  
  // Pull enable pin low to set FETs active and allow motor control
  digitalWrite(EN, LOW); 
}

void loop() {
  if (Serial.available()) {
    angle = Serial.parseInt();
    
    if (angle < 0) {
      digitalWrite(dir, LOW);
      angle = abs(angle);
    }
    else digitalWrite(dir, HIGH);
	
	// transmission ration 5; 200 steps, 16 microsteps;
    steps = angle * 44.44; 
    
	// allow only half step positions
	// steps = steps - (steps % 8);
	
    if (steps != 0) {
      Step();	  
      Serial.println("done"); 
    }
  }
  else delay(10);
}


void Step(){  
  for (x = 1; x < steps; x++)  {
    digitalWrite(stp, HIGH); //Trigger one step forward
    delay(1.5);
    digitalWrite(stp, LOW);
    if ((x < 480 && x <= steps/2) || (x > steps - 480 && x > steps/2)) delay(0.5);
    if ((x < 240 && x <= steps/2) || (x > steps - 240 && x > steps/2)) delay(0.5);
    if ((x < 160 && x <= steps/2) || (x > steps - 160 && x > steps/2)) delay(0.5);
    if ((x < 80 && x <= steps/2) || (x > steps - 80 && x > steps/2)) delay(1);
    if (x % 160 == 100) analogWrite(LED, 255);
    if (x % 160 == 0) analogWrite(LED, 50);
  } 
}
