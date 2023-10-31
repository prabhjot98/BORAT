#include <ESP8266WiFi.h>
#include <espnow.h>

uint8_t DADS_ADDRESS[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00}; // fill out MAC address here
uint8_t ROOM_NUMBER = 99; // replace number here
const int REED_PIN = 5;

typedef struct msg{
  uint8_t roomNumber;
  bool isOpen;
} msg;
msg myMsg;

void setup() {
  // read the pin that's connected to the reed switch
  pinMode(REED_PIN,INPUT);
  int switchStatus = digitalRead(REED_PIN);
  myMsg.roomNumber = ROOM_NUMBER;
  myMsg.isOpen = switchStatus;

  // send the status over to DAD
  WiFi.mode(WIFI_STA);
  esp_now_init();
  esp_now_set_self_role(ESP_NOW_ROLE_CONTROLLER);
  esp_now_add_peer(DADS_ADDRESS, ESP_NOW_ROLE_SLAVE, 1, NULL, 0);
  esp_now_send(DADS_ADDRESS, (uint8_t *) &myMsg, sizeof(myMsg));

  ESP.deepSleep(60e6); // sleep for 60 seconds
}

void loop() {}