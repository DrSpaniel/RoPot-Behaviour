
//To create the web server:
/***************************************************************************************
     Title: AP_SimpleWebServer
     Author: Tom Igoe
     Date: 25 Nov 2012
     Availability: https://www.arduino.cc/en/Tutorial/WiFiNINAAP_SimpleWebServer

***************************************************************************************/

//To create notifications:
/***************************************************************************************
     Title: AP_SimpleWebServer
     Author: Tom Igoe
     Date: 25 Nov 2012
     Availability: https://www.arduino.cc/en/Tutorial/WiFiNINAAP_SimpleWebServer

***************************************************************************************/

//audio credit: 15.ai


#include <SPI.h>
#include <WiFiNINA.h>
#include <stdio.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Servo.h>
#include <Arduino.h>
#include "Adafruit_LEDBackpack.h"

#include "arduino_secrets.h"
#include "Adafruit_seesaw.h"
#include "Adafruit_Soundboard.h"



//HEY YOU TURN THIS SHITSHOW INTO FUCKING FUNCTIONS!!!!!!!!!!!!!!!!!!


///////please enter your sensitive data in the Secret tab/arduino_secrets.h
char ssid[] = SECRET_SSID;        // your network SSID (name)
char pass[] = SECRET_PASS;    // your network password (use for WPA, or use as key for WEP)
int keyIndex = 0;                // your network key index number (needed only for WEP)

Adafruit_seesaw ss;
Servo servo_1;//RoPot arm1
Servo servo_2;//Ropot arm2

#define SFX_RST 4   //this pin is for the reset of the soundboard
#define MATRIX_EYES         0
#define MATRIX_MOUTH_LEFT   1
#define MATRIX_MOUTH_MIDDLE 2
#define MATRIX_MOUTH_RIGHT  3

Adafruit_Soundboard sfx = Adafruit_Soundboard(&Serial1, NULL, SFX_RST); //initializing the soundboard and passing arguments serial1 (RX and TX on the nano), and SFX_RST which is the reset pin mentioned above


//Adafruit_8x8matrix matrix = Adafruit_8x8matrix();
Adafruit_8x8matrix matrix[4] =
{ // Array of Adafruit_8x8matrix objects
  Adafruit_8x8matrix(), Adafruit_8x8matrix(),
  Adafruit_8x8matrix(), Adafruit_8x8matrix()
};

static const uint8_t matrixAddr[] = { 0x72, 0x77, 0x76, 0x73 }; //FACE ADDRESSES
int led =  LED_BUILTIN;
int status = WL_IDLE_STATUS;
WiFiServer server(80);

void setup()
{

  Serial1.begin(9600);
  //matrix.begin(0x70);  //matrix
 // matrix2.begin(0x76);
 
   for (uint8_t i = 0; i < 4; i++)
  {
    matrix[i].begin(matrixAddr[i]);
    matrix[i].setRotation(1);
  }
  
  servo_1.attach(9);
  servo_2.attach(10);
  servo_1.write(0);
  servo_2.write(0);
  if (!sfx.reset())      //is the soundboard doesnt reset, then complain also.
  {
    Serial.println("Not found");
    while (1);
  }
  Serial.println("SFX board found");

  //Initialize serial and wait for port to open:
  Serial.begin(9600);
  //  while (!Serial) {
  //    ; // wait for serial port to connect. Needed for native USB port only
  //  }

  Serial.println("Access Point Web Server");

  pinMode(led, OUTPUT);      // set the LED pin mode

  // check for the WiFi module:
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");
    // don't continue
    while (true);
  }

  String fv = WiFi.firmwareVersion();
  if (fv < WIFI_FIRMWARE_LATEST_VERSION) {
    Serial.println("Please upgrade the firmware");
  }

  // by default the local IP address will be 192.168.4.1
  // you can override it with the following:
  WiFi.config(IPAddress(10, 0, 0, 1));

  // print the network name (SSID);
  Serial.print("Creating access point named: ");
  Serial.println(ssid);

  // Create open network. Change this line if you want to create an WEP network:
  status = WiFi.beginAP(ssid, pass);
  if (status != WL_AP_LISTENING) {
    Serial.println("Creating access point failed");
    // don't continue
    while (true);
  }

  // wait 10 seconds for connection:
  delay(10000);

  // start the web server on port 80
  server.begin();

  // you're connected now, so print out the status
  printWiFiStatus();

  if (!ss.begin(0x36))
  {
    Serial.println("ERROR! seesaw not found");
    while (1);
  }
  else
  {
    Serial.print("seesaw started! version: ");
    Serial.println(ss.getVersion(), HEX);
  }


  //Serial1.println("#5");    //just says i feel great
  //make happy face and put here
}

