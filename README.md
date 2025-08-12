📌 Overview
This project is an assistive device designed for visually impaired users.  
It integrates obstacle detection, fire detection, GPS location tracking, emergency SMS/call functionality, and AI-based object detection with audio feedback.  
The system is divided into three main parts:

1. Part 1 – Sensors Module (Arduino UNO)  
   - Detects obstacles using an ultrasonic sensor (HC-SR04)  
   - Detects fire using a flame sensor (KY-026)  
   - Alerts the user with a buzzer  

2. Part 2 – GPS & GSM Module (Arduino Nano)  
   - Sends GPS location via SMS  
   - Makes an emergency call using push buttons, mic, and speaker  
   - Uses SIM800L GSM and NEO-6M GPS modules  

3. Part 3 – AI Object Detection (ESP32-CAM + Arduino Nano + DFPlayer Mini)  
   - Detects objects using an Edge Impulse FOMO model  
   - Sends detection results to Arduino Nano via UART  
   - Plays corresponding audio files stored on DFPlayer Mini
