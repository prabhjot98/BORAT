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
  Serial.println("Message received.");
  // We don't use mac to verify the sender
  // Let us transform the incomingData into our message structure
 memcpy(&myMsg, incomingData, sizeof(myMsg));
 Serial.println("=== Data ===");
 Serial.print("Mac address: ");
 for (int i = 0; i < 6; i++) {
     Serial.print("0x");
     Serial.print(mac[i], HEX);
     Serial.print(":");
 }
 Serial.println(myMsg.roomNumber);
 Serial.println(myMsg.isOpen ? "true" : "false");
 Serial.println();
 isOpen[myMsg.roomNumber-1] = myMsg.isOpen;
 Serial.println(isOpen[myMsg.roomNumber-1] ? "true" : "false");
}

void handleRoot(){
  String html = F("{");
  for (int i = 0; i < sizeof(isOpen);i++){
    html += String(isOpen[i] ? "true" : "false") + ",";
  }
  html += "}";
  server.send(200, F("text/plain"), html);
}

void setup() {
 Serial.begin(115200);
 WiFi.mode(WIFI_STA);

 esp_wifi_set_mac(WIFI_IF_STA, &DADS_ADDRESS[0]);
 
 // Initializing the ESP-NOW
 if (esp_now_init() != 0) {
  Serial.println("Problem during ESP-NOW init");
  return;
 }
 esp_now_register_recv_cb(onDataReceiver);

  Serial.print("\nStarting HelloServer on " + String(ARDUINO_BOARD));
  Serial.println(" with " + String(SHIELD_TYPE));
  Serial.println(WEBSERVER_WT32_ETH01_VERSION);

  WT32_ETH01_onEvent();
  ETH.begin(ETH_PHY_ADDR, ETH_PHY_POWER);
  ETH.config(MY_IP, MY_GW, MY_SN, MY_DNS);
  WT32_ETH01_waitForConnect();
  server.on(F("/"), handleRoot);
  server.begin();

  Serial.print(F("HTTP EthernetWebServer is @ IP : "));
  Serial.println(ETH.localIP());
}
 
void loop(){
 server.handleClient();
}