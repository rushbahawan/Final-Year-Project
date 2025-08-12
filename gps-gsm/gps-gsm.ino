// libraries import 
#include <SoftwareSerial.h>
#include <TinyGPS++.h>

#define CALL_BUTTON A0
#define SMS_BUTTON A1 // analogue pins definition

SoftwareSerial sim800(10, 11);       // GSM pins
SoftwareSerial gpsSerial(2, 3);      // Neo6M GPS pins
TinyGPSPlus gps;  // extract longitude and latitude

bool isCalling = false; // call ongoing off
bool lastCallBtnState = LOW;
bool lastSMSBtnState = LOW;

void setup() {

  pinMode(CALL_BUTTON, INPUT_PULLUP);
  pinMode(SMS_BUTTON, INPUT_PULLUP);  // buttons as input(high)

  sim800.begin(9600);
  gpsSerial.begin(9600); // start serial communication
  Serial.begin(9600);

  Serial.println("System Ready");
  delay(2000);
}

void loop() {
  // read GPS data and give it to tinygps to make sense 
  while (gpsSerial.available()) {
    gps.encode(gpsSerial.read());
  }

  // display GPS data on serial monitor 
  if (gps.location.isValid()) { // is gps able to get coordinates from satellite
    Serial.print("Latitude: ");
    Serial.println(gps.location.lat(), 6);
    Serial.print("Longitude: ");
    Serial.println(gps.location.lng(), 6);
    Serial.print("Satellites: ");
    Serial.println(gps.satellites.value());
    Serial.print("Altitude: ");
    Serial.println(gps.altitude.meters());
    Serial.print("Date: ");
    Serial.println(gps.date.isValid() ? gps.date.value() : 0);
    Serial.print("Time: ");
    Serial.println(gps.time.isValid() ? gps.time.value() : 0);
    Serial.println("---------------------------");
  } else {
    Serial.println("Waiting for GPS fix..."); // waiting for coordinates
  }

  int callBtnState = digitalRead(CALL_BUTTON); // current state of buttons
  int smsBtnState = digitalRead(SMS_BUTTON);

  // call logic
  if (callBtnState == HIGH && lastCallBtnState == LOW) { // button pressed
    if (!isCalling) { // call not ongoing
      Serial.println("Calling Cmdt...");
      makeCall("+923015558222");
      isCalling = true;
    } else { // call ongoing 
      Serial.println("Hanging Up...");
      hangUp();
      isCalling = false;
    }
    delay(500);
  }

  // SMS logic
  if (smsBtnState == HIGH && lastSMSBtnState == LOW) {
    Serial.println("Sending SMS...");
    String msg = "Emergency Alert!\n";

    if (gps.location.isValid()) { // coordinates found
      msg += "Lat: " + String(gps.location.lat(), 6) + "\n";
      msg += "Lon: " + String(gps.location.lng(), 6) + "\n"; // message format 
    } else {
      msg += "GPS not valid\n"; // coordinates not found / cant connect to satellite
    }

    sendSMS("+923015558222", msg);
    Serial.println("SMS Sent");
    delay(500);
  }

  lastCallBtnState = callBtnState; // button state
  lastSMSBtnState = smsBtnState;
  delay(50); 
}
// call action
void makeCall(String number) {
  sim800.println("AT");
  delay(1000);
  sim800.println("ATD" + number + ";"); // gsm module calls the number
  delay(3000);
  Serial.println("Call initiated to " + number);
}

void hangUp() {
  sim800.println("ATH");
  delay(1000);   // hang up
  Serial.println("Call ended");
}
// sms action
void sendSMS(String number, String message) {
  sim800.println("AT"); // gsm module commands use AT
  delay(1000);
  sim800.println("AT+CMGF=1");
  delay(1000);
  sim800.println("AT+CMGS=\"" + number + "\"");
  delay(1000);
  sim800.print(message);
  delay(500);
  sim800.write(26); // CTRL+Z end of message so send sms inn gsm lang
  delay(5000);
  Serial.println("SMS sent: " + message);
}
