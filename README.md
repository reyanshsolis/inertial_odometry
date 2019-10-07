# AGV_Encoders_IMU

# Project Goal

To develope accurate and precise encoders using two Innertial Mesurement Units

# Components Used

1. IMU
2. Communication Modules(XBEE or NRF)
3. Arduino Nano
4. Arduino Mega
5. 9V Battery

# Description

This paper describes the design and development of a non-intrusive inertial sensor used for determining the odometry of a ground vehicle. The sensors are meant to be placed on the rim of the wheels of a ground vehicle and determine the angular velocity of wheels and the angular velocity of the vehicle about its center axis perpendicular to the ground plane. The mathematical model incorporates the electronic gyroscope data of left and right wheel sensor and the relationship between the angular velocity about the central axis of wheels on either side while turning, to calculate the turning angular velocity of car and angular velocities of wheels which in turn determines the linear velocity of the car. These sensors can be used to replace the conventional optical or hall-effect based speed sensors which are difficult to be used with existing models of the ground vehicle as they require specific placement and specially designed mounts. 
The primary advantage of this sensor is that it can be placed at any position on the wheel, at any angle with respect to the wheel plane. The determined parameters do not depend on the placement of the sensor on the wheel in any way, as it only requires the resultant of angular velocities on each tire. Multiple noises and bias filters such as Kalman Filter for bias reduction and Extended Kalman Filter and Madgwick Filter among others for noise reduction have been tested on this model and the obtained results have been compared and analyzed in this paper with respect to the quality of the output, computation time and resultant lag.
Different modules for wireless communication (nrf24l01+, Zigbee Xbee S2C) were also used for obtaining the best performance for this specific purpose and the results have been analyzed in terms of data rate, range and security, and reliability among others. This sensor has been tested on different types of vehicles such as cars, rickshaws and mobile robots such as Husky and Jackal under varying road conditions and the performance was compared with optical encoders that are currently being used for odometry estimation.   

