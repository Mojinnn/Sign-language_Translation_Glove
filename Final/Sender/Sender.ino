#include <SPI.h> // Thu vien giao tiep SPI dung cho NRF24
#include "RF24.h" // Thu vien dieu khien NRF24
#include <Wire.h> // Thu vien giao tiep I2C dung cho MPU6050

// Dinh nghia chan cho cac flex sensor
#define flexPin_tro  A0
#define flexPin_giua A1
#define flexPin_nhan A2
#define flexPin_ut   A3
#define MPU_ADDR 0x68  // Dinh nghia dia chi cua MPU6050

RF24 radio(9, 10); // CE = 9, CSN = 10
const byte address[6] = "00001";

int prevAccelX = 0;  // Luu gia tri gia toc X truoc do

void setup() {
    Serial.begin(115200);
    Wire.begin();
    radio.begin();
    radio.openWritingPipe(address);
    radio.setPALevel(RF24_PA_MIN);
    radio.stopListening();
    
    // Khởi động MPU6050
    Wire.beginTransmission(MPU_ADDR);
    Wire.write(0x6B);
    Wire.write(0);
    Wire.endTransmission();
}

float readTheta() {
    Wire.beginTransmission(MPU_ADDR);
    Wire.write(0x3B);  // Doc thanh ghi ACCEL_XOUT_H
    Wire.endTransmission(false);
    Wire.requestFrom(MPU_ADDR, 6, true);  // Yeu cau 6 byte du lieu cho gia toc

    if (Wire.available() >= 6) {
        int accelX = Wire.read() << 8 | Wire.read();
        int accelY = Wire.read() << 8 | Wire.read();
        int accelZ = Wire.read() << 8 | Wire.read();

        // Kiem tra chuyen dong truc X
        float deltaX = abs(accelX - prevAccelX);
        int threshold = 500;  // Nguong phat hien chuyen dong

        // Cap nhat gia tri truoc do
        prevAccelX = accelX;

        if (deltaX > threshold) {
          Serial.println("MPU ĐANG DI CHUYỂN THEO TRỤC X!");
        }
        else {
          // Tinh toan goc nghieng theta
          float Ax = accelX / 16384.0;
          float Ay = accelY / 16384.0;
          float Az = accelZ / 16384.0;
          float theta = acos(Az / sqrt(Ax * Ax + Ay * Ay + Az * Az)) * 180.0 / PI;
          return theta;
        }

        
    }
    return -1;
}

int readFlexSensors() {
    int flexValue_tro  = (analogRead(flexPin_tro) > 900) ? 1 : 0;
    int flexValue_giua = (analogRead(flexPin_giua) > 900) ? 1 : 0;
    int flexValue_nhan = (analogRead(flexPin_nhan) > 900) ? 1 : 0;
    int flexValue_ut   = (analogRead(flexPin_ut) > 950) ? 1 : 0;

    return flexValue_tro * 1000 + flexValue_giua * 100 + flexValue_nhan * 10 + flexValue_ut;
}

void loop() {
    float theta = readTheta();
    int flexdata = readFlexSensors();

    struct DataPacket {
        float theta;
        int flexdata;
    } data;

    data.theta = theta;
    data.flexdata = flexdata;

    radio.write(&data, sizeof(data));

    Serial.print("Sent - Theta: ");
    Serial.print(theta);
    Serial.print(", Flexdata: ");
    Serial.println(flexdata);
    
    delay(200);
}
