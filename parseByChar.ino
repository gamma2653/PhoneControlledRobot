/*
       DIY Arduino Robot Arm Smartphone Control
        by Dejan, www.HowToMechatronics.com
*/

#include <SoftwareSerial.h>
#include <Servo.h>

Servo servo01;
Servo servo02;
Servo servo03;
Servo servo04;
Servo servo05;
Servo servo06;

SoftwareSerial Bluetooth(12, 13); // Arduino(RX, TX) - HC-05 Bluetooth (TX, RX)

int servo1Pos, servo2Pos, servo3Pos, servo4Pos, servo5Pos, servo6Pos; // current position
int servo1PPos, servo2PPos, servo3PPos, servo4PPos, servo5PPos, servo6PPos; // previous position
int servo01SP[50], servo02SP[50], servo03SP[50], servo04SP[50], servo05SP[50], servo06SP[50]; // for storing positions/steps
int speedDelay = 20;
int index = 0;
String dataIn = "";
const String END_OF_MSG = ";";

void setup() {
  servo01.attach(11);
  servo02.attach(10);
  servo03.attach(9);
  servo04.attach(8);
  servo05.attach(7);
  servo06.attach(6);
  Serial.begin(9600);
  Bluetooth.begin(9600); // Default baud rate of the Bluetooth module
  Bluetooth.setTimeout(1);
  delay(20);
  // Robot arm initial position
  servo1PPos = 90;
  servo01.write(servo1PPos);
  servo2PPos = 150;
  servo02.write(servo2PPos);
  servo3PPos = 35;
  servo03.write(servo3PPos);
  servo4PPos = 140;
  servo04.write(servo4PPos);
  servo5PPos = 85;
  servo05.write(servo5PPos);
  servo6PPos = 80;
  servo06.write(servo6PPos);
}
void moveServo(Servo servo, int& ppos, int pos, int delayTime) {
  if (ppos > pos) {
    // We use for loops so we can control the speed of the servo
    // If previous position is bigger then current position
    for ( int j = ppos; j >= pos; j--) {   // Run servo down
      servo.write(j);
      delay(delayTime);    // defines the speed at which the servo rotates
    }
  }
  // If previous position is smaller then current position
  if (ppos < pos) {
    for ( int j = ppos; j <= pos; j++) {   // Run servo up
      servo.write(j);
      delay(delayTime);
    }
  }
  ppos = pos;   // set current position as previous position
}
void loop() {
  // Check for incoming data
  if (Bluetooth.available() > 0) {
    dataIn = dataIn + Bluetooth.readString(); // Read the data as string
  }
  // Handle servo motor sliders
  int indexOfEnd = dataIn.indexOf(END_OF_MSG);
  while (indexOfEnd != -1) {
    Serial.print("Data had end of line character: ");
    Serial.println(dataIn);
    String servoType = dataIn.substring(0, 2);
    String datum = dataIn.substring(2, indexOfEnd);
    Serial.print("Possible servo datum: ");
    Serial.println(datum);


    if (dataIn.startsWith("s1")) {
      // Move Servo 1
      servo1Pos = datum.toInt();  // Convert the string into integer
      Serial.print("Moving servo 1 to: ");
      Serial.println(servo1Pos);
      moveServo(servo01, servo1PPos, servo1Pos, 20);
      // Remove command from buffer
      dataIn = dataIn.substring(indexOfEnd + 1, dataIn.length());
    } else if (dataIn.startsWith("s2")) {
      // Move Servo 2
      servo2Pos = datum.toInt();
      Serial.print("Moving servo 2 to: ");
      Serial.println(servo2Pos);
      moveServo(servo02, servo2PPos, servo2Pos, 20);
      dataIn = dataIn.substring(indexOfEnd + 1, dataIn.length());
    } else if (dataIn.startsWith("s3")) {
      // Move Servo 3
      servo3Pos = datum.toInt();
      Serial.print("Moving servo 3 to: ");
      Serial.println(servo3Pos);
      moveServo(servo03, servo3PPos, servo3Pos, 20);
      dataIn = dataIn.substring(indexOfEnd + 1, dataIn.length());
    } else if (dataIn.startsWith("s4")) {
      // Move Servo 4
      servo4Pos = datum.toInt();
      Serial.print("Moving servo 4 to: ");
      Serial.println(servo4Pos);
      moveServo(servo04, servo4PPos, servo4Pos, 20);
      dataIn = dataIn.substring(indexOfEnd + 1, dataIn.length());
    } else if (dataIn.startsWith("s5")) {
      // Move Servo 5
      servo5Pos = datum.toInt();
      Serial.print("Moving servo 5 to: ");
      Serial.println(servo5Pos);
      moveServo(servo05, servo5PPos, servo5Pos, 20);
      dataIn = dataIn.substring(indexOfEnd + 1, dataIn.length());
    } else if (dataIn.startsWith("s6")) {
      // Move Servo 6
      servo6Pos = datum.toInt();
      Serial.print("Moving servo 6 to: ");
      Serial.println(servo6Pos);
      moveServo(servo06, servo6PPos, servo6Pos, 20);
      dataIn = dataIn.substring(indexOfEnd + 1, dataIn.length());
      
    } else if (dataIn.startsWith("SAVE;")) {
      servo01SP[index] = servo1PPos;  // save position into the array
      servo02SP[index] = servo2PPos;
      servo03SP[index] = servo3PPos;
      servo04SP[index] = servo4PPos;
      servo05SP[index] = servo5PPos;
      servo06SP[index] = servo6PPos;
      index++;                        // Increase the array index
      dataIn = dataIn.substring(5, dataIn.length());
    } else if (dataIn.startsWith("RUN;")) {
      runservo();  // Automatic mode - run the saved steps
      dataIn = dataIn.substring(4, dataIn.length());
    } else if (dataIn.startsWith("RESET;")) {
      memset(servo01SP, 0, sizeof(servo01SP)); // Clear the array data to 0
      memset(servo02SP, 0, sizeof(servo02SP));
      memset(servo03SP, 0, sizeof(servo03SP));
      memset(servo04SP, 0, sizeof(servo04SP));
      memset(servo05SP, 0, sizeof(servo05SP));
      memset(servo06SP, 0, sizeof(servo06SP));
      index = 0;  // Index to 0
      dataIn = dataIn.substring(6, dataIn.length());
    } else {
      break;
    }
    indexOfEnd = dataIn.indexOf(END_OF_MSG);
  }


}


