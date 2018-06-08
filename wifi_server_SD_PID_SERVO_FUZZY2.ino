/*  Connects to the home WiFi network
    Asks some network parameters
    Starts WiFi server with fix IP and listens
    Receives and sends messages to the client
    Communicates: wifi_client_01.ino
*/
float setPoint = 2350.0;

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <SPI.h>
#include <SD.h>

const uint8_t chipSelect = 4;

uint64_t t = 0;

#include <NTPClient.h>
// change next line to use with another board/shield
#include <ESP8266WiFi.h>
//#include <WiFi.h> // for WiFi shield
//#include <WiFi101.h> // for WiFi 101 shield or MKR1000
#include <WiFiUdp.h>

WiFiUDP ntpUDP;

// You can specify the time server pool and the offset (in seconds, can be
// changed later with setTimeOffset() ). Additionaly you can specify the
// update interval (in milliseconds, can be changed using setUpdateInterval() ).
NTPClient timeClient(ntpUDP, "192.168.8.1", (4 * 60 * 60 + 30 * 60), 60000);    //time.nist.gov

const uint8_t ledPin = 3;
char ssid[] = "fasa_unv";           // SSID of your home WiFi
char pass[] = "12345678";            // password of your home WiFi
WiFiServer server2(80);

IPAddress ip(192, 168, 9, 20);            // IP address of the server
IPAddress gateway(192, 168, 8, 1);        // gateway of your network
IPAddress subnet(255, 255, 248, 0);       // subnet mask of your network

////////////////////////////////////////////////////
float err;   //errorr abs(setpoint - currentValue)
int dir = 1;     //temp direction
float curVal, preVal;
int reagan;
int coeff;
int sampleDelay;

///////////////////////////////////////////////// amir:

#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

ESP8266WebServer server(80);

String data = "";

void handleRoot() {
  server.send(200, "text/plain", data);
}

void handleNotFound(){
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET)?"GET":"POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i=0; i<server.args(); i++){
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
}

/////////////////////////////////////////////////



#include <Servo.h>

Servo myservo;  // create servo object to control a servo
// twelve servo objects can be created on most boards
int pos;
int valvePos, prePos;
int servoLow = 100;
int servoHi = 40;


void setup() {
  Serial.begin(115200);                   // only for debug
  //while (!Serial);
  pinMode(ledPin, OUTPUT);
  
  WiFi_INIT();



  SD_INIT(chipSelect);

  timeClient.begin();

  ///////////////////////////////

  myservo.attach(5);  // attaches the servo on GIO2 to the servo object (pin = 3)

  ////////////////////////////// amir:

  if (MDNS.begin("esp8266")) {
    Serial.println("MDNS responder started");
  }

  server.on("/", handleRoot);

  server.on("/inline", [](){
    server.send(200, "text/plain", "this works as well");
  });

  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");
  
  /////////////////////////////

}

