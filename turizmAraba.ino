#include <SoftwareSerial.h>
#include <DFRobotDFPlayerMini.h>
#include <SPI.h>
#include <MFRC522.h>
#include <WiFiS3.h>
#include "Settings.h"
#include "LineFollow.h"
#include "RFID.h"

// WiFi ağ bilgileri
const char* ssid = "LAB5"; // WiFi adını buraya girin
const char* password = ""; // WiFi şifresini buraya girin

// Web Server için gerekli değişkenler
WiFiServer server(80);
String header;
bool wifiConnected = false; // WiFi bağlantı durumunu kontrol etmek için

// Ses dosyalarının internet bağlantıları
const char* sesDosyalariURL[7] = {
  "http://turizmaraba.semihnoir.com/MP3/0001.mp3",
  "http://turizmaraba.semihnoir.com/MP3/0002.mp3",
  "http://turizmaraba.semihnoir.com/MP3/0003.mp3",
  "http://turizmaraba.semihnoir.com/MP3/0004.mp3",
  "http://turizmaraba.semihnoir.com/MP3/0005.mp3",
  "http://turizmaraba.semihnoir.com/MP3/0006.mp3",
  "http://turizmaraba.semihnoir.com/MP3/0007.mp3"
};

// Bölge isimleri
const char* bolgeIsimleri[7] = {
  "İç Anadolu",
  "Karadeniz",
  "Doğu Anadolu",
  "Güneydoğu Anadolu",
  "Akdeniz",
  "Ege",
  "Marmara"
};

int mevcutBolgeIndex = -1; // Şu an çalınan bölge indeksi
bool sesCaliyor = false;   // Ses çalma durumu

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
  
  // WiFi Bağlantısı Kur
  connectToWiFi();
  
  // Web Server'ı başlat
  if (wifiConnected) {
    server.begin();
    Serial.println("Web Server başlatıldı");
  } else {
    Serial.println("WiFi bağlantısı yok, sadece normal işlevler çalışacak");
  }
}

