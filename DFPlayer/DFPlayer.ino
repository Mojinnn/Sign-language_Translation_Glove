/* Cach lenh quan trong su dung DFPlayer

myDFPlayer.play(1);  // Phát bài số 1 trong thư mục gốc
myDFPlayer.playFolder(2, 5); // Phát bài số 5 trong thư mục số 2 (thư mục phải đặt tên là "02" trên thẻ nhớ)
myDFPlayer.pause();   // Tạm dừng bài hát
myDFPlayer.start();   // Tiếp tục phát bài hát
myDFPlayer.next();    // Chuyển sang bài tiếp theo
myDFPlayer.previous(); // Quay lại bài trước
myDFPlayer.volume(15);  // Đặt âm lượng (0-30)
myDFPlayer.volumeUp();  // Tăng âm lượng
myDFPlayer.volumeDown(); // Giảm âm lượng
myDFPlayer.loop(1); // Lặp lại bài số 1
myDFPlayer.randomAll();  // Phát ngẫu nhiên các bài hát
Serial.println(myDFPlayer.readState());  // Đọc trạng thái (đang phát hay dừng)
Serial.println(myDFPlayer.readVolume()); // Đọc mức âm lượng hiện tại
Serial.println(myDFPlayer.readCurrentFileNumber()); // Đọc số bài hát đang phát

*/

#include "SoftwareSerial.h"
#include "DFRobotDFPlayerMini.h"

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
  myDFPlayer.play(5);
  Serial.println("Dang hat");
  delay(100000);
}


