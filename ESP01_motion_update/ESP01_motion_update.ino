#include <SoftwareSerial.h>
#include <stdlib.h>

#define IN 1
#define OUT 2
#define TOKEN 3001  //첫번째 숫자가 홈아이디(3),모션이 1, 조도가 2
String serverHost = "3.36.174.74";

const int trigPin = 6;
const int echoPin = 5;
const int motionPin = 8;
//초음파 센서 핀설정

SoftwareSerial ser(10, 11); // RX/TX 설정, serial 객체생성

// 핀설정
int pre = -1;
String str = "";
int isRegister = 0;

int judge(int Pin) {
  if (pre == -1) {
    pre = Pin;
    return 0;
  }
  if (pre == Pin) {
    return 0;
  }
  pre = Pin;
  return Pin == motionPin ? IN : OUT;
  // 0 무시 - 1. 초기값 -1 , 2. pre == pin
  // 1 in
  // 2 out
}



void setup() {
  //알림 LED 아웃풋 설정
  pinMode(motionPin, INPUT);
  pinMode(echoPin, INPUT);   // echoPin 입력
  pinMode(trigPin, OUTPUT);  // trigPin 출력
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
    String getStr = "GET /sensor/unregistered/register?token=";
    getStr += TOKEN;
    getStr += "&type=";
    getStr += "IN_OUT";
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
    
    long duration, distance;
    int motionpin = digitalRead(motionPin);
    
    digitalWrite(trigPin, HIGH);  // trigPin에서 초음파 발생(echoPin도 HIGH)
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);
    duration = pulseIn(echoPin, HIGH);    // echoPin 이 HIGH를 유지한 시간을 저장 한다.
    distance = ((float)(340 * duration) / 1000) / 2;

    Serial.println(distance);
    Serial.println(motionpin);

    delay(500);
    //
    if (motionpin == HIGH) {
      int result = judge(motionPin);
      if (result == 1) {      //result == 1 in임을 서버로 전달
        Serial.println("IN");
        str = "IN";
    while(isRegister ==1){
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
        String getStr = "GET /sensor/update?status=";
        getStr += str;
        getStr += "&token=";
        getStr += TOKEN;
        getStr += "\r\n\r\n";

        // Send Data
        cmd = "AT+CIPSEND=";
        cmd += String(getStr.length());
        ser.println(cmd);

        if (ser.find(">")) {
          ser.print(getStr);
          isRegister = 2;
        }
        else {
          ser.println("AT+CIPCLOSE");
          // alert user
          Serial.println("AT+CIPCLOSE");
        }
        //최소 업로드 간격 delay
        delay(5000);
        }
      }
    }
    
    if (distance >= 200) {
      int result = judge(echoPin);
      if (result == 2) {      //result == 2 out임을 서버로 전달
        Serial.println("OUT");
        str = "OUT";
        // TCP 연결
        while(isRegister == 1){
        String cmd = "AT+CIPSTART=\"TCP\",\"";
        cmd += serverHost; //접속 IP
        cmd += "\",8080";  //접속 포트, 80
        ser.println(cmd);

        if (ser.find("Error")) {
          Serial.println("AT+CIPSTART error");
          return;
        }

        // GET 방식으로 보내기 위한 String, Data 설정
        String getStr = "GET /sensor/update?status=";
        getStr += str;
        getStr += "&token=";
        getStr += TOKEN;
        getStr += "\r\n\r\n";


        // Send Data
        cmd = "AT+CIPSEND=";
        cmd += String(getStr.length());
        ser.println(cmd);

        if (ser.find(">")) {
          ser.print(getStr);
          isRegister = 2;
        }
        else {
          ser.println("AT+CIPCLOSE");
          // alert user
          Serial.println("AT+CIPCLOSE");
        }

        //최소 업로드 간격 delay
        delay(5000);
        }
      }
    }
  }

}
