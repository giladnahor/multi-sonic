/*

  Ultrasonic Sensor HC-SR04
  Multi Sensor support with low latency
  Crated by Gilad Nahor.

  This code supports using more than one sensor in parallel.
  This is usfull if latency is an issue.
  Each read can take up to 0.2s depending on the distance and the measurment success.
  Therefore it is usful to do more than one measurment in parrallel.
  The max delay can be caused due to long distance or by missing to echo.
  In this case the sensor will delay the end of the measurment and will give a very long distance....

  To speed up things we monitor all echo pins in parrallel.
  In addition we have a timeout configuration which can be set by the max distance required in the project.
  When this timout is reached the function will finish even if the sensor is still busy.
  In this case you will need to wait before starting the next measurment but you will be free to run other code.

  If one of the measurments failed the function will return 1. If all passed it will return 0.
  See example code i =n the bottom
*/


//parameters
#define NumOfSensors 3
const int TIMEOUT = 30000;//in uSec. For max range M in meters set: TIMEOUT = M x 6060
//Explanation (M x 2(round trip) ) / 330(speed of sound) x 1e6 (for uSec)

// defines pins numbers
const int TRIG_PIN = 7;
const int ECHO_PIN0 = 8;
const int ECHO_PIN1 = 6;
const int ECHO_PIN2 = 4;
const int echoPins[] = {ECHO_PIN0, ECHO_PIN1, ECHO_PIN2};

//Setup function
void ultrasonic_setup() {
  Serial.begin(9600); // Starts the serial communication
  pinMode(TRIG_PIN, OUTPUT); // Sets the trigPin as an Output
  for (int i = 0; i < NumOfSensors; i++) {
    pinMode(echoPins[i], INPUT); // Sets the echoPins as an Input
  }
}

//Read sensors function
int readSensors(int distance[]) {
  long duration[NumOfSensors];

  int done[NumOfSensors];
  int allDone;

  //Resetting done flags
  for (int i = 0; i < NumOfSensors; i++) {
    done[i] = 0;
  }
  allDone = 0;

  //Checking last mesurment was complete (Should be done if last measurment as ended by timeout)
  while (!allDone) {
    allDone = 1;
    for (int i = 0; i < NumOfSensors; i++) {
      if (digitalRead(echoPins[i])) {
        allDone = 0;
      }
    }
  }

  //Resetting done flags
  for (int i = 0; i < NumOfSensors; i++) {
    done[i] = 0;
  }
  allDone = 0;

  unsigned long time[NumOfSensors];//Array holding time samples for all sensors
  unsigned long Basetime;//Array holding time samples for all sensors

  // Clears the trigPin
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  Basetime = micros();//Begin count
  delayMicroseconds(470);//delay from trig to start of echo pulse is 0.46ms during this time the ultrasonic pulse is sent.

  // The echo pin HIGH period is the sound wave travel time in microseconds
  while (!allDone) {
    for (int i = 0; i < NumOfSensors; i++) {
      if (!done[i] && !digitalRead(echoPins[i])) {//If this measurment was not done yet and this echo pin in LOW measure time
        time[i] = micros();
        done[i] = 1;
      }
    }
    //Check if all measurments are done
    allDone = 1;
    for (int i = 0; i < NumOfSensors; i++) {
      if (!done[i]) allDone = 0;
    }
    //Check TIMEOUT
    if ((micros() - Basetime) > TIMEOUT) {
      allDone = 1;
      //Serial.print("bummer ");
      return 1;//Read timed out
    }

  }
  for (int i = 0; i < NumOfSensors; i++) {
    duration[i] = time[i] - Basetime - 460;//delay from trig to start of pulse is 0.46ms
    // Calculating the distance
    distance[i] = done[i] ? (int) duration[i] * 0.017 : -1; //If measurment failed return -1
  }
  return 0;//All good Man
}

//Example code
// defines variables
int distance[NumOfSensors];//Measurments vector in cm

void setup() {
  ultrasonic_setup();
}
void loop() {

  // Prints the distance on the Serial Monitor
  //Serial.print("Distance: ");
  if (!readSensors(distance)) {
    for (int i = 0; i < NumOfSensors; i++) {
      Serial.print(distance[i]);
      Serial.print(" ");
    }
    Serial.println();
  }
}

