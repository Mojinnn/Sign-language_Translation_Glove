#include "SoftwareSerial.h"
#include "DFRobotDFPlayerMini.h"
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define flexPin_tro  A0
#define flexPin_giua A1
#define flexPin_nhan A2
#define flexPin_ut A3
#define MPU_ADDR 0x68  // Địa chỉ I2C của MPU6050

LiquidCrystal_I2C lcd(0x27 , 16, 2);

SoftwareSerial mySoftwareSerial(10, 11); // TX = 10, RX = 11
DFRobotDFPlayerMini myDFPlayer;
void printDetail(uint8_t type, int value);

void setup() {
  //Module Audio & FlexSensor
  mySoftwareSerial.begin(9600);  // Giao tiếp với DFPlayer Mini
  Serial.begin(115200);

  if (!myDFPlayer.begin(mySoftwareSerial)) { 
    Serial.println("Không tìm thấy DFPlayer Mini!");
    while (true) {
      delay(0);
    }
  }
  Serial.println("DFPlayer Mini sẵn sàng!");
  delay(1000);
  myDFPlayer.volume(30);  // Đặt âm lượng (0-30)

  //Module LCD
  lcd.init();
  lcd.backlight();

  //Module MPU6050
  Wire.begin();
  // Đánh thức MPU6050 (nó mặc định ở chế độ ngủ)
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x6B);  // Thanh ghi PWR_MGMT_1
  Wire.write(0);     // Đặt về 0 để đánh thức
  Wire.endTransmission(true);
}

int prevAccelX = 0;         // Lưu giá trị gia tốc trước đó

// Hien thi LCD
void LCD_display(const char* text) {
  lcd.clear(); 
  delay(100);
  lcd.setCursor(1, 0);
  lcd.print(text);
}

// Doc flex sensor
int readFlexSensors() {
  // Đọc giá trị từ các cảm biến flex và chuyển thành dạng nhị phân
  int flexValue_tro = analogRead(flexPin_tro);
  int binaryValue_tro = (flexValue_tro > 900) ? 1 : 0;

  int flexValue_giua = analogRead(flexPin_giua);
  int binaryValue_giua = (flexValue_giua > 900) ? 1 : 0;

  int flexValue_nhan = analogRead(flexPin_nhan);
  int binaryValue_nhan = (flexValue_nhan > 900) ? 1 : 0;

  int flexValue_ut = analogRead(flexPin_ut);
  int binaryValue_ut = (flexValue_ut > 950) ? 1 : 0;

  // Ghép các giá trị nhị phân thành một số nguyên
  int binaryValue = binaryValue_tro * 1000 + 
                    binaryValue_giua * 100 + 
                    binaryValue_nhan * 10 + 
                    binaryValue_ut * 1;

  return binaryValue;
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
      Serial.print("Theta: "); Serial.print(theta); Serial.println("°");

      int binaryValue = readFlexSensors();

      Serial.println("---------");
      Serial.print("Ngon tay: ");
      Serial.println(binaryValue);

      if (theta > 80 && theta < 105) { // neu nguoc chieu MPU, con thuan chieu la 65 < theta < 85
        if (binaryValue == 1111) {
          myDFPlayer.play(1);
          Serial.println("Hello");
          LCD_display("Hello");
          delay(1000);
        }
        else if (binaryValue == 1001) {
          myDFPlayer.play(5);
          Serial.println("I Love You");
          LCD_display("I Love You");
          delay(1000);
        }
      }
      else if (theta > 130 && theta < 150) { // neu nguoc chieu MPU, con thuan chieu la 10 < theta < 30
        if (binaryValue == 1100) {
          myDFPlayer.play(3);
          Serial.println("No");
          LCD_display("No");
          delay(1000);
        }
        else if (binaryValue == 0) {
          myDFPlayer.play(4);
          Serial.println("Yes");
          LCD_display("Yes");
          delay(1000);
        }
      }
    }

    // Cập nhật giá trị trước đó
    prevAccelX = accelX;
  } else {
    Serial.println("Không đọc được dữ liệu từ MPU6050!");
  }

  delay(200);

}