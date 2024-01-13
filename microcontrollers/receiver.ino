#include <WiFi.h>
#include <esp_wifi.h>
#include <esp_now.h>
#include <WebServer_WT32_ETH01.h>

IPAddress MY_IP(0, 0, 0, 0); // put ip of DAD here
IPAddress MY_GW(0, 0, 0, 0); // put gateway ip here
IPAddress MY_SN(255, 255, 255, 0);
IPAddress MY_DNS(8, 8, 8, 8); // Google DNS Server IP

uint8_t DADS_ADDRESS[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00}; // fill out MAC address here

typedef struct msg{
 uint8_t roomNumber;
 bool isOpen;
} msg;

msg myMsg;
bool isOpen[6];
WebServer server(80);
 
void onDataReceiver(const uint8_t * mac, const uint8_t *incomingData, int len) {
 memcpy(&myMsg, incomingData, sizeof(myMsg));
 isOpen[myMsg.roomNumber-1] = myMsg.isOpen;
}

void handleRoot(){
 String html = F("[");
 for (int i = 0; i < sizeof(isOpen);i++){
  if (i==2){
    continue;
  }
  html += String("{\"name\": \"Breakout Room ") + String(i+1) + "\",\"status\": " + String(isOpen[i] ? "\"closed\"" : "\"open\"") + "},";
 }
 html.setCharAt(html.length()-1, ']');
 server.send(200, F("application/json"), html);
}

void setup() {
 Serial.begin(115200);
 WiFi.mode(WIFI_STA);

 esp_wifi_set_mac(WIFI_IF_STA, &DADS_ADDRESS[0]);
 
 if (esp_now_init() != 0) {
  Serial.println("Problem during ESP-NOW init");
  return;
 }
 esp_now_register_recv_cb(onDataReceiver);

 WT32_ETH01_onEvent();
 ETH.begin(ETH_PHY_ADDR, ETH_PHY_POWER);
 ETH.config(MY_IP, MY_GW, MY_SN, MY_DNS);
 WT32_ETH01_waitForConnect();
 server.on(F("/"), handleRoot);
 server.begin();
}
 
void loop(){
 server.handleClient();
}