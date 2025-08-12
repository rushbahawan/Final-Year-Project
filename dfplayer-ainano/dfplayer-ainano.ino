// libraries import 
#include <SoftwareSerial.h> // extra serial ports
#include <DFRobotDFPlayerMini.h>

SoftwareSerial dfSerial(7, 6);     // dfplayer pins
SoftwareSerial myUART(11, 10);     // esp32 pins (connection)

DFRobotDFPlayerMini dfPlayer; // object creation to control vol etc

String lastLabel = ""; // reset so no repeated sounds
bool dfPlayerReady = false;  // initialising dfplayer
String readBuffer = "";  // stores string from esp serial connection

unsigned long lastPlayTime = 0;
const unsigned long cooldown = 2000; // 2 seconds(wait)

void setup() {
  Serial.begin(9600);
  dfSerial.begin(9600);  //communication speeds
  myUART.begin(115200); //esp has a higher speed

  dfSerial.listen(); // activate serial connection
  if (dfPlayer.begin(dfSerial)) { // initialisation loop
    Serial.println(" DFPlayer initialized.");
    dfPlayer.volume(20); // volume
    dfPlayerReady = true;
  } else {
    Serial.println(" DFPlayer init failed.");
  }

  myUART.listen(); // listen to esp
}

void loop() {
  String raw = readObject(); // read data from esp/ switch to esp
  if (raw.length() > 0) {
    Serial.println(" Raw object data: " + raw);
    String label = extractLabel(raw);
    label.trim();
    label.toLowerCase(); // object label to lowercase

    if (label != "" && label != lastLabel && isConfident(raw)) { //not empty, diff, passes threshold – play sound
      lastLabel = label;
      Serial.println(" New object detected: " + label);
      playObjectSound(label);
    } else {
      Serial.println(" Skipping — same label or low confidence");
    }
  }
}
// reads object data (connection)
String readObject() {
  while (myUART.available()) {
    char c = myUART.read();
    if (c == '\n') { // full message recieved
      String result = readBuffer; // store
      readBuffer = ""; 
      result.trim();
      return result; //clear
    } else {
      readBuffer += c;
    }
  }
  return "";
}
// incoming data format
String extractLabel(String raw) {
  int firstComma = raw.indexOf(',');
  int secondComma = raw.indexOf(',', firstComma + 1);
  if (firstComma == -1 || secondComma == -1) return "";
// extract label
  return raw.substring(firstComma + 1, secondComma);
}
// extract float confidence value
bool isConfident(String raw) {
  int secondComma = raw.indexOf(',', raw.indexOf(',') + 1);
  int thirdComma = raw.indexOf(',', secondComma + 1);
  if (secondComma == -1 || thirdComma == -1) {
    Serial.println(" Confidence parse failed");
    return false;
  }

  String confStr = raw.substring(secondComma + 1, thirdComma);
  float confidence = confStr.toFloat();
  Serial.println(" Confidence: " + String(confidence));
  return confidence > 0; // confidence set according to model accuracy in esp
}

void playObjectSound(String label) {
  if (!dfPlayerReady) {
    Serial.println(" DFPlayer not ready"); // dfplayer initialization check
    return;
  }

  unsigned long now = millis();
  if (now - lastPlayTime < cooldown) {
    Serial.println("Cooldown active — skipping playback");
    return;
  }
  lastPlayTime = now; // current time updated

  Serial.println(" Switching to DFPlayer..."); // switch to dfplayer
  dfSerial.listen();
  delay(50);

  Serial.println(" Playing sound for: " + label); // sound played according to the label
  int track = getTrackNumber(label);
  Serial.println(" Track number: " + String(track));

  if (track > 0) {
    dfPlayer.play(track); //play track if available
    delay(100);
  } else {
    Serial.println(" No track mapped for: " + label);
  }

  Serial.println(" Switching back to myUART..."); // switch to esp
  myUART.listen();
}

int getTrackNumber(String label) { //labels for tracks in sd card
  if (label == "stairs") return 1;
  if (label == "tree") return 4;
  if (label == "phone") return 5;
  if (label == "no_objects") return 3;
  if (label == "laptop") return 6;
  return 0;
}
