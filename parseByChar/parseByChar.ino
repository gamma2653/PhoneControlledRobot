/*
       DIY Arduino Robot Arm Smartphone Control
        by Dejan, www.HowToMechatronics.com
        Updated by Christopher, www.github.com/gamma2653
*/

#include <SoftwareSerial.h>
#include <Servo.h>

#define DEBUG

#define N_SERVOS 6
#define SERVO_PIN_START 11
#define USE_PIN_POOL

Servo servo[N_SERVOS];

SoftwareSerial Bluetooth(12, 13); // Arduino(RX, TX) - HC-05 Bluetooth (TX, RX)


int servoPos[N_SERVOS]; // Current position
int servoPPos[N_SERVOS]; // Previous position
int servoSP[N_SERVOS][50]; // for storing positions/steps
int speedDelay = 20; // in miliseconds
int servo_unit = 10;
int index = 0; // Current servo being set
String dataIn = ""; // data buffer for bluetooth module
const String END_OF_MSG = ";"; // Delimiter used to interpret incoming signals
const String SPLIT_OF_MSG = ":";
const int PIN_POOL[] = {11, 10, 9, 8, 7, 6};

//If necessary, intialize servoPos to initial positions
//servoPos = {...}

void setup() {
  for(int i = 0; i<N_SERVOS; i++){
  #ifdef USE_PIN_POOL
    servo[i].attach(PIN_POOL[i]);
  #else
    servo[i].attach(SERVO_PIN_START-i);
  #endif
  }
#ifdef DEBUG
  Serial.begin(9600);
#endif
  Bluetooth.begin(9600); // Default baud rate of the Bluetooth module
  Bluetooth.setTimeout(1);
  delay(20); // Wait a second in case Bluetooth is also still booting
}


void move_servos() {
  // Cycle through servos to update
  for(int i = 0; i < sizeof(servo)/sizeof(Servo); i++){
    int move_to = servoPPos[i];
    if(abs(servoPPos[i] - servoPos[i]) < servo_unit){
      // If servo is almost at unit, just complete it (close enough principle)
      move_to = servoPos[i];
    }else if(servoPPos[i] > servoPos[i]){
      // If servo farther than it needs to be, subtract unit from where it is to find where it will be
      move_to = servoPPos[i]-servo_unit;
    }else if(servoPPos[i] < servoPos[i]){
      // If servo is less than it should be, add unit
      move_to = servoPPos[i]+servo_unit;
    }else{
      // Should have been caught by first case, skip servo
      continue;      
    }
    servo[i].write(move_to);
  #ifdef DEBUG
    Serial.println("Servo "+String(i)+" moving from "+String(servoPPos[i])+" to "+String(move_to));
  #endif
    servoPPos[i] = move_to;
  }
}


/*
 * Meant to be used in loop() while not holding up other operations. Returns non-empty when command is found.
 * CMD format: `s[servo_num]:[angle];`
 */
void lazy_process_command(SoftwareSerial& port, String& buffer){

  if(port.available() > 0){
    // Update buffer with new values
    buffer = buffer + port.readString();
    // Check for end of command
    int indexOfEnd = buffer.indexOf(END_OF_MSG);
    // In case of multiple commands that need processing, process them all.
    while(indexOfEnd != -1){
      int indexOfSplit = buffer.indexOf(SPLIT_OF_MSG);
      if(indexOfSplit == -1){
        // Malformed command, forward to next command and continue
        buffer = buffer.substring(indexOfEnd+1);
        indexOfEnd = buffer.indexOf(END_OF_MSG);
        continue;
      }
      String servoType = buffer.substring(0, indexOfSplit);
      String datum = buffer.substring(indexOfSplit+1, indexOfEnd);
    #ifdef DEBUG
      Serial.println("Got command: "+buffer.substring(0, indexOfEnd+1));
      Serial.println("Understood- Servo:["+servoType+"] datum:["+datum+"]");
    #endif
      // Read from buffer, now forward it
      buffer = buffer.substring(indexOfEnd+1);
      indexOfEnd = buffer.indexOf(END_OF_MSG);
      // Process data collected
      int servo_id = servoType.substring(1).toInt(); //If properly formatted, this should give servo_num
      int angle = datum.toInt();
    #ifdef DEBUG
      Serial.println("Processing command: "+String(servo_id)+" to "+String(angle));
    #endif
      // Got the data we need, now set it!      
      servoPos[servo_id] = angle;
      // And that's it!      
    }
  }
}


void loop() {
  lazy_process_command(Bluetooth, dataIn);
  move_servos();
}

// TODO: Implement servoSP
