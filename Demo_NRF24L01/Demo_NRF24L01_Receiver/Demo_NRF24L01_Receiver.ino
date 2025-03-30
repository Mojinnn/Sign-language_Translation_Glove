#include <SPI.h>
#include "RF24.h"
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "SoftwareSerial.h"
#include "DFRobotDFPlayerMini.h"

RF24 radio(9, 10);  // CE = 9, CSN = 10 (SPI cho NRF24L01)
const byte address[6] = "00001";  // Dia chi giao tiep NRF24L01

LiquidCrystal_I2C lcd(0x27, 16, 2);  // LCD I2C 16x2
SoftwareSerial dfSerial(3, 4);  // RX = 3, TX = 4 (DFPlayer Mini)
DFRobotDFPlayerMini myDFPlayer;

struct DataPacket {
    float theta;
    int flexdata;
} data;

void setup() {
    Serial.begin(115200);
    Wire.begin();
    
    lcd.init();
    lcd.backlight();
    lcd.setCursor(0, 0);
    lcd.print("Initializing...");

    // Khởi động DFPlayer Mini
    dfSerial.begin(9600);
    if (!myDFPlayer.begin(dfSerial)) {
        Serial.println("Không tìm thấy DFPlayer Mini!");
        lcd.clear();
        lcd.print("DFPlayer loi !");
        while (true) {}
    }
    myDFPlayer.volume(30);

    // Khởi động NRF24L01
    if (!radio.begin()) {
        Serial.println("NRF24L01 lỗi!");
        lcd.clear();
        lcd.print("NRF24 lỗi!");
        while (true) {}
    }
    radio.openReadingPipe(0, address);
    radio.setPALevel(RF24_PA_LOW);
    radio.startListening();

    Serial.println("Receiver sẵn sàng!");
}

void LCD_display(const char* text) {
    lcd.clear();
    lcd.setCursor(1, 0);
    lcd.print(text);
}

void loop() {
    if (radio.available()) {
        radio.read(&data, sizeof(data));

        Serial.print("Nhận dữ liệu - Theta: ");
        Serial.print(data.theta);
        Serial.print("°, Flexdata: ");
        Serial.println(data.flexdata);

        if (data.theta > 65 && data.theta < 85) {
            if (data.flexdata == 1111) {
                myDFPlayer.play(1);
                Serial.println("Phát: Hello");
                LCD_display("Hello");
                delay(1000);
            }
            else if (data.flexdata == 1001) {
                myDFPlayer.play(4);
                Serial.println("Phát: I Love You");
                LCD_display("I Love You");
                delay(1000);
            }
        }
        else if (data.theta > 15 && data.theta < 35) {
            if (data.flexdata == 1100) {
                myDFPlayer.play(3);
                Serial.println("Phát: No");
                LCD_display("No");
                delay(1000);
            }
            else if (data.flexdata == 0) {
                myDFPlayer.play(5);
                Serial.println("Phát: Yes");
                LCD_display("Yes");
                delay(1000);
            }
        }
    }
}
