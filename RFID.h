#define SS_PIN 10  // RC522 SS pini
#define RST_PIN 9  // RC522 RST pini

MFRC522 rfid(SS_PIN, RST_PIN);

// Türkiye bölgeleri ve UID eşleşmeleri
struct Bolge {
  byte ID[7];
  byte uidLen; // UID uzunluğu
  String isim;
};

// UID'leri kendi kartlarının ID'leri ile değiştir!
Bolge bolgeler[14] = {
  // DİKDÖRTGEN (7 bayt)
  {{0x1D, 0x08, 0xD7, 0x6C, 0x08, 0x10, 0x80}, 7, "İç Anadolu"},
  {{0x1D, 0x4B, 0x2A, 0x6C, 0x08, 0x10, 0x80}, 7, "Karadeniz"},
  {{0x1D, 0xB8, 0xDA, 0x6D, 0x08, 0x10, 0x80}, 7, "Doğu Anadolu"},
  {{0x1D, 0x53, 0xDA, 0x6D, 0x08, 0x10, 0x80}, 7, "Güneydoğu Anadolu"},
  {{0x1D, 0x06, 0xFD, 0x6D, 0x08, 0x10, 0x80}, 7, "Akdeniz"},
  {{0x1D, 0xF0, 0xEB, 0x6D, 0x08, 0x10, 0x80}, 7, "Ege"},
  {{0x1D, 0xCD, 0x4C, 0x6E, 0x08, 0x10, 0x80}, 7, "Marmara"},
  // YUVARLAK (4 bayt)
  {{0x3F, 0x50, 0x46, 0x42}, 4, "İç Anadolu"},
  {{0x9F, 0x60, 0x44, 0x42}, 4, "Karadeniz"},
  {{0xDF, 0x31, 0x3C, 0x42}, 4, "Doğu Anadolu"},
  {{0xDF, 0xE1, 0x3C, 0x42}, 4, "Güneydoğu Anadolu"},
  {{0x8F, 0xAC, 0x4F, 0x42}, 4, "Akdeniz"},
  {{0x7F, 0x9B, 0x34, 0x42}, 4, "Ege"},
  {{0x2F, 0x76, 0x3C, 0x42}, 4, "Marmara"}
};

void rfidSetup() {
  SPI.begin();
  rfid.PCD_Init();
  Serial.println("Kartı okutun...");
}

void rfidLoop() {
  if (!rfid.PICC_IsNewCardPresent()) return;
  if (!rfid.PICC_ReadCardSerial()) return;

  Serial.print("Okunan UID (uzunluk: ");
  Serial.print(rfid.uid.size);
  Serial.print("): ");
  for (int i = 0; i < rfid.uid.size; i++) {
    Serial.print(rfid.uid.uidByte[i] < 0x10 ? " 0" : " ");
    Serial.print(rfid.uid.uidByte[i], HEX);
  }
  Serial.println();

  bool eslesme = false;

  // Tüm bölgeleri kontrol et (hem dikdörtgen hem yuvarlak kartlar için)
  for (int i = 0; i < 14; i++) {
    Serial.print("Bölge ");
    Serial.print(i);
    Serial.print(" (");
    Serial.print(bolgeler[i].isim);
    Serial.print(") UID uzunluğu: ");
    Serial.print(bolgeler[i].uidLen);
    Serial.print(" - UID: ");
    for (int j = 0; j < bolgeler[i].uidLen; j++) {
      Serial.print(bolgeler[i].ID[j] < 0x10 ? " 0" : " ");
      Serial.print(bolgeler[i].ID[j], HEX);
    }
    Serial.println();

    bool ayni = true;
    if (bolgeler[i].uidLen == 4 && rfid.uid.size >= 4) {
      // Sadece ilk 4 baytı karşılaştır
      for (int j = 0; j < 4; j++) {
        if (rfid.uid.uidByte[j] != bolgeler[i].ID[j]) {
          ayni = false;
          break;
        }
      }
    } else if (bolgeler[i].uidLen == 7 && rfid.uid.size == 7) {
      // Tüm 7 baytı karşılaştır
      for (int j = 0; j < 7; j++) {
        if (rfid.uid.uidByte[j] != bolgeler[i].ID[j]) {
          ayni = false;
          break;
        }
      }
    } else {
      ayni = false;
    }

    if (ayni) {
      Serial.print("Kart ");
      Serial.print(bolgeler[i].isim);
      Serial.println(" ile eşleşti!");
      eslesme = true;
      break;
    }
  }

  if (!eslesme) {
    Serial.println("Bölge bulunamadı, tanımlı değil.");
  }

  rfid.PICC_HaltA();
  rfid.PCD_StopCrypto1();
}