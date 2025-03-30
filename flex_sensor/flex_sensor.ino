#include "SoftwareSerial.h"
#include "DFRobotDFPlayerMini.h"

const int flexPin_cai = A0;
const int flexPin_tro = A1;

SoftwareSerial mySoftwareSerial(10, 11); // TX = 10, RX = 11
DFRobotDFPlayerMini myDFPlayer;
void printDetail(uint8_t type, int value);


void setup() {
  mySoftwareSerial.begin(9600);  // Giao tiếp với DFPlayer Mini
  Serial.begin(115200);

  if (!myDFPlayer.begin(mySoftwareSerial)) { 
    Serial.println("Không tìm thấy DFPlayer Mini!");
    while (true) {
      delay(0);
    }
  }
  Serial.println("DFPlayer Mini sẵn sàng!");
  myDFPlayer.volume(30);  // Đặt âm lượng (0-30)
}

void loop() {
  int flexValue_cai;
  int flexValue_tro;
  // flexValue_cai = analogRead(flexPin_cai);
  // Serial.print("ngon cai: ");
  // Serial.println(flexValue_cai);

  flexValue_tro = analogRead(flexPin_tro);
  int binaryValue = (flexValue_tro > 400) ? 1 : 0;

  Serial.print("ngon tro: ");
  Serial.println(flexValue_tro);
  Serial.print("ngon tro: ");
  Serial.println(binaryValue);

  if (binaryValue == 1) {
    myDFPlayer.play(1);
    Serial.println("Hello");
    delay(1000);
  }

  delay(200);
  
  }