char str_buf[100];
const int str_buf_len = 100;
float tempC = 0;
int whole_temp = 0;
int frac_temp = 0;

int loopnum = 0;

int thresh = 0;

unsigned long previousMillis = 0;   //used for timer

const long interval = 16000;   //make this 5 seconds so it checks every 5 seconds.

int state = 0;    //var is used as a ticker, to see what the next freakout should be.

int next_state = 0;

static const uint8_t PROGMEM // Bitmaps are stored in program memory
  blinkImg[][8] = {    // Eye animation frames
{ B00000000,
  B00011000,
  B00111100,
  B01100110,
  B11000011,
  B00000000,
  B00000000,
  B00000000 },
{ B00000000,
  B11000011,
  B01100110,
  B00111100,
  B00011000,
  B00000000,
  B00000000,
  B00000000
},
{ B00000000,         // Fully closed eye
  B00000000,
  B00000000,
  B00000000,
  B00000000,
  B01111110,
  B00000000,
  B00000000 
  },
{ B00000000,
  B00000000,
  B00000000,
  B11111111,
  B00011000,
  B00011000,
  B00000000,
  B00000000
},
{ B00000000,
  B01000100,
  B00101000,
  B00010000,
  B00101000,
  B01000100,
  B00000000,
  B00000000
},
{ B00000000,
  B00000000,
  B00000000,
  B00000000,
  B00000000,
  B00000000,
  B00000000,
  B00000000
}
};
 static const uint8_t PROGMEM 
 mouthImg[][24] = {                 // Mouth animation frames
  { B00000000, B00000000, B00000000,
    B11111111, B11111111, B11111111,
    B01111111, B11111111, B11111110,
    B00111111, B11111111, B11111100,
    B00001111, B11111111, B11110000,
    B00000001, B11111111, B10000000,
    B00000000, B00000000, B00000000,
    B00000000, B00000000, B00000000 },
  {
    B00000000, B00000000, B00000000,
    B00000000, B00000000, B00000000,
    B00000000, B00111100, B00000000,
    B00000000, B11100111, B00000000,
    B00000011, B10000001, B11000000,
    B00001110, B00000000, B01110000,
    B00000000, B00000000, B00000000,
    B00000000, B00000000, B00000000 },
  { B00000000, B00000000, B00000000, // Mouth position C
    B00000000, B00000000, B00000000,
    B00000000, B00000000, B00000000,
    B00000011, B11111111, B11000000,
    B00000000, B00000000, B00000000,
    B00000000, B00000000, B00000000,
    B00000000, B00000000, B00000000,
    B00000000, B00000000, B00000000 },
  {
    B00000000, B00000000, B00000000,
    B00000000, B00000000, B00000000,
    B00000111, B11111111, B11100000,
    B00001111, B11111111, B11110000,
    B00011111, B11111111, B11111000,
    B00111111, B11111111, B11111100,
    B00000000, B00000000, B00000000,
    B00000000, B00000000, B00000000 },
  { B00000000, B00000000, B00000000, // dead mouth
    B00000000, B00000000, B00000000,
    B00001111, B11111111, B11110000,
    B00000000, B00000100, B10010000,
    B00000000, B00000100, B10010000,
    B00000000, B00000100, B00010000,
    B00000000, B00000011, B11100000,
    B00000000, B00000000, B00000000 },
  { B00000000, B10000001, B00000000, // sick mouth
    B00000000, B01000010, B00000000,
    B00000000, B00100100, B00000000, 
    B00000000, B00011000, B00000000,
    B00000000, B00100100, B00000000, 
    B00000000, B01000010, B00000000,
    B00000000, B10000001, B00000000, 
    B00000000, B00000000, B00000000 },
    {
    B00000000, B00000000, B00000000, //optional blank face
    B00000000, B00000000, B00000000,
    B00000000, B00000000, B00000000,
    B00000000, B00000000, B00000000,
    B00000000, B00000000, B00000000,
    B00000000, B00000000, B00000000,
    B00000000, B00000000, B00000000,
    B00000000, B00000000, B00000000 }};

