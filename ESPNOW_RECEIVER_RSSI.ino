#include <esp_now.h>
#include <WiFi.h>

uint8_t broadcastAddress[] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };

typedef struct struct_message {
  float temp;
  float hum;
  float pres;
  int rssi; // Add RSSI field to the message structure
} struct_message;

struct_message BME280Readings;
struct_message incomingReadings;

esp_now_peer_info_t peerInfo;

void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  // Acknowledgment logic can be added here if needed
}

void OnDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len) {
  memcpy(&incomingReadings, incomingData, sizeof(incomingReadings));
  Serial.print("Bytes received: ");
  Serial.println(len);
  Serial.println("Incoming Readings from Sender:");
  Serial.print("Temperature: ");
  Serial.print(incomingReadings.temp);
  Serial.println(" ºC");
  Serial.print("Humidity: ");
  Serial.print(incomingReadings.hum);
  Serial.println(" %");
  Serial.print("Pressure: ");
  Serial.print(incomingReadings.pres);
  Serial.println();

  // Get RSSI value using WiFi class
  int rssi = WiFi.RSSI();

  // Acknowledgment data can be sent back to the sender using the sender's MAC address
  BME280Readings.temp = 100; // Some acknowledgment value
  BME280Readings.hum = 200;  // Some acknowledgment value
  BME280Readings.pres = 300; // Some acknowledgment value
  BME280Readings.rssi = rssi; // Include RSSI in the acknowledgment

  esp_now_send(mac, (uint8_t *)&BME280Readings, sizeof(BME280Readings));

  delay(10); // Delay added to ensure successful acknowledgment

  Serial.print("Acknowledgment sent with success. RSSI: ");
  Serial.println(rssi);
}

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);

  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  esp_now_register_send_cb(OnDataSent);

  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Failed to add peer");
    return;
  }

  esp_now_register_recv_cb(OnDataRecv);
}

void loop() {
  // Your main receiver logic can go here
}
