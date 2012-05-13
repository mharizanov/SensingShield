/*
 
 Modified by harizanov.com to fetch a .bin file from Internet and save to SD card
 
 DNS and DHCP-based Web client
 Circuit:
 * Ethernet shield attached to pins 10, 11, 12, 13
 
 created 18 Dec 2009
 by David A. Mellis
 modified 12 April 2011
 by Tom Igoe, based on work by Adrian McEwen

 
 
 */

#include <SPI.h>
#include <Ethernet.h>
#include <SD.h>
File myFile;

// Enter a MAC address for your controller below.
// Newer Ethernet shields have a MAC address printed on a sticker on the shield
byte mac[] = {  0x00, 0xAA, 0xBB, 0xCC, 0xDE, 0x02 };
char serverName[] = "www.harizanov.com";

// Initialize the Ethernet client library
// with the IP address and port of the server 
// that you want to connect to (port 80 is default for HTTP):
EthernetClient client;

void setup() {
  // start the serial library:
  Serial.begin(9600);
  
    Serial.print("Initializing SD card...");
  // On the Ethernet Shield, CS is pin 4. It's set as an output by default.
  // Note that even if it's not used as the CS pin, the hardware SS pin 
  // (10 on most Arduino boards, 53 on the Mega) must be left as an output 
  // or the SD library functions will not work. 
   pinMode(10, OUTPUT);
   
  if (!SD.begin(4)) {
    Serial.println("SD initialization failed!");
    return;
  }
  Serial.println("SD initialization done.");
  
  
  
  // start the Ethernet connection:
  if (Ethernet.begin(mac) == 0) {
    Serial.println("Failed to configure Ethernet using DHCP");
    // no point in carrying on, so do nothing forevermore:
    while(true);
  }
  // give the Ethernet shield a second to initialize:
  delay(1000);  
  
  Serial.println("connecting...");

  // if you get a connection, report back via serial:
  
  if (client.connect(serverName, 80)) {
    Serial.println("connected");
    // Make a HTTP request:
    client.println("GET /~hariabql/app.bin HTTP/1.0");
    client.println();
    readPastHeader(&client);
  } 
  else {
    // you didn't get a connection to the server:
    Serial.println("connection failed");
  }
}

void loop()
{
  // if there are incoming bytes available 
  // from the server, read them and print them:
  if (client.available()) {

  // Check to see if the file exists: 
  if (SD.exists("app.bin")) {
    Serial.println("file exists, deleting..");
    SD.remove("app.bin");
  }
  else {
    Serial.println("file doesn't exist.");  
  }
 
    myFile = SD.open("app.bin", FILE_WRITE);
      
    if (myFile) {
    Serial.print("Writing to file...");
    
      while (client.connected()) {
        byte c = client.read();
        Serial.write('.');
        myFile.write(c);
      }
    
      // close the file:
      myFile.close();
      Serial.println("");
      Serial.println("done.");
	  
	  /*
	  cli();
	  asm volatile("jmp 0x7000");
	  */
      
  } else {
    // if the file didn't open, print an error:
    Serial.println("error opening file");
  }  
      

  }

  // if the server's disconnected, stop the client:
  if (!client.connected()) {
    Serial.println();
    Serial.println("disconnecting.");
    client.stop();

    // do nothing forevermore:
    while(true);
  }
}


bool readPastHeader(Client *pClient)
{
  bool bIsBlank = true;
  while(true)
  {
    if (pClient->available()) 
    {
      char c = pClient->read();
      if(c=='\r' && bIsBlank)
      {
        // throw away the /n
        c = pClient->read();
        return true;
      }
      if(c=='\n')
        bIsBlank = true;
      else if(c!='\r')
        bIsBlank = false;
      }
   }
}
