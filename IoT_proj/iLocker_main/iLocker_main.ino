// iLocker main controller       esp8266 v2.7.4

#include <ESP8266WiFi.h>      // Start from v2.5, the core used BeraSSL. Before that, it was axTLS.
#include <WiFiClientSecure.h>
#include <WiFiUdp.h>
#include <Arduino.h>
#include <UniversalTelegramBot.h>
#include <ArduinoJson.h>
#include <string.h>

// Pin no. mappings of UNO & D1-R2 are different. 
#define RL_pin 15     // D10
#define YL_pin 13     // D11
#define GL_pin 12     // D12
#define LOCK_pin 5    // D3
#define TRIGGER 16    // D2
/*
More about pin difference:
https://www.instructables.com/Programming-the-WeMos-Using-Arduino-SoftwareIDE/
*/
const char* ssid = "{wifi ssid}";
const char* pwd = "{wifi password}";
String BOTtoken = "{your TGbot token}";

String su_ID = {user id};   // root user ID, for further system setting
String current_user = "";
bool su_login = false;

/* UDP: communicates with ESP nodes */
#define UDP_PORT1 3000
#define UDP_PORT2 3001
WiFiUDP UDP1, UDP2;
char packet[10];
String old_pak = "";
IPAddress CamIn_IP(192, 168, 1, 70);      // IPs of cam modules
IPAddress CamOut_IP(192, 168, 1, 244);
void handleUDPmessage();
void UDPsent(uint8_t);


// BearSSL::X509List rootCert(TELEGRAM_CERTIFICATE_ROOT);
WiFiClientSecure clientTCP;
UniversalTelegramBot bot(BOTtoken, clientTCP);

int botRequestDly = 1000;                 //
bool lock_state = false;                  // lock state
unsigned long last_camIn_check = 0;       // record last time check esp32 cam
unsigned long last_camOut_check = 0;      //
unsigned long lastTimeBotRan = 0;         //
bool stat=0;                              //


void setup() {
  // Initialize serial baud rate and pin mode.
  Serial.begin(115200);
  pinMode(RL_pin, OUTPUT);
  pinMode(YL_pin, OUTPUT);
  pinMode(GL_pin, OUTPUT);
  pinMode(LOCK_pin, OUTPUT);
  pinMode(TRIGGER, INPUT);
  digitalWrite(TRIGGER, 0);           // reset the status of trigger

  // Wi-Fi setting
  WiFi.mode(WIFI_STA);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, pwd);

  /* 
  // We haven't checked neither the CAcert is valid nor the function is worked.
  // Thus, we chose to transmit insercuely.
  */
  clientTCP.setInsecure();
  // clientTCP.setTrustAnchors(&rootCert);
  //clientTCP.setCACert(TELEGRAM_CERTIFICATE_ROOT, sizeof(TELEGRAM_CERTIFICATE_ROOT)); // Add root certificate for api.telegram.org
  
  // Confirm Wifi connection
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println();

  Serial.print("Main Controller IP Address: ");
  Serial.println(WiFi.localIP());
  // Serial.println(WiFi.macAddress());
  Serial.print("Connect to TG bot: "); 
  Serial.println(bot.getMe());          // Check connection with API if the token is valid.

  /* UDP initialize */
  UDP1.begin(UDP_PORT1);
  UDP2.begin(UDP_PORT2);
  Serial.printf("Listening UDP on port %d & %d\n", UDP_PORT1, UDP_PORT2);
}

void loop() {
  // put your main code here, to run repeatedly:
  if (millis() > lastTimeBotRan + botRequestDly)  {
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    while (numNewMessages) {
      Serial.println("got response");
      handleNewMessages(numNewMessages);
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    }
    lastTimeBotRan = millis();
  }
  //Serial.println(digitalRead(TRIGGER));
  if(digitalRead(TRIGGER) && !lock_state && strlen(&current_user[0])) {
    lock_state = 1;
    digitalWrite(TRIGGER, !lock_state);
    digitalWrite(LOCK_pin, lock_state);
    UDPsent(2);          // taking pictures
    String lockedstr = "Your delivery has arrived and has been safely locked.";
    bot.sendSimpleMessage(current_user, lockedstr, "");
  }
  
  LED_state(millis());  // refresh led states
  UDPsent(0);           // ping cam nodes

  delay(300);
}