void loop() {
  // Web istemcilerini kontrol et (sadece WiFi bağlıysa)
  if (wifiConnected) {
    WiFiClient client = server.available();
    if (client) {
      handleClient(client);
    }
  }
  
  if (digitalRead(butonBlack) == HIGH) {
    renkSayaci = 1;
    beepLong();
    isMoving = !isMoving;  // Butona basıldığında hareket durumunu değiştir
    if (isMoving) {
      Serial.println("Butona basıldı, hareket başlıyor...");
      myDFPlayer.playMp3Folder(yolculukSesi);
      mevcutBolgeIndex = -1; // Yolculuk sesi çalınıyor
      sesCaliyor = true;
      unsigned long startTime = millis();
      while (millis() - startTime < 10000) {
        if (digitalRead(butonBlack) == HIGH) {
          while (digitalRead(butonBlack) == HIGH)
            ;  // Butonun bırakılmasını bekle
          myDFPlayer.stop();
          sesCaliyor = false;
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
    
    // Web sitesi için mevcut bölge bilgisini güncelle
    mevcutBolgeIndex = bolgeIndex;
    sesCaliyor = true;

    // Bekleme süresi (ilgili sesin süresi)
    int beklemeSuresi = sesSureleri[bolgeIndex];
    unsigned long startTime = millis();  // Süre başlangıcını kaydet

    while (millis() - startTime < beklemeSuresi) {
      // Web istemcilerini kontrol et (sadece WiFi bağlıysa)
      if (wifiConnected) {
        WiFiClient client = server.available();
        if (client) {
          handleClient(client);
        }
      }
      
      // Buton kontrolü
      if (digitalRead(butonBlack) == HIGH) {
        myDFPlayer.stop();  // Butona basıldığında sesi durdur
        sesCaliyor = false;
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
    sesCaliyor = false;

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

// WiFi bağlantısı kurma fonksiyonu
void connectToWiFi() {
  Serial.print("WiFi ağına bağlanılıyor: ");
  Serial.println(ssid);

  WiFi.begin(ssid);
  
  int attempts = 0;
  wifiConnected = false; // Başlangıçta bağlantı yok

  // En fazla 10 saniye (20 deneme) bekleyelim
  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    delay(500);
    Serial.print(".");
    attempts++;
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println();
    Serial.println("WiFi bağlantısı kuruldu");
    Serial.print("IP adresi: ");
    Serial.println(WiFi.localIP());
    wifiConnected = true; // Bağlantı başarılı
  } else {
    Serial.println();
    Serial.println("WiFi bağlantısı başarısız oldu. Normal işlevlerle devam ediliyor.");
    WiFi.disconnect(); // Bağlantı denemesini sonlandır
    wifiConnected = false; // Bağlantı yok
  }
}

// Web istemcilerini yönetme fonksiyonu
void handleClient(WiFiClient client) {
  String currentLine = "";
  unsigned long currentTime = millis();
  unsigned long previousTime = currentTime;
  const long timeoutTime = 2000; // 2 saniye timeout

  while (client.connected() && currentTime - previousTime <= timeoutTime) {
    currentTime = millis();
    if (client.available()) {
      char c = client.read();
      header += c;
      if (c == '\n') {
        if (currentLine.length() == 0) {
          client.println("HTTP/1.1 200 OK");
          client.println("Content-type:text/html");
          client.println("Connection: close");
          client.println();
          
          // Web sayfası HTML - Sadeleştirilmiş tasarım
          client.println("<!DOCTYPE html><html>");
          client.println("<head><meta charset='UTF-8'><meta name='viewport' content='width=device-width, initial-scale=1'>");
          client.println("<title>Turizm Araba</title>");
          client.println("<style>");
          client.println("body { font-family: Arial, sans-serif; text-align: center; margin: 0; padding: 20px; background-color: #f0f0f0; }");
          client.println("h1 { color: #2c3e50; }");
          client.println(".container { max-width: 800px; margin: 0 auto; background-color: white; padding: 20px; border-radius: 10px; box-shadow: 0 0 10px rgba(0,0,0,0.1); }");
          client.println(".status { margin: 20px 0; padding: 15px; border-radius: 5px; font-size: 1.2em; }");
          client.println(".playing { background-color: #d4edda; color: #155724; }");
          client.println(".not-playing { background-color: #f8d7da; color: #721c24; }");
          client.println(".audio-player { width: 100%; margin: 20px 0; }");
          client.println(".refresh-btn { margin-top: 20px; padding: 10px 20px; background-color: #007bff; color: white; border: none; border-radius: 5px; cursor: pointer; font-size: 1em; }");
          client.println(".refresh-btn:hover { background-color: #0056b3; }");
          client.println("</style>");
          client.println("</head>");
          
          client.println("<body>");
          client.println("<div class='container'>");
          client.println("<h1>Turizm Araba</h1>");
          
          // Çalma durumu gösterimi - Sadece mevcut çalınan ses
          if (sesCaliyor) {
            if (mevcutBolgeIndex >= 0 && mevcutBolgeIndex < 7) {
              client.println("<div class='status playing'>");
              client.print("<p>Şu anda <strong>");
              client.print(bolgeIsimleri[mevcutBolgeIndex]);
              client.println("</strong> bölgesi sesli rehberi çalınıyor</p>");
              client.println("</div>");
              
              // Çalınan bölge için ses oynatıcı
              client.println("<div class='audio-player'>");
              client.println("<p>Sesli rehberi dinleyin:</p>");
              client.print("<audio controls autoplay><source src='");
              client.print(sesDosyalariURL[mevcutBolgeIndex]);
              client.println("' type='audio/mpeg'>Tarayıcınız audio etiketini desteklemiyor.</audio>");
              client.println("</div>");
            } else {
              client.println("<div class='status playing'>");
              client.println("<p>Yolculuk sesi çalınıyor</p>");
              client.println("</div>");
            }
          } else {
            client.println("<div class='status not-playing'>");
            client.println("<p>Şu anda ses çalınmıyor</p>");
            client.println("</div>");
          }
          
          // Manuel yenileme butonu
          client.println("<button class='refresh-btn' onclick='location.reload()'>Sayfayı Yenile</button>");
          
          client.println("</div>"); // container kapanışı
          client.println("</body></html>");
          
          break;
        } else {
          currentLine = "";
        }
      } else if (c != '\r') {
        currentLine += c;
      }
    }
  }
  
  header = "";
  client.stop();
}