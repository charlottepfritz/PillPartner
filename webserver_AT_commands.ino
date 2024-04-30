#include <SoftwareSerial.h>
SoftwareSerial esp8266(10,11);
#define serialCommunicationSpeed 115200
#define DEBUG true

const int inputPin = 4;

void setup()
{
  Serial.begin(serialCommunicationSpeed);
  esp8266.begin(serialCommunicationSpeed);

  pinMode(inputPin, INPUT);
  InitWifiModule();
}
void loop()
{
  int inputValue = digitalRead(inputPin);
  if(esp8266.available())
 {  
  if(inputValue == HIGH){
    if(esp8266.find("+IPD,"))
    {
     delay(1000);
     int connectionId = esp8266.read()-48;
     String webpage = "<!DOCTYPE html><html lang=\"en\"><head><meta charset=\"UTF-8\"><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\"><title>PillPartner</title><style>body {background-color: #f5dd90ff;color: #324376ff; } h1 {color: #f76c5eff;}</style></head><body><h1>Pill Partner</h1><p>The pill was taken!</p></body></html>";
     sendData("AT+CIPSERVER=1,80\r\n", 1500, DEBUG);
     String cipSend = "AT+CIPSEND=";
     cipSend += connectionId;
     cipSend += ",";
     cipSend +=webpage.length();
     cipSend +="\r\n";
     sendData(cipSend,1000,DEBUG);
     sendData(webpage,1000,DEBUG);
     String closeCommand = "AT+CIPCLOSE=";
     closeCommand+=connectionId; // append connection id
     closeCommand+="\r\n";
     sendData(closeCommand,3000,DEBUG);
    }
 }else {
    if(esp8266.find("+IPD,"))
    {
     delay(1000);
     int connectionId = esp8266.read()-48;
     String webpage = "<!DOCTYPE html><html lang=\"en\"><head><meta charset=\"UTF-8\"><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\"><title>PillPartner</title><style>body {background-color: #324376ff;color: #f5dd90ff; }        h1 {color: #f76c5eff; }</style></head><body><h1>Pill Partner</h1><p>The pill has not been taken!</p></body></html>";
     sendData("AT+CIPSERVER=1,80\r\n", 1500, DEBUG);
     String cipSend = "AT+CIPSEND=";
     cipSend += connectionId;
     cipSend += ",";
     cipSend +=webpage.length();
     cipSend +="\r\n";
     sendData(cipSend,1000,DEBUG);
     sendData(webpage,1000,DEBUG);
     String closeCommand = "AT+CIPCLOSE=";
     closeCommand+=connectionId; // append connection id
     closeCommand+="\r\n";
     sendData(closeCommand,3000,DEBUG);
    }
 }
  }
}
String sendData(String command, const int timeout, boolean debug)
{
    String response = "";
    esp8266.print(command);
    long int time = millis();
    while( (time+timeout) > millis())
    {
      while(esp8266.available())
      {
        char c = esp8266.read();
        response+=c;
      }
    }
    if(debug)
    {
      Serial.print(response);
    }
    return response;
}
void InitWifiModule()
{
  sendData("AT+RST\r\n", 2000, DEBUG);
  sendData("AT+CWJAP=\"network\",\"pwd\"\r\n", 2000, DEBUG);
  delay (3000);
  sendData("AT+CWMODE=1\r\n", 1500, DEBUG);
  delay (1500);
  sendData("AT+CIFSR\r\n", 1500, DEBUG);
  delay (1500);
  sendData("AT+CIPMUX=1\r\n", 1500, DEBUG);
  delay (1500);
  sendData("AT+CIPSERVER=1,80\r\n", 1500, DEBUG);
}

// #include <SoftwareSerial.h>

// SoftwareSerial mySerial(10, 11);  //RX,TX

// //  Arduino pin 2 (RX) to ESP8266 TX
// //  Arduino pin 3 to voltage divider then to ESP8266 RX
// //  Connect GND from the Arduiono to GND on the ESP8266
// //  Pull ESP8266 CH_PD HIGH

// // When a command is entered in to the serial monitor on the computer 
// // the Arduino will relay it to the ESP8266

 
// int LEDPIN = 13;
 
// void setup() 
// {
//     pinMode(LEDPIN, OUTPUT);
 
//     Serial.begin(115200);     // communication with the host computer
//     //while (!Serial)   { ; }
 
//     // Start the software serial for communication with the ESP8266
//     mySerial.begin(115200);  

//     InitWifiModule();
 
//     // Serial.println("");
//     // Serial.println("Remember to to set Both NL & CR in the serial monitor.");
//     // Serial.println("Ready");
//     // Serial.println("");    
//     // delay(2000);
//     // mySerial.println("AT"); 
//     // delay(2000);
//     // mySerial.println("AT+CWMODE=1"); 
//     // delay(2000);
// }
 
// void loop() 
// {
//     // listen for communication from the ESP8266 and then write it to the serial monitor
//     if ( mySerial.available() )   {  Serial.write( mySerial.read() );  }
 
//     // listen for user input and send it to the ESP8266
//     if ( Serial.available() )    {  
//       mySerial.write( Serial.read() ); 
//       }
// }

// void InitWifiModule(){
//   mySerial.println("AT");
//   delay(1500);
//   mySerial.println("AT+CWMODE=1");
//   delay(1500);
//   mySerial.print("AT+CWJAP=\"network\",\"pwd\"\r\n");
//   delay(3000);
//   mySerial.println("AT+CIFSR");
//   delay(1500);
// }
