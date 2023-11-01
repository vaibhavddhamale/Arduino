#include <WiFi.h>
#include "arduino_secrets.h"

///////please enter your sensitive data in the Secret tab/arduino_secrets.h
char ssid[] = SECRET_SSID;    // your network SSID (name)
char pass[] = SECRET_PASS;    // your network password (use for WPA, or use as key for WEP)
int keyIndex = 0;             // your network key Index number (needed only for WEP)

int status = WL_IDLE_STATUS;
int buttonState = 0;
int counter = 0;

WiFiServer server(80);

void setup() 
{
  // put your setup code here, to run once:
  Serial.begin(9600);
  while (!Serial) 
  {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  
  Serial.println("Access Point Web Server");

  // pinMode(LEDR,OUTPUT);
  // pinMode(LEDG,OUTPUT);
  // pinMode(LEDB,OUTPUT);
  pinMode(D0, OUTPUT);
  pinMode(LED_D0, OUTPUT);
  pinMode(BTN_USER, INPUT); 

  // by default the local IP address of will be 192.168.3.1
  // you can override it with the following:
  // WiFi.config(IPAddress(10, 0, 0, 1));

  if(strlen(pass) < 8)
  {    
    Serial.println("Creating access point failed");
    Serial.println("The Wi-Fi password must be at least 8 characters long");
    // don't continue
    while(true);
  }
    
  // print the network name (SSID);
  Serial.print("Creating access point named: ");
  Serial.println(ssid);

  //Create the Access point
  status = WiFi.beginAP(ssid,pass);
  if(status != WL_AP_LISTENING)
  {
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
  
}

void loop() 
{
  buttonState = digitalRead(BTN_USER);
  if(buttonState == LOW){
    if(counter < 1){
      digitalWrite(D0, HIGH); // Sets the relay 1 on
      digitalWrite(LED_D0, HIGH);
      counter++;
    }
    else{
      digitalWrite(D0, LOW); // Sets the relay 1 off
      digitalWrite(LED_D0, LOW);
      counter = 0;
    }
    delay(100);
  }
  changeLights();
 // compare the previous status to the current status
  if (status != WiFi.status()) 
  {
    // it has changed update the variable
    status = WiFi.status();

    if (status == WL_AP_CONNECTED) 
    {
      // a device has connected to the AP
      Serial.println("Device connected to AP");
    } else 
    {
      // a device has disconnected from the AP, and we are back in listening mode
      Serial.println("Device disconnected from AP");
    }
  }

  WiFiClient client = server.available();   // listen for incoming clients

  if (client) {                             // if you get a client,
    Serial.println("new client");           // print a message out the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
  
    while (client.connected()) {            // loop while the client's connected
     
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        if (c == '\n') {                    // if the byte is a newline character

          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println();

            // the content of the HTTP response follows the header:
            client.print("<html><head>");
            client.print("<style>");
            client.print("* { font-family: sans-serif;}");
            client.print("body { padding: 2em; font-size: 2em; text-align: center;}");            
            client.print("a { -webkit-appearance: button;-moz-appearance: button;appearance: button;text-decoration: none;color: initial; padding: 25px;} #red{color:red;} #green{color:green;} #blue{color:blue;}");
            client.print("</style></head>");
            client.print("<body><h1> RELAY CONTROLS </h1>");
            client.print("<h2><span id=\"red\">RELAY1 </span> LED </h2>");
            client.print("<a href=\"/Hr\">ON</a> <a href=\"/Lr\">OFF</a>");
            // client.print("<h2> <span id=\"green\">GREEN</span> LED </h2>");
            // client.print("<a href=\"/Hg\">ON</a> <a href=\"/Lg\">OFF</a>");
            // client.print("<h2> <span id=\"blue\">BLUE</span> LED </h2>");
            // client.print("<a href=\"/Hb\">ON</a> <a href=\"/Lb\">OFF</a>");
            client.print("</body></html>");

            // The HTTP response ends with another blank line:
            client.println();
            // break out of the while loop:
            break;
          } else {      // if you got a newline, then clear currentLine:
            currentLine = "";
          }
        } else if (c != '\r') {    // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }

        // Check to see if the client request was "GET /H" or "GET /L":
        if (currentLine.endsWith("GET /Hr")) {
          //digitalWrite(LEDR, LOW);               // GET /Hr turns the Red LED on
          digitalWrite(D0, HIGH);
          digitalWrite(LED_D0, HIGH);
        }
        if (currentLine.endsWith("GET /Lr")) {
          //digitalWrite(LEDR, HIGH);                // GET /Lr turns the Red LED off
          digitalWrite(D0, LOW);
          digitalWrite(LED_D0, LOW);
        }
        // if (currentLine.endsWith("GET /Hg")){
        //   digitalWrite(LEDG, LOW);                // GET /Hg turns the Green LED on
        // }
        // if (currentLine.endsWith("GET /Lg")){
        //   digitalWrite(LEDG, HIGH);                // GET /Hg turns the Green LED on
        // }
        // if (currentLine.endsWith("GET /Hb")){
        //   digitalWrite(LEDB, LOW);                // GET /Hg turns the Green LED on
        // }
        // if (currentLine.endsWith("GET /Lb")){
        //   digitalWrite(LEDB, HIGH);                // GET /Hg turns the Green LED on
        // } 
        
      }
    }
    // close the connection:
    client.stop();
    Serial.println("client disconnected");
  }
  
}

void changeLights() 
{
  switch(counter){
    case 0:
      digitalWrite(LED_D0, LOW);
      // digitalWrite(LED_D1, LOW);
      // digitalWrite(LED_D2, LOW);
      // digitalWrite(LED_D3, LOW);
      break;
    case 1:
      digitalWrite(LED_D0, HIGH);
      break;
    // case 2:
    //   digitalWrite(LED_D1, HIGH);
    //   break;
    // case 3:
    //   digitalWrite(LED_D2, HIGH);
    //   break;
    // case 4:
    //   digitalWrite(LED_D3, HIGH);
    //   break;
  }
  delay(100);
}

void printWiFiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your Wi-Fi shield's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print where to go in a browser:
  Serial.print("To see this page in action, open a browser to http://");
  Serial.println(ip);
}