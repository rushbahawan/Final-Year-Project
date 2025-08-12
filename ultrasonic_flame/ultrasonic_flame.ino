// pins
const int buzzerPin = 12;       // shared buzzer pin
const int fireSensorPin = 8;    // fire sensor pin
const int trigPin = 9;          // ultrasonic trig pin
const int echoPin = 10;         // ultrasonic echo pin

// variables for ultrasonic sensor
long duration;
int distance;
int safetyDistance;

void setup() {
  Serial.begin(9600); // start serial communication

  // pin modes
  pinMode(buzzerPin, OUTPUT);
  pinMode(fireSensorPin, INPUT);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
}

void loop() {
  // flame sensor 
  int fireValue = digitalRead(fireSensorPin);

  // ultrasonic sensor
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  duration = pulseIn(echoPin, HIGH);
  distance = duration * 0.034 / 2;
  safetyDistance = distance;

  // serial monitor 
  Serial.print("Fire: ");
  Serial.print(fireValue);
  Serial.print("Distance: ");
  Serial.println(distance);

  // buzzer function
  if (fireValue == 0) {
    // flame detected
    tone(buzzerPin, 1000); // 1000 Hz tone
    delay(3000);           // 3 sec delay
  } 
  else if (safetyDistance <= 40) {
    // object close
    tone(buzzerPin, 2000); // 2000 Hz tone
    delay(50);
    noTone(buzzerPin);
    delay(50);
  } 
  else {
    // buzzer off
    noTone(buzzerPin);
  }

  delay(200); // delay time for sensor stability
}
