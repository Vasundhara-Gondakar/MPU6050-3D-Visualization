#include <WiFi.h>
#include <WiFiUdp.h>
#include <Wire.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>

// ===== Access Point credentials =====
const char* ssid = "ESP32_AP";       // ESP32 network name
const char* password = "123456789";   // Password

// ===== UDP Settings =====
WiFiUDP udp;
IPAddress broadcastIP(192,168,4,255); // broadcast address
const int udpPort = 4210;             // UDP port

// ===== MPU6050 Setup =====
Adafruit_MPU6050 mpu;

void setup() {
  Serial.begin(115200);

  // Start ESP32 as Wi-Fi Access Point
  WiFi.softAP(ssid, password);
  IPAddress IP = WiFi.softAPIP();
  Serial.print("ESP32 AP started. IP address: ");
  Serial.println(IP);   // usually 192.168.4.1

  // Start UDP
  udp.begin(udpPort);
  Serial.println("UDP ready (broadcast mode).");

  // Initialize MPU6050
  if (!mpu.begin()) {
    Serial.println("Failed to find MPU6050 chip");
    while (1) delay(10);
  }

  // Configure sensor
  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);

  Serial.println("MPU6050 initialized.");
}

void loop() {
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  // Prepare float buffer (6 values)
  float data[6];
  data[0] = a.acceleration.x;
  data[1] = a.acceleration.y;
  data[2] = a.acceleration.z;
  data[3] = g.gyro.x;
  data[4] = g.gyro.y;
  data[5] = g.gyro.z;

  // Debug print to Serial Monitor
  Serial.print("Accel (m/s^2): X=");
  Serial.print(data[0]); Serial.print(" Y=");
  Serial.print(data[1]); Serial.print(" Z=");
  Serial.println(data[2]);

  Serial.print("Gyro (rad/s): X=");
  Serial.print(data[3]); Serial.print(" Y=");
  Serial.print(data[4]); Serial.print(" Z=");
  Serial.println(data[5]);
  Serial.println("---------------------");

  // Send as binary packet (broadcast)
  udp.beginPacket(broadcastIP, udpPort);
  udp.write((uint8_t*)data, sizeof(data)); // send raw floats
  udp.endPacket();

  delay(5); // ~200 Hz update rate
}
