#include <Wire.h>

const int MPU_ADDR = 0x68;  // Địa chỉ I2C của MPU6050
int prevAccelX = 0;         // Lưu giá trị gia tốc trước đó

void setup() {
  Serial.begin(9600);
  Wire.begin();

  // Đánh thức MPU6050 (nó mặc định ở chế độ ngủ)
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x6B);  // Thanh ghi PWR_MGMT_1
  Wire.write(0);     // Đặt về 0 để đánh thức
  Wire.endTransmission(true);
}

void loop() {
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x3B);  // Bắt đầu đọc từ thanh ghi ACCEL_XOUT_H
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_ADDR, 6, true);  // Yêu cầu 6 byte dữ liệu (gia tốc XYZ)

  if (Wire.available() >= 6) {
    int accelX = Wire.read() << 8 | Wire.read();
    int accelY = Wire.read() << 8 | Wire.read();
    int accelZ = Wire.read() << 8 | Wire.read();

    // Kiểm tra nếu có sự thay đổi đáng kể ở trục X
    float deltaX = abs(accelX - prevAccelX); // Độ thay đổi gia tốc X
    int threshold = 500;  // Ngưỡng phát hiện chuyển động (điều chỉnh theo thực tế)

    Serial.print("AccX: "); Serial.print(accelX);
    
    if (deltaX > threshold) {
      Serial.println("  🚨 Đang di chuyển theo trục X!");
    } else {
      // Chuyển đổi giá trị ADC sang gia tốc (g = 9.81m/s²)
      float Ax = accelX / 16384.0; // MPU6050 có độ nhạy ±2g
      float Ay = accelY / 16384.0;
      float Az = accelZ / 16384.0;

      // Tính góc Roll và góc so với mặt phẳng ngang
      float roll  = atan2(Ay, Az) * 180.0 / PI;
      float theta = acos(Az / sqrt(Ax * Ax + Ay * Ay + Az * Az)) * 180.0 / PI;

      // Xuất kết quả
      Serial.print("Roll: "); Serial.print(roll); Serial.print("°");
      Serial.print("Theta: "); Serial.print(theta); Serial.println("°");
      if (theta > 80 && theta < 105) { // neu nguoc chieu MPU, con thuan chieu la 65 < theta < 85
        if (roll > -110 && roll < -90) { // neu nguoc chieu MPU, con thuan chieu la -70 < theta < -90
          Serial.println("Thank you");
        }
        else {
          Serial.println("Hello & I love you");

        }
      }
      else if (theta > 130 && theta < 150) { // neu nguoc chieu MPU, con thuan chieu la 10 < theta < 30
        Serial.println("Yes or No");
      }
    }

    // Cập nhật giá trị trước đó
    prevAccelX = accelX;
  } else {
    Serial.println("Không đọc được dữ liệu từ MPU6050!");
  }

  delay(200);
}
