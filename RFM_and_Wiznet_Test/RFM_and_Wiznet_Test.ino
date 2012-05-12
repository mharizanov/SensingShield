/*
Test that the  Wiznet 5100 and RFM12B on Arduino Ethernet work at the same time with the SensingShield

*/

#include "SPI.h"
#include "Ethernet.h"
#include "WebServer.h"

#include <JeeLib.h>
MilliTimer sendTimer;
char payload[] = "Hello from Arduino Ethernet!";
byte needToSend;

static uint8_t mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
static uint8_t ip[] = { 192, 168, 2, 2 };
#define PREFIX ""
WebServer webserver(PREFIX, 80);
void helloCmd(WebServer &server, WebServer::ConnectionType type, char *, bool)
{
  server.httpSuccess();
  if (type != WebServer::HEAD)
  {
    P(helloMsg) = "<h1>Hello, World!</h1>";
    server.printP(helloMsg);
  }
}

void setup()
{
  rf12_initialize(5, RF12_868MHZ, 33);
  Serial.begin(9600);
  delay(1500);
  Serial.println("Starting..");

  Ethernet.begin(mac, ip);
  webserver.setDefaultCommand(&helloCmd);
  webserver.addCommand("index.html", &helloCmd);
  webserver.begin();
}

void loop()
{
  char buff[64];
  int len = 64;

      if (rf12_recvDone() && rf12_crc == 0) {
        Serial.print("OK ");
        for (byte i = 0; i < rf12_len; ++i)
            Serial.write(rf12_data[i]);
        Serial.println();
      }

      if (sendTimer.poll(3000))
      needToSend = 1;

      if (needToSend && rf12_canSend()) {
      needToSend = 0;
      rf12_sendStart(0, payload, sizeof payload);
       Serial.println("Sent...");
      }


  webserver.processConnection(buff, &len);
}
