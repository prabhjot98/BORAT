#include <esp_now.h>
#include <WiFi.h>

uint8_t ROOM_NUMBER = 1;
uint8_t DADS_ADDRESS[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
RTC_DATA_ATTR int bootCount = 0;
gpio_num_t SENSOR_PIN = GPIO_NUM_18;
esp_now_peer_info_t peerInfo;

typedef struct msg{
  uint8_t roomNumber;
  bool isOpen;
} msg;
msg myMsg;

void setup() {
  // deep sleep config
  setCpuFrequencyMhz(80);
  esp_sleep_enable_ext0_wakeup(SENSOR_PIN, 1);
  gpio_sleep_set_pull_mode(SENSOR_PIN, GPIO_PULLDOWN_ONLY);
  

  // figure out if the room is open or not
  if (bootCount % 2 == 1){
    myMsg.isOpen = true;
  }else{
    myMsg.isOpen = false;
  }
  ++bootCount;

  // send msg to DAD
  myMsg.roomNumber = ROOM_NUMBER;
  memcpy(peerInfo.peer_addr, DADS_ADDRESS, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;
  esp_now_add_peer(&peerInfo);
  WiFi.mode(WIFI_STA);
  esp_now_send(DADS_ADDRESS, (uint8_t *) &myMsg, sizeof(myMsg));
  WiFi.mode(WIFI_MODE_NULL);

  // prevent the sensor from waking from deep sleep if it's still next to the magnet
  while (digitalRead(SENSOR_PIN) > 0){}

  // go back to sleep
  esp_deep_sleep_start();
}

void loop() {}