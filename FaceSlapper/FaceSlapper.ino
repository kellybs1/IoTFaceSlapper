  /*
  Web Server
  A simple web server
  Circuit:
  * Ethernet shield attached to pins 10, 11, 12, 13
  */
  //Borrowed heavily from
  //http://www.instructables.com/id/Easy-Basic-Arduino-Ethernet-Controller/?ALLSTEPS
  //-------------------------------------------------------------------------------------------------------
 
  //Libraries
  #include <SPI.h>
  #include <Ethernet.h>
  #include <Servo.h>
  //This Twitter library is a 3rd party library that needs to be installed manually: 
  //http://playground.arduino.cc/Code/TwitterLibrary
  // http://arduino-tweet.appspot.com/ 
  #include <Twitter.h>
  //Key for Twitter posting
  Twitter twitter("twitter api key goes here");
  int msgCount = 0;
  
  //servo control object
  Servo servo;  
  
  //Enter a MAC address 
  byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
  
  // Assign an IP address for the controller:
  //Tweeting will fail without a working DNS - I have used Google DNS 
  //Router 2 - This one is for full internet access
  IPAddress ip(192, 168, 1, 111); //your local ip address
  IPAddress myDns(8, 8, 4, 4); //google dns - best to leave this
  IPAddress gateway(192, 168, 1, 254); //gateway ip address, ie router
  IPAddress subnet(255, 255, 255, 0); //network subnet
  
  //Initialize the Ethernet server library with a port 
  EthernetServer server(8081);
  //String for recieving HTTP requests 
  String readString;
  // Declare Pin numbers for devices
  int rLed = 8; //Red LED
  int gLed = 7; //Green LED
  int buzz = 9;
  const int servoPin = 5;
  //Servo positions
  const int forward = 90;
  const int back = 180;
  //for pausing between actions
  unsigned long previousMillis = 0; 
  
  void setup()
  {
  // Extra Set up code:
  pinMode(rLed, OUTPUT); 
  pinMode(gLed, OUTPUT);
  pinMode(buzz, OUTPUT);
  
  //enable serial data print
  Serial.begin(9600);
  //start Ethernet
  Ethernet.begin(mac, ip, myDns, gateway);
  server.begin();
  Serial.print("Server is at ");
  Serial.println(Ethernet.localIP());  
  //Light up "ready" LED
  digitalWrite(rLed, LOW);
  digitalWrite(gLed, HIGH);
  }
  
  void loop()
  {
  //turn the ready LED on if it is time to accept actions
  //The delay (70 seconds) stops too many attempts in too short a time, and avoids Twitter spam detection
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= 70000)   
  {   
     //turn on Ready LED, turn off busy LED
     digitalWrite(rLed, LOW);
     digitalWrite(gLed, HIGH);
  }
  //listen for incoming clients
  EthernetClient client = server.available();
  if (client)
  {
    Serial.println("new client"); 
    while (client.connected())
    {
      if (client.available()) 
      {
        char c = client.read();
        //read char by char HTTP request
        if (readString.length() < 100)
        {
          //store characters to string
          readString += c;
          Serial.write(c);
          // if you've gotten to the end of the line (received a newline
          // character) and the line is blank, the http request has ended,
          // so you can send a reply
          //if HTTP request has ended
          if (c == '\n') {
            Serial.println(readString); //print to serial monitor for debuging
            // HTML begins
            client.println("HTTP/1.1 200 OK"); //send new page
            client.println("<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01//EN\">");
            client.println("Content-Type: text/html");
            client.println();
            client.println("<HTML>");
            client.println("<HEAD>");
            client.println("<style type=\"text/css\">");
            client.println("* {");
            client.println("text-align:center;");
            client.println("background-color: #000000;");
            client.println("color: #808080;");
            client.println("margin-top: 5px; margin-left: 30px; margin-right: 30px; padding:0px;");                      
            client.println("font-family: \"Century Gothic\";");
            client.println("}");
            client.println("a {");
            client.println("color: #FFFFFF;");
            client.println("text-decoration: none;");
            client.println("}");
            client.println("h1 {");
            client.println("font-size: 38px;");
            client.println("}");
            client.println("h2 {");
            client.println("font-size: 20px; ");
            client.println("}");
            client.println(".buttons {");
            client.println("font-size: 25px; ");
            client.println("}");
            client.println("</style>");    
            client.println("<TITLE>Internet Face Slapper</TITLE>");
            client.println("</HEAD>");
            client.println("<BODY>");
            client.println("<div class =\"content\">");  
            client.println("<a href=\"http://192.168.4.127:8081\"><h1>Internet Face Slapper</a></h1>");
            client.println("<h2>By Brendan Kelly</h2>");
            client.println("<div class=\"buttons\">");
            client.println("<center>");
            client.println("<br />");
            client.println("<a href=\"/?noise\"\">Noise</a><br />"); 
            client.println("<br />");
            client.println("<a href=\"/?facepunch\"\">Slap 'em</a><br />"); 
            client.println("</center>");
            client.println("</div>");
            client.println("</div>");
            client.println("</BODY>");
            client.println("</HTML>");
            //Small delay for client to recieve data
            delay(20);
            //stopping client
            client.stop();
  
           //This code is run when corresponding links are clicked on the webpage         
           if (currentMillis - previousMillis >= 70000)   //only initiate if delay has passed
           {   
              //turn on Ready LED, turn off busy LED
             digitalWrite(rLed, LOW);
             digitalWrite(gLed, HIGH);    
             //trigger slapping        
              if (readString.indexOf("?facepunch") > 0)
              {
                //turn off Ready LED, turn on busy LED
                digitalWrite(rLed, HIGH);
                digitalWrite(gLed, LOW);
                //swing the arm
                servo.attach(servoPin);
                servo.write(forward);
                delay(400);
                servo.write(back);
                delay(400);
                servo.detach();  
                //post the tweet
                postTweet();                          
              }
              else
              {
                 //trigger noise
                 if (readString.indexOf("?noise") > 0)
                {
                  //turn off Ready LED, turn on busy LED
                  digitalWrite(rLed, HIGH);
                  digitalWrite(gLed, LOW);
                  //make noise
                  tone(buzz, 392, 500);
                  delay(500);
                  tone(buzz, 262, 1000);                
                }
              }   
              previousMillis = currentMillis;          
            }
                                  
            //clearing string for next read
            readString="";
 
            // give the web browser time to receive the data
            delay(10);
            // close the connection:
            client.stop();
            Serial.println("client disonnected"); 
          }
        }
      }
    }
  }
  } 

  //Post the "successful slap" tweet
  void postTweet()
  {     
      //compose tweet string
      String str1 = "Slap #";
      String str2 = String(msgCount+1);
      String str3 = " successful!";
      String str4 = str1 + str2 + str3;
      char msg[str4.length()];
      //add each character from string to char array
      for (int i = 0; i < str4.length(); i++)
      {
        msg[i] = str4.charAt(i);
      }
      //send the tweet - serial output is for trouble shooting / notation
     Serial.println("connecting ...");
      if (twitter.post(msg)) 
      {
        int status = twitter.wait();
        if (status == 200) 
        {
           Serial.println("OK.");
           msgCount++;
        } 
        else 
        {
            Serial.print("failed : code ");
            Serial.println(status);
        }
      } 
      else 
      {
         Serial.println("connection failed.");
      }
  }

