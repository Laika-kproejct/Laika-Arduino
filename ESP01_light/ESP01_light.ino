#include <SoftwareSerial.h>
#include <stdlib.h>

#define TOKEN 2002 //첫번째 숫자가 홈아이디(3),모션이 1, 조도가 2

String serverHost = "3.36.174.74";

int lightPin = 0;

int val = 0;
int val2 = 0;
int num = 0;
int isRegister = 0;

SoftwareSerial ser(10, 11); // RX/TX 설정, serial 객체생성

void setup() {
  //시리얼통신속도 9600보드레이트 설정
  Serial.begin(9600);
  //소프트웨어 시리얼 시작
  ser.begin(9600);
  //ESP8266 Reset
  ser.println("AT+RST");
}

void loop() {

  if (isRegister == 0) {
    String cmd = "AT+CIPSTART=\"TCP\",\"";
    cmd += serverHost; //접속 IP
    cmd += "\",8080";  //접속 포트, 80
    ser.println(cmd);
    if (ser.find("Error")) {
      Serial.println("AT+CIPSTART error");
      return;
    }
    String getStr = "GET /sensor/register?token=";
    getStr += TOKEN;
    getStr += "&type=";
    getStr += "LIGHT";
    getStr += "\r\n\r\n";

    // Send Data
    cmd = "AT+CIPSEND=";
    cmd += String(getStr.length());
    ser.println(cmd);

    if (ser.find(">")) {
      ser.print(getStr);
      isRegister = 1;
    }
    else {
      ser.println("AT+CIPCLOSE");
      // alert user
      Serial.println("AT+CIPCLOSE");
    }
  }
  else {
    isRegister = 1;
    val2 = val;
    val = analogRead(lightPin);
    delay(500);
    num = val - val2;
    if (num < 0) {
      num = (-1) * num;
    }
    if (num > 50) {
      // String 변환
      char buf[16];
      String strMove = dtostrf(val, 4, 1, buf);

      Serial.println(strMove);

      // TCP 연결
      String cmd = "AT+CIPSTART=\"TCP\",\"";
      cmd += serverHost; //접속 IP
      cmd += "\",8080";  //접속 포트, 80
      ser.println(cmd);

      if (ser.find("Error")) {
        Serial.println("AT+CIPSTART error");
        return;
      }

      // GET 방식으로 보내기 위한 String, Data 설정
      String getStr = "GET /sensor/update?token=";
      getStr += TOKEN;
      getStr += "\r\n\r\n";

      // Send Data
      cmd = "AT+CIPSEND=";
      cmd += String(getStr.length());
      ser.println(cmd);

      if (ser.find(">")) {
        ser.print(getStr);
      }
      else {
        ser.println("AT+CIPCLOSE");
        // alert user
        Serial.println("AT+CIPCLOSE");
      }

      //최소 업로드 간격 delay
      delay(1000);
    }
  }
}
