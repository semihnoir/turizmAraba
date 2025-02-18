#include <SoftwareSerial.h>
#include <DFRobotDFPlayerMini.h>
#include "Settings.h"
#include "LineFollow.h"
#include "ColorDedection.h"

void setup() {
  pinMode(butonBlack, INPUT);
  pinMode(buzzer, OUTPUT);
  pinMode(ENA, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(ENB, OUTPUT);

  pinMode(sensorPinLeft, INPUT);
  pinMode(sensorPinRight, INPUT);
  pinMode(sensorPinMiddle, INPUT);

  Serial.begin(9600);
  mySerial.begin(9600);

  if (!myDFPlayer.begin(mySerial)) {
    Serial.println("DFPlayer Mini bağlantısı başarısız!");
    while (true)
      ;
  }
  Serial.println("DFPlayer Mini hazır.");
  myDFPlayer.volume(24);  // Ses seviyesi 0-30

  pinMode(s0, OUTPUT);  //S0, S1, S2 ve S3 pinlerini OUTPUT olarak tanımlıyoruz
  pinMode(s1, OUTPUT);
  pinMode(s2, OUTPUT);
  pinMode(s3, OUTPUT);
  pinMode(sensorOut, INPUT);  //OUT pinini INPUT olarak tanımlıyoruz
  digitalWrite(s1, LOW);      //Frekans ölçeğimizi %20 olarak tanımlıyoruz
  digitalWrite(s0, HIGH);

  myDFPlayer.playMp3Folder(8);
  delay(5000);

  // Kalibrasyon işlemi
  Serial.println("Beyaz yüzeyi kalibre etmek için siyah butona basın.");
  myDFPlayer.playMp3Folder(9);
  while (digitalRead(butonBlack) == LOW)
    ;  // Butona basılmasını bekleyin
  beepShort();
  calibrateWhite();
  Serial.println("Beyaz kalibrasyon tamamlandı.");

  Serial.println("Siyah yüzeyi kalibre etmek için siyah butona basın.");
  myDFPlayer.playMp3Folder(10);
  while (digitalRead(butonBlack) == LOW)
    ;  // Butona basılmasını bekleyin
  beepShort();
  calibrateBlack();
  Serial.println("Siyah kalibrasyon tamamlandı.");
 
  Serial.println("Kırmızı yüzeyi kalibre etmek için siyah butona basın.");
  myDFPlayer.playMp3Folder(12);  // Kırmızı kalibrasyonu için ses dosyası
  while (digitalRead(butonBlack) == LOW)
    ;  // Butona basılmasını bekleyin
  beepShort();
  calibrateRed();
  Serial.println("Kırmızı kalibrasyon tamamlandı.");

  myDFPlayer.playMp3Folder(11);
}

void loop() {
  if (digitalRead(butonBlack) == HIGH) {
    renkSayaci = 1;
    beepLong();
    myDFPlayer.playMp3Folder(0);
    delay(10000);
    isMoving = !isMoving;  // Butona basıldığında hareket durumunu değiştir
    if (isMoving) {
      Serial.println("Butona basıldı, hareket başlıyor...");
    } else {
      Serial.println("Butona basıldı, hareket duruyor...");
      stopMotors();
    }
    while (digitalRead(butonBlack) == HIGH)
      ;  // Buton bırakılana kadar bekle
  }

  if (isMoving) {
    lineFollow();  // Çizgi takibini aktif et
  }

  if (renkSayaci != 0) {
    if (isMoving) {
      readColorValues();
      String color = detectColor(K, Y, M);
      Serial.println("Algılanan Renk: " + color);

      if (color == "Siyah" || color == "Beyaz" || color == "Bilinmiyor") {  // Siyah ise harekete devam et
        moveForward();
      } else if (color == "Kırmızı") {
        stopMotors();
        playSound(color);  // Renk adını parametre olarak gönder
        moveForward();
      }
    }
  } else {
    stopMotors();
    isMoving = !isMoving;
  }
}

void playSound(String color) {
  if (renkSayaci != 0) {
    Serial.println("Ses çalınıyor.");
    Serial.println("index: " + String(renkSayaci));
    myDFPlayer.playMp3Folder(renkSayaci);

    // Bekleme süresi (ilgili sesin süresi)
    int beklemeSuresi = sesSureleri[renkSayaci - 1];
    unsigned long startTime = millis();  // Süre başlangıcını kaydet

    while (millis() - startTime < beklemeSuresi) {
      // Buton kontrolü
      if (digitalRead(butonBlack) == HIGH) {
        myDFPlayer.stop();  // Butona basıldığında sesi durdur
        Serial.println("Ses durduruldu.");
        delay(500);  // Buton debounce için kısa bekleme
        break;       // Döngüden çık
      }
      delay(10);  // Buton kontrolü için kısa bekleme
    }

    renkSayaci++;          // Sayacı artır
    if (renkSayaci > 7) {  // 7 renk için
      renkSayaci = 0;      // bitir
    }
  }
}

// Function to play a short beep sound (normal beep)
void beepShort() {
  digitalWrite(buzzer, HIGH);
  delay(100);  // Short beep duration
  digitalWrite(buzzer, LOW);
  delay(100);  // Short pause between beeps
}

// Function to play a long beep sound (indicating start)
void beepLong() {
  digitalWrite(buzzer, HIGH);
  delay(500);  // Long beep duration
  digitalWrite(buzzer, LOW);
  delay(200);  // Pause between beeps
}