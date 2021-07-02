#include<SoftwareSerial.h> 
// The SoftwareSerial library has been developed to allow serial communication on other digital pins of the Arduino (excluding 0 and 1), using software to replicate the functionality
SoftwareSerial Serial1(2,3); // Assigning TX arduino line to pin D2, Assigning RX arduino line to pin D3
SoftwareSerial gps(10,11); // Assigning GPS TX line to D10
#include<LiquidCrystal.h> // This library allows an Arduino board to control LiquidCrystal displays (LCDs) based on a compatible chipset, which is found on most text-based LCDs
LiquidCrystal lcd(4,5,6,7,8,9); // Assigning pins D4 to D9 of the Arduino to the LCD
#define x A1 // Assigning x pin of Accelerometer to A1 pin of arduino
#define y A2 // Assigning y pin of Accelerometer to A2 pin of arduino
#define z A3 // Assigning z pin of Accelerometer to A3 pin of arduino
#define SpeedLimit 60 // Defining Speed Limit of the vehicle
int xsample=0; // Initializing x-axis sample to 0, required for Accelerometer Callibration
int ysample=0; // Initializing y-axis sample to 0, required for Accelerometer Callibration
int zsample=0; // Initializing z-axis sample to 0, required for Accelerometer Callibration
#define samples 10 // Defining sample required for Accelerometer Callibration
#define minVal -50 // Defining minimum sensitivity value of Accelerometer
#define MaxVal 50 // Defining maximum sensitivity value of Accelerometer
int buzzer = 13; // Defining pin D13 of arduino to buzzer
int i=0,k=0; // Declaring global variables
int  gps_status=0; // Variable storing status of GPS in Numeric value
float latitude=0; // Floating type variable to store Latitude of the vehicle
float logitude=0; // Floating type variable to store Longitude of the vehicle                  
String Speed=""; // String variable to store speed of the vehicle acquired from the GPS
String gpsString=""; // String variable to store incoming data from the GPS
char test[]="$GPRMC"; // Recommended minimum specific GPS/Transit data. $GPRMC string mainly contains velocity, time, date and position
// Syntax of $GPRMC:
// $GPRMC,123519.000,A, 7791.0381,N, 06727.4434,E,022.4,084.4,230394,003.1,W*6A
// $GPRMC,HHMMSS.SSS,A,latitude,N,longitude,E,speed,angle,date,MV,W,CMD
void initModule(String cmd, char *res, int t)  // Function to initialize the GSM module and checking its response using AT commands
// AT means ATTENTION. This command is used to control GSM module. After receiving AT Command GSM Module respond with OK. It means GSM module is working fine.
{
  while(1) 
  {
    Serial.println(cmd);
    Serial1.println(cmd);
    delay(100);
    while(Serial1.available()>0)
    {
       if(Serial1.find(res))
       {
        Serial.println(res);
        delay(t);
        return;
       }
       else
       {
        Serial.println("Error");
       }
    }
    delay(t);
  }
}
void setup() // Function to initialize hardware and software serial communication, LCD, GPS, GSM module and Accelerometer
{
  pinMode(buzzer, OUTPUT);
  Serial1.begin(9600); // Opens serial port, sets data rate to 9600 bps
  Serial.begin(9600); // Opens serial port, sets data rate to 9600 bps
  lcd.begin(16,2); // Initializes the interface to the LCD screen, and specifies the dimensions (no.of.column,no.of.rows) of the display.
  lcd.print("Accident Alert  "); // Displaying text in LCD screen
  lcd.setCursor(0,1); // Positions the LCD cursor,i.e., set the location at which subsequent text written to the LCD will be displayed. Syntax: lcd.setCursor(col,row)
  lcd.print("     System     "); // Displaying text in LCD screen 
  delay(2000); // Pauses the program for the amount of time (in milliseconds) specified as parameter, here 2 second
  lcd.clear(); // Clears the LCD screen and positions the cursor in the upper-left corner.
  lcd.print("Initializing"); // Displaying text in LCD screen
  lcd.setCursor(0,1);
  lcd.print("Please Wait..."); // Displaying text in LCD screen
  delay(1000);  
  Serial.println("Initializing...."); // Prints data to the serial port as human-readable ASCII text followed by a carriage return character (ASCII 13, or '\r') and a newline character (ASCII 10, or '\n').
  initModule("AT","OK",1000); // The initModules() method initializes external UI component's instances and links them to local variables. 
  initModule("ATE1","OK",1000);
  initModule("AT+CPIN?","READY",1000);  
  initModule("AT+CMGF=1","OK",1000);     
  initModule("AT+CNMI=2,2,0,0,0","OK",1000);  
  Serial.println("Initialized Successfully");
  lcd.clear();
  lcd.print("Initialized"); // Displaying text in LCD screen
  lcd.setCursor(0,1);
  lcd.print("Successfully"); // Displaying text in LCD screen
  delay(2000);
  lcd.clear();
  lcd.print("Callibrating "); // Accelerometer callibration process is done here 
  lcd.setCursor(0,1);
  lcd.print("Accelerometer"); // Displaying text in LCD screen
  /* In this, we have taken some samples and then find the average values for the x-axis, y-axis, and z-axis. And store them in a variable.
   *  Then we have used these sample values to read changes in accelerometer axis when vehicle gets tilt (accident). */
  for(int i=0;i<samples;i++)   
  {    
    xsample+=analogRead(x); // Reads the value from the specified analog pin, here x-pin, and stores it in xsample
    ysample+=analogRead(y); // Reads the value from the specified analog pin, here y-pin, and stores it in ysample
    zsample+=analogRead(z); // Reads the value from the specified analog pin, here z-pin, and stores it in zsample
  }
  xsample/=samples; // Dividing xsample value by 10
  ysample/=samples; // Dividing ysample value by 10
  zsample/=samples; // Dividing zsample value by 10
  Serial.println(xsample);
  Serial.println(ysample);
  Serial.println(zsample);
  delay(1000);  
  lcd.clear();
  lcd.print("Waiting For GPS"); // Displaying text in LCD screen
  lcd.setCursor(0,1);
  lcd.print("     Signal    "); // Displaying text in LCD screen
  delay(2000);
  gps.begin(9600); // Initializes the GPS, sets data rate to 9600 bps
  get_gps(); // Calls get_gps() function to get GPS coordinates
  show_coordinate(); // Calls show_coordinate() function to extract coordinates from the GPS string and convert them into Decimal values
  delay(2000);
  lcd.clear();
  lcd.print("GPS is Ready"); // Displaying text in LCD screen
  delay(1000);
  lcd.clear();
  lcd.print("System Ready"); // Displaying text in LCD screen
  Serial.println("System Ready..");
}
void loop() // Function to read accelerometer axis values and implementing a calculation to extract changes with the help of samples that are taken in Accelerometer Calibration above
// If any changes are more or less than defined level then Arduino sends a message to the predefined number
{
    int value1=analogRead(x);
    int value2=analogRead(y);
    int value3=analogRead(z);
    int xValue=xsample-value1; // calculating sensitivity value of the accelerometer with respect to x-axis
    int yValue=ysample-value2; // calculating sensitivity value of the accelerometer with respect to y-axis
    int zValue=zsample-value3; // calculating sensitivity value of the accelerometer with respect to z-axis    
    Serial.print("x=");
    Serial.println(xValue); 
    Serial.print("y=");
    Serial.println(yValue);
    Serial.print("z=");
    Serial.println(zValue);
    if(xValue < minVal || xValue > MaxVal  || yValue < minVal || yValue > MaxVal  || zValue < minVal || zValue > MaxVal) // checking if the sensitivity of the accelerometer is not in the pre-defined limits
    {
      get_gps(); // calling function to acquire gps coordinates
      show_coordinate(); // calling function for extracting coordinates from the GPS string and convert them into Decimal values
      lcd.clear();
      lcd.print("Sending SMS "); // Displaying text in LCD screen
      Serial.println("Sending SMS");
      Send();
      Serial.println("SMS Sent");
      delay(2000);
      lcd.clear();
      lcd.print("System Ready"); // Displaying text in LCD screen
    }       
}
void gpsEvent() // Function to get GPS status
{
  gpsString=""; // Initializing gpsString to null value
  while(1)
  {
    while (gps.available()>0) // Serial incoming data from GPS
    {
      char inChar = (char)gps.read();
      gpsString+= inChar; // Store incoming data from GPS to temparary string str[]
      i++; 
  //  Serial.print(inChar);
      if (i < 7)                      
      {
        if(gpsString[i-1] != test[i-1]) // Check for right string
        {
          i=0;
          gpsString=""; //$GPRMC
        }
      }
      if(inChar=='\r')
      {
        if(i>60)
        {
          gps_status=1;
          break;
        }
        else
        {
          i=0;
        }
      }
    }
    if(gps_status)
      break;
  }
}
void get_gps() // Function to get GPS coordinates
{
  lcd.clear();
  lcd.print("Getting GPS Data"); // Displaying text in LCD screen
  lcd.setCursor(0,1);
  lcd.print("Please Wait....."); // Displaying text in LCD screen
   gps_status=0;
   int x=0;
   while(gps_status==0)
   {
    gpsEvent();
    int str_lenth=i;
    coordinate2dec();
    i=0;
    x=0;
    str_lenth=0;
   }
}
void show_coordinate() // Function for extracting coordinates from the GPS string and convert them into Decimal values
{
    lcd.clear();
    lcd.print("Lat:"); // Displaying text in LCD screen
    lcd.print(latitude); // Displaying text in LCD screen
    lcd.setCursor(0,1);
    lcd.print("Log:"); // Displaying text in LCD screen
    lcd.print(logitude); // Displaying text in LCD screen
    Serial.print("Latitude:"); 
    Serial.println(latitude);
    Serial.print("Longitude:");
    Serial.println(logitude);
    Serial.print("Speed(knots)=");
    Serial.println(Speed);
    delay(2000);
    lcd.clear();
    lcd.print("Speed(knots):"); // Displaying text in LCD screen
    lcd.setCursor(0,1);
    lcd.print(Speed); // Displaying text in LCD screen
}
//void Send(void);
void coordinate2dec() // Function for displaying values over serial monitor and LCD
{
  String lat_degree="";
  for(i=20;i<=21;i++)         
    lat_degree+=gpsString[i]; // Storing Degrees of latitude      
  String lat_minut="";
  for(i=22;i<=28;i++)         
    lat_minut+=gpsString[i]; // Storing Minutes of latitude 
  String log_degree="";
  for(i=32;i<=34;i++)
    log_degree+=gpsString[i]; // Storing Degrees of longitude
  String log_minut="";
  for(i=35;i<=41;i++)
    log_minut+=gpsString[i]; // Storing Minutes of longitude    
  Speed="";
  for(i=45;i<48;i++)          //extract speed from string
    Speed+=gpsString[i];
  int count = 0;
  if(Speed>String(SpeedLimit)) // checking if vehicle exceeds predefined speed limit
  {
    lcd.clear();
    lcd.print("  Over Speeding  "); // Displaying text in LCD screen
    digitalWrite(buzzer, HIGH); // Function calling to set the buzzer to On mode
    for(int j=1;j<=5;j++)
      count+=1000;
  }
  if(count==5000)
  {
    digitalWrite(buzzer, HIGH); // Function calling to set the buzzer to On mode
    Send();
  }
  else
  {
    lcd.print("  Normal Speed   "); // Displaying text in LCD screen
    digitalWrite(buzzer, LOW); // Function calling to set the buzzer to OFF mode
    count=0;
  }
  float minut= lat_minut.toFloat(); // converting Minutes of latitude to float
  minut=minut/60; // converting minutes to hours
  float degree=lat_degree.toFloat(); // converting Degrees of latitude to float
  latitude=degree+minut; // calculating latitude  
  minut= log_minut.toFloat(); // converting Minutes of longitude to float
  minut=minut/60; // converting minutes to hours
  degree=log_degree.toFloat(); // converting Degrees of longitude to float
  logitude=degree+minut; // calculating longitude  
}
void Send() // Function for sending alert SMS to the predefined number
{ 
   Serial1.println("AT");
   delay(500);
   serialPrint();
   Serial1.println("AT+CMGF=1"); // Selecting TEXT mode of SMS
   delay(500);
   serialPrint();
   Serial1.print("AT+CMGS="); // Assigning recipients mobile number
   Serial1.print('"');
   Serial1.print("9007117750");    // Mobile number for SMS alert
   Serial1.println('"');
   delay(500);
   serialPrint();
   Serial1.print("Latitude:");
   Serial1.println(latitude);
   delay(500);
   serialPrint();
   Serial1.print(" longitude:");
   Serial1.println(logitude);
   delay(500);
   serialPrint();
   Serial1.print(" Speed:");
   Serial1.print(Speed);
   Serial1.println("knots");
   delay(500);
   serialPrint();
   Serial1.print("http://maps.google.com/maps?&z=15&mrt=yp&t=k&q=");
   Serial1.print(latitude,6);
   Serial1.print("+");              //28.612953, 77.231545   //28.612953,77.2293563
   Serial1.print(logitude,6);
   Serial1.write(26); // Writes binary data to the serial port
   delay(2000);
   serialPrint();
}

void serialPrint() // function to print data as long as no. of bytes available for reading from the serial port is > 0
{
  while(Serial1.available()>0) 
  // Serail.available() gets the number of bytes (characters) available for reading from the serial port. This is data that’s already arrived and stored in the serial receive buffer (which holds 64 bytes).
  {
    Serial.print(Serial1.read());
  }
}