int pos = 0;
int pos2 = 0;
int notif = 0;
int dieAlready=0;
void loop()
{
  uint16_t capread = ss.touchRead(0);
  int hPercentage = (capread * 100) / thresh;
  unsigned long currentMillis = millis();
  int count=0;
  int count2=0;
  int thankYou;
  tempC = ss.getTemp();
  delay(100);
  whole_temp = floor(tempC);                                //extracts the whole number and disregards decimal from the temperature value
  frac_temp = (int)((tempC * 100.0 - whole_temp * 100.0));  // finds the decimal for the temperature value
  snprintf(str_buf, 100, "%d.%d C", whole_temp, frac_temp); // concatenate the whole number and the decimal
  
  
  // compare the previous status to the current status
  if (status != WiFi.status()) {
    // it has changed update the variable
    status = WiFi.status();

    if (status == WL_AP_CONNECTED) {
      // a device has connected to the AP
      Serial1.println("#2");    //this says threshold set
      Serial.println("Device connected to AP");
    } else {
      // a device has disconnected from the AP, and we are back in listening mode
      Serial1.println("#0");    //this says client connected, but it doesnt play nice
      Serial.println("Device disconnected from AP");
    }
  }

  WiFiClient client = server.available();   // listen for incoming clients

  if (client) {                             // if you get a client,
    Serial.println("new client");           // print a message out the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected()) {            // loop while the client's connected
      delayMicroseconds(10);                // This is required for the Arduino Nano RP2040 Connect - otherwise it will loop so fast that SPI will never be served.
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        if (c == '\n') {                    // if the byte is a newline character

          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {

            // GUI website content
            // initialize HTML webpage
            client.print("<!DOCTYPE html>");
            client.print("<html lang=\"en\">");
            // min/max scale sets the font size
            client.print("<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.5,maximum-scale=2, user-scalable=yes\">");
            client.print("<body style=\"background-color:#5AA9E6;\">");
            client.println();

            client.print("<head>");
            client.print("<meta http-equiv=\"refresh\" content=\"15\">");
           
            client.print("<title>Hello World!</title>");
            client.print("</head>");

            client.print("<body>");
            client.print("<H1>Hello, RoPot!</H1>");
            client.print("<p>&#128516;</p>");
            client.print("Click <a href=\"/H\"><button>here</button></a> to set threshold!<br>");
            client.println();
            client.print("<p>Plant temperature:</p>");
            client.print(str_buf);
            Serial.println(ss.getTemp());
            Serial.println(str_buf);
            Serial.println();
            client.println();
            client.print("<p>Hydration percentage:</p>");
            Serial.println(ss.touchRead(0));
            Serial.println(capread);
            Serial.println();
            client.println();
            client.print(hPercentage);
            client.print("%");
            client.println();
           // client.print("<p>Threshold:</p>");
           // client.print(thresh, DEC);
            client.println();

            client.print("</body>");

            client.print("<style>");
            client.print(".callout {position: middle; max-width: 215px;}");
            client.print(".callout-header {padding: 15px 15px;background:  #E63946;font-size: 25px;color: white;}");
            client.print(".callout-container {padding: 15px;background-color: #32746D;color: white}");
            client.print("</style>");
            //Display the 1st notification, if the RoPot has entered stage 1 of dehydration
            if (notif == 1) {
              //client.print("<meta http-equiv=\"refresh\" content=\"0\">");
              client.print("<div class=\"callout\">");
              client.print("<div class=\"callout-header\">IMPORTANT!</div>");
              client.print("<div class=\"callout-container\"><p><strong>Your plant needs water!</strong></p></div>");
              client.print("</div>");
            }
            //Display the 2nd notification, if the RoPot has entered stage 2 of dehydration
            if (notif == 2) {
              //client.print("<meta http-equiv=\"refresh\" content=\"0\">");
              client.print("<div class=\"callout\">");
              client.print("<div class=\"callout-header\">IMPORTANT!!</div>");
              client.print("<div class=\"callout-container\"><p><strong>Your plant is getting annoyed, water it now!</strong></p></div>");
              client.print("</div>");
            }
            //Display the 3rd notification, if the RoPot has entered stage 3 of dehydration
            if (notif == 3) {
              //client.print("<meta http-equiv=\"refresh\">");
              client.print("<div class=\"callout\">");
              client.print("<div class=\"callout-header\">IMPORTANT!!!</div>");
              client.print("<div class=\"callout-container\"><p><strong>Urgent! Your plant needs water!</strong></p></div>");
              client.print("</div>");
            }
            if (notif == 4) {
             // client.print("<meta http-equiv=\"refresh\">");
              client.print("<div class=\"callout\">");
             client.print("<div class=\"callout-header\">Threshold can't be set</div>");
              client.print("<div class=\"callout-container\"><p><strong>There is no plant!</strong></p></div>");
              client.print("</div>");
            }
             if (notif == 5) {
              //client.print("<meta http-equiv=\"refresh\">");
              client.print("<div class=\"callout\">");
              client.print("<div class=\"callout-header\">Threshold can't be set</div>");
              client.print("<div class=\"callout-container\"><p><strong>Your plant too dry!</strong></p></div>");
              client.print("</div>");
            }
              if (notif == 6) {
               //client.print("<meta http-equiv=\"refresh\">");
              client.print("<div class=\"callout\">");
              client.print("<div class=\"callout-header\">IMPORTANT</div>");
              client.print("<div class=\"callout-container\"><p><strong>Your plant is dying! Water it now!</strong></p></div>");
              client.print("</div>");
            }
            client.print("</body>");

            // The HTTP response ends with another blank line:
            client.println();
            // break out of the while loop:
            break;
          }
          else {      // if you got a newline, then clear currentLine:
            currentLine = "";
          }
        }
        else if (c != '\r') {    // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }

        // Check to see if the client request was "GET /H" or "GET /L":
        if (currentLine.endsWith("GET /H"))
        {
         thresh = 0;
          if(capread <= 400){
          Serial1.println("#10");//no plant
          notif=4;
          client.print("<meta http-equiv=\"refresh\" content=\"0; URL=10.0.0.1/L\" />");
        }
          if(capread > 400 && capread < 600){
          notif=5;
          Serial1.println("#7");// give water already
        }
        if(capread >= 600){
          notif=0;
          thresh = capread; //threshold becomes the value on th soil sensor
          Serial1.println("#1");    //make the soundboard say 'threshold set!'
          client.print("<meta http-equiv=\"refresh\" content=\"0; URL=10.0.0.1/L\" />");
        }
        }
        if (currentLine.endsWith("GET /L"))
        {
          // GET /L turns the LED off
        }

      }
    }
    // close the connection:
    client.stop();
    Serial.println("client disconnected");
  }

  loopnum++;
