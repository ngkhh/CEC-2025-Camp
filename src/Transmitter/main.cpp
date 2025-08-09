#include <Arduino.h>
#include <esp_now.h>
#include <WiFi.h>

// Structure example to receive data
// Must match the sender structure
// REPLACE WITH YOUR RECEIVER MAC Address
uint8_t broadcastAddress[] = {0xAC, 0x15, 0x18, 0xE8, 0x60, 0x24}; // ac:15:18:e8:60:24

// Structure example to send data
// Must match the receiver structure
typedef struct struct_message
{
  int sendX;
  int sendY;
} struct_message;

// Create a struct_message called myData
struct_message myData;

esp_now_peer_info_t peerInfo;

// callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status)
{
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

#define VRX_PIN 32 // Arduino pin connected to VRX pin
#define VRY_PIN 33 // Arduino pin connected to VRY pin

int xValue = 0; // To store value of the X axis
int yValue = 0; // To store value of the Y axis

void setup()
{
  Serial.begin(115200);
  pinMode(VRX_PIN, INPUT);
  pinMode(VRY_PIN, INPUT);
  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK)
  {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Once ESPNow is successfully Init, we will register for Send CB to
  // get the status of Trasnmitted packet
  esp_now_register_send_cb(OnDataSent);

  // Register peer
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  // Add peer
  if (esp_now_add_peer(&peerInfo) != ESP_OK)
  {
    Serial.println("Failed to add peer");
    return;
  }
}

void loop()
{

  // read analog X and Y analog values
  xValue = analogRead(VRX_PIN);
  yValue = analogRead(VRY_PIN);

  myData.sendX = xValue;
  myData.sendY = yValue;
  // print data to Serial Monitor on Arduino IDE
  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *)&myData, sizeof(myData));

  if (result == ESP_OK)
  {
    Serial.println("Sent with success");
  }
  else
  {
    Serial.println("Error sending the data");
  }

  Serial.print("X: ");
  Serial.println(xValue);

  Serial.print("Y: ");
  Serial.println(yValue);
  delay(50);
}
