# multi-sonic
Multi Sensor support with low latency for Ultrasonic Sensor HC-SR04

  Created by Gilad Nahor.

  This code supports using more than one sensor in parallel.
  This is usfull if latency is an issue.
  Each read can take up to 0.2s depending on the distance and the measurment success.
  Therefore it is useful to do more than one measurment in parrallel.
  The max delay can be caused due to long distance or by missing to echo.
  In this case the sensor will delay the end of the measurment and will give a very long distance....

  To speed up things we monitor all echo pins in parrallel.
  All trigger pins are connected to the same Arduino pin saving latency and wiring.
  In addition we have a timeout configuration which can be set by the max distance required in the project.
  When this timout is reached the function will finish even if the sensor is still busy.
  In this case you will need to wait before starting the next measurment but you will be free to run other code.

  If one of the measurments failed the function will return 1. If all passed it will return 0.
  