// //Blank Face
//          matrix[MATRIX_EYES].clear();
//          matrix[MATRIX_EYES].drawBitmap(0, 0, blinkImg[5], 8, 8, LED_ON);
//          drawMouth(mouthImg[6]);
//          for(uint8_t i=0; i<4; i++) matrix[i].writeDisplay();


  if ( currentMillis - previousMillis >= interval)   //allow interval time to pass
  {
    previousMillis = currentMillis;
    state = next_state;
    
if(hPercentage !=0){
    if (hPercentage <= 80)
    {
      //these switch Cases hold the behaviour of the RoPot 'Freakout'
      delay(5000);
      switch (state)
      {
        case 0:                                             //Stage 1 of dehydration
          Serial1.println("#5"); 
          //SAD FACE
          matrix[MATRIX_EYES].clear();
          matrix[MATRIX_EYES].drawBitmap(0, 0, blinkImg[1], 8, 8, LED_ON);
          drawMouth(mouthImg[1]);
          for(uint8_t i=0; i<4; i++) matrix[i].writeDisplay();
          notif = 1;                                        //allows for the 1st notification to be displayed
          next_state = 1;                                   //transition to stage 2 of dehydration
          //thankYou=1;
          break;

        case 1:                                             //Stage 2 of dehydration
          Serial1.println("#6"); //hey ive been waiting 30 min
          //ANNOYED FACE
          matrix[MATRIX_EYES].clear();
          matrix[MATRIX_EYES].drawBitmap(0, 0, blinkImg[2], 8, 8, LED_ON);
          drawMouth(mouthImg[2]);
          for(uint8_t i=0; i<4; i++) matrix[i].writeDisplay();
          notif = 2;                                        //allows for the 2nd notification to be displayed
          next_state = 2;                                   //transition to stage 3 of dehydration
          //thankYou=1;
          break;

        case 2:                                             //Stage 3 of dehydration
          
          
          count++;
          if (dieAlready<=3){
          Serial1.println("#7");// give water already
          //ANGRY FACE
          matrix[MATRIX_EYES].clear();
          matrix[MATRIX_EYES].drawBitmap(0, 0, blinkImg[3], 8, 8, LED_ON);
          drawMouth(mouthImg[3]);
          for(uint8_t i=0; i<4; i++) matrix[i].writeDisplay();
          angryArms(1);                                     //begin RoPot arm movement
          angryArms(0);                                     //Stops the arms from moving
          notif = 3;                                        //allows for the 3rd notification to be displayed
          }
         if (dieAlready == 4) {
         //Dead Face
          matrix[MATRIX_EYES].clear();
          matrix[MATRIX_EYES].drawBitmap(0, 0, blinkImg[4], 8, 8, LED_ON);
          drawMouth(mouthImg[4]);
          for(uint8_t i=0; i<4; i++) matrix[i].writeDisplay();
          notif = 6;                                        //allows for the 3rd notification to be displayed
          Serial1.println("#11");// im dying
          }
          
          
          
          next_state = 2;                                   //allows the Ropot to kep looping stage 3 untill watered
          dieAlready += count;
          break;

        default:

          break;
      }
    }
    else if (hPercentage >= 120)
    {
        //Drowning Face
          matrix[MATRIX_EYES].clear();
          matrix[MATRIX_EYES].drawBitmap(0, 0, blinkImg[2], 8, 8, LED_ON);
          drawMouth(mouthImg[5]);
          for(uint8_t i=0; i<4; i++) matrix[i].writeDisplay();
         Serial1.println("#9");// im dying
    }
    else if(hPercentage > 60){
   
      // Serial1.println("#8");// im dying
      dieAlready=0;
   // count=0;
      notif = 0;
      next_state = 0;
      //happy face
      matrix[MATRIX_EYES].clear();
      matrix[MATRIX_EYES].drawBitmap(0, 0, blinkImg[0], 8, 8, LED_ON);
      drawMouth(mouthImg[0]);
      for(uint8_t i=0; i<4; i++) matrix[i].writeDisplay();
    }
}
  }

    //Dead Face
