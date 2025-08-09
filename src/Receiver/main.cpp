#include <esp_now.h>
#include <WiFi.h>
#include <ESP32Servo.h>

// -------------------- Pins --------------------
static const int servoPin = 13;

// DRV8833 pins
const int AIN1 = 14;  
const int AIN2 = 27;  
const int BIN1 = 26;  
const int BIN2 = 25;  

// -------------------- Objects --------------------
Servo steering;  // servo control

int steerAngle;
int driveValue;

// -------------------- Deadband settings --------------------
const int DEAD_LOW  = 2000;  // below this = reverse
const int DEAD_HIGH = 2001;  // above this = forward
const int CENTER_LOW = 1950; // low end of deadband
const int CENTER_HIGH = 2050; // high end of deadband

// -------------------- ESP-NOW data structure --------------------
typedef struct struct_message {
  int receiveX;
  int receiveY;
} struct_message;

struct_message receiveData;

// -------------------- Motor control function --------------------
void setMotors(int driveVal) {
  if (driveVal >= CENTER_LOW && driveVal <= CENTER_HIGH) {
    // Deadband: stop
    analogWrite(AIN1, 0);
    analogWrite(AIN2, 0);
    analogWrite(BIN1, 0);
    analogWrite(BIN2, 0);
    return;
  }

  int speed;
  if (driveVal < DEAD_LOW) {
    // Reverse
    speed = map(driveVal, 0, DEAD_LOW, 255, 0);
    analogWrite(AIN1, 0);
    analogWrite(AIN2, speed);
    analogWrite(BIN1, 0);
    analogWrite(BIN2, speed);
  }
  else if (driveVal > DEAD_HIGH) {
    // Forward
    speed = map(driveVal, DEAD_HIGH, 4095, 0, 255);
    analogWrite(AIN1, speed);
    analogWrite(AIN2, 0);
    analogWrite(BIN1, speed);
    analogWrite(BIN2, 0);
  }
}

// -------------------- ESP-NOW callback --------------------
void OnDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len) {
  memcpy(&receiveData, incomingData, sizeof(receiveData));

  int receivedX = receiveData.receiveX;
  int receivedY = receiveData.receiveY;

  driveValue = map(receivedX, 0, 4095, 0, 4095);
  steerAngle = map(receivedY, 0, 4095, 0, 180);

  steering.write(steerAngle);
  setMotors(driveValue);

  // Debug printing
  Serial.printf("X: %d  Y: %d  Mapped Drive: %d  Steering: %d\n", 
                receivedX, receivedY, driveValue, steerAngle);
}

void setup() {
  Serial.begin(115200);
  Serial.println("Initialising Receiver");

  WiFi.mode(WIFI_STA);

  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Motor pins
  pinMode(AIN1, OUTPUT);
  pinMode(AIN2, OUTPUT);
  pinMode(BIN1, OUTPUT);
  pinMode(BIN2, OUTPUT);

  steering.attach(servoPin);
  steering.write(90);

  esp_now_register_recv_cb(esp_now_recv_cb_t(OnDataRecv));
  Serial.println("Set up done");
}

void loop() {
  // nothing — all handled in OnDataRecv()
}
