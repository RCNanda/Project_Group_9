INTRODUCTION
------------------------

	•	There are two files- one is  AccidentDetection.ino and another one is Circuit Diagram.png
	•	AccidentDetection.ino : This file contains the programming demonstration explaining all the program methods, 
		variables and constant values. The file is in .ino format.
	•	Circuit Diagram.png: This file contains the pictorial representation of the circuit diagram of the proposed model, 
		marking the necessary components. The file is in .png format.


REQUIREMENTS
-----------------------
	•	Language: The programming in AccidentDetection.ino file is written in C++ programming language. 
	•	Package:  The SoftwareSerial library i.e. <SoftwareSerial.h> has been developed to allow serial communication 
		on other digital pins of the Arduino (excluding 0 and 1), using software to replicate the functionality.
		The <LiquidCrystal.h> library is used to allow an Arduino board to control LiquidCrystal displays (LCDs) based on a compatible chipset, 
		which is found on most text-based LCDs.


EXECUTION PROCESS
------------------------------
	I.	At first Arduino IDE software version 1.8.13 was installed.
	II.	After that, a void setup() function is used to initialize hardware and software serial communication, LCD, GPS, GSM module and Accelerometer.
	III.	Next, a void loop() function is used to read accelerometer axis values and implement a calculation to extract changes with the help of samples 
		that are taken in Accelerometer Calibration above.
	IV.	Then the program is compiled using sketch option and run using upload option.
	V.	GPS ,GSM module ,LCD are connected to the arduino via circuits. TX port of GSM module is connected to D10 of arduino. RX and TX ports of GPS 
		are connected to the D2 and D3  of arduino. An LCD of 16x2 and a buzzer is also connected to arduino.  
	VI.	Now in hardware, at first the Accident Detection model is starting and the GSM module is initializing. 
	VII.	Then the accelerometer calibrates by storing the default xyz coordinate values (set to origin) of the vehicle with respect to the road. 
	VIII.	Then the GPS module starts and receives the starting speed and location of the vehicle.
	IX.	Now, the GPS is detecting the speed and location of the vehicle continuously, which will be displayed in the LCD. When the accelerometer detects 
		the car is being tilted to one side, the alarm goes off and the message pops in the LCD, accident detected, car is tilting.
	X.	The current speed, latitude and longitude of the car will be displayed and a message will pop up “SMS sent”. This means all the necessary information 
		has already been sent to both vehicle owner and Emergency services mobile number. This has been done by the GSM module.




REQUIRED INPUT
-------------------------

	•	Hardware:
		I.	GPS Receiver
		II.	GSM Modem SIM900D
		III.	Accelerometer ADXL335
		IV.	Arduino Uno R3
		V.	Resistor
		VI.	Buzzer
	•	Software:
		I.	Arduino IDE



SHARING INFORMATION
---------------------------------
	•	GitHub Link:  https://github.com/RCNanda/Project_Group_9.git
	•	Report Link: https://docs.google.com/document/d/1tVjX-Ytbnayio-oAIIIiQv8ZM2BGYNlIyRaft5aOmJw/edit?usp=sharing