// Automatic mode custom function - run the saved steps
void runservo() {
  while (dataIn != "RESET") {   // Run the steps over and over again until "RESET" button is pressed
    for (int i = 0; i <= index - 2; i++) {  // Run through all steps(index)
      if (Bluetooth.available() > 0) {      // Check for incomding data
        dataIn = Bluetooth.readString();
        if ( dataIn == "PAUSE") {           // If button "PAUSE" is pressed
          while (dataIn != "RUN") {         // Wait until "RUN" is pressed again
            if (Bluetooth.available() > 0) {
              dataIn = Bluetooth.readString();
              if ( dataIn == "RESET") {
                break;
              }
            }
          }
        }
        // If speed slider is changed
        if (dataIn.startsWith("ss")) {
          String dataInS = dataIn.substring(2, dataIn.length());
          speedDelay = dataInS.toInt(); // Change servo speed (delay time)
        }
      }
      // Servo 1
      moveServo(servo01, servo01SP[i], servo01SP[i + 1], speedDelay);

      // Servo 2
      moveServo(servo02, servo02SP[i], servo02SP[i + 1], speedDelay);

      // Servo 3
      moveServo(servo03, servo03SP[i], servo03SP[i + 1], speedDelay);

      // Servo 4
      moveServo(servo04, servo04SP[i], servo04SP[i + 1], speedDelay);

      // Servo 5
      moveServo(servo05, servo05SP[i], servo05SP[i + 1], speedDelay);

      // Servo 6
      moveServo(servo06, servo06SP[i], servo06SP[i + 1], speedDelay);
    }
  }
}