void loop ()
{
  ////////////////////////////////////////amir:
  server.handleClient();
  ///////////////////////////////////////
  String dataString = "";
  bool a = timeClient.update();
  //Serial.println(a);

  WiFiClient client = server2.available();
  if (client) {
    if (client.connected())
    {
      //digitalWrite(ledPin, HIGH);  // to show the communication only (inverted logic)
      LED_BLINK(100);
      //Serial.println(".");
      String request = client.readStringUntil('\r');    // receives the message from the client
      dataString = request + ", " + timeClient.getEpochTime() + ", " + timeClient.getFormattedTime();
      Serial.print("EpochTime:"); Serial.println(timeClient.getEpochTime());
      //Serial.print("From client: "); Serial.println(dataString);
      client.flush();
      curVal = request.toFloat() * 100;
      //myPID.Compute();
      ////////////////////////////////////////////////////////////////////
/*      err = abs(setPoint - curVal);
      dir = sign((int)(setPoint) - (int)(curVal));

      if(err < 5)
      {
        coeff = 0;
        sampleDelay = 10000;
        reagan = 1;
      }else if (err < 25)
      {
        coeff = 5;
        sampleDelay = 10000;
        reagan = 2;
      } else if (err < 50)
      {
        coeff = 10;
        sampleDelay = 10000;
        reagan = 3;
      } else if (err < 200)
      {
        coeff = 20;
        sampleDelay = 10000;
        reagan = 4;
      } else if (err < 400)
      {
        coeff = 40;
        sampleDelay = 10000;
        reagan = 5;
      } else
      {
        coeff = 60;
        sampleDelay = 10000;
        reagan = 6;
      }

      pos += coeff * dir;
      pos = constrain(pos, 0, 255);

      ///////////////////////////////////////////////////////////////////
      //client.println("Hi client! No, I am listening.\r"); // sends the answer to the client
      LED_BLINK(100);
      //digitalWrite(ledPin, LOW);
      //
      valvePos = map(pos, 0, 255, servoLow, servoHi);
      
      if(pos != prePos)
      {
        //Serial.println("whyyyyyyy");
        myservo.write(valvePos);
      }



      dataString += ", " + String(setPoint) + ", " + String(curVal) + ", " + String(preVal) + ", " + String(err) + ", " + String(dir) + ", " + String(reagan) + ", " + String(pos) + ", " + String(prePos);

      prePos = pos;
      preVal = curVal;
*/    
      Serial.print("From client: "); Serial.println(dataString);
      ////////////////////////////
      if (request != "GET / HTTP/1.1"){
        data2SD(dataString);
        data = dataString + "\n";
      }
      ///////////////////////////
    }
    client.stop();                // tarminates the connection with the client
  }

  
  if(timeClient.getEpochTime() - t > 1800){
    myservo.write(servoLow);
    t = timeClient.getEpochTime();
    data = "";
    Serial.print("Lowtime: ");Serial.println(timeClient.getEpochTime());
  }
  else if(timeClient.getEpochTime() - t > 1200){
    myservo.write(servoHi);   
  }
}


void SD_INIT(uint8_t chipSelect)
{

  Serial.print("Initializing SD card...");

  // see if the card is present and can be initialized:
  if (!SD.begin(chipSelect))
  {
    Serial.println("Card failed, or not present");
    // don't do anything more:
    while (!SD.begin(chipSelect))
    {
      Serial.print("-");
      LED_BLINK(250);
    }
  }
  Serial.println("card initialized.");
  LED_BLINK(500);
  LED_BLINK(500);
  LED_BLINK(500);
  LED_BLINK(500);
}

void LED_BLINK(uint16_t bRate)
{
  digitalWrite(ledPin, HIGH);
  delay(bRate);
  digitalWrite(ledPin, LOW);
  delay(bRate);
}

void WiFi_INIT(void)
{
  WiFi.config(ip, gateway, subnet);       // forces to use the fix IP
  WiFi.begin(ssid);                 // connects to the WiFi router
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  server2.begin();                         // starts the server
  Serial.println("Connected to wifi");
  Serial.print("Status: "); Serial.println(WiFi.status());  // some parameters from the network
  Serial.print("IP: ");     Serial.println(WiFi.localIP());
  Serial.print("Subnet: "); Serial.println(WiFi.subnetMask());
  Serial.print("Gateway: "); Serial.println(WiFi.gatewayIP());
  Serial.print("SSID: "); Serial.println(WiFi.SSID());
  Serial.print("Signal: "); Serial.println(WiFi.RSSI());
  Serial.print("Networks: "); Serial.println(WiFi.scanNetworks());
}

bool data2SD(String dataString)
{
  // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
  File dataFile = SD.open("datalogPID.txt", FILE_WRITE);

  // if the file is available, write to it:
  if (dataFile)
  {
    dataFile.println(dataString);
    dataFile.close();
    // print to the serial port too:
    //Serial.println(dataString);
    return 0;
  }
  // if the file isn't open, pop up an error:
  else
  {
    Serial.println("error opening datalogPID.txt");
    return 1;
  }
}


int sign(int x)
{
  if (x > 0) return 1;
  if (x < 0) return -1;
  return 0;
}


