#include <SoftwareSerial.h>
#include <DFRobotDFPlayerMini.h>
#include <L298N.h>

#define s0 8
#define s1 9
#define s2 10
#define s3 11
#define sensorOut 12

// Motor pinleri
const unsigned int IN1 = 6;
const unsigned int IN2 = 5;
const unsigned int IN3 = 4;
const unsigned int IN4 = 3;
const unsigned int ENA = 7;
const unsigned int ENB = 2;
#define butonBlack 14
#define buzzer 13

L298N motorLeft(ENA, IN1, IN2);
L298N motorRight(ENB, IN3, IN4);

SoftwareSerial mySerial(0, 1);  // RX, TX
DFRobotDFPlayerMini myDFPlayer;

bool isMoving = false;  // Buton durumu için bayrak

int K, Y, M = 0;                                                                           //3 ana renk için değişken tanımlıyoruz
int renkSayaci = 1;                                                                        // Renk sırasını takip etmek için sayaç
String renkler[] = { "Kırmızı", "Yeşil", "Mavi", "Kırmızı", "Yeşil", "Mavi", "Kırmızı" };  // Renk sırası (7 renk için)

// Ses dosyalarının çalınma süreleri (milisaniye cinsinden)
int sesSureleri[] = { 22000, 24000, 25000, 21000, 22000, 26000, 30000 };  // 1-7.mp3 için süreler (0.mp3 süresi kullanılmıyor)

// Kalibrasyon değerleri
int whiteCalibration[3] = { 0, 0, 0 };  // Beyaz kalibrasyon değerleri (Kırmızı, Yeşil, Mavi)
int blackCalibration[3] = { 0, 0, 0 };  // Siyah kalibrasyon değerleri (Kırmızı, Yeşil, Mavi)

void setup() {
  pinMode(butonBlack, INPUT);
  pinMode(buzzer, OUTPUT);

  Serial.begin(9600);
  mySerial.begin(9600);

  if (!myDFPlayer.begin(mySerial)) {
    Serial.println("DFPlayer Mini bağlantısı başarısız!");
    while (true)
      ;
  }
  Serial.println("DFPlayer Mini hazır.");
  myDFPlayer.volume(21);  // Ses seviyesi 0-30

  motorLeft.setSpeed(50);
  motorRight.setSpeed(50);

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
      moveForward();
    } else {
      Serial.println("Butona basıldı, hareket duruyor...");
      stopMotors();
    }
    while (digitalRead(butonBlack) == HIGH)
      ;  // Buton bırakılana kadar bekle
  }

  if (renkSayaci != 0) {
    if (isMoving) {
      readColorValues();
      String color = detectColor(K, Y, M);
      Serial.println("Algılanan Renk: " + color);

      if (color == "Beyaz") {  // Beyaz ise harekete devam et
        moveForward();
      } else if (color != "Bilinmiyor") {
        stopMotors();
        playSound(color);  // Renk adını parametre olarak gönder
        moveForward();
      }
    }
  }
  else{
    stopMotors();
    isMoving = !isMoving;
  }
}

void readColorValues() {
  digitalWrite(s2, LOW);  //Kırmızıyı filtrelemek için
  digitalWrite(s3, LOW);
  K = pulseIn(sensorOut, LOW);  //OUT pini üzerindeki LOW süresini okur
  Serial.print("Kırmızı= ");
  Serial.print(K);  //Kırmızı için aldığımız değeri serial monitöre yazdır
  Serial.print("  ");
  delay(50);               //50 milisaniye bekle
  digitalWrite(s2, HIGH);  //Yeşili filtrelemek için
  digitalWrite(s3, HIGH);
  Y = pulseIn(sensorOut, LOW);  //OUT pini üzerindeki LOW süresini okur
  Serial.print("Yeşil= ");
  Serial.print(Y);  //Yeşil için aldığımız değeri serial monitöre yazdır
  Serial.print("   ");
  delay(50);              //50 milisaniye bekle
  digitalWrite(s2, LOW);  //Maviyi filtrelemek için
  digitalWrite(s3, HIGH);
  M = pulseIn(sensorOut, LOW);  //OUT pini üzerindeki LOW süresini okur
  Serial.print("Mavi= ");
  Serial.print(M);  //Mavi için aldığımız değeri serial monitöre yazdır
  Serial.println();
  delay(50);  //50 milisaniye bekle
}

String detectColor(int K, int Y, int M) {
  // Kalibrasyon değerlerini kullanarak renkleri tespit et
  if (K < whiteCalibration[0] && Y < whiteCalibration[1] && M < whiteCalibration[2]) {
    return "Beyaz";
  } else if (K > blackCalibration[0] && Y > blackCalibration[1] && M > blackCalibration[2]) {
    return "Siyah";
  } else if (K < Y && K < M) {
    return "Kırmızı";
  } else if (Y < K && Y < M) {
    return "Yeşil";
  } else if (M < K && M < Y) {
    return "Mavi";
  } else {
    return "Bilinmiyor";
  }
}

void playSound(String color) {
  if (renkSayaci != 0) {
    Serial.println("Ses çalınıyor.");

    // Renk sırasını bul
    int renkIndex = -1;
    for (int i = 0; i < 7; i++) {  // 7 renk için döngü
      if (renkler[i] == color) {
        renkIndex = i;
        Serial.println("Çalınan ses: " + String(renkSayaci) + ".mp3");
        break;
      }
    }

    if (renkIndex != -1) {              // Renk listede bulunduysa
      int trackNumber = renkIndex + 1;  // Şarkı numarası (1-7 arası)
      Serial.println("index: " + String(renkSayaci));
      myDFPlayer.playMp3Folder(renkSayaci);

      // Bekleme süresi (ilgili sesin süresi)
      int beklemeSuresi = sesSureleri[renkSayaci - 1];
      delay(beklemeSuresi);  // Şarkı süresi kadar bekle

      renkSayaci++;          // Sayacı artır
      if (renkSayaci > 7) {  // 7 renk için
        renkSayaci = 0;      // bitir
      }
    }
  }
}

void moveForward() {
  motorLeft.forward();
  motorRight.forward();
  Serial.println("İleri gidiyor");
}

void stopMotors() {
  motorLeft.stop();
  motorRight.stop();
  Serial.println("Durdu");
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

// Beyaz yüzeyi kalibre et
void calibrateWhite() {
  readColorValues();
  whiteCalibration[0] = K;
  whiteCalibration[1] = Y;
  whiteCalibration[2] = M;
}

// Siyah yüzeyi kalibre et
void calibrateBlack() {
  readColorValues();
  blackCalibration[0] = K;
  blackCalibration[1] = Y;
  blackCalibration[2] = M;
}