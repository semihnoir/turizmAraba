#include <SoftwareSerial.h>
#include <DFRobotDFPlayerMini.h>
#include <SPI.h>
#include <MFRC522.h>
#include "Settings.h"
#include "LineFollow.h"
#include "RFID.h"

int sesSureleri[7] = {};  // Ses dosyalarının sürelerini tutan dizi
int bolgeSesleri[7]{};    // Bolge seslerini tutan dizi
int yolculukSesi = 0;

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

  // RFID başlat
  SPI.begin();
  rfid.PCD_Init();
  Serial.println("RFID okuyucu hazır. Kartı okutun...");

  if (!myDFPlayer.begin(mySerial)) {
    Serial.println("DFPlayer Mini bağlantısı başarısız!");
    while (true)
      ;
  }
  Serial.println("DFPlayer Mini hazır.");
  myDFPlayer.volume(volume);  // Ses seviyesi 0-30

  // Başlangıç sesini çal (8.mp3)
  Serial.println("Başlangıç sesi çalınıyor...");
  myDFPlayer.playMp3Folder(8);
  delay(4000);

  renkSayaci = 1;  // Başlangıçta 1'den başlat

  // Dil seçimi için buton kontrolü
  bool isEnglish = false;
  unsigned long startTime = 0;

  // Önce butonun basılmasını bekle
  while (digitalRead(butonBlack) == LOW) {
    delay(10);
  }

  // Buton basıldığında zamanı kaydet
  startTime = millis();

  // Buton bırakılana kadar bekle ve süreyi kontrol et
  while (digitalRead(butonBlack) == HIGH) {
    if (millis() - startTime >= 2000) {
      isEnglish = true;  // 2 saniye geçtiyse İngilizce mod
      beepShort();
      break;
    }
    delay(10);
  }

  // Dil seçimine göre dizileri kopyala
  if (isEnglish) {
    Serial.println("İngilizce mod seçildi");
    yolculukSesi = yolculukSesleri[1];
    // İngilizce dizileri kopyala
    for (int i = 0; i < 7; i++) {
      sesSureleri[i] = sesSureleriEn[i];
      bolgeSesleri[i] = bolgeSesleriEn[i];
    }
  } else {
    Serial.println("Türkçe mod seçildi");
    yolculukSesi = yolculukSesleri[0];
    // Türkçe dizileri kopyala
    for (int i = 0; i < 7; i++) {
      sesSureleri[i] = sesSureleriTr[i];
      bolgeSesleri[i] = bolgeSesleriTr[i];
    }
    beepShort();
    delay(200);
    beepShort();
  }

  // Debug için dizi içeriklerini yazdır
  Serial.println("Seçilen dilde ses dosyası numaraları:");
  Serial.print("Bölge sesleri: ");
  for (int i = 0; i < 7; i++) {
    Serial.print(bolgeSesleri[i]);
    Serial.print(" ");
  }
  Serial.println();

  // Butonun bırakılmasını bekle
  while (digitalRead(butonBlack) == HIGH) {
    delay(10);
  }

  myDFPlayer.playMp3Folder(9);
  Serial.println("Sistem hazır. Başlamak için siyah butona basın.");
}

void loop() {
  if (digitalRead(butonBlack) == HIGH) {
    renkSayaci = 1;
    beepLong();
    isMoving = !isMoving;  // Butona basıldığında hareket durumunu değiştir
    if (isMoving) {
      Serial.println("Butona basıldı, hareket başlıyor...");
      myDFPlayer.playMp3Folder(yolculukSesi);
      unsigned long startTime = millis();
      while (millis() - startTime < 10000) {
        if (digitalRead(butonBlack) == HIGH) {
          while (digitalRead(butonBlack) == HIGH)
            ;  // Butonun bırakılmasını bekle
          myDFPlayer.stop();
          break;
        }
        delay(10);
      }
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

  // RFID kart okuma
  if (rfid.PICC_IsNewCardPresent() && rfid.PICC_ReadCardSerial()) {
    Serial.print("UID:");
    for (int i = 0; i < rfid.uid.size; i++) {
      Serial.print(rfid.uid.uidByte[i] < 0x10 ? " 0" : " ");
      Serial.print(rfid.uid.uidByte[i], HEX);
    }
    Serial.println();

    bool eslesme = false;
    int bolgeIndex = -1;

    for (int i = 0; i < 7; i++) {
      if (memcmp(rfid.uid.uidByte, bolgeler[i].ID, 4) == 0) {
        Serial.print("Kart ");
        Serial.print(bolgeler[i].isim);
        Serial.println(" ile eşleşti!");
        eslesme = true;
        bolgeIndex = i;
        break;
      }
    }

    if (eslesme && bolgeIndex >= 0) {
      playSound(bolgeIndex);
    } else {
      Serial.println("Bölge bulunamadı, tanımlı değil.");
    }

    rfid.PICC_HaltA();
    rfid.PCD_StopCrypto1();
  }
}

void playSound(int bolgeIndex) {
  if (renkSayaci != 0) {
    stopMotors();  // Motoru durdur

    Serial.println("Ses çalınıyor.");
    Serial.print("Çalınacak ses dosyası numarası: ");
    Serial.println(bolgeSesleri[bolgeIndex]);  // Debug için ekledik

    // Ses dosyasını çal
    myDFPlayer.playMp3Folder(bolgeSesleri[bolgeIndex]);

    // Bekleme süresi (ilgili sesin süresi)
    int beklemeSuresi = sesSureleri[bolgeIndex];
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

    moveForward();
    delay(250);
    stopMotors();
    renkSayaci++;  // Sayacı artır

    // renkSayaci 7'den büyükse
    if (renkSayaci > 7) {
      Serial.println("Tüm bölgeler tamamlandı!");
      renkSayaci = 7;    // 7'de tut, 0 yapmak yerine
      isMoving = false;  // Hareketi durdur
      beepLong();
      beepLong();
    }

    Serial.print("Bölge indeksi: ");
    Serial.println(bolgeIndex);
    Serial.print("Mevcut renkSayaci: ");
    Serial.println(renkSayaci);
  }
}

// Function to play a short beep sound (normal beep)
void beepShort() {
  myDFPlayer.playMp3Folder(10);
  delay(500);
}

// Function to play a long beep sound (indicating start)
void beepLong() {
  myDFPlayer.playMp3Folder(11);
  delay(1000);
}