#include <esp_now.h>
#include <WiFi.h>

uint8_t broadcastAddress[] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };
uint8_t receiverAddress[6];

float temperature = 22;
float humidity = 33;
float pressure = 55;

typedef struct struct_message {
  float temp;
  float hum;
  float pres;
} struct_message;

struct_message BME280Readings;
struct_message incomingReadings;

esp_now_peer_info_t peerInfo;

void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");

  if (status == ESP_NOW_SEND_SUCCESS) {
    Serial.println("Acknowledgment received from Receiver");

    // Include relevant data in the acknowledgment message
    struct_message acknowledgment;
    acknowledgment.temp = 100; // Some acknowledgment value
    acknowledgment.hum = 200;  // Some acknowledgment value
    acknowledgment.pres = 300; // Some acknowledgment value

    // Send acknowledgment to the receiver's address
    esp_err_t result = esp_now_send(receiverAddress, (uint8_t *)&acknowledgment, sizeof(acknowledgment));

    if (result == ESP_OK) {
      Serial.println("Acknowledgment sent with success");
    } else {
      Serial.println("Error sending the acknowledgment");
    }
  }
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

  // Set the receiver's address for acknowledgment
  memcpy(receiverAddress, peerInfo.peer_addr, 6);
}

void loop() {
  BME280Readings.temp = temperature;
  BME280Readings.hum = humidity;
  BME280Readings.pres = pressure;

  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *)&BME280Readings, sizeof(BME280Readings));

  if (result == ESP_OK) {
    Serial.println("Sent with success");
  } else {
    Serial.println("Error sending the data");
  }

  // Delay added to allow time for acknowledgment reception
  delay(1000);
}