//          matrix[MATRIX_EYES].clear();
//          matrix[MATRIX_EYES].drawBitmap(0, 0, blinkImg[4], 8, 8, LED_ON);
//          drawMouth(mouthImg[4]);
//          for(uint8_t i=0; i<4; i++) matrix[i].writeDisplay();

    //Drowning Face
//          matrix[MATRIX_EYES].clear();
//          matrix[MATRIX_EYES].drawBitmap(0, 0, blinkImg[2], 8, 8, LED_ON);
//          drawMouth(mouthImg[5]);
//          for(uint8_t i=0; i<4; i++) matrix[i].writeDisplay();

   //Blank Face
//          matrix[MATRIX_EYES].clear();
//          matrix[MATRIX_EYES].drawBitmap(0, 0, blinkImg[5], 8, 8, LED_ON);
//          drawMouth(mouthImg[6]);
//          for(uint8_t i=0; i<4; i++) matrix[i].writeDisplay();
}


void angryArms(int j)
{
  if (j == 1)
  {
    servo_1.write(45);
    for (int k = 1; k <= 6; k++)
    {
      for (pos2 = 0; pos <= 45; pos += 5)
      { // goes from 0 degrees to 180 degrees
        // in steps of 1 degree
        //servo_1.write(pos);              // tell servo to go to position in variable 'pos'
        servo_2.write(pos2);
        delay(15);                       // waits 15 ms for the servo to reach the position
      }
      for (pos = 45; pos >= 0; pos -= 5)
      { // goes from 180 degrees to 0 degrees
        //servo_1.write(pos);              // tell servo to go to position in variable 'pos'
        servo_1.write(pos);
        delay(15);                       // waits 15 ms for the servo to reach the position
      }
      
    
      for (pos = 0; pos <= 45; pos += 5)
      { // goes from 0 degrees to 180 degrees
        // in steps of 1 degree
        //servo_1.write(pos);              // tell servo to go to position in variable 'pos'
        servo_1.write(pos);
        delay(15);                       // waits 15 ms for the servo to reach the position
      }
      for (pos2 = 45; pos >= 0; pos -= 5)
      { // goes from 180 degrees to 0 degrees
        //servo_1.write(pos);              // tell servo to go to position in variable 'pos'
        servo_2.write(pos2);
        delay(15);                       // waits 15 ms for the servo to reach the position
      }
      servo_1.write(0);
      servo_2.write(0);
    
    
  }
  j++;
  }
}

void printWiFiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print where to go in a browser:
  Serial.print("To see this page in action, open a browser to http://");
  Serial.println(ip);
  Serial1.println("#3");    //make this say 'wifi up!'

}
// Draw mouth image across three adjacent displays
void drawMouth(const uint8_t *img) {
  for(uint8_t i=0; i<3; i++) {
    matrix[MATRIX_MOUTH_LEFT + i].clear();
    matrix[MATRIX_MOUTH_LEFT + i].drawBitmap(i * -8, 0, img, 24, 8, LED_ON);
  }
}
